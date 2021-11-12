// Who likes long types?
#pragma once

#include <cstddef> // For std::size_t.
#include <cstdint> // For std::*int*_t.

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

namespace pinguim::inline float_aliases
{
    using f32 = float;
    using f64 = double;
    //using f128 ? f80 ? fl64 = long double;
    // Different compilers have different sizes, maybe fl64 is the best name.
}
