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
extern "C" void registerEventData(const char *id, float period, void (*fn)(), int limit) {
    std::string sid(id);
    getRuntime()->registerEvent(sid, period, fn, limit);
}

/**
 * Function responsable of loading data from extern functions to the runtime
 */
extern "C" void scheduleEvent(const char *id) {
    std::string sid(id);
    getRuntime()->schedule(sid);
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