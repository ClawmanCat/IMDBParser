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
        template <bool Male> inline ActorParserResult actor_parser_fn(std::string_view column) {
            ActorParserResult result;

            auto raise_anomaly = [&](std::string message, bool except = true) {
                auto fn = except ? &raise_exception : &raise_warning;

                fn(join_variadic("",
                    except ? "Failed to parse column" : "Warning while parsing column",
                    " \"",
                    column,
                    "\": ",
                    message,
                    except ? " (Column will be skipped.)" : ""
                ));
            };


            auto name_appearance_split = split_on_first_recurring(column, '\t');
            if (name_appearance_split == std::nullopt) { raise_anomaly("Column does not contain name/movie[] pair.", true); return {}; }


            // Parse Actor Name
            auto actor_details = capture_groups(name_appearance_split.value()[0], std::regex(R"REGEX((?:['"]\s*(.+)\s*['"],\s+)?(.+))REGEX"));
            if (actor_details.size() < 1) { raise_anomaly("Column does not contain a valid actor name.", true); return {}; }

            ModelActor actor;
            actor.is_male = Male;

            if (actor_details.size() == 2) {
                actor.nickname = actor_details[0];
                actor.name = actor_details[1];
            } else {
                actor.name = actor_details[0];
            }


            if (contains_any(actor.name, "~`!@#$%^&*_+=[]{}\\|'\";:,<>/?0123456789")) 
                //raise_anomaly("Unexpected characters found in actor name. Column might be parsed incorrectly!", false);


            if (auto pos = actor.name.find(','); pos != std::string::npos && pos != 0) {
                // lastname, firstname
                auto split_name = capture_groups(actor.name, std::regex("(\\S+),\\s+(\\S+)"));

                actor.firstname = split_name[1];
                actor.lastname  = matches_any(split_name[0], "", "-", "--", ".") ? "" : split_name[0];  // Null values.
            } else {
                raise_anomaly("Actor name does not match lastname, firstname pattern. These fields will be NULL.", false);
            }

            std::get<std::vector<ModelActor>>(result).push_back(actor);


            // Parse Actor Appearances
            for (const auto& line : split(name_appearance_split.value()[1], std::regex("\r?\n"))) {
                auto appearance_details = capture_groups(
                    line, 
                    std::regex(R"REGEX(\s*(.+)\s+\((\d\d\d\d)(?:\/([IVXLCDM]+))?\)\s*(?:\((TV|V)\)\s*)?(?:(\{.+\})\s*)?(?:\((?:(uncredited)|(?:as\s+(.+)))\)\s*)?(?:(\[.+\])\s*)?(?:(\<\d+\>))?)REGEX")
                );

                if (appearance_details.size() < 2) { raise_anomaly("Failed to parse actor movie listing.", true); continue; }

                // Required Fields
                ModelActorAppearance appearance;
                appearance.actor       = actor.name;
                appearance.appeared_in = capture_groups(appearance_details[0], std::regex("\"?(.+)\"?.*"))[0];
                appearance.release_yr  = std::stoi(std::string(appearance_details[1]));

                appearance.media_type = matches(appearance_details[1], std::regex("\\s*\".+\"\\s*(mini)\\s*")) 
                    ? ModelActorAppearance::MediaType::MINI_TV_SERIES
                    : matches(appearance_details[1], std::regex("\\s*\".+\"\\s*"))
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

                if (has_next() && (matches_any(get_next(), "TV", "V"))) {
                    appearance.release_type = (get_next() == "TV") ? ModelActorAppearance::ReleaseType::TV : ModelActorAppearance::ReleaseType::VIDEO;
                    ++fieldcount;
                } else appearance.release_type = ModelActorAppearance::ReleaseType::CINEMA;

                if (has_next() && is_surrounded_with(get_next(), '{', '}')) {
                    appearance.scene = desurround(get_next());
                    ++fieldcount;
                }

                if (has_next() && !is_surrounded_with(get_next(), '[', ']') && !is_surrounded_with(get_next(), '<', '>')) {
                    appearance.credited_as = get_next();
                    ++fieldcount;
                }

                if (has_next() && is_surrounded_with(get_next(), '[', ']')) {
                    appearance.role = desurround(get_next());
                    ++fieldcount;
                }

                if (has_next()) {
                    appearance.billing_position = std::stoi(std::string(desurround(get_next())));
                }


                std::get<std::vector<ModelActorAppearance>>(result).push_back(appearance);
            }


            return result;
        }
    }


    inline typename Detail::ActorParserModels::template Apply<AsDataParser> ActorParser {
        &Detail::actor_parser_fn<true>,
        "\r\n\r\n",
        240,
        20152625
    };


    inline typename Detail::ActorParserModels::template Apply<AsDataParser> ActressParser {
        &Detail::actor_parser_fn<false>,
        "\r\n\r\n",
        242,
        12099756
    };


    // For testing only
    inline typename Detail::ActorParserModels::template Apply<AsDataParser> ActressTestParser {
        &Detail::actor_parser_fn<false>,
        "\r\n\r\n",
        0,
        9695
    };
}