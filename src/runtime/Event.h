/**
 * @file Event.h
 * @brief Contains the definition of the runtime event structure.
 *
 * @author Adrián Zamora Sánchez
 */

#include "math.h"
#include "spdlog/spdlog.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ffi.h>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#pragma once

/// This class represents a Event.
class Event {
    using EventFn = void (*)();

    std::string id;                  ///< Event ID
    std::chrono::milliseconds ticks; ///< Miliseconds for periodic execution
    int execLimit;                   ///< Execution limit
    int execCounter = 0;             ///< Execution counter

    EventFn fnPtr = nullptr; ///< Compilated function pointer

    // arg management
    int argCount = 0;          ///< Number of total arguments
    std::vector<int> argTypes; ///< Type codes from Event.cpp
    std::vector<void *> argv;  ///< Set at schedule call by mainLLVM())

    struct ArgSlot {
        alignas(16) std::array<std::uint8_t, 16> bytes{};
    };
    std::vector<ArgSlot> ownedArgs;

    std::mutex argsMutex;

    std::atomic<bool> running{false};
    std::thread worker;

  public:
    /**
     * @brief Default Event constructor.
     * @param id Identifier for this Event.
     * @param f Its executable code (a extern function).
     * @param t Ticks associated with the periodic execution.
     * @param execLimit Limit of executions.
     */
    Event(std::string id, float t, EventFn fnPtr, int argCount, const int *argTypes, int limit);

    /// @brief  Default Event destructor, important in thread termination management
    ~Event();

    /// Executes the event code.
    void execute();

    void setArgs(void **newArgv);
    void setArgsCopy(void **incoming);

    /**
     * @brief Getter for the worker thread.
     * @return Worker thread of this Event.
     */
    std::thread &getWorker() { return worker; }

    /**
     * @brief Getter for id.
     * @return Returns the identifier of the Event.
     */
    std::string &getID() { return id; }

    /// Sets the running flag to false for safe thread termination.
    void stopEvent() { running.store(false); };

    /// Sets the running flag to true for event execution.
    void startEvent() {
        if (running.load())
            return;

        running.store(true);
        getWorker() = std::thread([this]() { execute(); });
    };

    /**
     * @brief Getter for running flag.
     * @return `true` if the Event is running, `false` otherwise.
     */
    bool getEventRunningFlag() { return running.load(); };

    /// Prints the event data.
    void print() const { spdlog::debug("Event: {}", id); };
};