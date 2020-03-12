#pragma once

#include "IDataModel.hpp"

#include <type_traits>
#include <string_view>
#include <vector>

namespace IMDBParser {
    template <typename Derived> class IDataParser {
    public:
        using Model = typename Derived::Model;
        static_assert(std::is_base_of_v<IDataModel<Model>, Model>, "Data model must implement IDataModel.");

        constexpr static std::string_view seperator = Derived::seperator;

        static std::vector<Model> ParseColumn(std::string_view data) {
            return Derived::ParseColumn(data);
        }
    };
}