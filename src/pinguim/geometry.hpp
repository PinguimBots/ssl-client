#pragma once

#include "pinguim/aliases.hpp"

namespace pinguim
{
    namespace geometry {};
    namespace geo = geometry;
};

namespace pinguim::geometry
{
    template <typename NumT>
    struct point
    {
        NumT x;
        NumT y;

        constexpr auto operator[](const auto i) const -> NumT { return i == 0 ? x : y; }

        [[nodiscard]] auto operator+(const point& other) const -> point;
        [[nodiscard]] auto operator-(const point& other) const -> point;
        [[nodiscard]] auto operator*(const NumT& num) const -> point;
        [[nodiscard]] auto operator/(const NumT& num) const -> point;

        [[nodiscard]] auto distance(const point& to) const -> NumT;
        [[nodiscard]] auto length() const -> NumT;
        [[nodiscard]] auto midpoint(const point& p) const -> point;

        [[nodiscard]] auto rotated(float radians, const point& axis = {0, 0}) const -> point;
        auto rotate(float radians, const point& axis = {0, 0}) -> point&;

        // NOTE: Returns {Nan, Nan} if length() == 0 due to division by 0,
        // use normalized_or({0, 0}) if that's a problem.
        [[nodiscard]] auto normalized() const -> point;
        auto normalize() -> point&;

        [[nodiscard]] auto normalized_or(const point& p) const -> point;
        auto normalize_or(const point& p) -> point&;
    };

    template <typename NumT>
    [[nodiscard]] auto operator*(NumT num, const point<NumT>& p) -> point<NumT>;
    template <typename NumT>
    [[nodiscard]] auto operator/(NumT num, const point<NumT>& p) -> point<NumT>;

    using rpoint = point<double>;
    using fpoint = point<float>;
    using ipoint = point<i32>;
    using upoint = point<u32>;
}

// Extern instantiations (for faster #include time), instantiated in geometry.cpp.
//
// This + implementations being in the .cpp file means if you want these functions
// for other types (which realistically you won't don't) you will need to implement
// them yourself.
//
// This is a tradeoff between #include time and extendability.
//
// You can use LTO to regain (probably) regain the runtime-speed lost from not using
// constexpr but then linking takes forever.

extern template struct pb::geo::point<double>;
extern template struct pb::geo::point<float>;
extern template struct pb::geo::point<pb::i32>;
extern template struct pb::geo::point<pb::u32>;

extern template auto pinguim::geometry::operator*<double> (double, const point<double>&)   -> point<double>;
extern template auto pinguim::geometry::operator/<double> (double, const point<double>&)   -> point<double>;
extern template auto pinguim::geometry::operator*<float>  (float, const point<float>&)     -> point<float>;
extern template auto pinguim::geometry::operator/<float>  (float, const point<float>&)     -> point<float>;
extern template auto pinguim::geometry::operator*<pb::i32>(pb::i32, const point<pb::i32>&) -> point<pb::i32>;
extern template auto pinguim::geometry::operator/<pb::i32>(pb::i32, const point<pb::i32>&) -> point<pb::i32>;
extern template auto pinguim::geometry::operator*<pb::u32>(pb::u32, const point<pb::u32>&) -> point<pb::u32>;
extern template auto pinguim::geometry::operator/<pb::u32>(pb::u32, const point<pb::u32>&) -> point<pb::u32>;
