#include "pinguim/imgui/plumber.hpp"

#include <GL/glew.h>

#include <SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

// TODO: return info about what errored out (nonstd::expected<plumber, error_type>)
auto pinguim::imgui::make_plumber(const char* windowname) -> std::optional<plumber>
{
    static bool is_initialized = false;

    if(is_initialized) {return std::nullopt;}

    auto mario = plumber{};

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {return std::nullopt;}
    mario.sdl_context = {nullptr, [](auto){SDL_Quit();}};

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    auto window = SDL_CreateWindow(
        windowname,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(window == nullptr) {return std::nullopt;}
    mario.sdl_window = {window, [](auto w){SDL_DestroyWindow(w);}};

    // And the graphics context
    auto gl_context = SDL_GL_CreateContext(window);
    if(gl_context == nullptr) {return std::nullopt;}
    mario.sdl_glcontext = {gl_context, [](auto ctx) {SDL_GL_DeleteContext(reinterpret_cast<SDL_GLContext>(ctx));}};

    if(SDL_GL_MakeCurrent(window, gl_context) != 0) {return std::nullopt;}
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if(glewInit() != GLEW_OK) {return std::nullopt;}

    IMGUI_CHECKVERSION();

    auto imgui_context = ImGui::CreateContext();
    if(imgui_context == nullptr) {return std::nullopt;}
    ImGui::SetCurrentContext(imgui_context);
    mario.imgui_context = {imgui_context, [](auto ctx){ImGui::DestroyContext(static_cast<ImGuiContext*>(ctx));}};

    ImGui::GetIO().IniFilename = nullptr;

    if(ImGui_ImplSDL2_InitForOpenGL(window, gl_context) == false) {return std::nullopt;}
    mario.imgui_sdl2_context = {nullptr, [](auto){ImGui_ImplSDL2_Shutdown();}};

    if(ImGui_ImplOpenGL3_Init("#version 130") == false) {return std::nullopt;}
    mario.imgui_opengl3_context = {nullptr, [](auto){ImGui_ImplOpenGL3_Shutdown();}};

    is_initialized = true;
    return mario;
}

auto pinguim::imgui::plumber::handle_event() -> bool
{
    auto quit = false;
    handle_event([&](auto& e){ quit = quit_handler(e); });
    return quit;
}

auto pinguim::imgui::plumber::quit_handler(SDL_Event& e) -> bool
{
    if (e.type == SDL_QUIT) {return true;}
    if (e.type == SDL_WINDOWEVENT &&
        e.window.event == SDL_WINDOWEVENT_CLOSE &&
        e.window.windowID == SDL_GetWindowID(sdl_window.get()))
    {
        return true;
    }

    return false;
};


auto pinguim::imgui::plumber::handle_event(event_handler& handler) -> void
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        handler(event);
    }
}
// Same as above.
auto pinguim::imgui::plumber::handle_event(event_handler&& handler) -> void
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        handler(event);
    }
}
// Same as above.
auto pinguim::imgui::plumber::handle_event(const event_handler& handler) -> void
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        handler(event);
    }
}

auto pinguim::imgui::plumber::begin_frame() -> void
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdl_window.get());
    ImGui::NewFrame();
}

auto pinguim::imgui::plumber::draw_frame() -> void
{
    auto& io = ImGui::GetIO();

    ImGui::Render();
    glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(sdl_window.get());
}
