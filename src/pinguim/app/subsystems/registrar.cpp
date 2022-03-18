#include "pinguim/app/subsystems/registrar.hpp"

#include <vector>

struct pinguim::app::subsystems::registrar::impl
{
    subsystem_id_t last_id = 0;

    std::vector<registry> subsystems = {};
};

auto pinguim::app::subsystems::registrar::instance() -> registrar&
{
    static registrar r;
    return r;
}

auto pinguim::app::subsystems::registrar::invalid_id() -> subsystem_id_t
{ return 0; }

pinguim::app::subsystems::registrar::registrar() : pimpl{ new impl } {}
pinguim::app::subsystems::registrar::~registrar() { delete pimpl; }

auto pinguim::app::subsystems::registrar::register_input_subsystem(
    input_subsystem_maker maker,
    const char* name
) -> subsystem_id_t
{
    auto const id = ++(pimpl->last_id);

    pimpl->subsystems.push_back({reinterpret_cast<void*>(maker), type_enum::input, name, id});

    return id;
}

auto pinguim::app::subsystems::registrar::register_logic_subsystem(
    logic_subsystem_maker maker,
    const char* name
) -> subsystem_id_t
{
    auto const id = ++(pimpl->last_id);

    pimpl->subsystems.push_back({reinterpret_cast<void*>(maker), type_enum::logic, name, id});

    return id;
}

auto pinguim::app::subsystems::registrar::register_output_subsystem(
    output_subsystem_maker maker,
    const char* name
) -> subsystem_id_t
{
    auto const id = ++(pimpl->last_id);

    pimpl->subsystems.push_back({reinterpret_cast<void*>(maker), type_enum::output, name, id});

    return id;
}


auto pinguim::app::subsystems::registrar::register_misc_subsystem(
    misc_subsystem_maker maker,
    const char* name
) -> subsystem_id_t
{
    auto const id = ++(pimpl->last_id);

    pimpl->subsystems.push_back({reinterpret_cast<void*>(maker), type_enum::misc, name, id});

    return id;
}

auto pinguim::app::subsystems::registrar::begin() const -> registry const*
{ return pimpl->subsystems.data(); }

auto pinguim::app::subsystems::registrar::end() const -> registry const*
{ return pimpl->subsystems.data() + pimpl->subsystems.size(); }