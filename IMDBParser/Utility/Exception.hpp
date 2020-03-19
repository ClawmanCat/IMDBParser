#pragma once

#include <string_view>

namespace IMDBParser {
    extern void raise_exception(std::wstring_view message);
    extern void raise_warning(std::wstring_view message);
}