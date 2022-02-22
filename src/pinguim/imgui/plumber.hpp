// Connects SDL2, ImGui and GLEW together and provides a drawing context that
// closes all resources once destroyed.
#pragma once

#include <functional>
#include <optional>
#include <memory> // std::unique_ptr.

// Forward declarations
struct SDL_Window;
union SDL_Event;

namespace pinguim::imgui
{
    struct plumber
    {
        using event_handler = std::function<void(SDL_Event&)>;
        using cleanup_ptr = std::unique_ptr<void, void(*)(void*)>;
        using sdl_window_cleanup_ptr = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>;

        // Non copyable.
        plumber(const plumber&)                    = delete;
        auto operator=(const plumber&) -> plumber& = delete;

        plumber()                             = default;
        plumber(plumber&&)                    = default;
        ~plumber()                            = default;
        auto operator=(plumber&&) -> plumber& = default;

        cleanup_ptr sdl_context           = {nullptr, [](auto){}};
        sdl_window_cleanup_ptr sdl_window = {nullptr, [](auto){}};
        cleanup_ptr sdl_glcontext         = {nullptr, [](auto){}};
        cleanup_ptr imgui_context         = {nullptr, [](auto){}};
        cleanup_ptr imgui_sdl2_context    = {nullptr, [](auto){}};
        cleanup_ptr imgui_opengl3_context = {nullptr, [](auto){}};

        struct {float x, y, z, w;} clear_color = {0.45f, 0.55f, 0.60f, 1.00f};

        // Returns true if you need to quit (close button pressed)
        auto handle_event()                     -> bool;
        auto quit_handler(SDL_Event&)           -> bool;

        auto handle_event(event_handler&)       -> void;
        auto handle_event(event_handler&&)      -> void;
        auto handle_event(const event_handler&) -> void;

        auto begin_frame() -> void;
        auto draw_frame() -> void;
    };

    auto make_plumber(const char* windowname = "pinguimbots") -> std::optional<plumber>;
}
