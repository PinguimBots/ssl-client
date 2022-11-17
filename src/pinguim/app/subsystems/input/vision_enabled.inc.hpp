#pragma once

#include "pinguim/app/subsystems/input/vision/colors.hpp"
#include "pinguim/app/subsystems/types.hpp"
#include "pinguim/imgui/img.hpp"
#include "pinguim/aliases.hpp"
#include "pinguim/chrono.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp> // For VideoCapture.

#include <variant>
#include <string>

namespace pinguim::app::subsystems::input
{
    struct vision : public pinguim::app::input_subsystem
    {
        vision();

        auto update_gameinfo(game_info&, float delta_seconds) -> bool override;

    private:

        auto draw_capture_type_dropdown(bool& is_first_frame) -> void;

        auto config_camera_capture(bool) -> void;
        auto fetch_camera_frame(bool)    -> void;
        auto config_file_capture(bool)   -> void;
        auto fetch_file_frame(bool)      -> void;

        // Uses settings from file_capture_config and
        // overrides currframe with the new frame.
        //
        // Also skips frames if necessary.
        //
        // Assumes video.isOpened(), and config is file_capture.
        auto read_frame_from_file_capture() -> u16;

        // Enum mostrado no dropdown
        enum class capture_type_enum {none, camera, file};
        capture_type_enum capture_type = capture_type_enum::none;

        struct file_capture
        {
            float video_len_seconds;
            float original_fps;

            // Configurables.
            bool replay;
            bool is_discovering;
            float playback_speed_modifier;


            float target_frametime_seconds;
            // We need to keep track of this to carry over any loose
            // time when skipping frames in order not to desync over
            // time.
            float loose_frametime;
            stopwatch<> frametime_pacer;
        };

        struct camera_capture
        {
            u32 index;
        };

        std::variant<
            std::monostate, // Quando não inicializado.
            file_capture,
            camera_capture
        > config;

        //pipeline camera_pipeline;

        // A fonte dos frames, pode ser uma camera, arquivo, foto, etc.
        cv::VideoCapture video;
        cv::Mat currframe;

        Colors colors = Colors();
    };
}
