#include "pinguim/math.hpp"
#include "pinguim/geometry.hpp"

#include <cassert>

#include <iostream>

namespace m = pb::m;
using pb::geo::ipoint;
using pb::geo::rpoint;

int main()
{
    assert(m::map(50, ipoint{10, 50}, ipoint{100, 1000}) == m::map(50, 10, 50, 100, 1000));

    std::cout << m::map(91, ipoint{0, 600}, rpoint{-0.75, 0.75}) << std::endl;
}