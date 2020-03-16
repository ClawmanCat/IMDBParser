#pragma once

#include <IMDBParser/Utility/Typedefs.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/TupleUtils.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <tuple>

namespace IMDBParser {
    template <typename... Models> class DataParser;

    template <typename... Ts> using AsParseResult = std::tuple<std::vector<Ts>...>;
    template <typename... Ts> using AsDataParser  = DataParser<Ts...>;


    template <typename... Models> class DataParser {
    public:
        using ParseResult  = AsParseResult<Models...>;
        using ColumnParser = Fn<ParseResult, std::string_view>;


        DataParser(ColumnParser&& parsefn, std::string&& seperator, std::size_t begin, std::size_t end) 
            : parser(std::move(parsefn)), seperator(std::move(seperator)), begin(begin), end(end) {}

        DataParser(ColumnParser&& parsefn, std::regex&& seperator, std::size_t begin, std::size_t end)
            : parser(std::move(parsefn)), seperator(std::move(seperator)), begin(begin), end(end) {}


        ParseResult parse(std::string_view data) const {
            data = get_lines(data, begin, end);

            std::vector<std::string_view> split_view;
            std::visit([&](const auto& s) { split_view = split(data, s); }, seperator);

            ParseResult result;
            for (auto view : split_view) result = merge_vector_tuples(std::vector{ std::move(result), parser(view) });

            return result;
        }
    private:
        ColumnParser parser;
        std::variant<std::string, std::regex> seperator;
        std::size_t begin, end;
    };
}