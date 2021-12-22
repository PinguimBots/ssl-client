// General utilities and shorthands for converting between types.
#pragma once

#include <type_traits>

/// TODO: summarize.

namespace pinguim::cvt::converters
{
    template <typename To>
    struct static_cast_to {
        template <typename From>
        constexpr auto operator()(From&& v) const { return static_cast<To>( std::forward<From>(v) ); }
    };
}

namespace pinguim::cvt
{
    template <typename To, typename Converter>
    struct to_t
    {
        template <typename From>
        constexpr auto operator<<(From&& v) const
        { return Converter{}( std::forward<From>(v) ); }
    };
    // Just a template variable to keep things neat and short(er).
    // Enables writing: to<type> << some_value;
    // Instead of:      to_t<type, converter>() << some_value;
    // Assumes converter is static_cast_to<To>.
    template <
        typename To,
        typename Converter = converters::static_cast_to<To>
    >
    inline constexpr auto to = to_t<To, Converter>{};

    struct to_underlying_t
    {
        template <typename From>
        constexpr auto operator<<(From&& v) const
        { return to< std::underlying_type_t<From> > << std::forward<From>(v); }
    };
    inline constexpr auto to_underlying = to_underlying_t{};
    inline constexpr auto tou           = to_underlying_t{};

    // Warning, this is highly magical.
    struct to_expected_t
    {
        // Forward declaration, not necessary but helps understand what's going on.
        template <typename From>
        struct implicitly_convertible_to_anything;

        template<typename From>
        constexpr auto operator<<(From&& v) const
        { return implicitly_convertible_to_anything<From>{ std::forward<From>(v) }; }

        // Magic happens here!
        template <typename From>
        struct implicitly_convertible_to_anything
        {
            /// TODO: investigate, copying may occur here.
            From v;

            template<typename ExpectedType>
            constexpr operator ExpectedType() const &
            { return to<ExpectedType> << v; }

            template <typename ExpectedType>
            constexpr operator ExpectedType() &&
            { return to<ExpectedType> << std::move(v); }
        };
    };
    inline constexpr auto to_expected = to_expected_t{};
    inline constexpr auto toe         = to_expected_t{};
}
