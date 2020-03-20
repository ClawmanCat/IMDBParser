#pragma once

#include <IMDBParser/Utility/Exception.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/Traits.hpp>
#include <IMDBParser/Utility/Memory.hpp>
#include <IMDBParser/Utility/Unreachable.hpp>
#include <IMDBParser/Models/FieldLayout.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <string_view>
#include <cstdint>
#include <sstream>
#include <filesystem>


namespace IMDBParser::FileIO {
    namespace fs = std::filesystem;


    namespace Detail {
        inline void raise_io_exception(std::wstring_view path, bool read) {
            raise_exception(join_variadic(L"",
                L"Failed to ",
                (read ? L"read from" : L"write to"),
                L" file ",
                path,
                L": ",
                to_wstring(strerror(errno))
            ));
        }
    }


    inline std::vector<std::wstring> read(std::wstring_view path) {
        std::wifstream stream(to_nstring(path));
        stream.imbue(std::locale("en-US"));

        if (!stream.good()) {
            Detail::raise_io_exception(path, true);
            return {};
        }

        std::vector<std::wstring> result;
        std::wstring tmp;

        while (std::getline(stream, tmp)) result.push_back(tmp);

        return result;
    }


    template <typename String>
    inline void write(std::wstring_view path, const std::vector<String>& text) {
        fs::path dir = fs::path(to_nstring(path)).parent_path();
        if (!fs::is_directory(dir)) fs::create_directories(dir);

        // I don't know much about Unicode or locale, but for some reason std::wofstream doesn't work here.
        // no locale => some Unicode chars get serialized incorrectly, some cause the stream to close.
        // en_US locale (same as the wifstream) => the file is completely empty.
        //
        // If this doesn't work on your system, good luck: Unicode in C++ is completely broken.
        std::ofstream stream(to_nstring(path));
        
        if (!stream.good()) {
            Detail::raise_io_exception(path, false);
            return;
        }

        for (const auto& line : text) stream << to_nstring(line) << '\n';
    }


    template <typename Model>
    inline void write_csv(std::wstring_view path, const std::vector<Model>& data) {
        constexpr std::wstring_view seperator = L";";


        const std::vector<FieldLayout> layout = Model::GetLayout();

        std::vector<std::wstring> result;


        std::wstring header;
        for (const auto& field : layout) header.append(join_variadic(L"", to_wstring(field.name), seperator));
        header.pop_back();  // Trailing comma

        result.push_back(header);


        for (const auto& obj : data) {
            std::wstringstream row;

            for (const auto& field : layout) {
                const void* address = field.addressor_fn(&obj);

                if (!address) {
                    row << seperator;
                    continue;
                }

                switch (field.type) {
                    case FieldLayout::INT:
                        row << Memory::parse_signed(address, field.size) << seperator;
                        break;
                    case FieldLayout::UINT:
                        row << Memory::parse_unsigned(address, field.size) << seperator;
                        break;
                    case FieldLayout::FLOAT:
                        row << Memory::parse_float(address, field.size) << seperator;
                        break;
                    case FieldLayout::NVARCHAR:
                        row << Memory::parse_string(address, field.size) << seperator;
                        break;
                    default: unreachable_path;
                }
            }

            std::wstring s = row.str();
            s.pop_back();  // Trailing comma

            result.push_back(std::move(s));
        }


        write(path, result);
    }


    inline std::wstring_view stem(std::wstring_view path) {
        auto pos = path.rfind(L'.');

        if (pos != std::wstring_view::npos) path.remove_suffix(path.size() - pos);

        return path;
    }
}