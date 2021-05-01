/// General quality of life stuff.
#pragma once

#include <cstddef> // For std::size_t.
#include <cstdint>

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
//     PINGUIM_QOL_TUPLIFY(fira_message::Field, 4);
// And now you can do:
//     auto [len, width, gwidth, gdepth] = fira_message::field();

#define PINGUIM_QOL_TUPLIFY(classname, tuplesize)                   \
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
