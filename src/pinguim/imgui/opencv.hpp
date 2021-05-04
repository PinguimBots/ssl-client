#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>

#include <imgui.h>

#include <GL/glew.h>

// Forward decl
namespace pinguim::imgui::opencv { struct img; }
// Alias.
namespace pinguim::imgui { namespace ocv = opencv; }

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
        const pinguim::imgui::opencv::img&,
        const ImVec2& uv0 = ImVec2(0, 0),
        const ImVec2& uv1 = ImVec2(1, 1),
        const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
        const ImVec4& border_col = ImVec4(0, 0, 0, 0)
    ) -> void;
}

namespace pinguim::imgui::opencv
{
    // Just a small wrapper for displaying cv::Mat in ImGui.
    struct img
    {
        friend void ImGui::Image(const img&, const ImVec2&, const ImVec2&, const ImVec4&, const ImVec4&);

        // NOTE: If opencv is built WITH_OPENGL=OFF, expects mat to be BGR.
        img(cv::InputArray);
        img(img&&);
        auto operator=(img&&) -> img&;
        ~img();

        img()                              = delete;
        img(const img&)                    = delete;
        auto operator=(const img&) -> img& = delete;

    private:
        // Best case scenario is if opencv was compiled WITH_OPENGL=ON,
        // in which case we will use cv::ogl::Texture2D, otherwise we
        // do the conversion to opengl texture ourselves.
        union
        {
            cv::ogl::Texture2D cv_handle;

            struct {
                GLuint handle;
                struct {int cols; int rows;} size;
            } gl;
        };
    };
}
