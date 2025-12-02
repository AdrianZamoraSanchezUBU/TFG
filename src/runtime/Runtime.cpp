#include "Runtime.h"

Runtime &Runtime::get() {
    static Runtime instance;
    return instance;
}

void Runtime::launchEventThread(Event *ev) {
    ev->getWorker() = std::thread([ev]() { ev->execute(); });
}

void Runtime::registerEvent(int id, float time, void (*fn)()) {
    events.emplace_back(new Event(id, time, fn));
}

void Runtime::start() {
    return;
}