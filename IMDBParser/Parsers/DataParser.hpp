#pragma once

#include <IMDBParser/Utility/Typedefs.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/TupleUtils.hpp>
#include <IMDBParser/Utility/ThreadUtils.hpp>
#include <IMDBParser/Utility/SilentCounter.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <tuple>
#include <atomic>



// TODO: Refactor this into a normal method. (Would require replacing capture with parameters everywhere, maybe use std::bind?)
#define IMDBPARSER_ANOMALY_HANDLER(Name)                                                    \
auto Name = [&](std::string_view message, bool except = true) {                             \
    auto fn = except ? &raise_exception : &raise_warning;                                   \
                                                                                            \
    fn(join_variadic(L"",                                                                   \
        except ? L"Failed to parse row" : L"Warning while parsing row",                     \
        L" \"",                                                                             \
        row,                                                                                \
        L"\": ",                                                                            \
        to_wstring(message),                                                                \
        except ? L" (Row will be skipped.)" : L""                                           \
        ));                                                                                 \
                                                                                            \
    except ? ++err_count : ++warn_count;                                                    \
}



namespace IMDBParser {
    template <typename... Models> class DataParser;

    template <typename... Ts> using AsParseResult = std::tuple<std::vector<Ts>...>;
    template <typename... Ts> using AsDataParser  = DataParser<Ts...>;


    template <typename... Models> class DataParser {
    public:
        using ParseResult  = AsParseResult<Models...>;
        using RowParser = Fn<ParseResult, std::wstring_view, std::atomic_uint&, std::atomic_uint&>;  // Row data, warning_count, error_count


        DataParser(RowParser&& parsefn, std::wstring&& seperator, std::size_t begin, std::size_t end) 
            : parser(std::move(parsefn)), seperator(std::move(seperator)), begin(begin), end(end) {}

        DataParser(RowParser&& parsefn, std::wregex&& seperator, std::size_t begin, std::size_t end)
            : parser(std::move(parsefn)), seperator(std::move(seperator)), begin(begin), end(end) {}


        template <typename String>  // Can't use string_view here since we can't convert the container.
        ParseResult parse(const std::vector<String>& data) const {
            std::atomic_uint warn_count = 0, err_count = 0;
            
            std::size_t end = this->end;
            if (end >= data.size()) {
                std::cout << "WARNING: Set endpoint lies outside file bounds and will be reduced.\n";
                end = data.size() - 1;
            }

            std::wstring cat;
            cat.reserve((end - begin) * 75);


            for (unsigned i = begin; i < end; ++i) cat.append(join_variadic(L"", data[i], L"\r\n"));


            std::vector<std::wstring_view> split_view;
            std::visit([&](const auto& s) { split_view = split(cat, s); }, seperator);

            SilentCounter counter { warn_count, err_count };
            ParseResult result = parallel_map_serial_combine(
                split_view, 
                [&](std::wstring_view view) { 
                    counter.increment();
                    return parser(view, warn_count, err_count); 
                },

                [&](std::vector<ParseResult>&& subresults) { return merge_vector_tuples(std::move(subresults)); }
            );

            std::wcout << "Parser completed with " << warn_count << " warnings and " << err_count << " errors.\n";
            std::wcout << "(Out of " << split_view.size() << " total entries.)\n";

            return result;
        }
    private:
        RowParser parser;
        std::variant<std::wstring, std::wregex> seperator;
        std::size_t begin, end;
    };
}