#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ffi.h>
#include <iostream>
#include <math.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#pragma once

/// This class represents a Event.
class Event {
    std::string id;
    std::chrono::milliseconds ticks;
    int execLimit;
    int execCounter = 0;

    void *fnPtr = nullptr;

    int argCount = 0;
    std::vector<int> argTypes; // codes: 1=int,2=float,3=ptr,...
    std::vector<void *> argv;  // punteros a valores reales (se setean en schedule)
    std::vector<std::vector<uint8_t>> ownedArgs;

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
    Event(std::string id, float t, void *fnPtr, int argCount, const int *argTypes, int limit);

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
    void print() const { std::cout << "event: " + id << std::endl; };
};