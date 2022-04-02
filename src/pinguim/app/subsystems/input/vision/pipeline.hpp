#pragma once

#include "pinguim/vsss/game_info.hpp"

#include <opencv2/core/mat.hpp>

namespace pinguim::app::subsystems::input
{
    struct vision_pipeline
    {
        void execute(game_info &gi, cv::Mat currframe);
    };
}