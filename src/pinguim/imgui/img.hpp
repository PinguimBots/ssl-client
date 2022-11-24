#pragma once

#include "pinguim/aliases.hpp"

// Forward decls.
namespace cv { class Mat; }
using GLuint = pinguim::u32;
namespace pinguim::imgui { struct img; }

namespace pinguim::ImGui::detail
{
    struct V2 { float x, y; };
    struct V4 { float x, y, z, w; };
}

namespace pinguim::ImGui
{
    auto Image(
        const pinguim::imgui::img&,
        const detail::V2& uv0 = {0, 0},
        const detail::V2& uv1 = {1, 1},
        const detail::V4& tint_col = {1, 1, 1, 1},
        const detail::V4& border_col = {0, 0, 0, 0}
    ) -> void;

    // If you call it with an rvalue we'll keep it alive until the
    // end of the frame for you since we're such good guys.
    auto Image(
        pinguim::imgui::img&&,
        const detail::V2& uv0 = {0, 0},
        const detail::V2& uv1 = {1, 1},
        const detail::V4& tint_col = {1, 1, 1, 1},
        const detail::V4& border_col = {0, 0, 0, 0}
    ) -> void;
}

namespace pinguim::imgui
{
    // Just a small wrapper for displaying OpenGL stuff in ImGui.
    struct img
    {
        friend void ImGui::Image(
            const img&,
            const ImGui::detail::V2&,
            const ImGui::detail::V2&,
            const ImGui::detail::V4&,
            const ImGui::detail::V4&
        );

        img() = default;

        img(cv::Mat const&, unsigned int gl_format = 0x80E0 /* a.k.a GL_BGR */);
        img(GLuint handle, int cols, int rows);
        img(img&&);

        auto operator=(img&&) -> img&;

        ~img();

        // Make it non-copyable.
        img(const img&)                    = delete;
        auto operator=(const img&) -> img& = delete;

    private:
        struct gl_texture {
            GLuint handle;
            int cols;
            int rows;
        };
        gl_texture texture;
    };
}