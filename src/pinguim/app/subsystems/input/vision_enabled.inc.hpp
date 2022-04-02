#pragma once

#include "pinguim/app/subsystems/types.hpp"
#include "pinguim/imgui/img.hpp"
#include "pinguim/aliases.hpp"
#include "pinguim/chrono.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp> // For VideoCapture.

#include <optional>
#include <string>

namespace pinguim::app::subsystems::input
{
    struct vision : public pinguim::app::input_subsystem
    {
        vision();

        auto update_gameinfo(game_info&, float delta_seconds) -> bool override;

    private:

        auto init_file_capture(std::string path) -> bool;
        auto init_camera_capture() -> void;

        // Uses settings from file_capture_config and
        // overrides currframe with the new frame.
        //
        // Also skips frames if necessary.
        //
        // Assumes video.isOpened(), and file_capture_config.hasValue().
        auto read_frame_from_file_capture() -> u16;

        enum class capture_type_enum {none, camera, file};

        capture_type_enum capture_type = capture_type_enum::none;

        struct file_capture_config_struct
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

        struct camera_capture_config_struct
        {
            u32 index;
        };

        std::optional<file_capture_config_struct> file_capture_config;
        std::optional<camera_capture_config_struct> camera_capture_config;

        //pipeline camera_pipeline;

        cv::VideoCapture video;
        cv::Mat currframe;
    };
}
