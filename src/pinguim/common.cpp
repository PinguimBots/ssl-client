#include "pinguim/common.hpp"

namespace pinguim {
    auto to_pair(const pinguim::point& p) -> std::pair<double, double>
    {
        return {p.real(), p.imag()};
    }

    auto to_pair(const pinguim::wpoint& p) -> std::pair<int, int>
    {
        return {p.real(), p.imag()};
    }
}
