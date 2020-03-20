#include <IMDBParser/Utility/SilentCounter.hpp>
#include <IMDBParser/ParseController.hpp>

namespace IMDBParser {
    void SilentCounter::increment(void) {
        if (ParseController::instance().has_argument(L"--silence-all")) {
            if (unsigned v = ++count; v % 100'000 == 0) {
                std::wcout << "Still working... (" << v << " rows parsed, " << warnings << " warnings, " << errors << " errors.)\n";
            }
        }
    }
}