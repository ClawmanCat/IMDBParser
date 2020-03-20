#pragma once

#include <string>
#include <atomic>
#include <iostream>

namespace IMDBParser {
    class SilentCounter {
    public:
        SilentCounter(std::atomic_uint& warnings, std::atomic_uint& errors)
            : count(0),
              warnings(warnings),
              errors(errors)
        {}


        void increment(void);
    private:
        std::atomic_uint count;
        std::atomic_uint &warnings, &errors;
    };
}