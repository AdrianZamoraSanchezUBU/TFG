#pragma once
#include "Event.h"
#include <mutex>
#include <vector>

/**
 * @brief Class for the language runtime structure.
 */
class Runtime {
    std::vector<Event *> events;
    std::mutex eventsMutex;
    float t;

  public:
    /**
     * @brief Default constructor.
     */
    Runtime(){};

    /**
     * Getter for the static item.
     */
    static Runtime &get();

    /**
     * Launches an event
     */
    void launchEventThread(Event *ev);

    /**
     * Saves the event data
     */
    void registerEvent(std::string id, float time, void (*fn)());

    /**
     * Prints the event list data
     */
    void printEventList();

    /**
     * Starts the time and event tracking
     */
    void start();
};