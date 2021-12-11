#pragma once

#include <variant>

#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>

#include <imgui.h>

#include <GL/glew.h>

// Forward decl
namespace pinguim::imgui { struct img; }

namespace ImGui
{
    auto Image(
        const cv::ogl::Texture2D&,
        const ImVec2& uv0 = ImVec2(0, 0),
        const ImVec2& uv1 = ImVec2(1, 1),
        const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
        const ImVec4& border_col = ImVec4(0, 0, 0, 0)
    ) -> void;

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
    // Just a small wrapper for displaying OpenGL stuff in ImGui.
    struct img
    {
        friend void ImGui::Image(const img&, const ImVec2&, const ImVec2&, const ImVec4&, const ImVec4&);

        // NOTE: If opencv is built WITH_OPENGL=OFF, expects mat to be BGR.
        img(cv::InputArray);
        img(GLuint handle, int cols, int rows);
        img(img&&);
        auto operator=(img&&) -> img&;
        ~img();

        // Make it non-copyable and non-moveable.
        img()                              = delete;
        img(const img&)                    = delete;
        auto operator=(const img&) -> img& = delete;

    private:
        // Best case scenario is if opencv was compiled WITH_OPENGL=ON,
        // in which case we will use cv::ogl::Texture2D, otherwise we
        // do the conversion to opengl texture ourselves.
        struct gl_texture {
            GLuint handle;
            int cols;
            int rows;
        };
        std::variant<gl_texture, cv::ogl::Texture2D> texture;
    };
}