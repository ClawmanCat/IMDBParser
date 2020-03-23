#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <optional>
#include <string>
#include <array>

namespace IMDBParser {
    struct ModelRating : public IDataModel<ModelRating> {
        // Would normally use an enum but variable names can't start with a number.
        std::array<std::wstring, 10> score_distribution;

        unsigned votes;
        unsigned score;
        std::wstring title;
        std::wstring episode;
        std::optional<unsigned> release_yr, release_nr;

        IMDBPARSER_ENUM_CLASS(MediaType, int, UNDEFINED, MOVIE, TV_SERIES, VIDEO_GAME) media_type = MediaType::UNDEFINED;
        IMDBPARSER_ENUM_CLASS(ReleaseType, int, UNDEFINED, CINEMA, TV, VIDEO) release_type = ReleaseType::UNDEFINED;


        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelRating, score_distribution),
                PARSER_GEN_FIELD_LAYOUT(ModelRating, votes),
                PARSER_GEN_FIELD_LAYOUT(ModelRating, score),
                PARSER_GEN_FIELD_LAYOUT(ModelRating, title),
                PARSER_GEN_FIELD_LAYOUT(ModelRating, episode),
                PARSER_GEN_FIELD_LAYOUT(ModelRating, release_yr),
                PARSER_GEN_FIELD_LAYOUT(ModelRating, release_nr),
                PARSER_GEN_FIELD_LAYOUT(ModelRating, media_type),
                PARSER_GEN_FIELD_LAYOUT(ModelRating, release_type)
            };
        }
    };
}