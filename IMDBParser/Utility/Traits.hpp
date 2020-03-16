#pragma once

#include <type_traits>
#include <cstdint>
#include <cstddef>


namespace IMDBParser {
    template <typename T, typename... Ts> constexpr inline bool is_any_of_v = ((std::is_same_v<T, Ts>) || ...);


    // For preventing early expansion of static assert
    template <typename T> class always_false {
    private:
        class secret_class {};
    public:
        constexpr static bool value = std::is_same_v<T, secret_class>;
    };

    template <typename T> constexpr inline bool always_false_v = always_false<T>::value;


    template <typename... Ts> struct Pack {
        constexpr static std::size_t size = sizeof...(Ts);

        template <template <typename...> typename X> using Apply = X<Ts...>;
    };
}