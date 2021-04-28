#include <pinguim/vsss/strategy.hpp>

#include "pinguim/cvt.hpp"

auto pinguim::vsss::Strategy::discreet_to_real(pinguim::vsss::wpoint wpoint) -> pinguim::vsss::point
{

    auto [iin, jin] = pinguim::vsss::to_pair(wpoint);

    double xout = iin * dx - imin * dx + xmin;
    double yout = jin * dy - jmin * dy + ymin;

    if (xout > xmax)
    {
        xout = xmax;
    }
    else if (xout < xmin)
    {
        xout = xmin;
    }

    if (yout > ymax)
    {
        yout = ymax;
    }
    else if (yout < ymin)
    {
        xout = ymin;
    }

    return {xout, yout};
}

auto pinguim::vsss::Strategy::real_to_discreet(pinguim::vsss::point point) -> pinguim::vsss::wpoint
{
    auto [xin, yin] = pinguim::vsss::to_pair(point);

    int iout = cvt::to_expected << std::round((imax-1)*((xin-xmin)/xT)) + imin;
    int jout = cvt::to_expected << std::round((jmax-1)*((yin-ymin)/yT)) + jmin;

    if (iout < imin)
    {
        iout = imin;
    }
    else if (iout > imax)
    {
        iout = imax;
    }

    if (jout < jmin)
    {
        jout = jmin;
    }
    else if (jout > jmax)
    {
        jout = jmax;
    }

    return {iout, jout};
}