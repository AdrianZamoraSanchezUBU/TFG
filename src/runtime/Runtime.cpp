#include "Runtime.h"
#include <iostream>

Runtime &Runtime::get() {
    static Runtime instance;
    return instance;
}

void Runtime::launchEventThread(Event *ev) {
    ev->getWorker() = std::thread([ev]() { ev->execute(); });
}

void Runtime::registerEvent(std::string id, float time, void (*fn)()) {
    events.emplace_back(new Event(id, time, fn));
}

void Runtime::printEventList() {
    std::cout << "\nPrinting event list:" << std::endl;
    for (Event *ev : events) {
        ev->print();
    }
}

void Runtime::start() {
    for (Event *ev : events) {
        ev->print();
        ev->execute();
    }
}