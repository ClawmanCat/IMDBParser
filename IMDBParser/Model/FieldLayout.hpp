#pragma once

#include <string_view>
#include <cstddef>

namespace IMDBParser {
    struct FieldLayout {
        std::string_view name;
        std::size_t offset, size;

        enum {
            VARCHAR,
            NVARCHAR,
            CHAR,
            NCHAR,
            INT,
            FLOAT
        } type;
    };
}