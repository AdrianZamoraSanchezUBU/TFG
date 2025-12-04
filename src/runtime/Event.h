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

    std::atomic<bool> running{true};
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

    /// Sets the running flag for safe thread termination.
    void stopEvent() { running.store(false); };

    /// Prints the event data.
    void print() const { std::cout << "event: " + id << std::endl; };
};