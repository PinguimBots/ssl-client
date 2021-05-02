#pragma once

#include <type_traits>
#include <string_view>
#include <utility> // For std::forward.
#include <array>

#include <cstddef> // For std::size_t.
#include <cstdint> // For std::*int*_t.

#include "pinguim/conf.hpp"

// Type aliases for integers.
namespace pinguim::inline integer_aliases
{
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;
}

namespace pinguim::utils
{
    // When you need to make a std::array of some type but want to
    // decide the size by the amount of arguments.
    // e.g:
    //     auto arr = array_of<u8>(1, 2, 3, 4, 5); // arr is of type std::array<u8, 5>.
    template <typename T, typename... U>
    constexpr auto array_of(U&&... u) {
        return std::array<T, sizeof...(U)>{ std::forward<U>(u)... };
    }
    template <typename T, typename... U>
    constexpr auto arr(U&&... u) {
        return array_of<T>( std::forward<U>(u)... );
    }

    // from https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/56766138#56766138
    // Thanks!
    template<typename T, bool EnableShortInts = true>
    constexpr auto type_name() -> std::string_view
    {
        if constexpr (EnableShortInts && std::is_integral_v<T> && sizeof(T) <= sizeof(u64))
        {
            constexpr const char* unsigned_atlas[] = {"u8", "u16", "u32", "u64"};
            constexpr const char* signed_atlas[] = {"i8", "i16", "i32", "i64"};

            if constexpr (std::is_signed_v<T>){return signed_atlas[sizeof(T) - 1];}
            else return unsigned_atlas[sizeof(T) - 1];
        }

        std::string_view name, prefix, suffix;
        #if defined(PINGUIM_IS_CLANG)
            name   = __PRETTY_FUNCTION__;
            prefix = "std::string_view pinguim::utils::type_name() [T = ";
            suffix = "]";
        #elif defined(PINGUIM_IS_GCC)
            name   = __PRETTY_FUNCTION__;
            prefix = "constexpr std::string_view pinguim::utils::type_name() [with T = ";
            suffix = "; std::string_view = std::basic_string_view<char>]";
        #elif defined(PINGUIM_IS_MSVC)
            name   = __FUNCSIG__;
            prefix = "class std::basic_string_view<char,struct "
                    "std::char_traits<char> > __cdecl pinguim::utils::type_name<";
            suffix = ">(void)";
        #endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }
    template <typename T>
    constexpr auto type_name(T&&) -> std::string_view
    {
        return type_name< T >();
    }
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

#define PINGUIM_UTILS_TUPLIFY(classname, tuplesize)                   \
    template <>                                                     \
    struct std::tuple_size< classname >                             \
    {                                                               \
        static const std::size_t value = tuplesize;                 \
    };                                                              \
    template <std::size_t I>                                        \
    struct std::tuple_element< I, classname >                       \
    {                                                               \
        using type = decltype(get<I>(std::declval< classname >())); \
    }
