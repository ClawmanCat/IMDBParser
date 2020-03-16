#pragma once

#include <string_view>
#include <cstddef>
#include <iostream>
#include <cstdio>
#include <cctype>

namespace IMDBParser {
    inline void raise_exception(std::string_view message) {
        std::cout << "An exception occurred:\n";
        std::cout << message << '\n';

        //auto& controller = ParseController::instance();
        if (/*controller.has_argument("--nopause-except")*/ false) {
            std::cout << "(Run without --nopause-except to pause on exceptions.)\n";
        } else {
            std::cout << "'Press X to exit or any other key to continue... (or run with --nopause-except to automatically continue.)";
            
            char c = (char) std::getchar();
            if (std::tolower(c) == 'x') std::exit(-1);
        }
    }
}