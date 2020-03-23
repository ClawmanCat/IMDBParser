#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/Genre.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/ParserUtils.hpp>

#include <string_view>
#include <string>
#include <regex>
#include <algorithm>
#include <array>


namespace IMDBParser {
    namespace Detail {
        using GenreParserResult = AsParseResult<ModelGenre>;

        inline GenreParserResult genre_parser_fn(std::wstring_view row, std::atomic_uint& warn_count, std::atomic_uint& err_count) {
            IMDBPARSER_ANOMALY_HANDLER(raise_anomaly);
            ModelGenre result;

            auto genre_details = capture_groups(row, std::wregex(LR"REGEX((.+)\s+\((?:(?:(\?{4})|(\d{4}))(?:\/([IVXLCDM]+))?)\)\s*(\{.+\})?\s*(?:\((TV|V|VG)\))?\s*(\{.+\})?\t+(.+))REGEX"));
            if (genre_details.size() < 3) {
                raise_anomaly("Failed to parse genre listing.", true);
                return {};
            }


            unsigned fieldcount = movie_parser_common_section(result, genre_details);
            result.genre = genre_details[fieldcount];

            return {{ result }};
        }
    }


    inline DataParser<ModelGenre> GenreParser {
        &Detail::genre_parser_fn,
        L"\r\n",
        383,
        2346710
    };
}