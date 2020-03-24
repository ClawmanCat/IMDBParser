#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/Lifespan.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/Typedefs.hpp>
#include <IMDBParser/Utility/ParserUtils.hpp>

#include <string_view>
#include <string>
#include <regex>
#include <algorithm>


namespace IMDBParser {
    namespace Detail {
        using LifespanParserResult = AsParseResult<ModelLifespan>;

        inline LifespanParserResult lifespan_parser_fn(std::wstring_view row, std::atomic_uint& warn_count, std::atomic_uint& err_count) {
            IMDBPARSER_ANOMALY_HANDLER(raise_anomaly);
            ModelLifespan result;


            auto starts_with = [](std::wstring_view src, std::wstring_view match) {
                return src.find(match) == 0;
            };

            auto parse_date = [&](auto& model, bool is_bd, std::wstring_view data) {
                constexpr std::array months = {
                    L"January", L"February", L"March", L"April", L"May", L"June", L"July", L"August", L"September", L"October", L"November", L"December"
                };
                auto get_month = [&](std::wstring_view s) -> std::size_t { return std::find(months.begin(), months.end(), s) - months.begin() + 1; };


                using component_t = std::optional<unsigned>&;

                component_t year  = is_bd ? model.birthyr    : model.deathyr;
                component_t month = is_bd ? model.birthmonth : model.deathmonth;
                component_t day   = is_bd ? model.birthday   : model.deathday;

                auto date_components = capture_groups(data, std::wregex(LR"REGEX(D.:\s*(\d\d?)\s*(January|February|March|April|May|June|July|August|September|October|November|December)\s*(\d\d\d\d))REGEX"));
                if (date_components.size() != 3) {
                    raise_anomaly("Failed to parse date string. Column will be NULL for this row.", false);
                    return;
                }

                day   = to_number(date_components[0]).value();
                month = get_month(date_components[1]);
                year  = to_number(date_components[2]).value();
            };

            auto parse_name = [&](auto& model, std::wstring_view data) {
                auto name_components = capture_groups(data, std::wregex(LR"REGEX(NM:\s*((?:([\'\"].+[\'\"]),?\s*)?(?:(?:([^,]+),\s*(.+))|(.+))))REGEX"));
                if (name_components.size() == 0) {
                    raise_anomaly("Failed to parse name string. Column will be NULL for this row.", false);
                    return;
                }
                model.name = name_components[0];

                unsigned fieldcount = 1;
                if (is_surrounded_with(name_components[fieldcount], L'\'', L'\'') || is_surrounded_with(name_components[fieldcount], L'"', L'"')) {
                    model.nickname = name_components[fieldcount];
                    ++fieldcount;
                }

                if (name_components.size() - fieldcount < 2) {
                    raise_anomaly("Name does not contain lastname, firstname pair. These fields will be NULL.", false);
                    return;
                }

                model.lastname  = name_components[fieldcount++];
                model.firstname = name_components[fieldcount++];
            };


            for (std::wstring_view line : split(row, L"\r\n")) {
                if (starts_with(line, L"NM:")) parse_name(result, line);
                if (starts_with(line, L"DB:")) parse_date(result, true,  line);
                if (starts_with(line, L"DD:")) parse_date(result, false, line);
            }

            return {{ result }};
        }
    }


    inline DataParser<ModelLifespan> LifespanParser {
        &Detail::lifespan_parser_fn,
        L"-------------------------------------------------------------------------------",
        5,
        11626596
    };
}