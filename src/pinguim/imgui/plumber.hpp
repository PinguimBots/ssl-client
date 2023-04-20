// Connects SDL2, ImGui and GLEW together and provides a drawing context that
// closes all resources once destroyed.
#pragma once

// Forward declarations
struct SDL_Window;
union SDL_Event;
struct ImGuiContext;

namespace pinguim::imgui
{
    struct plumber
    {
        // Non copyable.
        plumber(const plumber&)                    = delete;
        auto operator=(const plumber&) -> plumber& = delete;

        // But moveable.
        plumber(plumber&&);
        auto operator=(plumber&&) -> plumber&;

        plumber() = default;
        ~plumber();

        bool          sdl_context           = false;
        SDL_Window*   sdl_window            = nullptr;
        void*         sdl_glcontext         = nullptr;
        ImGuiContext* imgui_context         = nullptr;
        bool          imgui_sdl2_context    = false;
        bool          imgui_opengl3_context = false;

        struct {float x, y, z, w;} clear_color = {0.45f, 0.55f, 0.60f, 1.00f};

        // Calls the default event handler.
        // Returns true if you need to quit (close button pressed)
        auto handle_event() -> bool;

        // If you want to handle the event yourself.
        using event_handler = void(*)(SDL_Event&, void*);
        auto handle_events(event_handler, void* userdata = nullptr) -> void;

        auto begin_frame(bool rebuild_fonts = false) -> void;
        auto draw_frame() -> void;
    };

    // After calling this, you own this pointer, make sure to delete it.
    auto make_plumber(const char* windowname = "pinguimbots", bool vsync_enabled = false) -> plumber*;
}
