#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/Rating.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/ParserUtils.hpp>

#include <string_view>
#include <string>
#include <regex>
#include <algorithm>


namespace IMDBParser {
    namespace Detail {
        using RatingParserResult = AsParseResult<ModelRating>;

        inline RatingParserResult rating_parser_fn(std::wstring_view row, std::atomic_uint& warn_count, std::atomic_uint& err_count) {
            IMDBPARSER_ANOMALY_HANDLER(raise_anomaly);
            ModelRating result;

            auto rating_details = capture_groups(row, std::wregex(LR"REGEX(([\.\*0-9]{10})\s+(\d+)\s+(\d?\d\.\d)\s+(.+)\s+\((?:(?:(\?{4})|(\d{4}))(?:\/([IVXLCDM]+))?)\)\s*(\{.+\})?\s*(?:\((TV|V|VG)\))?)REGEX"));
            if (rating_details.size() < 5) {
                raise_anomaly("Failed to parse rating listing.", true);
                return {};
            }


            for (unsigned i = 0; i < rating_details[0].length(); ++i) {
                wchar_t ch = rating_details[0][i];

                if      (ch == L'.') result.score_distribution[i] = L"NONE";
                else if (ch == L'*') result.score_distribution[i] = L"ALL";
                else result.score_distribution[i] = join_variadic(L"", std::wstring() + ch, L"X%");
            }

            result.votes = to_number(rating_details[1]).value();
            
            auto pos = rating_details[2].find(L".");
            auto subnr = [&](auto off, auto cnt) { return to_number(std::wstring_view(&*(rating_details[2].begin() + off), cnt)).value(); };
            result.score = 10 * subnr(0, pos) + subnr(pos + 1, 1);


            movie_parser_common_section(result, rating_details, 3);

            return {{ result }};
        }
    }


    inline DataParser<ModelRating> RatingParser {
        &Detail::rating_parser_fn,
        L"\r\n",
        29,
        678822
    };
}