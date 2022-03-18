#include "pinguim/geometry.hpp"

#include <cmath> // std::sin, std::cos, std::sqrt.
#include <numeric> // std::midpoint.

#include "pinguim/cvt.hpp"

// Implementations.

template <typename NumT>
auto pb::geo::point<NumT>::operator+(const point& other) const -> point
{ return {x + other.x, y + other.y}; }

template <typename NumT>
auto pb::geo::point<NumT>::operator-(const point& other) const -> point
{ return {x - other.x, y - other.y}; }

template <typename NumT>
auto pb::geo::point<NumT>::operator*(const NumT& num) const -> point
{ return {x * num, y * num}; }

template <typename NumT>
auto pb::geo::point<NumT>::operator/(const NumT& num) const -> point
{ return {x / num, y / num}; }

template <typename NumT>
auto pb::geo::operator*(NumT num, const point<NumT>& p) -> point<NumT>
{ return p * num; }

template <typename NumT>
auto pb::geo::operator/(NumT num, const point<NumT>& p) -> point<NumT>
{ return p / num; }

template <typename NumT>
auto pb::geo::point<NumT>::distance(const point& to) const -> NumT
{
    const auto diff = *this - to;
    return cvt::toe * std::sqrt(cvt::to<double>( diff.x*diff.x + diff.y*diff.y));
}

template <typename NumT>
auto pb::geo::point<NumT>::length() const -> NumT
{ return distance({0, 0}); }

template <typename NumT>
auto pb::geo::point<NumT>::midpoint(const point& p) const -> point
{ return {std::midpoint(x, p.x), std::midpoint(y, p.y)}; }

template <typename NumT>
auto pb::geo::point<NumT>::rotated(float radians, const point& axis) const -> point
{
    return
    {
        .x = cvt::toe( std::cos(cvt::to<double> * radians) * cvt::to<double>(x - axis.x) - std::sin(cvt::to<double> * radians) * cvt::to<double>(y - axis.y) + cvt::to<double> * axis.x ),
        .y = cvt::toe( std::sin(cvt::to<double> * radians) * cvt::to<double>(x - axis.x) + std::cos(cvt::to<double> * radians) * cvt::to<double>(y - axis.y) + cvt::to<double> * axis.y )
    };
}

template <typename NumT>
auto pb::geo::point<NumT>::rotate(float radians, const point& axis) -> point&
{ return (*this = rotated(radians, axis)); }

template <typename NumT>
auto pb::geo::point<NumT>::normalized() const -> point
{ return *this / length(); }

template <typename NumT>
auto pb::geo::point<NumT>::normalize() -> point&
{ return (*this = normalized()); }

template <typename NumT>
auto pb::geo::point<NumT>::normalized_or(const point& p) const -> point
{
    auto len = length();
    if(len >= 0) { return *this / len; }
    else         { return p; }
}

template <typename NumT>
auto pb::geo::point<NumT>::normalize_or(const point& p) -> point&
{ return (*this = normalized_or(p)); }

// Explicit instantiations.

template struct pb::geo::point<double>;
template struct pb::geo::point<float>;
template struct pb::geo::point<pb::i32>;
template struct pb::geo::point<pb::u32>;

template auto pinguim::geometry::operator*<double> (double, const point<double>&)   -> point<double>;
template auto pinguim::geometry::operator/<double> (double, const point<double>&)   -> point<double>;
template auto pinguim::geometry::operator*<float>  (float, const point<float>&)     -> point<float>;
template auto pinguim::geometry::operator/<float>  (float, const point<float>&)     -> point<float>;
template auto pinguim::geometry::operator*<pb::i32>(pb::i32, const point<pb::i32>&) -> point<pb::i32>;
template auto pinguim::geometry::operator/<pb::i32>(pb::i32, const point<pb::i32>&) -> point<pb::i32>;
template auto pinguim::geometry::operator*<pb::u32>(pb::u32, const point<pb::u32>&) -> point<pb::u32>;
template auto pinguim::geometry::operator/<pb::u32>(pb::u32, const point<pb::u32>&) -> point<pb::u32>;
