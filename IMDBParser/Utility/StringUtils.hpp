#pragma once

#include <vector>
#include <string_view>
#include <string>
#include <regex>
#include <algorithm>
#include <numeric>

namespace IMDBParser {
    inline std::vector<std::string_view> split(std::string_view source, std::string_view seperator) {
        std::vector<std::string_view> result;
        std::size_t last = 0;
        
        for (auto pos = source.find(seperator); pos != std::string_view::npos; pos = source.find(seperator, last)) {
            result.push_back(source.substr(last, pos - last));
            last = pos + seperator.length();
        }

        if (last < source.length() - seperator.length()) result.push_back(source.substr(last, source.length() - last));

        return result;
    }


    inline std::vector<std::string_view> split(std::string_view source, std::regex seperator) {
        using iterator = typename std::string_view::iterator;

        std::vector<std::string_view> result;

        std::regex_token_iterator<iterator> end;
        for (auto it = std::regex_token_iterator<iterator>(source.begin(), source.end(), seperator, -1); it != end; ++it) {
            // C++20 has a proper constructor for this, but it is not yet implemented in the MSVC STL.
            result.push_back(std::string_view(&*(it->first), (it->second - it->first)));
        }

        return result;
    }


    // Can't convert the container, even if the elements can be converted to string view,
    // so template the string type here instead.
    template <
        template <typename...> typename Container,
        typename String
    > inline std::string join(const Container<String>& strings, std::string_view seperator) {
        std::size_t length = 0;
        for (const auto& str : strings) length += str.length();

        std::string result;
        result.reserve(length);
        
        for (std::size_t i = 0; i < strings.size(); ++i) {
            result.append(strings[i]);
            if (i < strings.size() - 1) result.append(seperator);
        }

        return result;
    }


    template <typename... Ts> inline std::string join_variadic(std::string_view seperator, const Ts&... ts) {
        std::vector<std::string> v { std::string(ts)... };
        return join(v, seperator);
    }


    // Returns lines [begin, end) of the given string_view.
    // If the source string does not contain the given lines, the behaviour is undefined.
    inline std::string_view get_lines(std::string_view source, std::size_t begin, std::size_t end) {
        std::size_t diff = end - begin;

        while (begin-- > 0) source.remove_prefix(source.find('\n') + 1);

        auto end_pos = 0;
        while (diff-- > 0) end_pos = source.find('\n', end_pos) + 1;

        source.remove_suffix(source.size() - end_pos + 1);

        return source;
    }
}