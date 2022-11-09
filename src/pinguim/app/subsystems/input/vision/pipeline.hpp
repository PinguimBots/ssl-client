#pragma once

#include "pinguim/vsss/game_info.hpp"
#include "pinguim/app/subsystems/input/vision/colors.hpp"

#include <opencv2/core/mat.hpp>

namespace Pipeline
{
    void execute(pinguim::vsss::game_info &gi, cv::InputArray currframe, Colors colors);
}