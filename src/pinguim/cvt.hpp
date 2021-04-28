// General utilities and shorthands for converting between types.
#pragma once

/// TODO: summarize.

namespace pinguim::cvt
{
    // Curried static_cast.
    template <typename To>
    struct static_cast_to {
        template <typename From>
        static constexpr auto from(From&& v) {
            return static_cast<To>( std::forward<From>(v) );
        }

        // The function above wrapped in a class, to be used for passing as a template parameter.
        template <typename From>
        struct from_t
        {
            template <typename From_ = From>
            constexpr auto operator()(From_&& v) { return from( std::forward<From_>(v) ); }
        };
    };

    // To be used like: to_t<type, converter>() << some_value;
    template <
        typename To,
        template <typename> typename Converter
    >
    struct to_t
    {
        template <typename From>
        constexpr auto operator<<(From&& v) const { return Converter<From>{}( std::forward<From>(v) ); }
    };

    // Enables writing: to<type> << some_value;
    // Instead of:      to_t<type, converter>() << some_value;
    // Assumes converter is static_cast_to<To>.
    template <
        typename To,
        template <typename> typename Converter = static_cast_to<To>::template from_t
    >
    constexpr auto to = to_t<To, Converter>{};


    template <typename From>
    struct to_expected_t
    {
        /// TODO: investigate, copying may occur here.
        From v;

        template< class ExpectedType >
        constexpr operator ExpectedType() const &
        {
            return to<ExpectedType> << v;
        }

        template < class ExpectedType>
        constexpr operator ExpectedType() &&
        {
            return to<ExpectedType> << std::move(v);
        }
    };

    // This is *duct tape* meant to make things *just work*, PLEASE take the
    // time to remove it's use later.
    /// TODO: investigate, link errors may occur here (multiple definition).
    [[maybe_unused]] struct {
        template <typename From>
        constexpr auto operator<<(From&& v)
        {
            return to_expected_t<From>{ std::forward<From>(v) };
        }
    } to_expected;

}
