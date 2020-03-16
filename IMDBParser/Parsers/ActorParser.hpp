#pragma once

#include <IMDBParser/Parsers/DataParser.hpp>
#include <IMDBParser/Models/Actor.hpp>
#include <IMDBParser/Utility/Traits.hpp>

#include <string_view>


namespace IMDBParser {
    namespace Detail {
        using ActorParserModels = Pack<ModelActor, ModelActorAppearance>;
        

        template <bool Male> inline typename ActorParserModels::template Apply<AsParseResult> actor_parser_fn(std::string_view column) {
            return {};  // TODO: Parse the data.
        }
    }


    inline typename Detail::ActorParserModels::template Apply<AsDataParser> ActorParser {
        &Detail::actor_parser_fn<true>,
        std::regex("\r?\n\r?\n"),
        0,
        1000
    };


    inline typename Detail::ActorParserModels::template Apply<AsDataParser> ActressParser {
        &Detail::actor_parser_fn<false>,
        std::regex("\r?\n\r?\n"),
        0,
        1000
    };
}