#include "pinguim/imgui/img.hpp"

#include "pinguim/conf.hpp"

#include <utility> // For std::swap.

auto ImGui::Image(
    const cv::ogl::Texture2D& tex,
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& tint_col,
    const ImVec4& border_col
) -> void
{
    ImGui::Image( reinterpret_cast<void*>( static_cast<intptr_t>( tex.texId() ) ),
        ImVec2( static_cast<float>( tex.cols() ), static_cast<float>( tex.rows() ) ),
        uv0,
        uv1,
        tint_col,
        border_col
    );
}

auto ImGui::Image(
    const pinguim::imgui::img& img,
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& tint_col,
    const ImVec4& border_col
) -> void
{
    if (std::holds_alternative<cv::ogl::Texture2D>(img.texture)) {
        ImGui::Image(std::get<cv::ogl::Texture2D>(img.texture), uv0, uv1, tint_col, border_col);
    } else {
        const auto& imgdata = std::get<pinguim::imgui::img::gl_texture>(img.texture);
        ImGui::Image(
            reinterpret_cast<void*>( static_cast<intptr_t>( imgdata.handle ) ),
            ImVec2( static_cast<float>( imgdata.cols ), static_cast<float>( imgdata.rows ) ),
            uv0,
            uv1,
            tint_col,
            border_col
        );
    }
}

pinguim::imgui::img::img(cv::InputArray mat)
{
    if constexpr(pinguim::conf::opencv_with_opengl)
    {
        // Best case scenario, this *probably* avoids copies.
        texture = cv::ogl::Texture2D{mat};
    }
    else
    {
        // If mat is a cv::Mat this *probably* avoids copying, but
        // we can't really avoid glTexImage2D making a copy :(
        cv::Mat _mat;
        cv::cvtColor(mat, _mat, cv::COLOR_BGR2RGBA);

        auto gl = gl_texture{};

        glGenTextures(1, &gl.handle);
        glBindTexture(GL_TEXTURE_2D, gl.handle);
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
        gl.cols = _mat.cols;
        gl.rows = _mat.rows;

        texture = std::move(gl);
    }
}

pinguim::imgui::img::img(GLuint handle, int cols, int rows)
    : texture{ gl_texture{handle, cols, rows} }
{}

pinguim::imgui::img::img(img&& other)
{  *this = std::move(other); }

auto pinguim::imgui::img::operator=(img&& other) -> img&
{
    std::swap( texture, other.texture );
    return *this;
}

pinguim::imgui::img::~img()
{
    if (std::holds_alternative<gl_texture>(texture)) {
        const auto& gl = std::get<gl_texture>(texture);
        glDeleteTextures(1, &gl.handle);
    }
}