#pragma once

#include <IMDBParser/Utility/Traits.hpp>
#include <IMDBParser/Utility/Typedefs.hpp>
#include <IMDBParser/Utility/Enum.hpp>

#include <string_view>
#include <string>
#include <cstddef>
#include <type_traits>
#include <optional>


#define PARSER_GEN_FIELD_LAYOUT(Class, Field)                                               \
FieldLayout {                                                                               \
    #Field,                                                                                 \
    sizeof(decltype(Class::Field)),                                                         \
    IMDBParser::Detail::get_addressor_fn<decltype(Class::Field), offsetof(Class, Field)>(), \
    IMDBParser::Detail::get_layout_type<decltype(Class::Field)>()                           \
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
            if constexpr (is_template_instantiation_v<T, std::optional>) return get_layout_type<typename T::value_type>();

            else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)                                     return FieldLayout::INT;
            else if constexpr (std::is_integral_v<T>)                                                            return FieldLayout::UINT;
            else if constexpr (std::is_floating_point_v<T>)                                                      return FieldLayout::FLOAT;
            else if constexpr (std::is_base_of_v<EnumBase, T>)                                                   return FieldLayout::NVARCHAR;
            else if constexpr (is_any_of_v<T, std::string, std::wstring, std::string_view, std::wstring_view>)   return FieldLayout::NVARCHAR;

            else static_assert(always_false_v<T>, "Unknown layout type!");
        }


        template <typename T, std::size_t offset> constexpr inline const void* addressor_pod(const void* obj) {
            return reinterpret_cast<const uint8_t*>(obj) + offset;
        }


        template <typename T, std::size_t offset> constexpr inline const void* addressor_heap(const void* obj) {
            const void* address = addressor_pod<T, offset>(obj);
            return reinterpret_cast<const T*>(address)->c_str();
        }


        template <typename T, std::size_t offset> constexpr inline const void* addressor_enum(const void* obj) {
            const void* address = addressor_pod<T, offset>(obj);
            return &*(reinterpret_cast<const T*>(address)->to_string().cbegin());
        }


        template <typename T, std::size_t offset, Fn<const void*, const void*> fn> constexpr inline const void* addressor_optional(const void* obj) {
            const void* address = addressor_pod<T, offset>(obj);
            
            if (reinterpret_cast<const std::optional<typename T::value_type>*>(address)->has_value()) return fn(obj);
            else return nullptr;
        }


        template <typename T, std::size_t offset> constexpr inline Fn<const void*, const void*> get_addressor_fn(void) {
            if constexpr (is_template_instantiation_v<T, std::optional>) 
                return &addressor_optional<T, offset, get_addressor_fn<typename T::value_type, offset>()>;
            
            else if constexpr (is_any_of_v<T, std::string, std::wstring, std::string_view, std::wstring_view>)
                return &addressor_heap<T, offset>;

            else if constexpr (std::is_base_of_v<EnumBase, T>) 
                return &addressor_enum<T, offset>;
            
            else return &addressor_pod<T, offset>;
        }
    }
}