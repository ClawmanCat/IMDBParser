#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <optional>
#include <string>

namespace IMDBParser {
    struct ModelLifespan : public IDataModel<ModelLifespan> {
        std::wstring name;
        std::wstring nickname, firstname, lastname;

        std::optional<unsigned> birthyr, birthmonth, birthday;
        std::optional<unsigned> deathyr, deathmonth, deathday;

        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, name),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, nickname),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, firstname),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, lastname),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, birthday),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, birthmonth),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, birthyr),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, deathday),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, deathmonth),
                PARSER_GEN_FIELD_LAYOUT(ModelLifespan, deathyr)
            };
        }
    };
}