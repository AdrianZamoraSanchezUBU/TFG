#include "Event.h"
#include <ffi.h>
#include <iostream>
#include <stdexcept>

using EventFn = void (*)();

Event::Event(std::string id, float t, EventFn fnPtr, int argCount, const int *argTypesIn, int limit)
    : id(std::move(id)), ticks(static_cast<int>(std::ceil(t))), execLimit(limit), fnPtr(fnPtr), argCount(argCount),
      argTypes(argTypesIn, argTypesIn + argCount), argv(argCount, nullptr) {}

void Event::setArgs(void **newArgv) {
    std::lock_guard<std::mutex> lock(argsMutex);
    for (int i = 0; i < argCount; ++i) {
        argv[i] = newArgv[i];
    }
}

static size_t typeSize(int code) {
    switch (code) {
    case 1:
        return sizeof(int32_t); // TYPE_INT
    case 2:
        return sizeof(float); // TYPE_FLOAT
    case 3:
        return sizeof(void *); // TYPE_STRING / PTR
    default:
        return sizeof(void *);
    }
}

void Event::setArgsCopy(void **incoming) {
    std::lock_guard<std::mutex> lock(argsMutex);

    // Asegurar tamaños SIEMPRE
    if ((int)argv.size() != argCount)
        argv.assign(argCount, nullptr);

    if ((int)ownedArgs.size() != argCount)
        ownedArgs.assign(argCount, {});

    for (int i = 0; i < argCount; ++i) {
        if (!incoming[i]) {
            throw std::runtime_error("scheduleEventData: incoming argv[" + std::to_string(i) + "] is null");
        }

        size_t sz = typeSize(argTypes[i]);
        if (sz > ownedArgs[i].bytes.size()) {
            throw std::runtime_error("ArgSlot too small for arg " + std::to_string(i));
        }

        std::memcpy(ownedArgs[i].bytes.data(), incoming[i], sz);
        argv[i] = ownedArgs[i].bytes.data();
    }
}

static ffi_type *codeToFFI(int code) {
    switch (code) {
    case 1:
        return &ffi_type_sint32;
    case 2:
        return &ffi_type_float;
    case 3:
        return &ffi_type_pointer;
    default:
        return &ffi_type_pointer;
    }
}

void Event::execute() {
    // libffi preparation
    ffi_cif cif;
    std::vector<ffi_type *> ffiTypes(argCount);
    for (int i = 0; i < argCount; ++i) {
        ffiTypes[i] = codeToFFI(argTypes[i]);
    }

    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argCount, &ffi_type_void, ffiTypes.data()) != FFI_OK) {
        std::cerr << "ffi_prep_cif failed for event " << id << "\n";
        running.store(false);
        return;
    }

    while (running.load()) {
        try {
            // Copy argvb under mutex
            std::vector<void *> localArgv;
            localArgv.resize(argCount);

            {
                std::lock_guard<std::mutex> lock(argsMutex);
                localArgv = argv;
            }

            // Check for nullptr
            for (int i = 0; i < argCount; ++i) {
                if (!localArgv[i]) {
                    throw std::runtime_error("Event argv contains nullptr (missing schedule args)");
                }
            }

            for (int i = 0; i < argCount; ++i) {
                spdlog::debug("  arg[{}] storage={} (ptr) = {}", i, (void *)localArgv[i], *(int32_t *)localArgv[i]);
            }

            uint8_t dummy;
            ffi_call(&cif, FFI_FN(fnPtr), &dummy, localArgv.data());

        } catch (const std::exception &e) {
            std::cerr << "Exception in event '" << id << "': " << e.what() << "\n";
        } catch (...) {
            std::cerr << "Unknown exception in event '" << id << "'\n";
        }

        if (execLimit > 0 && ++execCounter >= execLimit)
            stopEvent();
        else
            ++execCounter;

        std::this_thread::sleep_for(ticks);
    }
}
