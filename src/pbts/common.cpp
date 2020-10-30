#include "pbts/common.hpp"

namespace pbts {
    auto to_pair(const pbts::point& p) -> std::pair<double, double>
    {
        return {p.real(), p.imag()};
    }
}
