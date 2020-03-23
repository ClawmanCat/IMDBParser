#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <optional>
#include <string>
#include <array>

namespace IMDBParser {
    struct ModelLocation : public IDataModel<ModelLocation> {
        std::wstring title;
        std::wstring episode;
        std::optional<unsigned> release_yr, release_nr;

        std::wstring full_address, country;
        std::wstring details;

        IMDBPARSER_ENUM_CLASS(MediaType, int, UNDEFINED, MOVIE, TV_SERIES, VIDEO_GAME) media_type = MediaType::UNDEFINED;
        IMDBPARSER_ENUM_CLASS(ReleaseType, int, UNDEFINED, CINEMA, TV, VIDEO) release_type = ReleaseType::UNDEFINED;


        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, title),
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, episode),
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, release_yr),
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, release_nr),
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, full_address),
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, country),
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, details),
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, media_type),
                PARSER_GEN_FIELD_LAYOUT(ModelLocation, release_type)
            };
        }
    };
}