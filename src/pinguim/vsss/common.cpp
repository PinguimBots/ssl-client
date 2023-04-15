#include "pinguim/vsss/common.hpp"

namespace pinguim::vsss::inline common {
    auto to_pair(const pinguim::vsss::point& p) -> std::pair<double, double>
    {
        return {p.real(), p.imag()};
    }

    auto to_pair(const pinguim::vsss::wpoint& p) -> std::pair<int, int>
    {
        return {p.real(), p.imag()};
    }
}
