#include "pinguim/imgui/opencv.hpp"

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
    const pinguim::imgui::opencv::img& img,
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& tint_col,
    const ImVec4& border_col
) -> void
{
    if constexpr(pinguim::conf::opencv_with_opengl) {
        ImGui::Image(img.cv_handle, uv0, uv1, tint_col, border_col);
    } else {
        ImGui::Image(
            reinterpret_cast<void*>( static_cast<intptr_t>( img.gl.handle ) ),
            ImVec2( static_cast<float>( img.gl.size.cols ), static_cast<float>( img.gl.size.rows ) ),
            uv0,
            uv1,
            tint_col,
            border_col
        );
    }
}

pinguim::imgui::ocv::img::img(cv::InputArray mat)
{
    if constexpr(pinguim::conf::opencv_with_opengl)
    {
        // Best case scenario, this *probably* avoids copies.
        cv_handle = decltype(cv_handle){mat};
    }
    else
    {
        // If mat is a cv::Mat this *probably* avoids copying, but
        // we can't really avoid glTexImage2D making a copy :(
        cv::Mat _mat;
        cv::cvtColor(mat, _mat, cv::COLOR_BGR2RGBA);

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
        gl.size.cols = _mat.cols;
        gl.size.rows = _mat.rows;
    }
}

pinguim::imgui::ocv::img::img(img&& other)
{
    *this = std::move(other);
}

auto pinguim::imgui::ocv::img::operator=(img&& other) -> img&
{
    if constexpr(pinguim::conf::opencv_with_opengl) {
        cv_handle = std::move(other.cv_handle);
    } else {
        std::swap(gl.handle, other.gl.handle);
        std::swap(gl.size,   other.gl.size);
    }

    return *this;
}

pinguim::imgui::ocv::img::~img()
{
    if constexpr(!pinguim::conf::opencv_with_opengl) {
        glDeleteTextures(1, &gl.handle);
    }
}
