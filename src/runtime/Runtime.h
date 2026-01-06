#pragma once
#include "Event.h"
#include "spdlog/spdlog.h"
#include <memory>
#include <mutex>
#include <vector>

/// Class for the language runtime structure.
class Runtime {
    using Fn = void (*)(void *frame);

    std::vector<std::shared_ptr<Event>> events;
    std::mutex eventsMutex;
    bool running = true;
    float t;

  public:
    /// Default constructor.
    Runtime(){};

    /**
     * @brief Getter for the static item.
     * @return Runtime object.
     */
    static Runtime &get();

    /**
     * @brief Executes an event.
     * @param ev Event to execute.
     */
    void launchEventThread(std::shared_ptr<Event> ev);

    /**
     * @brief Terminates a event.
     * @param id Given event identifier.
     */
    void terminateEvent(std::string id);

    /**
     * @brief Schredule a registered event.
     * @param id of the Event to schedule.
     */
    void scheduleEvent(std::string id, void **argv);

    /**
     * Checks the current Event status and manages the event list.
     */
    void checkEvents();

    /**
     * @brief Return the size of the Event list.
     * @return Amount of registered events.
     */
    int getEventCount() { return events.size(); };

    using EventFn = void (*)();
    /**
     * @brief Saves the event data.
     * @param id Identifier of the new Event.
     * @param period Time period of the new Event.
     * @param fnPtr Compilated event function name.
     * @param argCount Number of parameters of the function signature.
     * @param argTypes Types of the function parameters.
     * @param limit Number of limit executions for this Event, if set to 0 it has no numeric limit.
     */
    void registerEvent(std::string id, float period, EventFn fnPtr, int argCount, const int *argTypes, int limit);

    /// Prints the event list data.
    void printEventList();

    /// Stops the infinite loop and shutdowns all the Events.
    void stop();
};