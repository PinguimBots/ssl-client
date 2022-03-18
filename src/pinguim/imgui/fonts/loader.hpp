#pragma once

// Forward decl.
struct ImFont;

namespace pinguim::imgui::fonts
{
    auto init() -> void;

    auto need_rebuild() -> bool;
    auto notify_rebuild_status(bool is_done) -> void;

    auto kenney(float size) -> ImFont*;
}