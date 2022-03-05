#include "pinguim/cvt.hpp"

#include <cassert>

int main()
{
    assert(5.8f + pinguim::cvt::to<int> * 5.99 == 10.8f);
    assert(5.8f * pinguim::cvt::to<int> + 5.99 == 10.99);
    return 0;
}
