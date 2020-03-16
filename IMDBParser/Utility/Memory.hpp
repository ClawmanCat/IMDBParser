#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <string_view>

namespace IMDBParser::Memory {
    inline int64_t parse_signed(const void* memory, std::size_t count) {
        constexpr int64_t masks[] = { 0ll, ~0ll };

        // Two's complement makes this significantly more cumbersome.
        // If the source integer is less than 0, we need to set all bits more significant than the size of source.
        // After that, we can just copy in the old integer into the new one.
        // This is UB before C++20.
        int64_t dest = masks[(((uint8_t*) memory)[count - 1] & (1 << 7)) != 0] << 8 * count;
        memcpy(&dest, memory, count);

        return dest;
    }


    inline uint64_t parse_unsigned(const void* memory, std::size_t count) {
        uint64_t dest = 0;
        memcpy(&dest, memory, count);

        return dest;
    }


    inline double parse_float(const void* memory, std::size_t size) {
        double dest = 0;

        if      (size == sizeof(float))       dest = *reinterpret_cast<const float*>(memory);
        else if (size == sizeof(double))      dest = *reinterpret_cast<const double*>(memory);
        else if (size == sizeof(long double)) dest = *reinterpret_cast<const long double*>(memory);
        else    assert(false);  // Unknown floating point type.

        return dest;
    }


    inline std::string_view parse_string(const void* memory, std::size_t size) {
        return std::string_view((const char*) memory);
    }
}