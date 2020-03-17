#pragma once

#include <string_view>

namespace IMDBParser {
    extern void raise_exception(std::string_view message);
    extern void raise_warning(std::string_view message);
}