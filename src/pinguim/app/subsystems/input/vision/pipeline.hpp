#pragma once

#include "pinguim/vsss/game_info.hpp"
#include "pinguim/app/subsystems/input/vision/colors.hpp"

#include <opencv2/core/mat.hpp>

namespace Pipeline
{
<<<<<<< HEAD
    void execute(pinguim::vsss::game_info &gi, cv::InputArray currframe, Colors colors);
=======
    void execute(pinguim::vsss::game_info &gi, cv::Mat& currframe, Colors colors);
>>>>>>> e74e739 (refator(input/vision): reorganize main func)
}