#pragma once

#include <type_traits>

#include "pinguim/conf.hpp"
#include "pinguim/qol.hpp"

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