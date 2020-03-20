#pragma once

#include <boost/preprocessor.hpp>

#include <string_view>


#define IMDBPARSER_IMPL_ENUM_CONSTANT(Rep, Data, Elem) \
const static inline Data& Elem = static_initializer<Rep - 2>();

#define IMDBPARSER_IMPL_ENUM_STRING(Rep, Data, Elem) \
( BOOST_PP_WSTRINGIZE(Elem) )


#define IMDBPARSER_ENUM_CLASS(Name, UType, ...)                         \
struct Name : public IMDBParser::EnumBase {                             \
private:                                                                \
    template <std::size_t N>                                            \
    const static Name& static_initializer(void) {                       \
        const static Name instance { N };                               \
        return instance;                                                \
    }                                                                   \
public:                                                                 \
    UType value;                                                        \
                                                                        \
    BOOST_PP_SEQ_FOR_EACH(                                              \
        IMDBPARSER_IMPL_ENUM_CONSTANT,                                  \
        Name,                                                           \
        BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)                           \
    )                                                                   \
                                                                        \
    Name(void) = default;                                               \
    Name(UType value) : value(value) {}                                 \
                                                                        \
    Name& operator=(UType value) {                                      \
        this->value = value;                                            \
        return *this;                                                   \
    }                                                                   \
                                                                        \
    explicit operator UType(void) const { return value; }               \
                                                                        \
    bool operator==(const Name& o) const { return value == o.value; }   \
    bool operator!=(const Name& o) const { return value != o.value; }   \
    bool operator<=(const Name& o) const { return value <= o.value; }   \
    bool operator>=(const Name& o) const { return value >= o.value; }   \
    bool operator< (const Name& o) const { return value <  o.value; }   \
    bool operator> (const Name& o) const { return value >  o.value; }   \
                                                                        \
    std::wstring_view to_string(void) const {                           \
        constexpr std::wstring_view values[] = {                        \
            BOOST_PP_SEQ_ENUM(                                          \
                BOOST_PP_SEQ_FOR_EACH(                                  \
                    IMDBPARSER_IMPL_ENUM_STRING,                        \
                    _,                                                  \
                    BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)               \
                )                                                       \
            )                                                           \
        };                                                              \
                                                                        \
        return values[value];                                           \
    }                                                                   \
}


namespace IMDBParser {
    class EnumBase {};
}