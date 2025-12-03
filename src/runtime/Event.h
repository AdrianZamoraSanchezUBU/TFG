#include <atomic>
#include <chrono>
#include <iostream>
#include <math.h>
#include <string>
#include <thread>
#pragma once

/**
 * @brief This class represents a Event.
 */
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
     * @brief Default Event constructor
     * @param id Identifier
     * @param f Its executable code (a extern function)
     * @param t ticks associated with the execution
     */
    Event(std::string id, float t, Fn f) : id(id), ticks(static_cast<int>(std::ceil(t))), fn(f){};

    /**
     * @brief Executes the event code.
     */
    void execute();

    /**
     * @brief Getter for the worker thread.
     */
    std::thread &getWorker() { return worker; }

    /**
     * @brief Sets the running flag for safe thread termination.
     */
    void stopEvent() { running.store(false); };

    /**
     * @brief Sets the running flag for safe thread termination.
     */
    void print() const { std::cout << "event: " + id << std::endl; };
};