#include <iostream>
#include <chrono>
#include <thread>

#include "NativeInterface.h"

namespace c = std::chrono;

float sum = 0.0f;
int   num = 0;

void poll(NativeInterface *ni) {

    std::vector<int> v = {1, 5, 10, 25, 100, 250, 500};
    int idx = 0;

    const auto interval = c::seconds(20);
    auto next = c::high_resolution_clock::now() + interval;

    while (true) {
        if (ni->hasErrored()) {
            std::cout << "error occured: " << ni->getError() << std::endl;
            return;
        }

        auto events = ni->getEvents();

        if (c::high_resolution_clock::now() > next) {
            if (++idx >= v.size()) return;
            next += interval;
            std::cout << v[idx] << "ms - " << ni->contentions() << std::endl;
        }

        std::this_thread::sleep_for(c::milliseconds(v[idx]));
    }
}

int main() {
    NativeInterface *ni = new NativeInterface("/Users/rmeusel/Desktop/test");

    auto t = std::thread(poll, ni);
    t.join();

    return 0;
}
