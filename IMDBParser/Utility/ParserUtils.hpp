#pragma once

#include <IMDBParser/Utility/StringUtils.hpp>

#include <cstdlib>
#include <string>
#include <optional>
#include <algorithm>
#include <cctype>


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
    inline std::optional<unsigned> to_number(std::wstring_view str) {
        std::string s = to_nstring(str);
        for (char ch : s) if (!std::isdigit(ch)) return std::nullopt;
        return std::stoi(s);
    }


    template <typename Model>
    inline void movie_parser_release_info(Model& model, std::wstring_view str) {
        if (model.media_type == Model::MediaType::TV_SERIES) {
            model.release_type = Model::ReleaseType::TV;
        } else {
            if (str == L"VG") {
                model.media_type = Model::MediaType::VIDEO_GAME;
                model.release_type = Model::ReleaseType::UNDEFINED;
            }
            else if (str == L"TV") model.release_type = Model::ReleaseType::TV;
            else if (str == L"V") model.release_type = Model::ReleaseType::VIDEO;
            else model.release_type = Model::ReleaseType::CINEMA;
        }
    }


    // Many datafiles include a movie listing, the parsing is the same every time.
    // Returns the index of the first unparsed field, if any.
    template <typename Model, typename Data>
    inline unsigned movie_parser_common_section(Model& model, const Data& data, std::size_t offset = 0) {
        bool surrounded  = is_surrounded_with(data[offset + 0], L'"', L'"');
        model.title      = surrounded ? desurround(data[offset + 0]) : data[offset + 0];
        model.media_type = surrounded ? Model::MediaType::TV_SERIES : Model::MediaType::MOVIE;
        model.release_yr = to_number(data[offset + 1]);


        unsigned fieldcount = offset + 2;
        auto get_next = [&]() { return data[fieldcount]; };
        auto has_next = [&]() { return fieldcount < data.size(); };

        if (has_next() && is_roman_numeral(get_next())) {
            model.release_nr = parse_roman_numeral(get_next());
            ++fieldcount;
        }

        auto next = has_next() ? get_next() : L"";
        movie_parser_release_info(model, next);
        if (matches_any(next, L"TV", L"V", L"VG")) ++fieldcount;


        if (has_next() && is_surrounded_with(get_next(), L'{', L'}')) {
            model.episode = desurround(get_next());
            ++fieldcount;
        }


        return fieldcount;
    }

}