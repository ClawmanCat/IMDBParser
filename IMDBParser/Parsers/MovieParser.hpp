#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/Movie.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/ParserUtils.hpp>

#include <string_view>
#include <string>
#include <regex>
#include <algorithm>


namespace IMDBParser {
    namespace Detail {
        using MovieParserResult = AsParseResult<ModelMovie>;

        inline MovieParserResult movie_parser_fn(std::wstring_view row, std::atomic_uint& warn_count, std::atomic_uint& err_count) {
            IMDBPARSER_ANOMALY_HANDLER(raise_anomaly);
            ModelMovie result;


            auto movie_details = capture_groups(row, std::wregex(LR"REGEX((\"?.+\"?)\s+\((\d{4}|\?{4})(?:\/([IVXLCDM]+))?\)\s*(?:\((TV|V|VG)\))?\s*(\{.+\})?\s*\t+(\d{4}|\?{4})(?:-(\d{4}|\?{4}))?)REGEX"));
            if (movie_details.size() < 3) {
                raise_anomaly("Failed to parse movie listing.", true);
                return {};
            }


            unsigned fieldcount = movie_parser_common_section(result, movie_details);

            result.start_yr = to_number(movie_details[fieldcount++]);
            if (fieldcount < movie_details.size()) result.end_yr = to_number(movie_details[fieldcount]);


            return {{ result }};
        }
    }


    inline DataParser<ModelMovie> MovieParser {
        &Detail::movie_parser_fn,
        L"\r\n",
        16,
        3955399
    };
}