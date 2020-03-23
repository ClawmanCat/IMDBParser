#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/Location.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/ParserUtils.hpp>

#include <string_view>
#include <string>
#include <regex>
#include <algorithm>
#include <array>


namespace IMDBParser {
    namespace Detail {
        using LocationParserResult = AsParseResult<ModelLocation>;

        inline LocationParserResult location_parser_fn(std::wstring_view row, std::atomic_uint& warn_count, std::atomic_uint& err_count) {
            IMDBPARSER_ANOMALY_HANDLER(raise_anomaly);
            ModelLocation result;

            auto location_details = capture_groups(row, std::wregex(LR"REGEX((.+)\s+\((?:(?:(\?{4})|(\d{4}))(?:\/([IVXLCDM]+))?)\)\s*(\{.+\})?\s*(?:\((TV|V|VG)\))?\s*(\{.+\})?\t+((?:.+,\s+)?([^\t]+))(?:\t+\((.+)\))?)REGEX"));
            if (location_details.size() < 4) {
                raise_anomaly("Failed to parse location listing.", true);
                return {};
            }


            unsigned fieldcount = movie_parser_common_section(result, location_details);
            auto get_next_incr = [&]() { return location_details[fieldcount++]; };

            if (is_surrounded_with(location_details.back(), L'(', L')')) {
                result.details = location_details.back();
                location_details.pop_back();
            }

            result.full_address = get_next_incr();
            result.country = get_next_incr();


            return {{ result }};
        }
    }


    inline DataParser<ModelLocation> LocationParser {
        &Detail::location_parser_fn,
        L"\r\n",
        15,
        1070950
    };
}