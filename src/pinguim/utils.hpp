#pragma once

#include <type_traits>
#include <string_view>
#include <concepts>
#include <utility> // For std::forward.
#include <array>

#include "pinguim/conf.hpp"
#include "pinguim/aliases.hpp"

namespace pinguim::inline utils
{
    // Useful for templated static_asserts.
    template<typename...Ts>
    struct always_false : std::false_type {};

    // When you need to make a std::array of some type but want to
    // decide the size by the amount of arguments.
    // e.g:
    //     auto arr = array_of<u8>(1, 2, 3, 4, 5); // arr is of type std::array<u8, 5>.
    template <typename T, typename... U>
    requires (std::convertible_to<T, U> && ...)
    constexpr auto array_of(U&&... u)
    { return std::array<T, sizeof...(U)>{ static_cast<T>(std::forward<U>(u))... }; }
    template <typename... U>
    constexpr auto array_of(U&&... u)
    { return array_of< std::decay_t<std::common_type_t<U...>>, U... >( std::forward<U>(u)... ); }
    template <typename T, typename... U>
    requires (std::convertible_to<T, U> && ...)
    constexpr auto arr(U&&... u)
    { return array_of<T>( std::forward<U>(u)... ); }
    template <typename... U>
    constexpr auto arr(U&&... u)
    { return array_of( std::forward<U>(u)... ); }

    // from https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/56766138#56766138
    // Thanks!
    template<typename T, bool EnableShortInts = true, bool EnableShortFloats = true>
    constexpr auto type_name() -> std::string_view
    {
        if constexpr (EnableShortInts && std::is_integral_v<T> && sizeof(T) <= sizeof(u64))
        {
            constexpr const char* unsigned_atlas[] = {"u8", "u16", "dummy", "u32", "dummy", "dummy", "dummy", "u64"};
            constexpr const char* signed_atlas[]   = {"i8", "i16", "dummy", "i32", "dummy", "dummy", "dummy", "i64"};

            if constexpr (std::is_signed_v<T>){return signed_atlas[sizeof(T) - 1];}
            else return unsigned_atlas[sizeof(T) - 1];
        }
        if constexpr (EnableShortFloats && std::is_floating_point_v<T> && sizeof(T) <= sizeof(f64))
        {
            constexpr const char* float_atlas[] = {"dummy", "dummy", "dummy", "f32", "dummy", "dummy", "dummy", "f64"};
            return float_atlas[sizeof(T) - 1];
        }
        std::string_view name, prefix, suffix, suffix2;
        #if defined(PINGUIM_CONF_COMPILER_IS_CLANG)
            name   = __PRETTY_FUNCTION__;
            prefix = "std::string_view pinguim::type_name() [T = ";
            if constexpr (EnableShortInts) { suffix = ", EnableShortInts = true"; }
            else                           { suffix = ", EnableShortInts = false"; }
            if constexpr (EnableShortFloats) { suffix2 = ", EnableShortFloats = true]"; }
            else                             { suffix2 = ", EnableShortFloats = false]"; }
        #elif defined(PINGUIM_CONF_COMPILER_IS_GCC)
            name   = __PRETTY_FUNCTION__;
            prefix = "constexpr std::string_view pinguim::type_name() [with T = ";
            if constexpr (EnableShortInts) { suffix = "; bool EnableShortInts = true"; }
            else                           { suffix = "; bool EnableShortInts = false"; }
            if constexpr (EnableShortFloats) { suffix2 = "; bool EnableShortFloats = true; std::string_view = std::basic_string_view<char>]"; }
            else                             { suffix2 = "; bool EnableShortFloats = false; std::string_view = std::basic_string_view<char>]"; }
        #else
            static_assert(std::always_false<T>, "Please implement type_name() for this compiler");
        #endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        name.remove_suffix(suffix2.size());
        return name;
    }

    template <typename T, bool EnableShortInts = true, bool EnableShortFloats = true>
    constexpr auto type_name(T&&) -> std::string_view
    { return type_name< T, EnableShortInts, EnableShortFloats >(); }
}

// Macro for automating structured binding boilerplate.
// To use, you need to define a get() in the same namespace
// as the class you want to de-boilerplatefy, then call PINGUIM_QOL_TUPLIFY().
// e.g:
//     namespace fira_message {
//         template <std::size_t I>
//         auto get(const Field& f)
//         {
//             if constexpr(I == 0) {return f.length();}
//             else if constexpr(I == 1) {return f.width();}
//             else if constexpr(I == 2) {return f.goal_width();}
//             else if constexpr(I == 3) {return f.goal_depth();}
//         }
//     }
//     PINGUIM_UTILS_TUPLIFY(fira_message::Field, 4);
// And now you can do:
//     auto [len, width, gwidth, gdepth] = fira_message::field();
#define PINGUIM_UTILS_TUPLIFY(classname, tuplesize)                 \
    template <>                                                     \
    struct std::tuple_size< classname >                             \
    { static const std::size_t value = tuplesize; };                \
    template <std::size_t I>                                        \
    struct std::tuple_element< I, classname >                       \
    { using type = decltype(get<I>(std::declval< classname >())); }
