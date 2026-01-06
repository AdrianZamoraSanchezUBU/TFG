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
 * Function responsible of loading event data in the runtime.
 * @param id Identifier of the new Event.
 * @param period Time period of the new Event.
 * @param fnPtr Compilated event function name.
 * @param argCount Number of parameters of the function signature.
 * @param argTypes Types of the function parameters.
 * @param limit Number of limit executions for this Event, if set to 0 it has no numeric limit.
 */
extern "C" void
registerEventData(const char *id, float period, void (*fnPtr)(), int argCount, const int *argTypes, int limit) {
    getRuntime()->registerEvent(std::string(id), period, fnPtr, argCount, argTypes, limit);
}

/**
 * Function responsible of executing a event in the runtime.
 * @param id Event id to execute.
 * @param argv Arguments for the event execution.
 */
extern "C" void scheduleEventData(const char *id, void **argv) {
    getRuntime()->scheduleEvent(std::string(id), argv);
}

/**
 * Function responsible of stopping a event.
 * @param id Event id to terminate.
 */
extern "C" void exitEvent(const char *id) {
    getRuntime()->terminateEvent(std::string(id));
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