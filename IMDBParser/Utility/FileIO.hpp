#pragma once

#include <IMDBParser/Utility/Exception.hpp>
#include <IMDBParser/Utility/StringUtils.hpp>
#include <IMDBParser/Utility/Traits.hpp>
#include <IMDBParser/Utility/Memory.hpp>
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
        inline void raise_io_exception(std::string_view path, bool read) {
            raise_exception(join_variadic("",
                "Failed to ",
                (read ? "read from" : "write to"),
                " file ",
                path,
                ": ",
                strerror(errno)
            ));
        }
    }


    inline std::vector<std::string> read(std::string_view path) {
        std::ifstream stream(path.data());
        if (!stream.good()) {
            Detail::raise_io_exception(path, true);
            return {};
        }

        std::vector<std::string> result;
        std::string tmp;

        while (std::getline(stream, tmp)) result.push_back(tmp);

        return result;
    }


    template <typename String>
    inline void write(std::string_view path, const std::vector<String>& text) {
        fs::path dir = fs::path(path).parent_path();
        if (!fs::is_directory(dir)) fs::create_directories(dir);

        std::ofstream stream(path.data());
        if (!stream.good()) {
            Detail::raise_io_exception(path, false);
            return;
        }

        for (const auto& line : text) stream << line << '\n';
    }


    template <typename Model>
    inline void write_csv(std::string_view path, const std::vector<Model>& data) {
        const std::vector<FieldLayout> layout = Model::GetLayout();

        std::vector<std::string> result;


        std::string header;
        for (const auto& field : layout) header.append(join_variadic(field.name, ","));
        header.pop_back();  // Trailing comma

        result.push_back(header);


        for (const auto& obj : data) {
            std::stringstream row;

            for (const auto& field : layout) {
                const void* address = field.addressor_fn(&obj);

                switch (field.type) {
                    case FieldLayout::INT:
                        row << Memory::parse_signed(address, field.size) << ',';
                        break;
                    case FieldLayout::UINT:
                        row << Memory::parse_unsigned(address, field.size) << ',';
                        break;
                    case FieldLayout::FLOAT:
                        row << Memory::parse_float(address, field.size) << ',';
                        break;
                    case FieldLayout::NVARCHAR:
                        row << Memory::parse_string(address, field.size) << ',';
                        break;
                    default: __assume(0);
                }
            }

            std::string s = row.str();
            s.pop_back();  // Trailing comma

            result.push_back(std::move(s));
        }


        write(path, result);
    }


    inline std::string_view stem(std::string_view path) {
        auto pos = path.rfind('.');

        if (pos != std::string_view::npos) path.remove_suffix(path.size() - pos);

        return path;
    }
}