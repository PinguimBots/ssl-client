#pragma once

#include <imgui.h>

#include <GL/glew.h>

// Forward decl.
namespace pinguim::imgui { struct img; }

namespace ImGui
{
    auto Image(
        const pinguim::imgui::img&,
        const ImVec2& uv0 = ImVec2(0, 0),
        const ImVec2& uv1 = ImVec2(1, 1),
        const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
        const ImVec4& border_col = ImVec4(0, 0, 0, 0)
    ) -> void;
}

namespace pinguim::imgui
{
    // Just a small wrapper for displaying image-like things in ImGui (backend opengl).
    struct img
    {
        friend void ImGui::Image(const img&, const ImVec2&, const ImVec2&, const ImVec4&, const ImVec4&);

        img(img&&);
        auto operator=(img&&) -> img&;
        ~img();

        // Non copyable.
        img()                              = delete;
        img(const img&)                    = delete;
        auto operator=(const img&) -> img& = delete;

    private:
        GLuint handle;
        struct {int cols; int rows;} size;
    };
}
