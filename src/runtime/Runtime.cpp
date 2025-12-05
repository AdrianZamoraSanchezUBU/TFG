#include "Runtime.h"
#include <iostream>

Runtime &Runtime::get() {
    static Runtime instance;
    return instance;
}

void Runtime::launchEventThread(std::shared_ptr<Event> ev) {
    ev->getWorker() = std::thread([ev]() { ev->execute(); });
}

void Runtime::registerEvent(std::string id, float time, void (*fn)()) {
    std::cout << "\nRegistering the event: " + id << std::endl;
    events.emplace_back(std::make_shared<Event>(id, time, fn));
}

void Runtime::printEventList() {
    std::cout << "\nPrinting events " + std::to_string(events.size()) + " in the list:" << std::endl;
    for (const auto &ev : events) {
        ev->print();
    }
}

void Runtime::start() {
    while (events.size() > 0 && running) {
        for (const auto &ev : events) {
            if (ev->getEventRunningFlag() == true) {
                std::cout << "Running a event" << std::endl;
                launchEventThread(ev);
            }
        }
    }
}

void Runtime::schedule(std::string id) {
    for (const auto &ev : events) {
        if (ev->getID() == id && ev->getEventRunningFlag() == false) {
            std::cout << "Scheduling the event " + id << std::endl;
            ev->startEvent();
        }
    }
}

void Runtime::stop() {
    running = false;
}