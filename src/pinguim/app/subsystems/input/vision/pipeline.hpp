#pragma once

#include "pinguim/vsss/game_info.hpp"
#include "pinguim/app/subsystems/input/vision/colors.hpp"

#include <opencv2/core/mat.hpp>

namespace Pipeline
{
    cv::Mat execute(pinguim::vsss::game_info &gi, cv::Mat const& currframe, Colors colors);
}