#pragma once

#include <utility>

namespace IMDBParser {
    template <typename... Fns>
    struct Visitor : public Fns... {
        Visitor(Fns&&... fns) : Fns(std::forward<Fns>(fns))... {}

        using Fns::operator()...;
    };
}