#include "ParseController.hpp"

#include <vector>
#include <string>


int main(int argc, char** argv) {
    auto v = std::vector<std::string>(argv, argv + argc);
    //v.push_back("--silence-all");

    IMDBParser::ParseController::instance().controller_main(IMDBParser::all_to_wstring(v));
}