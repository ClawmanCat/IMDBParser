#include <IMDBParser/Utility/Exception.hpp>
#include <IMDBParser/ParseController.hpp>

#include <cstddef>
#include <iostream>
#include <cstdio>
#include <cctype>
#include <mutex>


namespace IMDBParser {
    std::mutex iostream_mtx = {};


    void raise_exception(std::wstring_view message) {
        std::lock_guard lock{ iostream_mtx };


        std::wcout << "ERROR: " << message << '\n';

        if (IMDBParser::ParseController::instance().has_argument(L"--pause-except")) {
            std::wcout << "'Press X to exit the program, or any other key to continue execution... (or run without --pause-except to automatically continue.)\n";

            char c = (char) std::getchar();
            if (std::tolower(c) == 'x') std::exit(-1);
        }
    }


    void raise_warning(std::wstring_view message) {
        std::lock_guard lock{ iostream_mtx };


        std::wcout << "WARNING: " << message << '\n';

        if (IMDBParser::ParseController::instance().has_argument(L"--pause-warning")) {
            std::wcout << "'Press any key to continue... (or run without --pause-warning to automatically continue.)\n";
            std::getchar();
        }
    }
}