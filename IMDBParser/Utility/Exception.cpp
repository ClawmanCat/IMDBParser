#include <IMDBParser/Utility/Exception.hpp>
#include <IMDBParser/ParseController.hpp>

#include <cstddef>
#include <iostream>
#include <cstdio>
#include <cctype>


namespace IMDBParser {
    void raise_exception(std::string_view message) {
        std::cout << "ERROR: " << message << '\n';

        if (IMDBParser::ParseController::instance().has_argument("--pause-except")) {
            std::cout << "'Press X to exit the program, or any other key to continue execution... (or run without --pause-except to automatically continue.)\n";

            char c = (char) std::getchar();
            if (std::tolower(c) == 'x') std::exit(-1);
        }
    }


    void raise_warning(std::string_view message) {
        std::cout << "WARNING: " << message << '\n';

        if (IMDBParser::ParseController::instance().has_argument("--pause-warning")) {
            std::cout << "'Press any key to continue... (or run without --pause-warning to automatically continue.)\n";
            std::getchar();
        }
    }
}