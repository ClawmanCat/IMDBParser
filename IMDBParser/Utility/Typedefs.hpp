#pragma once

namespace IMDBParser {
    template <typename Ret, typename... Args>
    using Fn = Ret(*)(Args...);
}