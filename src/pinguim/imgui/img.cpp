#include "pinguim/imgui/img.hpp"

#include "pinguim/cvt.hpp"
#include "pinguim/conf.hpp"

#if defined(PINGUIM_CONF_OPENCV_SUPPORT)
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp> // For color conversion stuff.
#endif

#include <utility> // For std::swap.

#include <imgui.h>
#include <imgui_internal.h> // For hooking.

#include <GL/glew.h>

auto pinguim::ImGui::Image(
    const pinguim::imgui::img& img,
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& tint_col,
    const ImVec4& border_col
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
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& tint_col,
    const ImVec4& border_col
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
namespace {

}
pinguim::imgui::img::img(cv::Mat const& mat, unsigned int gl_format)
    : texture{}
{
    auto type = GL_UNSIGNED_BYTE; // Missing: GL_HALF_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.

    switch(mat.type())
    {
        // redundant: case CV_8UC1: break;
        // redundant: case CV_8UC2: break;
        // redundant: case CV_8UC3: break;
        // redundant: case CV_8UC4: break;
        case CV_8SC1: [[fallthrough]];
        case CV_8SC2: [[fallthrough]];
        case CV_8SC3: [[fallthrough]];
        case CV_8SC4: type = GL_BYTE; break;
        case CV_16UC1: [[fallthrough]];
        case CV_16UC2: [[fallthrough]];
        case CV_16UC3: [[fallthrough]];
        case CV_16UC4: type = GL_UNSIGNED_SHORT; break;
        case CV_16SC1: [[fallthrough]];
        case CV_16SC2: [[fallthrough]];
        case CV_16SC3: [[fallthrough]];
        case CV_16SC4: type = GL_SHORT; break;
        // doesnt_exist: case CV_32UC1: [[fallthrough]]
        // doesnt_exist: case CV_32UC2: [[fallthrough]]
        // doesnt_exist: case CV_32UC3: [[fallthrough]]
        // doesnt_exist: case CV_32UC4: type = GL_UNSIGNED_INT; break;
        case CV_32SC1: [[fallthrough]];
        case CV_32SC2: [[fallthrough]];
        case CV_32SC3: [[fallthrough]];
        case CV_32SC4: type = GL_INT; break;
        // case CV_32FC2: break;
        // case CV_32FC3: break;
        // case CV_32FC4: break;
        // case CV_64FC2: break;
        // case CV_64FC3: break;
        // case CV_64FC4: break;
    }

    glGenTextures(1, &texture.handle);
    glBindTexture(GL_TEXTURE_2D, texture.handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        mat.cols,
        mat.rows,
        0,
        gl_format,
        type,
        mat.isContinuous() ? mat.ptr() : mat.clone().ptr()
    );
    texture.cols = mat.cols;
    texture.rows = mat.rows;
}
#else
pinguim::imgui::img::img(cv::Mat const&) : img{0, 0, 0} {}
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