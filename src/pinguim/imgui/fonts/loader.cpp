#include "pinguim/imgui/fonts/loader.hpp"

#include "pinguim/imgui/fonts/kenney_game_icons.hpp"

#include <imgui.h>
#include <imgui_internal.h> // For hooking.

#include <map>

bool fonts_need_rebuild = false;

auto pinguim::imgui::fonts::need_rebuild() -> bool
{ return fonts_need_rebuild; }

auto pinguim::imgui::fonts::notify_rebuild_status(bool is_done) -> void
{ fonts_need_rebuild = is_done ? false : fonts_need_rebuild; }

auto pinguim::imgui::fonts::init() -> void
{ ImGui::GetIO().Fonts->AddFontDefault(); }

auto pinguim::imgui::fonts::kenney(float size) -> ImFont*
{
    static auto loaded            = std::map<float, ImFont*>{};
    static ImWchar const ranges[] = { KENNEY_CODEPOINT_MIN, KENNEY_CODEPOINT_MAX, 0 };

    if(loaded.contains(size)) { return loaded[size]; }

    // Fonts need to be loaded before BeginFrame() so we defer that operation
    // and return the default font in the meantime.
    // See https://github.com/ocornut/imgui/pull/3761.
    auto hook     = ImGuiContextHook();
    hook.Type     = ImGuiContextHookType_EndFramePost;
    struct callbackdata { decltype(loaded)& loaded_fonts; float size; };
    hook.Callback = []([[maybe_unused]] auto* ctx, auto* h)
    {
        auto data = reinterpret_cast<callbackdata*>(h->UserData);

        // We need to check again because if case the font was loaded more than once
        // in the same frame there will be many identical hooks making the same font.
        if(loaded.contains(data->size))
        {
            delete data;
            h->Type = ImGuiContextHookType_PendingRemoval_;
            return;
        }

        auto font = ImGui::GetIO().Fonts->AddFontDefault();

        auto cfg = ImFontConfig{};
        cfg.OversampleH = cfg.OversampleV = 2;
        cfg.FontDataOwnedByAtlas = false;
        cfg.GlyphMinAdvanceX = data->size; // Monospace.
        cfg.GlyphMaxAdvanceX = data->size; // Monospace.
        cfg.GlyphOffset.y = 3.f;
        cfg.MergeMode = true;

        ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
            get_kenney_game_icons_compressed_data(),
            static_cast<int>(get_kenney_game_icons_compressed_size()),
            data->size,
            &cfg,
            ranges
        );
        ImGui::GetIO().Fonts->Build();
        fonts_need_rebuild = true;

        data->loaded_fonts[data->size] = font;
        delete data;
        h->Type = ImGuiContextHookType_PendingRemoval_;
    };
    hook.UserData = new callbackdata{loaded, size};

    ImGui::AddContextHook(ImGui::GetCurrentContext(), &hook);

    return nullptr;
}
