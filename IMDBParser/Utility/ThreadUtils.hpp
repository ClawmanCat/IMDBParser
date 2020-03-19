#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <type_traits>
#include <optional>


namespace IMDBParser {
    template <typename MapFn, typename ReduceFn, template <typename...> typename Container, typename Data>
    inline auto parallel_map_serial_combine(Container<Data>& data, MapFn map, ReduceFn reduce) {
        const static unsigned limit = std::thread::hardware_concurrency();
        using result = decltype(map(std::declval<Data&>()));

        std::atomic_uint tasks_done = 0;
        std::vector<std::thread> threads;

        std::vector<std::optional<result>> results;
        results.resize(data.size(), std::nullopt);


        for (unsigned i = 0; i < limit; ++i) {
            threads.push_back(std::thread([&]() {
                for (unsigned index = tasks_done++; index < data.size(); index = tasks_done++) {
                    results[index] = map(data[index]);
                }
            }));
        }


        for (auto& thread : threads) thread.join();

        std::vector<result> finished_results;
        finished_results.reserve(results.size());

        std::transform(
            std::make_move_iterator(results.begin()), 
            std::make_move_iterator(results.end()), 
            std::back_inserter(finished_results), 
            [](auto&& result) { return std::move(result.value()); }
        );

        return reduce(std::move(finished_results));
    }
}