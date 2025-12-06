#include "Runtime.h"
#include <iostream>

Runtime &Runtime::get() {
    static Runtime instance;
    return instance;
}

void Runtime::registerEvent(std::string id, float time, void (*fn)()) {
    events.emplace_back(std::make_shared<Event>(id, time, fn));
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