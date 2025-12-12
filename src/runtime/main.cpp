#include "Runtime.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

Runtime GLOBAL_RUNTIME;

/// Runtime getter from LLVM module
extern "C" Runtime *getRuntime() {
    return &GLOBAL_RUNTIME;
}

/**
 * Function responsable of loading data from extern functions to the runtime
 */
extern "C" void
registerEventData(const char *id, float period, void *fnPtr, int argCount, const int *argTypes, int limit) {
    getRuntime()->registerEvent(std::string(id), period, fnPtr, argCount, argTypes, limit);
}

/**
 * Function responsable of loading data from extern functions to the runtime
 */
extern "C" void scheduleEventData(const char *id, void **argv) {
    getRuntime()->scheduleEvent(std::string(id), argv);
}

/**
 * Main LLVM caller
 */
extern "C" int mainLLVM(void);
int main(int argc, char **argv) {

    int ret = mainLLVM();

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