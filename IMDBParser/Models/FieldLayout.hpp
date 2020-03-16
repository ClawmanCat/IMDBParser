#pragma once

#include <IMDBParser/Utility/Traits.hpp>
#include <IMDBParser/Utility/Typedefs.hpp>

#include <string_view>
#include <string>
#include <cstddef>
#include <type_traits>


#define PARSER_GEN_FIELD_LAYOUT(Class, Field)                                   \
FieldLayout {                                                                   \
    #Field,                                                                     \
    sizeof(decltype(Class::Field)),                                             \
    IMDBParser::Detail::get_addressor_fn<Class, offsetof(Class, Field)>(),      \
    IMDBParser::Detail::get_layout_type<decltype(Class::Field)>()               \
}


namespace IMDBParser {
    struct FieldLayout {
        std::string_view name;
        std::size_t size;
        // Containing object => address of data.
        Fn<const void*, const void*> addressor_fn;

        enum Type {
            NVARCHAR,
            INT,
            UINT,
            FLOAT
        } type;
    };


    namespace Detail {
        template <typename T> constexpr inline FieldLayout::Type get_layout_type(void) {
            if      constexpr (std::is_integral_v<T> && std::is_signed_v<T>)                                     return FieldLayout::INT;
            else if constexpr (std::is_integral_v<T>)                                                            return FieldLayout::UINT;
            else if constexpr (std::is_floating_point_v<T>)                                                      return FieldLayout::FLOAT;
            else if constexpr (std::is_enum_v<T>)                                                                return FieldLayout::INT;
            else if constexpr (is_any_of_v<T, std::string, std::u8string, std::string_view, std::u8string_view>) return FieldLayout::NVARCHAR;

            else static_assert(always_false_v<T>, "Unknown layout type!");
        }


        template <typename T, std::size_t offset> constexpr inline const void* addressor_pod(const void* obj) {
            return ((uint8_t*) obj) + offset;
        }


        template <typename T, std::size_t offset> constexpr inline const void* addressor_heap(const void* obj) {
            const void* address = addressor_pod<T, offset>(obj);
            return &*(reinterpret_cast<const T*>(address)->cbegin());
        }


        template <typename T, std::size_t offset> constexpr inline Fn<const void*, const void*> get_addressor_fn(void) {
            if constexpr (is_any_of_v<T, std::string, std::u8string, std::string_view, std::u8string_view>) return &addressor_heap<T, offset>;
            else return &addressor_pod<T, offset>;
        }
    }
}