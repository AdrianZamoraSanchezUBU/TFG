#include <atomic>
#include <chrono>
#include <iostream>
#include <math.h>
#include <string>
#include <thread>
#pragma once

/// This class represents a Event.
class Event {
    using Fn = void (*)();

    std::string id;
    Fn fn;
    std::chrono::milliseconds ticks;
    int execLimit;
    int execCounter;

    std::atomic<bool> running{false};
    std::thread worker;

  public:
    /**
     * @brief Default Event constructor.
     * @param id Identifier for this Event.
     * @param f Its executable code (a extern function).
     * @param t Ticks associated with the periodic execution.
     */
    Event(std::string id, float t, Fn f) : id(id), ticks(static_cast<int>(std::ceil(t))), fn(f){};

    /// Executes the event code.
    void execute();

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
    void startEvent() { running.store(true); };

    /**
     * @brief Getter for running flag.
     * @return `true` if the Event is running, `false` otherwise.
     */
    bool getEventRunningFlag() { return running.load(); };

    /// Prints the event data.
    void print() const { std::cout << "event: " + id << std::endl; };
};