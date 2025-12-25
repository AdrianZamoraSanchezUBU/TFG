#include "Runtime.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

Runtime GLOBAL_RUNTIME;

/// Runtime getter from LLVM module
extern "C" Runtime *getRuntime() {
    return &GLOBAL_RUNTIME;
}

using EventFn = void (*)();

/**
 * Function responsible of loading data from extern functions to the runtime
 */
extern "C" void
registerEventData(const char *id, float period, void (*fnPtr)(), int argCount, const int *argTypes, int limit) {
    fprintf(stderr, "registerEventData OK id=%s fnPtr=%p\n", id, (void *)fnPtr);
    fflush(stderr);
    getRuntime()->registerEvent(std::string(id), period, fnPtr, argCount, argTypes, limit);
    fprintf(stderr, "registerEventData RETURN\n");
    fflush(stderr);
}

/**
 * Function responsible of loading data from extern functions to the runtime
 */
extern "C" void scheduleEventData(const char *id, void **argv) {
    fprintf(stderr, "scheduleEventData id=%s argv=%p argv0=%p\n", id, (void *)argv, argv ? argv[0] : nullptr);
    fflush(stderr);
    getRuntime()->scheduleEvent(std::string(id), argv);
}

/**
 * Main LLVM caller
 */
extern "C" int mainLLVM(void);
int main(int argc, char **argv) {
    spdlog::debug("0");
    int ret = mainLLVM();
    spdlog::debug("0-1");

    // If there are events running the main loop keeps active
    while (true) {
        if (GLOBAL_RUNTIME.getEventCount() <= 0) {
            break;
        } else {
            GLOBAL_RUNTIME.checkEvents();
        }
    }

    return ret;
}