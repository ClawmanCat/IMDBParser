#pragma once

#include <IMDBParser/Models/IDataModel.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <optional>
#include <string>

namespace IMDBParser {
    struct ModelMovieLanguage : public IDataModel<ModelMovieLanguage> {
        std::wstring title;
        std::wstring episode;
        std::wstring language, details;
        std::optional<unsigned> release_yr, release_nr;

        IMDBPARSER_ENUM_CLASS(MediaType, int, UNDEFINED, MOVIE, TV_SERIES, VIDEO_GAME) media_type = MediaType::UNDEFINED;
        IMDBPARSER_ENUM_CLASS(ReleaseType, int, UNDEFINED, CINEMA, TV, VIDEO) release_type = ReleaseType::UNDEFINED;


        static std::vector<FieldLayout> GetLayout(void) {
            return {
                PARSER_GEN_FIELD_LAYOUT(ModelMovieLanguage, title),
                PARSER_GEN_FIELD_LAYOUT(ModelMovieLanguage, episode),
                PARSER_GEN_FIELD_LAYOUT(ModelMovieLanguage, language),
                PARSER_GEN_FIELD_LAYOUT(ModelMovieLanguage, details),
                PARSER_GEN_FIELD_LAYOUT(ModelMovieLanguage, release_yr),
                PARSER_GEN_FIELD_LAYOUT(ModelMovieLanguage, release_nr),
                PARSER_GEN_FIELD_LAYOUT(ModelMovieLanguage, media_type),
                PARSER_GEN_FIELD_LAYOUT(ModelMovieLanguage, release_type)
            };
        }
    };
}