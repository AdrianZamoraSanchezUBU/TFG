#include "Event.h"

void Event::execute() {
    while (running.load()) {
        try {
            fn();
        } catch (const std::exception &e) {
            std::cerr << "Exception in event: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown exception in event" << std::endl;
        }

        // Limit comprobation
        if (++execCounter == execLimit) {
            stopEvent();
        }

        std::this_thread::sleep_for(ticks);
    }
}