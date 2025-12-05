#pragma once
#include "Event.h"
#include <memory>
#include <mutex>
#include <vector>

/// Class for the language runtime structure.
class Runtime {
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
     * @brief Schredule a registered event.
     * @param id of the Event to schedule.
     */
    void schedule(std::string id);

    /**
     * @brief Saves the event data.
     * @param id Identifier of the new Event.
     * @param time Time period of the new Event.
     * @param fn Function to run when the Event is executed.
     */
    void registerEvent(std::string id, float time, void (*fn)());

    /// Prints the event list data.
    void printEventList();

    /// Starts the time and event scheduler.
    void start();

    /// Stops the infinite loop and shutdowns all the Events.
    void stop();
};