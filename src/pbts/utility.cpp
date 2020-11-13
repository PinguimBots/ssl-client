#include <pbts/strategy.hpp>

auto pbts::Strategy::discreet_to_real(pbts::wpoint wpoint) -> pbts::point
{

    auto [iin, jin] = pbts::to_pair(wpoint);

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

auto pbts::Strategy::real_to_discreet(pbts::point point) -> pbts::wpoint
{
    auto [xin, yin] = pbts::to_pair(point);

    int iout = std::round((imax-1)*((xin-xmin)/xT)) + imin;
    int jout = std::round((jmax-1)*((yin-ymin)/yT)) + jmin;

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