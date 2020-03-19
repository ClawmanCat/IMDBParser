#pragma once

#include "FieldLayout.hpp"

#include <vector>

namespace IMDBParser {
    template <typename Derived> class IDataModel {
    public:
        static std::vector<FieldLayout> GetLayout(void) {
            return Derived::GetLayout();
        }
    protected:
        IDataModel(void) = default;
    };
}