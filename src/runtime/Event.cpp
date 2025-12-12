#include "Event.h"
#include <ffi.h>
#include <iostream>
#include <stdexcept>

Event::Event(std::string id, float t, void *fnPtr, int argCount, const int *argTypesIn, int limit)
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

    if ((int)argv.size() != argCount) {
        argv.assign(argCount, nullptr);
        ownedArgs.assign(argCount, {});
    }

    for (int i = 0; i < argCount; ++i) {
        if (!incoming[i]) {
            throw std::runtime_error("scheduleEventData: incoming argv[" + std::to_string(i) + "] is null");
        }

        size_t sz = typeSize(argTypes[i]);
        ownedArgs[i].resize(sz);
        std::memcpy(ownedArgs[i].data(), incoming[i], sz);

        argv[i] = ownedArgs[i].data(); // <- puntero estable para libffi
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
        return &ffi_type_pointer; // o abort si prefieres
    }
}

void Event::execute() {
    // Prepara libffi una sola vez (tipos no cambian)
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
            // Copiar argv bajo mutex para evitar data race si schedule actualiza mientras ejecuta
            std::vector<void *> localArgv;
            localArgv.resize(argCount);

            {
                std::lock_guard<std::mutex> lock(argsMutex);
                localArgv = argv;
            }

            // (Opcional) validar que no haya nullptr
            for (int i = 0; i < argCount; ++i) {
                if (!localArgv[i]) {
                    throw std::runtime_error("Event argv contains nullptr (missing schedule args)");
                }
            }

            ffi_call(&cif, FFI_FN(fnPtr), nullptr, localArgv.data());

        } catch (const std::exception &e) {
            std::cerr << "Exception in event '" << id << "': " << e.what() << "\n";
        } catch (...) {
            std::cerr << "Unknown exception in event '" << id << "'\n";
        }

        if (++execCounter == execLimit)
            stopEvent();
        std::this_thread::sleep_for(ticks);
    }
}
