#include "Runtime.h"
#include <iostream>

Runtime &Runtime::get() {
    static Runtime instance;
    return instance;
}

void Runtime::checkEvents() {
    for (int i = 0; i < events.size();) {
        if (!events[i]->getEventRunningFlag()) {
            events[i]->stopEvent();
            events.erase(events.begin() + i);
        } else {
            ++i;
        }
    }
};

using EventFn = void (*)();

void Runtime::registerEvent(std::string id, float period, EventFn fnPtr, int argCount, const int *argTypes, int limit) {
    events.emplace_back(std::make_shared<Event>(id, period, fnPtr, argCount, argTypes, limit));
}

void Runtime::terminateEvent(std::string id) {
    std::shared_ptr<Event> eventToTerminate;

    {
        // Operation under mutex
        std::lock_guard<std::mutex> lock(eventsMutex);

        // Search the event in the list
        auto it = std::find_if(events.begin(), events.end(),
                               [&](const std::shared_ptr<Event> &e) { return e->getID() == id; });

        if (it == events.end()) {
            return; // Event not found
        }

        // Removes the event from the list
        eventToTerminate = *it;
        events.erase(it);
    }

    // Event stop signal
    eventToTerminate->stopEvent();
}

void Runtime::printEventList() {
    spdlog::debug("\nPrinting events {} in the list:", std::to_string(events.size()));
    for (const auto &ev : events) {
        ev->print();
    }
}

void Runtime::scheduleEvent(std::string id, void **argv) {
    for (auto &e : events) {
        if (e->getID() == id) {
            e->setArgsCopy(argv);
            e->startEvent();
            return;
        }
    }
}

void Runtime::stop() {
    running = false;
}