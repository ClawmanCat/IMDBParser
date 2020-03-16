#include "ParseController.hpp"

#include <vector>
#include <string>


int main(int argc, char** argv) {
    IMDBParser::ParseController::instance().controller_main(std::vector<std::string>(argv, argv + argc));
}