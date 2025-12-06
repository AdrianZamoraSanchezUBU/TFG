#include "Event.h"

void Event::execute() {
    std::cout << "\nRunning a event" << std::endl;
    while (running.load()) {
        try {
            fn();
            fflush(stdout);
        } catch (const std::exception &e) {
            std::cerr << "Exception in event: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown exception in event" << std::endl;
        }

        execCounter++;

        std::this_thread::sleep_for(ticks);
    }
    std::cout << "\nEvent stopped" << std::endl;
}