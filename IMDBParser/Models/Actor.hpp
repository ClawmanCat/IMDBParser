#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Parsers/DataParser.hpp>

#include <string>
#include <vector>

namespace IMDBParser {
    struct ModelActor : public IDataModel<ModelActor> {
        bool is_male;
        std::string name;
        std::string nickname;


        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelActor, is_male),
                PARSER_GEN_FIELD_LAYOUT(ModelActor, name),
                PARSER_GEN_FIELD_LAYOUT(ModelActor, nickname)
            };
        }
    };


    struct ModelActorAppearance : public IDataModel<ModelActorAppearance> {
        std::string actor;
        std::string appeared_in;


        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, actor),
                PARSER_GEN_FIELD_LAYOUT(ModelActorAppearance, appeared_in)
            };
        }
    };
}