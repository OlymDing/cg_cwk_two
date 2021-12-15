#pragma once
#include <chrono>
#include <iostream>

class Timer {

    private:
        std::chrono::_V2::system_clock::time_point start;

    public:
        Timer () {
            start = std::chrono::high_resolution_clock::now();
        }

        ~Timer () {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> count = end - start;

            std::cout << 1/count.count() << " fps" << std::endl; 
        }

};