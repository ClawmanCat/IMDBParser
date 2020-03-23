#pragma once

namespace IMDBParser {
    template <typename Ret, typename... Args>
    using Fn = Ret(*)(Args...);

    template <typename Class, typename T>
    using MemVar = T (Class::*);
}