#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/Movie.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>

#include <string_view>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>


namespace IMDBParser {
    namespace Detail {
        using MovieParserResult = AsParseResult<ModelMovie>;

        inline MovieParserResult movie_parser_fn(std::wstring_view row, std::atomic_uint& warn_count, std::atomic_uint& err_count) {
            auto to_number = [](std::wstring_view str) -> std::optional<unsigned> {
                std::string s = to_nstring(str);
                for (char ch : s) if (!std::isdigit(ch)) return std::nullopt;
                return std::stoi(s);
            };

            IMDBPARSER_ANOMALY_HANDLER(raise_anomaly);
            ModelMovie result;


            auto movie_details = capture_groups(row, std::wregex(LR"REGEX((?:\"?(.+)\"?)\s+\((\d{4}|\?{4})(?:\/([IVXLCDM]+))?\)\s*(?:\((TV|V|VG)\))?\s*(\{.+\})?\s*\t+(\d{4}|\?{4})(?:-(\d{4}|\?{4}))?)REGEX"));
            if (movie_details.size() < 3) {
                raise_anomaly("Failed to parse movie listing.", true);
                return {};
            }


            // Not catching the closing quote would make the regex significantly more complicated.
            if (movie_details[0].back() == L'"') movie_details[0].remove_suffix(1);
            result.title = movie_details[0];

            result.release_yr = to_number(movie_details[1]);


            unsigned fieldcount = 2;
            auto has_next = [&]() { return fieldcount < movie_details.size(); };
            auto get_next = [&]() { return movie_details[fieldcount]; };

            if (is_roman_numeral(get_next())) {
                result.release_nr = parse_roman_numeral(get_next());
                ++fieldcount;
            }

            if (matches_any(get_next(), L"TV", L"V")) {
                result.release_type = (get_next() == L"TV")
                    ? ModelMovie::ReleaseType::TV
                    : ModelMovie::ReleaseType::VIDEO;

                ++fieldcount;
            } else result.release_type = ModelMovie::ReleaseType::CINEMA;

            if (is_surrounded_with(get_next(), L'{', L'}')) {
                result.episode = desurround(get_next());
                ++fieldcount;
            }

            result.start_yr = to_number(get_next());
            ++fieldcount;

            if (has_next()) {
                result.end_yr = to_number(get_next());
            }


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