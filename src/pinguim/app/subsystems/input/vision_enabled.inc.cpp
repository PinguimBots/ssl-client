#include "pinguim/app/subsystems/input/vision.hpp"

#include "pinguim/app/subsystems/registrar.hpp"

#include "pinguim/app/subsystems/input/vision/pipeline.hpp"

#include "pinguim/imgui/fonts/kenney_game_icons.hpp"
#include "pinguim/imgui/fonts/loader.hpp"

#include "pinguim/imgui/widgets/toggle_button.hpp"
#include "pinguim/imgui/img.hpp"
#include "pinguim/imgui/fmt.hpp"

#include "pinguim/dont_forget.hpp"
#include "pinguim/renum.hpp"
#include "pinguim/cvt.hpp"

#include <ImGuiFileDialog.h>

#include <opencv2/videoio.hpp> // For cv::VideoCaptureProperties.

#include <fmt/core.h>

PINGUIM_APP_REGISTER_INPUT_SUBSYSTEM(pinguim::app::subsystems::input::vision, "Vision");

namespace pinguim::app::subsystems::input
{
    vision::vision()
        : video{}
    {}

    auto vision::update_gameinfo([[maybe_unused]] game_info& gi, [[maybe_unused]] float delta_seconds) -> bool
    {
        namespace ImGui = ::ImGui;

        auto kenney = pb::imgui::fonts::kenney(18);

        ImGui::PushFont(kenney);
        PINGUIM_DONT_FORGET( ImGui::PopFont() );

        ImGui::SetNextWindowSize({0, 0});
        ImGui::Begin("[INPUT] Vision");
        PINGUIM_DONT_FORGET( ImGui::End() );

        bool first_frame_of_new_capture_type = false;

        // Capture type dropdown.
        auto selected_str = std::string{ pinguim::renum::unqualified_value_name(capture_type) };
        ImGui::SetNextItemWidth(100);
        if(ImGui::BeginCombo("Capture Type", selected_str.c_str()))
        {
            for(const auto& [enum_value, view] : pinguim::renum::reflect<capture_type_enum>::enum_iterator())
            {
                if( ImGui::Selectable(std::string{ view.unqualified() }.c_str(), enum_value == capture_type) )
                {
                    capture_type = enum_value;
                    file_capture_config = std::nullopt;
                    camera_capture_config = std::nullopt;
                    video.release();
                    currframe.release();
                    first_frame_of_new_capture_type = true;
                }
            }
            ImGui::EndCombo();
        }

        // File dialog button.
        if(capture_type == capture_type_enum::file)
        {
            // "Select File" button.
            ImGui::SameLine();
            if(ImGui::Button("Select File"))
            { ImGuiFileDialog::Instance()->OpenDialog("CaptureFileDialog", "Select File", ".*", "."); }

            // Draw the dialog proper.
            std::string newfile = "";
            if (ImGuiFileDialog::Instance()->Display("CaptureFileDialog", ImGuiWindowFlags_NoCollapse, {500, 350}))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                { newfile = ImGuiFileDialog::Instance()->GetSelection().begin()->second; }

                ImGuiFileDialog::Instance()->Close();
            }

            // Try opening the file.
            if(!newfile.empty())
            { init_file_capture(newfile); }
        }

        if(capture_type == capture_type_enum::camera)
        {
            if(first_frame_of_new_capture_type)
            {
                camera_capture_config = {0};
                init_camera_capture();
            }
            auto& cfg = camera_capture_config.value();

            auto val_int = cvt::to<int> * cfg.index;
            auto value_changed = ImGui::InputInt("Camera Index", &val_int);
            if(value_changed) {
                cfg.index = val_int >= 0 ? cvt::to<u32> * val_int : 0;
                init_camera_capture();
            }
        }

        if(!video.isOpened()) { return false; }

        if(capture_type == capture_type_enum::file)
        {
            auto& cfg = file_capture_config.value();

            pb::ImGui::ToggleButton(KENNEY_ICON_RETURN, &cfg.replay);

            // Automate boilerplate
            #define PINGUIM_SPEED_CHECKBOX(speed, label)                                             \
                if( pb::ImGui::ToggleButton(label, cfg.playback_speed_modifier == speed, {40, 20}) ) \
                    cfg.playback_speed_modifier = cfg.playback_speed_modifier != speed ? speed : 1;

            PINGUIM_SPEED_CHECKBOX(1/32.f, "1/32X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(1/16.f, "1/16X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(1/8.f, "1/8X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(1/4.f, "1/4X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(1/2.f, "1/2X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(1.f, "1X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(2.f, "2X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(4.f, "4X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(8.f, "8X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(16.f, "16X"); ImGui::SameLine();
            PINGUIM_SPEED_CHECKBOX(32.f, "32X");

            #undef PINGUIM_SPEED_CHECKBOX

            auto target_fps = 1.f / cfg.target_frametime_seconds;
            ImGui::SetNextItemWidth(cvt::to<float> * video.get(cv::CAP_PROP_FRAME_WIDTH) - 40.f - ImGui::GetStyle().FramePadding.x * 2);
            ImGui::SliderFloat(
                "##fps slider",
                &target_fps,
                1.f,
                100.f,
                "%.3f FPS"
            );
            ImGui::SameLine();
            if(ImGui::Button(KENNEY_ICON_RETURN "##restore fps button", {40, 20}))
            { target_fps = cfg.original_fps; }

            cfg.target_frametime_seconds = 1.f / target_fps;

            read_frame_from_file_capture();

            pb::ImGui::Image({currframe});

            // Video timeline.
            ImGui::SetNextItemWidth(cvt::to<float> * video.get(cv::CAP_PROP_FRAME_WIDTH));
            auto video_timeline = cvt::to<float> * video.get(cv::CAP_PROP_POS_MSEC) / 1000;
            if(ImGui::SliderFloat(
                "##video timeline",
                &video_timeline,
                0.f, cfg.video_len_seconds,
                cfg.is_discovering ? "[Discovering] %.2f S" : "%.2f S"
            )) { video.set(cv::CAP_PROP_POS_MSEC, cvt::to<double> * video_timeline * 1000); }
        }

        if(capture_type == capture_type_enum::camera)
        {
            auto& cfg = camera_capture_config.value();

            video.read(currframe);

            pb::ImGui::Image({currframe});
        }

        if(capture_type == capture_type_enum::none) { return false; }

        // currframe: cv::Mat
        
        // Pipeline::execute(gi, currframe, colors);

        ImGui::Begin("Pipeline");

        // pb::ImGui::Image({currframe});

        ImGui::End();

        return true;
    }

    auto vision::init_file_capture(std::string path) -> bool
    {
        video.open(path);

        if(!video.isOpened()) { return false; }

        const auto fps = cvt::to<float> * video.get(cv::CAP_PROP_FPS);

        file_capture_config = file_capture_config_struct{
            .video_len_seconds = 0, // There are problems with trying to find out
                                    // a video length using OpenCV, we'll discover
                                    // the true length as we go, which means the first
                                    // playback's timeline will not be fully scrubbable.
            .original_fps = fps,

            .replay = true,
            .is_discovering = true,
            .playback_speed_modifier = 1,

            .target_frametime_seconds = 1 / fps,
            .loose_frametime = 0,
            .frametime_pacer = stopwatch<>{}
        };

        currframe = cv::Mat::zeros(
            {cvt::toe * video.get(cv::CAP_PROP_FRAME_WIDTH), cvt::toe * video.get(cv::CAP_PROP_FRAME_HEIGHT)},
            CV_8U
        );

        return true;
    }

    auto vision::init_camera_capture() -> void
    {
        video.open(cvt::to<i32> * camera_capture_config.value().index);
    }

    auto vision::read_frame_from_file_capture() -> u16
    {
        auto& cfg = file_capture_config.value();

        // We create a temporary to not override currframe with an
        // empty Mat if we failed reading.
        auto temp_mat = cv::Mat{};

        auto elapsed_dt = cfg.frametime_pacer.since_click() + cfg.loose_frametime;
        auto target_capture_dt = cfg.target_frametime_seconds / cfg.playback_speed_modifier;

        auto frames_read = 0_u16;

        while(elapsed_dt >= target_capture_dt)
        {
            elapsed_dt -= target_capture_dt;

            auto has_frame = video.read(temp_mat);

            if(!has_frame && cfg.replay)
            {
                cfg.is_discovering = false;
                video.set(cv::CAP_PROP_POS_AVI_RATIO, 0);
                has_frame = video.read(temp_mat);
            }
            else if(!has_frame && !cfg.replay)
            {
                cfg.is_discovering = false;
                cfg.frametime_pacer.restart();
                elapsed_dt = 0.f;
                break;
            }

            if(has_frame)
            {
                ++frames_read;

                cfg.video_len_seconds = std::max(
                    cfg.video_len_seconds,
                    cvt::to<float> * video.get(cv::CAP_PROP_POS_MSEC) / 1000.f
                );

                currframe = std::move(temp_mat);
            }
        }

        if(frames_read)
        {
            cfg.loose_frametime = elapsed_dt;
            cfg.frametime_pacer.click();
        }

        return frames_read;
    }
}
