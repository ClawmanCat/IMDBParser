#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <string>
#include <vector>

namespace IMDBParser {
    struct ModelActor : public IDataModel<ModelActor> {
        bool is_male = true;
        std::wstring name;
        std::wstring firstname, lastname;    // May be null
        std::wstring nickname;               // May be null


        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelActor, is_male),
                PARSER_GEN_FIELD_LAYOUT(ModelActor, name),
                PARSER_GEN_FIELD_LAYOUT(ModelActor, firstname),
                PARSER_GEN_FIELD_LAYOUT(ModelActor, lastname),
                PARSER_GEN_FIELD_LAYOUT(ModelActor, nickname)
            };
        }
    };


    struct ModelActorAppearance : public IDataModel<ModelActorAppearance> {
        std::wstring actor;                                          // ModelActor::name
        std::wstring appeared_in;                                    // The movie in question
        std::wstring role;                                           // May be null
        std::wstring episode;                                        // May be null
        std::wstring credited_as;                                    // May be null
        std::optional<unsigned> billing_position = std::nullopt;     // May be null
        std::optional<unsigned> release_yr = std::nullopt;
        unsigned release_nr = 0;                                     // If multiple movies were released with the same name in the year, each one has an unique number.
        
        IMDBPARSER_ENUM_CLASS(MediaType,   int, UNDEFINED, MOVIE, TV_SERIES, MINI_TV_SERIES, VIDEO_GAME) media_type = MediaType::UNDEFINED;
        IMDBPARSER_ENUM_CLASS(ReleaseType, int, UNDEFINED, CINEMA, TV, VIDEO) release_type = ReleaseType::UNDEFINED;



        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, actor),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, appeared_in),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, role),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, episode),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, credited_as),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, billing_position),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, release_yr),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, release_nr),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, media_type),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, release_type)
            };
        }
    };
}