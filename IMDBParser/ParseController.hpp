#pragma once

#include <IMDBParser/Utility/FileIO.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/TupleUtils.hpp>
#include <IMDBParser/Parsers/ActorParser.hpp>

#include <boost/preprocessor.hpp>

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
        constexpr static inline std::wstring_view DATA_FOLDER = BOOST_PP_WSTRINGIZE(PARSER_DATASOURCE_DIR);
        constexpr static inline std::wstring_view CSV_FOLDER  = BOOST_PP_WSTRINGIZE(PARSER_CSV_DIR);


        static ParseController& instance(void) {
            static ParseController i;
            return i;
        }


        const static inline std::tuple list_parsers = std::make_tuple(
            //std::tuple { L"actresses.list", ActressParser },
            //std::tuple { L"actors.list",    ActorParser   }
        );

        
        void controller_main(std::vector<std::wstring>&& args) {
            this->arguments = args;

            expand(list_parsers, [&](const auto& list_parser_pair, auto index) {
                const auto& [filename, parser] = list_parser_pair;

                // Merge the lines since the actual seperator might not be a newline.
                std::wcout << "Loading file " << filename << "...\n";
                const auto filecontents = FileIO::read(join_variadic(L"/", DATA_FOLDER, filename));
                std::wcout << "File loaded!\n";

                std::wcout << "Parsing file " << filename << "...\n";
                auto result = parser.parse(filecontents);
                std::wcout << "Parsing completed!\n";

                std::wcout << "Writing CSV for file " << filename << "...\n";
                expand(result, [&filename = filename] <typename Model> (const std::vector<Model>& data, auto index  ) {
                    std::wcout << "Writing CSV for model " << typename_string<Model>(true) << "...\n";
                    FileIO::write_csv<Model>(join_variadic(L"", CSV_FOLDER, L"/", FileIO::stem(filename), L"_", typename_string<Model>(true), L".csv"), data);
                    std::wcout << "Finished writing model!\n";
                });
                std::wcout << "Finished writing CSV!\n";
            });
        }
        
        
        bool has_argument(std::wstring_view arg) {
            return std::find(arguments.begin(), arguments.end(), arg) != arguments.end();
        }
    private:
        ParseController(void) = default;

        std::vector<std::wstring> arguments;
    };
}