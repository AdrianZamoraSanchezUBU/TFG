
#include "Event.h"
#include <ffi.h>
#include <iostream>
#include <stdexcept>

using EventFn = void (*)();

Event::Event(std::string id, float t, EventFn fnPtr, int argCount, const int *argTypesIn, int limit)
    : id(std::move(id)), ticks(static_cast<int>(std::ceil(t))), execLimit(limit), fnPtr(fnPtr), argCount(argCount),
      argTypes(argTypesIn, argTypesIn + argCount), argv(argCount, nullptr) {}

Event::~Event() {
    running.store(false);

    // Detach to prevent abortion
    if (worker.joinable())
        worker.join();
}

static size_t typeSize(int code) {
    switch (code) {
    case 1:
        return sizeof(int32_t); // TYPE_INT
    case 2:
        return sizeof(float); // TYPE_FLOAT
    case 3:
        return sizeof(void *); // TYPE_STRING & PTR
    default:
        return sizeof(void *);
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

void Event::setArgsCopy(void **incoming) {
    std::lock_guard<std::mutex> lock(argsMutex);

    // Checking the sizes
    if ((int)argv.size() != argCount)
        argv.assign(argCount, nullptr);

    if ((int)ownedArgs.size() != argCount)
        ownedArgs.assign(argCount, {});

    for (int i = 0; i < argCount; ++i) {
        if (!incoming[i]) {
            throw std::runtime_error("scheduleEventData: incoming argv[" + std::to_string(i) + "] is null");
        }

        // Resize to fit the value
        size_t sz = typeSize(argTypes[i]);
        if (sz > ownedArgs[i].bytes.size()) {
            throw std::runtime_error("ArgSlot too small for arg " + std::to_string(i));
        }

        // Data deserialization
        std::memcpy(ownedArgs[i].bytes.data(), incoming[i], sz);
        argv[i] = ownedArgs[i].bytes.data();
    }
}

void Event::execute() {
    ffi_cif cif;
    std::vector<ffi_type *> ffiTypes;
    ffiTypes.reserve(argCount);

    // libffi preparation
    for (int i = 0; i < argCount; ++i) {
        ffiTypes[i] = codeToFFI(argTypes[i]);
    }
    if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, argCount, &ffi_type_void, ffiTypes.data()) != FFI_OK) {
        std::cerr << "ffi_prep_cif failed for event " << id << "\n";
        running.store(false);
        return;
    }

    // Loading the call arguments
    while (running.load()) {
        try {
            // Copy argv under mutex
            std::vector<void *> localArgv;

            {
                std::lock_guard<std::mutex> lock(argsMutex);
                localArgv = argv;
            }

            // Check for nullptr
            if (argCount == 0) {
                if (!localArgv.empty()) {
                    throw std::runtime_error("Event has argCount=0 but argv is not empty");
                }

                // Calling with no argv
                ffi_call(&cif, FFI_FN(fnPtr), nullptr, nullptr);

            } else {
                // Call with argv
                if ((int)localArgv.size() != argCount) {
                    throw std::runtime_error("Event argv size mismatch (expected " + std::to_string(argCount) +
                                             ", got " + std::to_string(localArgv.size()) + ")");
                }

                for (int i = 0; i < argCount; ++i) {
                    if (!localArgv[i]) {
                        throw std::runtime_error("Event argv contains nullptr (missing schedule args)");
                    }
                }

                ffi_call(&cif, FFI_FN(fnPtr), nullptr, localArgv.data());
            }

        } catch (const std::exception &e) {
            std::cerr << "Exception in event '" << id << "': " << e.what() << "\n";
        } catch (...) {
            std::cerr << "Unknown exception in event '" << id << "'\n";
        }

        // Event execution limit management
        if (execLimit > 0 && ++execCounter > execLimit - 1)
            stopEvent();

        std::this_thread::sleep_for(ticks);
    }
}
