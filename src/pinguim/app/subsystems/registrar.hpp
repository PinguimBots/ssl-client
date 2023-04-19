#pragma once


// Forward decls
namespace pinguim::app
{
    struct input_subsystem;
    struct logic_subsystem;
    struct output_subsystem;
    struct misc_subsystem;
}

namespace pinguim::app::subsystems
{
    struct registrar
    {
        using input_subsystem_maker  = input_subsystem*(*)();
        using logic_subsystem_maker  = logic_subsystem*(*)();
        using output_subsystem_maker = output_subsystem*(*)();
        using misc_subsystem_maker   = misc_subsystem*(*)();
        using subsystem_id_t = unsigned long long;

        enum class type_enum {input, logic, output, misc};
        struct registry
        {
            void*          maker; // Its safe to cast this to the *_maker of the corresponding type.
            type_enum      type;
            const char*    name;
            subsystem_id_t id;
        };

        static registrar& instance();

        // This id is unused and should be used when you need to represent and uninitialized
        // subsystem id.
        static auto invalid_id() -> subsystem_id_t;

        registrar();
        ~registrar();

        // Registering invalidates the iterators below.
        auto register_input_subsystem(input_subsystem_maker, const char*) -> subsystem_id_t;
        auto register_logic_subsystem(logic_subsystem_maker, const char*) -> subsystem_id_t;
        auto register_output_subsystem(output_subsystem_maker, const char*) -> subsystem_id_t;
        auto register_misc_subsystem(misc_subsystem_maker, const char*) -> subsystem_id_t;

        auto begin() const -> registry const*;
        auto end()   const -> registry const*;

    private:
        struct impl;
        impl* pimpl;
    };
}

// Call the corresponding macro once on the .cpp file of your subsystem.

#define PINGUIM_APP_REGISTER_INPUT_SUBSYSTEM(cls, name)                                                \
    namespace {                                                                                        \
        auto registrar_id = pinguim::app::subsystems::registrar::instance().register_input_subsystem(  \
            []() -> pinguim::app::input_subsystem* { return new cls; },                                \
            name                                                                                       \
        );                                                                                             \
    }

#define PINGUIM_APP_REGISTER_LOGIC_SUBSYSTEM(cls, name)                                                \
    namespace {                                                                                        \
        auto registrar_id = pinguim::app::subsystems::registrar::instance().register_logic_subsystem(  \
            []() -> pinguim::app::logic_subsystem* { return new cls; },                                \
            name                                                                                       \
        );                                                                                             \
    }

#define PINGUIM_APP_REGISTER_OUTPUT_SUBSYSTEM(cls, name)                                                \
    namespace {                                                                                         \
        auto registrar_id = pinguim::app::subsystems::registrar::instance().register_output_subsystem(  \
            []() -> pinguim::app::output_subsystem* { return new cls; },                                \
            name                                                                                        \
        );                                                                                              \
    }

#define PINGUIM_APP_REGISTER_MISC_SUBSYSTEM(cls, name)                                                  \
    namespace {                                                                                         \
        auto registrar_id = pinguim::app::subsystems::registrar::instance().register_misc_subsystem(    \
            []() -> pinguim::app::misc_subsystem* { return new cls; },                                  \
            name                                                                                        \
        );                                                                                              \
    }
