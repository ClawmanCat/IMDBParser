#pragma once

#ifdef _WIN32
    #define unreachable_path __assume(0)
#else
    #define unreachable_path __builtin_unreachable()
#endif