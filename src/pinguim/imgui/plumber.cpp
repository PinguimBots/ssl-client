#include "pinguim/imgui/plumber.hpp"

#include "pinguim/imgui/fonts/loader.hpp"
#include "pinguim/standalone/forward.hpp"
#include "pinguim/cvt.hpp"

#include <GL/glew.h>

#include <SDL.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

pinguim::imgui::plumber::~plumber()
{
    if(imgui_opengl3_context) ImGui_ImplOpenGL3_Shutdown();
    if(imgui_sdl2_context)    ImGui_ImplSDL2_Shutdown();
    if(imgui_context)         ImGui::DestroyContext(imgui_context);
    if(sdl_glcontext)         SDL_GL_DeleteContext(sdl_glcontext);
    if(sdl_window)            SDL_DestroyWindow(sdl_window);
    if(sdl_context)           SDL_Quit();
}

pinguim::imgui::plumber::plumber(plumber&& other)
{ *this = s::move(other); }

auto pinguim::imgui::plumber::operator=(plumber&& other) -> plumber&
{
    #define PINGUIM_DETAIL_SWAP(a, b) { auto temp = a; a = b; b = temp; }

    PINGUIM_DETAIL_SWAP(sdl_context,           other.sdl_context);
    PINGUIM_DETAIL_SWAP(sdl_window,            other.sdl_window);
    PINGUIM_DETAIL_SWAP(sdl_glcontext,         other.sdl_glcontext);
    PINGUIM_DETAIL_SWAP(imgui_context,         other.imgui_context);
    PINGUIM_DETAIL_SWAP(imgui_sdl2_context,    other.imgui_sdl2_context);
    PINGUIM_DETAIL_SWAP(imgui_opengl3_context, other.imgui_opengl3_context);

    #undef PINGUIM_DETAIL_SWAP

    return *this;
}

// TODO: return info about what errored out (nonstd::expected<plumber, error_type>)
auto pinguim::imgui::make_plumber(const char* windowname, bool vsync_enabled) -> plumber*
{
    static bool is_initialized = false;

    if(is_initialized) {return nullptr;}

    auto mario = plumber{};

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {return nullptr;}
    mario.sdl_context = true;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    mario.sdl_window = SDL_CreateWindow(
        windowname,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(mario.sdl_window == nullptr) {return nullptr;}

    mario.sdl_glcontext = SDL_GL_CreateContext(mario.sdl_window);
    if(mario.sdl_glcontext == nullptr) {return nullptr;}

    if(SDL_GL_MakeCurrent(mario.sdl_window, mario.sdl_glcontext) != 0) {return nullptr;}
    SDL_GL_SetSwapInterval(vsync_enabled);

    if(glewInit() != GLEW_OK) {return nullptr;}

    IMGUI_CHECKVERSION();

    mario.imgui_context = ImGui::CreateContext();
    if(mario.imgui_context == nullptr) {return nullptr;}
    ImGui::SetCurrentContext(mario.imgui_context);

    ImGui::GetIO().IniFilename = nullptr;

    if(ImGui_ImplSDL2_InitForOpenGL(mario.sdl_window, mario.sdl_glcontext) == false) {return nullptr;}
    mario.imgui_sdl2_context = true;

    if(ImGui_ImplOpenGL3_Init("#version 120") == false) {return nullptr;}
    mario.imgui_opengl3_context = true;

    fonts::init();

    is_initialized = true;
    return new plumber{ s::move(mario) };
}

namespace pinguim::imgui::detail
{
    struct default_quit_handler_userdata { bool quit; SDL_Window* w; };

    auto default_quit_handler(SDL_Event& e, void* ud_ptr) -> void
    {
        auto& ud = *cvt::rc< default_quit_handler_userdata* >(ud_ptr);

        if (e.type == SDL_QUIT)
        { ud.quit = true; }
        else if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_CLOSE &&
            e.window.windowID == SDL_GetWindowID(ud.w)
        )
        { ud.quit = true; }
        else
        { ud.quit = false; }
    };
}

auto pinguim::imgui::plumber::handle_event() -> bool
{
    auto ud = detail::default_quit_handler_userdata{ false, sdl_window };
    handle_events(&detail::default_quit_handler, &ud);
    return ud.quit;
}

auto pinguim::imgui::plumber::handle_events(event_handler h, void* userdata) -> void
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        h(event, userdata);
    }
}

auto pinguim::imgui::plumber::begin_frame(bool rebuild_fonts) -> void
{
    if(rebuild_fonts) { ImGui_ImplOpenGL3_CreateFontsTexture(); } // TODO: Does this leak memory ?
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdl_window);
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
    SDL_GL_SwapWindow(sdl_window);
}
