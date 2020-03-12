#pragma once

namespace IMDBParser {
    template <typename Derived>
    class IDataModel {
    public:
        constexpr static auto GetLayout(void) {
            return Derived::GetLayout();
        }
    protected:
        IDataModel(void) = default;
    };
}