#include "pinguim/imgui/img.hpp"

#include <utility> // For std::swap.

auto ImGui::Image(
    const pinguim::imgui::img& img,
    const ImVec2& uv0,
    const ImVec2& uv1,
    const ImVec4& tint_col,
    const ImVec4& border_col
) -> void
{
    ImGui::Image(
        reinterpret_cast<void*>( static_cast<intptr_t>( img.handle ) ),
        ImVec2( static_cast<float>( img.size.cols ), static_cast<float>( img.size.rows ) ),
        uv0,
        uv1,
        tint_col,
        border_col
    );
}

pinguim::imgui::img::img(img&& other) { *this = std::move(other); }

auto pinguim::imgui::img::operator=(img&& other) -> img&
{
    std::swap(handle, other.handle);
    std::swap(size,   other.size);
    return *this;
}

pinguim::imgui::img::~img() { glDeleteTextures(1, &handle); }
