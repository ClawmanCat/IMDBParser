#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/MovieLanguage.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/ParserUtils.hpp>

#include <string_view>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>


namespace IMDBParser {
    namespace Detail {
        using MovieLanguageParserResult = AsParseResult<ModelMovieLanguage>;

        inline MovieLanguageParserResult movie_language_parser_fn(std::wstring_view row, std::atomic_uint& warn_count, std::atomic_uint& err_count) {
            IMDBPARSER_ANOMALY_HANDLER(raise_anomaly);
            
            ModelMovieLanguage result;

            auto language_details = capture_groups(row, std::wregex(LR"REGEX((\"?.+\"?)\s+\((\d{4}|\?{4})(?:\/([IVXLCDM]+))?\)\s*(?:\((TV|V|VG)\))?\s*(\{.+\})?\t+(\w+)\s*(?:\((.+)\))?)REGEX"));
            if (language_details.size() < 3) {
                raise_anomaly("Failed to parse movie language listing.", true);
                return {};
            }


            unsigned fieldcount = movie_parser_common_section(result, language_details);

            result.language = language_details[fieldcount++];
            if (fieldcount < language_details.size()) result.details = language_details[fieldcount];


            return {{ result }};
        }
    }


    inline DataParser<ModelMovieLanguage> MovieLanguageParser {
        &Detail::movie_language_parser_fn,
        L"\r\n",
        15,
        1913779
    };
}