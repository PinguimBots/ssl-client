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

    inline namespace literals
    {
        constexpr auto operator""_u8(unsigned long long val)  { return static_cast<u8>(val); }
        constexpr auto operator""_u16(unsigned long long val) { return static_cast<u16>(val); }
        constexpr auto operator""_u32(unsigned long long val) { return static_cast<u32>(val); }
        constexpr auto operator""_u64(unsigned long long val) { return static_cast<u64>(val); }

        constexpr auto operator""_i8(unsigned long long val)  { return static_cast<i8>(val); }
        constexpr auto operator""_i16(unsigned long long val) { return static_cast<i16>(val); }
        constexpr auto operator""_i32(unsigned long long val) { return static_cast<i32>(val); }
        constexpr auto operator""_i64(unsigned long long val) { return static_cast<i64>(val); }
    };
}

namespace pinguim::inline float_aliases
{
    using f32 = float;
    using f64 = double;
    //using f128 ? f80 ? fl64 = long double;
    // Different compilers have different sizes, maybe fl64 is the best name.
}

namespace pb = pinguim;
