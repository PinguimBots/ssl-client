#pragma once

#include "pinguim/aliases.hpp"

#include <imgui.h>


// Forward decls.
namespace cv { class Mat; }
using GLuint = pinguim::u32;
namespace pinguim::imgui { struct img; }

namespace pinguim::ImGui
{
    auto Image(
        const pinguim::imgui::img&,
        const ImVec2& uv0 = {0, 0},
        const ImVec2& uv1 = {1, 1},
        const ImVec4& tint_col = {1, 1, 1, 1},
        const ImVec4& border_col = {0, 0, 0, 0}
    ) -> void;

    // If you call it with an rvalue we'll keep it alive until the
    // end of the frame for you since we're such good guys.
    auto Image(
        pinguim::imgui::img&&,
        const ImVec2& uv0 = {0, 0},
        const ImVec2& uv1 = {1, 1},
        const ImVec4& tint_col = {1, 1, 1, 1},
        const ImVec4& border_col = {0, 0, 0, 0}
    ) -> void;
}

namespace pinguim::imgui
{
    // Just a small wrapper for displaying OpenGL stuff in ImGui.
    struct img
    {
        img() = default;

        img(cv::Mat const&, unsigned int gl_format, unsigned int gl_internalformat);
        img(GLuint handle, int cols, int rows);
        img(img&&);

        auto operator=(img&&) -> img&;

        ~img();

        // Make it non-copyable.
        img(const img&)                    = delete;
        auto operator=(const img&) -> img& = delete;

        struct gl_texture {
            GLuint handle;
            int cols;
            int rows;
        };
        gl_texture texture;
    };
}