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

void Runtime::registerEvent(std::string id, float period, void *fnPtr, int argCount, const int *argTypes, int limit) {
    events.emplace_back(std::make_shared<Event>(id, period, fnPtr, argCount, argTypes, limit));
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
            e->setArgsCopy(argv); // copia por valor
            e->startEvent();
            return;
        }
    }
}

void Runtime::stop() {
    running = false;
}