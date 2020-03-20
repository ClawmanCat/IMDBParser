#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <optional>
#include <string>

namespace IMDBParser {
    struct ModelMovie : public IDataModel<ModelMovie> {
        std::wstring title;
        std::wstring episode;
        std::optional<unsigned> release_yr, release_nr, start_yr, end_yr;

        IMDBPARSER_ENUM_CLASS(ReleaseType, int, UNDEFINED, CINEMA, TV, VIDEO) release_type = ReleaseType::UNDEFINED;


        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelMovie, title),
                PARSER_GEN_FIELD_LAYOUT(ModelMovie, episode),
                PARSER_GEN_FIELD_LAYOUT(ModelMovie, release_yr),
                PARSER_GEN_FIELD_LAYOUT(ModelMovie, release_nr),
                PARSER_GEN_FIELD_LAYOUT(ModelMovie, start_yr),
                PARSER_GEN_FIELD_LAYOUT(ModelMovie, end_yr),
                PARSER_GEN_FIELD_LAYOUT(ModelMovie, release_type)
            };
        }
    };
}