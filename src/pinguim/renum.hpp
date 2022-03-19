#pragma once

#include "pinguim/conf.hpp"

#include "pinguim/standalone/remove_cvref.hpp"

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

            #if defined(PINGUIM_CONF_COMPILER_IS_MSVC)

                for(auto colons = 0u; colons < 2; colons += *(--data) == ':') ++size;
                --data;
                while(*data != ',' && *data != ':') { --data; ++size; }
                ++size;

            #else

                for(auto colons = 0u; colons <= 2 && *data != ' '; colons += *(--data) == ':') ++size;

            #endif

            return {data+1, size-1};
        }
    };

    // Returns a string_view with the enum value.
    // If the value is not defined then it returns {nullptr, 0}.
    template <typename Enum, Enum Value>
    consteval auto enum_value_view()
    {
        #if defined(PINGUIM_CONF_COMPILER_IS_MSVC)

            // if valid:   auto __cdecl pinguim::renum::detail::enum_value_view<enum some_enum,some_enum::some_value>(void)
            // if invalid: auto __cdecl pinguim::renum::detail::enum_value_view<enum some_enum,(enum some_enum)some_value>(void)
            std::string_view name = __FUNCSIG__;
            std::string_view prefix = "auto __cdecl pinguim::renum::detail::enum_value_view<";
            std::string_view suffix = ">(void)";
            name.remove_prefix(prefix.size());
            name.remove_suffix(suffix.size());

            const char* fn = name.data();
            auto len = name.length();

            // To deal with nested templates.
            auto open_brackets = 0u;
            while(true)
            {
                ++fn;
                --len;

                open_brackets += *fn == '<';
                open_brackets -= *fn == '>';

                if(*fn == ',' && open_brackets == 0) break;
            }

            ++fn;
            --len;
            if(*fn == '(') { return view{nullptr, 0}; }

            return view{fn, len};

        #else

            // In GCC this fn will look like: 'constexpr auto detail::enum_value_view() [Enum = some_enum; Value = fully_qualified::some_enum::some_value]'
            // In Clang:                      'constexpr auto detail::enum_value_view() [Enum = some_enum, Value = fully_qualified::some_enum::some_value]'
            // They differ by punctuation, basically.
            // if valid:   'constexpr auto detail::enum_value_view() [Enum = some_enum, Value = fully_qualified::some_enum::some_value]'
            // if invalid: 'constexpr auto detail::enum_value_view() [Enum = some_enum, Value = (fully_qualified::some_enum)some_value]'
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

            if(*fn == '(') { return view{nullptr, 0}; }

            auto len = 0u;
            while(*(fn + len) != ']') ++len;
            return view{fn, len};

        #endif
    }

    // Just a holder for many things of the same type (generally ints).
    // Is used as an alternative to std::integer_sequence without #include'ing the header.
    template <typename I, I... Is>
    struct seq {};

    // Returns a sequence of the valid indexes for Enum between Lo and Hi.
    // We won't *actually* call this, we just decltype() it to get the
    // resulting type, cheeky.
    template <typename Enum, typename I, I Lo, I Hi, I... Accumulated>
    consteval auto enum_check_range_impl()
    {
        #if !defined(PINGUIM_CONF_COMPILER_IS_MSVC)
        // If the index is not valid we'll get a warning.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wconversion"
        #endif

        // If is done.
        if constexpr(Lo > Hi) { return seq<I, Accumulated...>{}; }
        // If is valid.
        else if constexpr(enum_value_view<Enum, Enum(Lo)>().begin != nullptr)
        { return enum_check_range_impl<Enum, I, Lo+1, Hi, Accumulated..., Lo>(); }
        // If is not valid.
        else { return enum_check_range_impl<Enum, I, Lo+1, Hi, Accumulated...>(); }

        #if !defined(PINGUIM_CONF_COMPILER_IS_MSVC)
        #pragma GCC diagnostic pop
        #endif
    }

    template <typename Enum, typename I, I Lo, I Hi>
    using enum_check_range = decltype( enum_check_range_impl<Enum, I, Lo, Hi>() );
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
        static_assert(sizeof...(Values) > 0, "Needs at least one value, increase CheckLo and CheckHi if you need");

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
    // between [CheckLo, CheckHi] using enum_check_range.
    template <typename Enum, auto CheckLo = -32, auto CheckHi = 32>
    using reflect = typename reflected<
        Enum,
        detail::enum_check_range< Enum, decltype(CheckLo), CheckLo, CheckHi >
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
        typename Reflected = reflect< pinguim::s::remove_cvref_t<Enum> >
    > constexpr auto qualified_value_name(Enum&& e) -> std::string_view
    { return Reflected{}[e].qualified(); }

    template<
        typename Enum,
        typename Reflected = reflect< pinguim::s::remove_cvref_t<Enum> >
    > constexpr auto unqualified_value_name(Enum&& e) -> std::string_view
    { return Reflected{}[e].unqualified(); }

    template<
        typename Enum,
        typename Reflected = reflect< pinguim::s::remove_cvref_t<Enum> >
    > constexpr auto semi_qualified_value_name(Enum&& e) -> std::string_view
    { return Reflected{}[e].semi_qualified(); }
}
