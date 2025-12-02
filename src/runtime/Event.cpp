#include "Event.h"

void Event::execute() {
    while (running.load()) {
        fn();
        execCounter++;

        std::this_thread::sleep_for(ticks);
    }
}