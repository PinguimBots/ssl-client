#include "pinguim/imgui/img.hpp"

#include "pinguim/cvt.hpp"
#include "pinguim/conf.hpp"

#if defined(PINGUIM_CONF_OPENCV_SUPPORT)
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp> // For color conversion stuff.
#endif

#include <utility> // For std::swap.

// To easily interop between imgui types and our detail:: placeholders.
#define IM_VEC2_CLASS_EXTRA ImVec2(const pinguim::ImGui::detail::V2& v) : x{v.x}, y{v.y} {}
#define IM_VEC4_CLASS_EXTRA ImVec4(const pinguim::ImGui::detail::V4& v) : x{v.x}, y{v.y}, z{v.z}, w{v.w} {}
#include <imgui.h>
#include <imgui_internal.h> // For hooking.

#include <GL/glew.h>

auto pinguim::ImGui::Image(
    const pinguim::imgui::img& img,
    const detail::V2& uv0,
    const detail::V2& uv1,
    const detail::V4& tint_col,
    const detail::V4& border_col
) -> void
{
    ::ImGui::Image(
        reinterpret_cast<void*>( static_cast<intptr_t>( img.texture.handle ) ),
        ImVec2( static_cast<float>( img.texture.cols ), static_cast<float>( img.texture.rows ) ),
        uv0,
        uv1,
        tint_col,
        border_col
    );
}

auto pinguim::ImGui::Image(
    pinguim::imgui::img&& img,
    const detail::V2& uv0,
    const detail::V2& uv1,
    const detail::V4& tint_col,
    const detail::V4& border_col
) -> void
{
    auto hook     = ImGuiContextHook();
    hook.Type     = ImGuiContextHookType_NewFramePre;
    hook.Callback = []([[maybe_unused]] auto* ctx, auto* h)
    {
        delete cvt::rc<pinguim::imgui::img*>(h->UserData);
        h->Type = ImGuiContextHookType_PendingRemoval_;
    };
    hook.UserData = new pinguim::imgui::img(std::move(img));

    ImGui::Image(
        *cvt::rc<pinguim::imgui::img*>(hook.UserData),
        uv0,
        uv1,
        tint_col,
        border_col
    );

    ::ImGui::AddContextHook(::ImGui::GetCurrentContext(), &hook);
}

#if defined(PINGUIM_CONF_OPENCV_SUPPORT)
pinguim::imgui::img::img(const cv::_InputArray& mat)
    : texture{}
{
    // If mat is a cv::Mat this *probably* avoids copying, but
    // we can't really avoid glTexImage2D making a copy :(
    cv::Mat _mat;
    cv::cvtColor(mat, _mat, cv::COLOR_BGR2RGBA);

    glGenTextures(1, &texture.handle);
    glBindTexture(GL_TEXTURE_2D, texture.handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _mat.cols,
        _mat.rows,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _mat.data
    );
    texture.cols = _mat.cols;
    texture.rows = _mat.rows;
}
#else
pinguim::imgui::img::img(const cv::_InputArray&) : img{0, 0, 0} {}
#endif

pinguim::imgui::img::img(GLuint handle, int cols, int rows)
    : texture{ handle, cols, rows }
{}

pinguim::imgui::img::img(img&& other)
{  *this = std::move(other); }

auto pinguim::imgui::img::operator=(img&& other) -> img&
{
    std::swap( texture, other.texture );
    return *this;
}

pinguim::imgui::img::~img()
{ glDeleteTextures(1, &texture.handle); }