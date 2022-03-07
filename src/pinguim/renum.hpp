#pragma once

#include "pinguim/conf.hpp"

#include <string_view>

// Implementation details to make this magic work.
// If you are here to learn how to use this header skip this part.
namespace pinguim::renum::detail
{
    // Just a string view with some more operations for convenience.
    struct view
    {
        const char* begin;
        std::size_t len;

        constexpr operator std::string_view() const
        { return qualified(); } // Implicit conversion.

        // Keeps the whole name 'namespace::namespace2::...::typename::value'.
        constexpr auto qualified() const -> std::string_view
        { return {begin, len}; }

        // Keeps just 'value'.
        constexpr auto unqualified() const -> std::string_view
        {
            auto _qualified = qualified();
            auto last_colon_idx = 0u;
            for(auto i = 0u; i < _qualified.size(); ++i) { if(_qualified[i] == ':') last_colon_idx = i+1; }
            _qualified.remove_prefix(last_colon_idx);
            return _qualified;
        }

        // Keeps 'typename::value'.
        constexpr auto semi_qualified() const -> std::string_view
        {
            const auto _unqualified = unqualified();
            auto data = _unqualified.data();
            auto size = _unqualified.size();

            for(auto colons = 0u; colons <= 2; colons += *(--data) == ':') ++size;

            return {data+1, size-1};
        }
    };

    // Returns a string_view with the enum value.
    // If the value is not defined then it returns {nullptr, 0}.
    template <typename Enum, Enum Value>
    consteval auto enum_value_view()
    {
        // In GCC this fn will look like: 'constexpr auto detail::enum_value_view() [Enum = some_enum; Value = some_value]'
        // In Clang:                      'constexpr auto detail::enum_value_view() [Enum = some_enum, Value = some_value]'
        const char* fn = __PRETTY_FUNCTION__;

        #if defined(PINGUIM_CONF_COMPILER_IS_GCC)
        while(*(fn++) != ';');
        #elif defined(PINGUIM_CONF_COMPILER_IS_CLANG)
        while(*(fn++) != ',');
        #else
        static_assert(false, "Please implement pinguim::renum::detail::enum_value_view for this compiler");
        #endif

        while(*(fn++) != '=');
        fn += 1; // Skip the space padding the '=' sign in 'Value = some_value'.

        // If the value is valid then the actual value name will be the first character pointed by fn,
        // otherwise it will be '(some_enum)value' where some_enum is the enum name value is the value.
        if(*fn == '(') { return view{nullptr, 0}; }

        auto len = 0u;
        while(*(fn + len) != ']') ++len;
        return view{fn, len};
    }

    // Just a holder for many things of the same type (generally ints).
    // Is used as an alternative to std::integer_sequence without #include'ing the header.
    template <typename I, I... Is>
    struct seq {};

    // Returns a sequence of the valid indexes for Enum between Lo and Hi.
    // We won't *actually* call this, we just decltype() it to get the
    // resulting type, cheeky.
    template <typename Enum, typename I, I Lo, I Hi, I... Accumulated>
    consteval auto make_enum_idx_seq()
    {
        // If the index is not valid we'll get a warning.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wconversion"

        // If is done.
        if constexpr(Lo > Hi) { return seq<I, Accumulated...>{}; }
        // If is valid.
        else if constexpr(enum_value_view<Enum, Enum(Lo)>().begin != nullptr)
        { return make_enum_idx_seq<Enum, I, Lo+1, Hi, Accumulated..., Lo>(); }
        // If is not valid.
        else { return make_enum_idx_seq<Enum, I, Lo+1, Hi, Accumulated...>(); }

        #pragma GCC diagnostic pop
    }

    // Some type_trait utils (didn't want to #include <type_traits>).
    template <typename T> struct remove_reference      { using type = T; };
    template <typename T> struct remove_reference<T&>  { using type = T; };
    template <typename T> struct remove_reference<T&&> { using type = T; };
    template <typename T> struct remove_cv                   { using type = T; };
    template <typename T> struct remove_cv<const T>          { using type = T; };
    template <typename T> struct remove_cv<volatile T>       { using type = T; };
    template <typename T> struct remove_cv<const volatile T> { using type = T; };
    template <typename T> using  remove_cvref_t = typename remove_cv< typename remove_reference<T>::type >::type;
}

namespace pinguim::renum
{
    // You can use this if you know what you're doing, otherwise just use the functions below.
    // If you are the end-user then you probably want to use reflect<enum> instead of this (reflected<enum>),
    // that type will compute <I... Values> automatically.
    template <typename Enum, typename I, I... Values>
    struct reflected
    {
        static_assert(
            ((detail::enum_value_view<Enum, Enum(Values)>().begin != nullptr) && ...),
            "Invalid value for enum reflection (did you try to manually use reflected<enum> ? please use reflect<enum> instead)."
        );

        static constexpr auto count           = sizeof...(Values);
        static constexpr I values[]           = { Values... };
        static constexpr detail::view views[] = { detail::enum_value_view<Enum, Enum(Values)>()... };

        constexpr auto operator[] (Enum const& e)
        {
            // TODO: maybe use some map here.
            for(auto i = 0u; i < count; ++i) { if(values[i] == I(e)) return views[i]; }
            return detail::view{"INVALID ENUM VALUE OR CHECK RANGE TOO SMALL", 43};
        }

        // Range-for compatibility related stuff:
        // Just some POD types to enable structured bindings (didn't want to #include <tuple>).
        struct value_iterator_v{ I    value; detail::view view; };
        struct enum_iterator_v { Enum value; detail::view view; };
        template <bool IsValueIterator = true> struct iterator
        {
            constexpr auto begin() const { return *this; }
            constexpr auto end()   const { return nullptr; }

            constexpr auto operator==(void*) const -> bool { return i == count; }
            constexpr auto operator*() const
            {
                if constexpr(IsValueIterator)
                { return value_iterator_v{ values[i],       views[i] }; }
                else
                { return enum_iterator_v { Enum(values[i]), views[i] }; }
            }
            constexpr auto operator++() { ++i; }

            private: std::size_t i = 0;
        };
        using value_iterator = iterator<true>;
        using enum_iterator  = iterator<false>;
    };

    // We also keep an alias that takes a sequence (like std::integer_sequence or detail::seq)
    // in order to extract the values from that.
    template <typename Enum, template <typename I, I...> typename Seq, typename I, I... Values>
    struct reflected<Enum, Seq<I, Values...>> { using type = reflected<Enum, I, Values...>; };

    // This is the type the end-user is going to use, from this we compute the valid values
    // between [CheckLo, CheckHi] using that cheeky function from before.
    template <typename Enum, typename I = long long int, I CheckLo = -32, I CheckHi = 32>
    using reflect = typename reflected<
        Enum,
        decltype( detail::make_enum_idx_seq<Enum, I, CheckLo, CheckHi>() )
    >::type;

    // The following are just aliases for convenience.
    // We have the reflected enum as a template argument in case the caller wants.
    // to customize the CheckLo and CheckHi template parameters.

    // Returns the fully qualified name.
    // E.g: returns
    //    "pinguim::app::subsystems::logic::direct_control::input_types::keyboard_AD"sv
    // instead of just
    //    "keyboard_AD"sv
    template<
        typename Enum,
        typename Reflected = reflect< detail::remove_cvref_t<Enum> >
    > constexpr auto qualified_value_name(Enum&& e) -> std::string_view
    { return Reflected{}[e].qualified(); }

    template<
        typename Enum,
        typename Reflected = reflect< detail::remove_cvref_t<Enum> >
    > constexpr auto unqualified_value_name(Enum&& e) -> std::string_view
    { return Reflected{}[e].unqualified(); }

    template<
        typename Enum,
        typename Reflected = reflect< detail::remove_cvref_t<Enum> >
    > constexpr auto semi_qualified_value_name(Enum&& e) -> std::string_view
    { return Reflected{}[e].semi_qualified(); }
}
