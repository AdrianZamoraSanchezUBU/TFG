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

void Runtime::registerEvent(std::string id, float time, void (*fn)(), int limit) {
    events.emplace_back(std::make_shared<Event>(id, time, fn, limit));
}

void Runtime::printEventList() {
    std::cout << "\nPrinting events " + std::to_string(events.size()) + " in the list:" << std::endl;
    for (const auto &ev : events) {
        ev->print();
    }
}

void Runtime::schedule(std::string id) {
    for (const auto &ev : events) {
        if (ev->getID() == id && ev->getEventRunningFlag() == false) {
            ev->startEvent();
        }
    }
}

void Runtime::stop() {
    running = false;
}