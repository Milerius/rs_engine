//
// Created by doom on 15/06/2018.
// Maintained by doom and Milerius.
//

#pragma once

#include <string_view>
#include <string>
#include <array>
#include <shiva/pp/PP.hpp>

/**
 * The ENUM macro allows preprocessor-based generation of reflective enumerations.
 * These enumerations can be easily converted back and forth to strings.
 * It is also possible to iterate through the available values of such an enumeration type.
 * \example
 * \code
 * #include <shiva/enums/enums.hpp>
​ *
 * ENUM(AnEnum, VALUE_A, VALUE_B);
​ *
 * AnEnum v(AnEnum::VALUE_A);
 * //v is now AnEnum::VALUE_A
 *
 * v = AnEnum::VALUE_B;
 * //v is now AnEnum::VALUE_B
 *
​ * std::string str = v.toString();
 * //str is now "VALUE_B"
 *
 * ​AnEnum s("VALUE_A");
 * //s is now AnEnum::VALUE_A
 *
 * for (const auto &cur : AnEnum::values()) {
 *  //Do stuff
 *  }
 * \endcode
 */

#define SIZEOF_TAB(tab)       (sizeof(tab) / sizeof((tab)[0]))

#define DO_CASE_STR(value)                                                \
    case value:                                                             \
        return #value;                                                      \

#define STRINGIFY_ELEM(v)       pp_stringviewify(v),

# define ENUM_MAKE(name, decl_kw, super_expr, ...)                        \
    class name                                                              \
    {                                                                       \
    public:                                                                 \
        decl_kw enum_##name super_expr                                      \
        {                                                                   \
            __VA_ARGS__                                                     \
        };                                                                  \
                                                                            \
        using EnumType = enum_##name;                                       \
                                                                            \
        [[maybe_unused]] constexpr name() noexcept : _value(pp_first_arg(__VA_ARGS__)) \
        {                                                                   \
        }                                                                   \
                                                                            \
        [[maybe_unused]] constexpr name(EnumType value) noexcept : _value(value) \
        {                                                                   \
        }                                                                   \
                                                                            \
        [[maybe_unused]] name &operator=(EnumType value) noexcept           \
        {                                                                   \
            _value = value;                                                 \
            return *this;                                                   \
        }                                                                   \
                                                                            \
        static constexpr const std::array<name,                             \
                            pp_count_args(__VA_ARGS__)> values() noexcept   \
        {                                                                   \
            return {{__VA_ARGS__}};                                         \
        }                                                                   \
                                                                            \
                                                                            \
        [[maybe_unused]] static constexpr size_t size() noexcept            \
        {                                                                   \
            return pp_count_args(__VA_ARGS__);                              \
        }                                                                   \
                                                                            \
        class InvalidConversion : public std::exception                     \
        {                                                                   \
        public:                                                             \
            const char *what() const noexcept override                      \
            {                                                               \
                return "Unable to convert this string to an enum value";    \
            }                                                               \
        };                                                                  \
                                                                            \
    private:                                                                \
        enum_##name _stringToValue(const std::string_view v)                \
        {                                                                   \
            static constexpr const std::string_view strTab[] = {            \
                pp_for_each(STRINGIFY_ELEM, __VA_ARGS__)                  \
            };                                                              \
            static constexpr const auto vals = values();                    \
                                                                            \
            for (size_t i = 0; i < SIZEOF_TAB(strTab); ++i) {             \
                if (strTab[i] == v) {                                       \
                    return vals[i];                                         \
                }                                                           \
            }                                                               \
            throw InvalidConversion();                                      \
        }                                                                   \
                                                                            \
    public:                                                                 \
        [[maybe_unused]] name(const std::string_view v) : _value(_stringToValue(v))          \
        {                                                                   \
        }                                                                   \
                                                                            \
        [[maybe_unused]] name &operator=(const std::string_view v)          \
        {                                                                   \
            _value = _stringToValue(v);                                     \
            return *this;                                                   \
        }                                                                   \
                                                                            \
        [[maybe_unused]] operator EnumType() const noexcept                 \
        {                                                                   \
            return _value;                                                  \
        }                                                                   \
                                                                            \
        [[maybe_unused]] std::string toString() const noexcept              \
        {                                                                   \
            switch (_value) {                                               \
                pp_for_each(DO_CASE_STR, __VA_ARGS__)                     \
                default:                                                    \
                    return "";                                              \
            }                                                               \
        }                                                                   \
                                                                            \
        static std::string toString(name v)                                 \
        {                                                                   \
            return v.toString();                                            \
        }                                                                   \
                                                                            \
    private:                                                                \
        EnumType _value;                                                    \
    };

#define ENUM(name, ...) ENUM_MAKE(name, enum, , __VA_ARGS__)