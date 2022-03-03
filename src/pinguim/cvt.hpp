// General utilities and shorthands for converting between types.
//
// The types/constants defined here only do their conversions
// when an arithmetic operation is exercized upon them
// or operator() is explictly called.
//
// You can use them as constants that you can use in math
// that does the specified conversion OR functions.
//
// E.g: `5.8f * cvt::to<int>`     will return `int{5}`.
// E.g: `int x = 5.8f * cvt::toe` will compile and `x == 5`.
// E.g: `cvt::to<int>(5.8f)`      will return `int{5}`.
//
// The reason for this arithmetic-based interface is that you will
// usually use these conversions when dealing with numbers so it
// made the most ergonomic sense to encode them as math operations.
//
// Keep in mind that arithmetic precedence still aplies, that is,
// multiplications will be executed before additions and so on.
// E.g:
//     5.2f + cvt::to<int> * 5.99 == 10.2f
//     5.2f * cvt::to<int> + 5.99 == 10.99
#pragma once

#include <type_traits>
#include <utility> // For std::forward.

namespace pinguim::cvt
{
    inline namespace converters
    {
        // Non-special versions of the *_cast 'functions'.

        template <typename To> struct sc; // static_cast.
        template <typename To> struct dc; // dynamic_cast.
        template <typename To> struct rc; // reinterpret_cast.
        template <typename To> struct cc; // const_cast.

        // Custom converters.

        // Converts to the std::underlying_t (used for getting
        // the base type of enums).
        struct underlying;

        // Uses magic to 'guess' the expected type and convert to it.
        // Can only be used in situations where the compiler can
        // infer the final type.
        struct expected;
    }

    // Just a wrapper around a Converter, you will never use this directly.
    // Provides all the arithmetic operators.
    //
    // This class is really simple, has only 1 function and that is operator(),
    // you can call it to convert the argument using Converter.
    // The magic is that all the arithmetic operations are configured to perform
    // this conversion, they are defined below, in the implementation section.
    template <typename Converter>
    struct into
    {
        template <typename From>
        constexpr auto operator()(From&& from) const;
    };

    // You can use the following like constants of functions.
    // E.g: `5.8f * cvt::to<double>` or `cvt::to<double>(5.8f)`.

    template <typename To, typename Converter = sc<To>>
    inline constexpr auto to = into<Converter>{};

    inline constexpr auto to_underlying  = into<underlying>{};
    inline constexpr auto tou            = into<underlying>{};

    inline constexpr auto to_expected  = into<expected>{};
    inline constexpr auto toe          = into<expected>{};
}

// Implementations below.

namespace pinguim::cvt
{
    template <typename Converter>
    template <typename From>
    constexpr auto into<Converter>::operator()(From&& from) const
    { return Converter{}( std::forward<From>(from) ); }

    // operator*
    template <typename T, typename Converter>
    constexpr auto operator*(T&& val, const into<Converter>& cvt)
    { return cvt( std::forward<T>(val) ); }
    template <typename T, typename Converter>
    constexpr auto operator*(const into<Converter>& cvt, T&& val)
    { return cvt( std::forward<T>(val) ); }

    // operator/
    template <typename T, typename Converter>
    constexpr auto operator/(T&& val, const into<Converter>& cvt)
    { return cvt( std::forward<T>(val) ); }
    template <typename T, typename Converter>
    constexpr auto operator/(const into<Converter>& cvt, T&& val)
    { return cvt( std::forward<T>(val) ); }

    // operator%
    template <typename T, typename Converter>
    constexpr auto operator%(T&& val, const into<Converter>& cvt)
    { return cvt( std::forward<T>(val) ); }
    template <typename T, typename Converter>
    constexpr auto operator%(const into<Converter>& cvt, T&& val)
    { return cvt( std::forward<T>(val) ); }

    // operator+
    template <typename T, typename Converter>
    constexpr auto operator+(T&& val, const into<Converter>& cvt)
    { return cvt( std::forward<T>(val) ); }
    template <typename T, typename Converter>
    constexpr auto operator+(const into<Converter>& cvt, T&& val)
    { return cvt( std::forward<T>(val) ); }

    // operator-
    template <typename T, typename Converter>
    constexpr auto operator-(T&& val, const into<Converter>& cvt)
    { return cvt( std::forward<T>(val) ); }
    template <typename T, typename Converter>
    constexpr auto operator-(const into<Converter>& cvt, T&& val)
    { return cvt( std::forward<T>(val) ); }
}

namespace pinguim::cvt::inline converters
{
    template <typename To>
    struct sc
    {
        template <typename From>
        constexpr auto operator()(From&& from) const
        { return static_cast<To>( std::forward<From>(from) ); }
    };

    template <typename To>
    struct dc
    {
        template <typename From>
        constexpr auto operator()(From&& from) const
        { return dynamic_cast<To>( std::forward<From>(from) ); }
    };

    template <typename To>
    struct rc
    {
        template <typename From>
        constexpr auto operator()(From&& from) const
        { return reinterpret_cast<To>( std::forward<From>(from) ); }
    };

    template <typename To>
    struct cc
    {
        template <typename From>
        constexpr auto operator()(From&& from) const
        { return const_cast<To>( std::forward<From>(from) ); }
    };

    struct underlying
    {
        template <typename From>
        constexpr auto operator()(From&& from) const
        { return static_cast< std::underlying_type_t<From> >( std::forward<From>(from) ); }
    };

    struct expected
    {
        // Magic happens here!
        // This type is implictly convertible to anything and
        // the compiler fills in the blanks with the correct type.
        template <typename From>
        struct implicit_cvt
        {
            From v;

            template<typename ExpectedType>
            constexpr operator ExpectedType() const &
            { return to<ExpectedType>(v); }

            template <typename ExpectedType>
            constexpr operator ExpectedType() &&
            { return to<ExpectedType>(std::move(v)); }
        };

        template <typename From>
        constexpr auto operator()(From&& from) const
        { return implicit_cvt<From>{ std::forward<From>(from) }; }
    };
}
