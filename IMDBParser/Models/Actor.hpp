#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <string>
#include <vector>

namespace IMDBParser {
    struct ModelActor : public IDataModel<ModelActor> {
        bool is_male;
        std::string name;
        std::string firstname, lastname;    // May be null
        std::string nickname;               // May be null


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
        std::string actor;                  // ModelActor::name
        std::string appeared_in;            // The movie in question
        std::string role;                   // May be null
        std::string scene;                  // May be null
        std::string credited_as;            // May be null
        unsigned billing_position;          // May be null
        unsigned release_yr, release_nr;    // If multiple movies were released with the same name in the year, each one has an unique number.
        
        IMDBPARSER_ENUM_CLASS(MediaType,   int, MOVIE, TV_SERIES, MINI_TV_SERIES) media_type;
        IMDBPARSER_ENUM_CLASS(ReleaseType, int, CINEMA, TV, VIDEO) release_type;



        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, actor),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, appeared_in),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, role),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, scene),
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