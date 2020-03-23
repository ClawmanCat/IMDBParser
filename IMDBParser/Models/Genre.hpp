#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <optional>
#include <string>

namespace IMDBParser {
    struct ModelGenre : public IDataModel<ModelGenre> {
        std::wstring title;
        std::wstring episode;
        std::optional<unsigned> release_yr, release_nr;

        std::wstring genre;

        IMDBPARSER_ENUM_CLASS(MediaType, int, UNDEFINED, MOVIE, TV_SERIES, VIDEO_GAME) media_type = MediaType::UNDEFINED;
        IMDBPARSER_ENUM_CLASS(ReleaseType, int, UNDEFINED, CINEMA, TV, VIDEO) release_type = ReleaseType::UNDEFINED;


        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelGenre, title),
                PARSER_GEN_FIELD_LAYOUT(ModelGenre, episode),
                PARSER_GEN_FIELD_LAYOUT(ModelGenre, release_yr),
                PARSER_GEN_FIELD_LAYOUT(ModelGenre, release_nr),
                PARSER_GEN_FIELD_LAYOUT(ModelGenre, genre),
                PARSER_GEN_FIELD_LAYOUT(ModelGenre, media_type),
                PARSER_GEN_FIELD_LAYOUT(ModelGenre, release_type)
            };
        }
    };
}