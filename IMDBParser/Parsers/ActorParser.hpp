#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/Actor.hpp>
#include <IMDBParser/Utility/Traits.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/Exception.hpp>

#include <string_view>
#include <regex>


namespace IMDBParser {
    namespace Detail {
        using ActorParserModels = Pack<ModelActor, ModelActorAppearance>;
        using ActorParserResult = typename ActorParserModels::template Apply<AsParseResult>;
        

        // An entry in the actor list looks as follows:
        // (nickname)?(fullname)(\r\n)?(\t+(medianame)\s+\((year(/number)?)\)\s+(target)?(\{(appeared_when)\})?\s+(\(credited_as\))?\s+\[(role)\]?\<(billing_pos)\>?)+
        // 
        // ... where 'nickname' matches the pattern ['"]\s*(nickname = .+)\s*['"],\s+
        //
        // ... where fullname usually is one of 'firstname lastname' or 'lastname, firstname' but this can't be matched because the list contains a lot of 
        // irregular bullshit. ($haniqua the rabbit, '67 Chevrolet Impala, etc.)
        // 
        // ... where medianame matches one of the following:
        //          .+              => Movie
        //          \".+\"          => Television Series
        //          \".+\"\s+(mini) => Television Mini-Series
        //
        // ... where target is either (TV) or (V). (Made for TV and made for video respectively.)
        //
        // TODO: Add command line flag to automatically remove questionable entries.
        // TODO: Support unicode.
        template <bool Male> inline ActorParserResult actor_parser_fn(std::wstring_view column, std::atomic_uint& warn_count, std::atomic_uint& err_count) {
            if (ParseController::instance().has_argument(L"--silence-all")) {
                static std::atomic_uint count = 0;

                if (unsigned v = ++count; v % 100'000 == 0) 
                    std::wcout << "Still working... (" << v << " rows parsed, " << warn_count << " warnings, " << err_count << " errors.)\n";
            }


            ActorParserResult result;

            auto raise_anomaly = [&](std::string_view message, bool except = true) {
                auto fn = except ? &raise_exception : &raise_warning;

                fn(join_variadic(L"",
                    except ? L"Failed to parse column" : L"Warning while parsing column",
                    L" \"",
                    column,
                    L"\": ",
                    to_wstring(message),
                    except ? L" (Column will be skipped.)" : L""
                ));

                except ? ++err_count : ++warn_count;
            };


            auto name_appearance_split = split_on_first_recurring(column, L'\t');
            if (name_appearance_split == std::nullopt) { raise_anomaly("Column does not contain name/movie[] pair.", true); return {}; }


            // Parse Actor Name
            auto actor_details = capture_groups(name_appearance_split.value()[0], std::wregex(LR"REGEX((?:['\"]\s*(.+)['\"][.,]?\s*)?(?:(.+)))REGEX"));
            if (actor_details.size() < 1) { raise_anomaly("Column does not contain a valid actor name.", true); return {}; }

            ModelActor actor;
            actor.is_male = Male;

            if (actor_details.size() == 2) {
                actor.nickname = actor_details[0];
                actor.name = actor_details[1];
            } else {
                actor.name = actor_details[0];
            }


            if (auto pos = actor.name.find(L", "); pos != std::wstring::npos && pos != 0) {
                // lastname, firstname
                auto split_name = capture_groups(actor.name, std::wregex(L"(.+),\\s+(.+)"));

                actor.firstname = split_name[1];
                actor.lastname  = matches_any(split_name[0], L"", L"-", L"--", L".") ? L"" : split_name[0];  // Null values.
            } else {
                raise_anomaly("Actor name does not match lastname, firstname pattern. These fields will be NULL.", false);
            }

            std::get<std::vector<ModelActor>>(result).push_back(actor);


            // Parse Actor Appearances
            for (const auto& line : split(name_appearance_split.value()[1], std::wregex(L"\r?\n"))) {
                auto appearance_details = capture_groups(
                    line, 
                    std::wregex(LR"REGEX(\s*(.+)\s+\((\d\d\d\d|\?\?\?\?)(?:\/([IVXLCDM]+))?\)\s*(?:\((TV|V|VG)\)\s*)?(?:(\{.+\})\s*)?(?:\(([^\)]+)\)\s*)?(?:(\[.+\])\s*)?(?:(\<\d+\>))?)REGEX")
                );

                if (appearance_details.size() < 2) { raise_anomaly("Failed to parse actor movie listing.", true); continue; }

                // Required Fields
                ModelActorAppearance appearance;
                appearance.actor       = actor.name;
                appearance.appeared_in = capture_groups(appearance_details[0], std::wregex(L"\\\"?([^\\\"]+)\\\"?"))[0];
                appearance.release_yr  = (appearance_details[1] == L"????") ? std::nullopt : (std::optional<unsigned>) std::stoi(std::wstring(appearance_details[1]));

                appearance.media_type = partial_matches(appearance_details[0], std::wregex(LR"REGEX(\".+\"\s+\(mini\))REGEX")) 
                    ? ModelActorAppearance::MediaType::MINI_TV_SERIES
                    : partial_matches(appearance_details[1], std::wregex(LR"REGEX(\".+\"\s*)REGEX"))
                        ? ModelActorAppearance::MediaType::TV_SERIES
                        : ModelActorAppearance::MediaType::MOVIE;


                
                // Optional Fields
                unsigned fieldcount = 2;
                auto has_next = [&]() { return fieldcount < appearance_details.size(); };
                auto get_next = [&]() { return appearance_details[fieldcount]; };

                if (has_next() && is_roman_numeral(get_next())) {
                    appearance.release_nr = parse_roman_numeral(get_next());
                    ++fieldcount;
                }

                if (has_next() && (matches_any(get_next(), L"TV", L"V", L"VG"))) {
                    appearance.release_type = (get_next() == L"TV") 
                        ? ModelActorAppearance::ReleaseType::TV 
                        : (get_next() == L"VG")
                            ? ModelActorAppearance::ReleaseType::VIDEO_GAME
                            : ModelActorAppearance::ReleaseType::VIDEO;

                    ++fieldcount;
                } else appearance.release_type = ModelActorAppearance::ReleaseType::CINEMA;

                if (has_next() && is_surrounded_with(get_next(), L'{', L'}')) {
                    appearance.scene = desurround(get_next());
                    ++fieldcount;
                }

                if (has_next() && !is_surrounded_with(get_next(), L'[', L']') && !is_surrounded_with(get_next(), L'<', L'>')) {
                    appearance.credited_as = get_next();
                    ++fieldcount;
                }

                if (has_next() && is_surrounded_with(get_next(), L'[', L']')) {
                    appearance.role = desurround(get_next());
                    ++fieldcount;
                }

                if (has_next()) {
                    appearance.billing_position = std::stoi(std::wstring(desurround(get_next())));
                }


                std::get<std::vector<ModelActorAppearance>>(result).push_back(std::move(appearance));
            }

            return result;
        }
    }


    inline typename Detail::ActorParserModels::template Apply<AsDataParser> ActorParser {
        &Detail::actor_parser_fn<true>,
        L"\r\n\r\n",
        240,
        20152625
    };


    inline typename Detail::ActorParserModels::template Apply<AsDataParser> ActressParser {
        &Detail::actor_parser_fn<false>,
        L"\r\n\r\n",
        242,
        12097756
    };


    // For testing only
    inline typename Detail::ActorParserModels::template Apply<AsDataParser> ActressTestParser {
        &Detail::actor_parser_fn<false>,
        L"\r\n\r\n",
        0,
        9695
    };
}