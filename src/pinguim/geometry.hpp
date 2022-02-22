#pragma once

#include <utility> // std::pair.
#include <complex>
#include <array>
#include <tuple>
#include <cmath> // std::sin and std::cos.

#include "pinguim/aliases.hpp"

namespace pinguim
{
    namespace geometry {};
    namespace geo = geometry;
};

namespace pinguim::geometry
{
    template<typename NumT>
    struct point
    {
        NumT x;
        NumT y;

        constexpr auto as_arr()     { return std::array<NumT, 2>{x, y}; }
        constexpr auto as_tuple()   { return std::tuple<NumT, NumT>{x, y}; }
        constexpr auto as_pair()    { return std::pair<NumT, NumT>{x, y}; }
        constexpr auto as_complex() { return std::complex<NumT>{x, y}; }

        static constexpr auto rotated(const point& p, float radians, const point& axis = {0, 0}) -> point;
        static constexpr auto rotate(point& p, float radians, const point& axis = {0, 0}) -> point&
        { return (p = point::rotated(p, radians, axis)); }
        constexpr auto rotate(float radians, const point& axis = {0, 0}) -> point&
        { return point::rotate(*this, radians, axis); }
        constexpr auto rotated(float radians, const point& axis = {0, 0}) -> point
        { return point::rotated(*this, radians, axis); }
    };

    using rpoint = point<double>;
    using fpoint = point<float>;
    using ipoint = point<i32>;
    using upoint = point<u32>;
}

template<typename NumT>
constexpr auto pb::geo::point<NumT>::rotated(const point& p, float radians, const point& axis) -> point
{
    return {
        .x = std::cos(radians) * (p.x - axis.x) - std::sin(radians) * (p.y - axis.y) + axis.x,
        .y = std::sin(radians) * (p.x - axis.x) + std::cos(radians) * (p.y - axis.y) + axis.y
    };
}
