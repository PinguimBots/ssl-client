#include "pinguim/scoped.hpp"

#include <exception>
#include <cassert>

auto do_exception_tests() -> void;
// auto do_disarm_tests() -> void;
// auto do_move_tests() -> void;

int main() {
    do_exception_tests();

    return 0;
}

auto do_exception_tests() -> void
{
    bool called = false;
    auto set_called = [&]{ called = true; };

    { auto x = pinguim::scoped{set_called}; }
    assert(called);

    called = false;

    pinguim::scoped{set_called};
    assert(called);

    called = false;

    { auto x = pinguim::scoped_except{set_called}; }
    assert(!called);

    try
    {
        auto x = pinguim::scoped_except{set_called};
        throw std::exception{};
    }
    catch (std::exception& e)
    { assert(called); }

    called = false;

    { auto x = pinguim::scoped_noexcept{set_called}; }
    assert(called);

    called = false;

    try
    {
        auto x = pinguim::scoped_noexcept{set_called};
        throw std::exception{};
    }
    catch (std::exception& e)
    { assert(!called); }
}
