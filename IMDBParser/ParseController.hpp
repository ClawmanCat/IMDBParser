#pragma once

#include <IMDBParser/Utility/FileIO.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/TupleUtils.hpp>
#include <IMDBParser/Parsers/ActorParser.hpp>

#include <array>
#include <tuple>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <string_view>
#include <type_traits>
#include <iostream>


namespace IMDBParser {
    class ParseController {
    public:
        // Provided by CMake
        constexpr static inline std::string_view DATA_FOLDER = PARSER_DATASOURCE_DIR;
        constexpr static inline std::string_view CSV_FOLDER  = PARSER_CSV_DIR;


        static ParseController& instance(void) {
            static ParseController i;
            return i;
        }


        const static inline std::tuple list_parsers {
            std::tuple { "actresses.list", ActressParser },
            std::tuple { "actors.list",    ActorParser   }
        };

        
        void controller_main(std::vector<std::string>&& args) {
            this->arguments = args;

            expand(list_parsers, [&](const auto& list_parser_pair, auto index) {
                const auto& [filename, parser] = list_parser_pair;

                // Merge the lines since the actual seperator might not be a newline.
                std::cout << "Loading file " << filename << '\n';
                const auto filecontents = join(FileIO::read(join_variadic("/", DATA_FOLDER, filename)), "\n");
                std::cout << "File Loaded\n";

                auto result = parser.parse(filecontents);

                std::cout << "Writing CSV for file " << filename << '\n';
                expand(result, [&filename = filename] <typename Model> (const std::vector<Model>& data, auto index  ) {
                    FileIO::write_csv<Model>(join_variadic("", CSV_FOLDER, "/", FileIO::stem(filename), ".csv"), data);
                });
                std::cout << "Finished writing CSV\n";
            });
        }
        
        
        bool has_argument(std::string_view arg) {
            return std::find(arguments.begin(), arguments.end(), arg) != arguments.end();
        }
    private:
        ParseController(void) = default;

        std::vector<std::string> arguments;
    };
}