#pragma once

#define KENNEY_CODEPOINT_MIN 0xe000
#define KENNEY_CODEPOINT_MAX 0xe00d

#define KENNEY_ICON_EXCLAMATION "\ue000"
#define KENNEY_ICON_INFORMATION "\ue001"
#define KENNEY_ICON_WARNING "\ue002"
#define KENNEY_ICON_HBARS "\ue003"
#define KENNEY_ICON_VBARS "\ue004"
#define KENNEY_ICON_PAUSE "\ue005"
#define KENNEY_ICON_STOP "\ue006"
#define KENNEY_ICON_HSCROLL "\ue007"
#define KENNEY_ICON_VSCROLL "\ue008"
#define KENNEY_ICON_QUESTION "\ue009"
#define KENNEY_ICON_UNLOCKED "\ue00a"
#define KENNEY_ICON_LOCKED "\ue00b"
#define KENNEY_ICON_IMPORT "\ue00c"
#define KENNEY_ICON_RETURN "\ue00d"

auto get_kenney_game_icons_compressed_data() -> unsigned const*;
auto get_kenney_game_icons_compressed_size() -> unsigned;
