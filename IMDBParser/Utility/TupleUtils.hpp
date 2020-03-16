#pragma once

#include <tuple>
#include <vector>
#include <type_traits>

namespace IMDBParser {
    template <typename T, T val> struct Arg { constexpr static T value = val; };


    template <typename Tuple, typename Pred, std::size_t N = 0>
    constexpr inline void expand(Tuple& tuple, Pred pred) {
        constexpr std::size_t size = std::tuple_size_v<std::decay_t<Tuple>>;

        if constexpr (N < size) pred(std::get<N>(tuple), Arg<std::size_t, N>{});
        if constexpr (N + 1 < size) expand<Tuple, Pred, N + 1>(tuple, pred);
    }


    template <typename... Ts>
    constexpr inline std::tuple<std::vector<Ts>...> merge_vector_tuples(std::vector<std::tuple<std::vector<Ts>...>>&& tuples) {
        std::tuple<std::vector<Ts>...> result;
        
        for (auto& tuple : tuples) {
            expand(tuple, [&](auto& elem, auto index) {
                auto& dest = std::get<decltype(index)::value>(result);
                std::move(elem.begin(), elem.end(), std::back_inserter(dest));
            });
        }

        return result;
    }
}