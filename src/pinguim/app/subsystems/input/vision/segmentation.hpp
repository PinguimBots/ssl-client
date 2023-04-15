#pragma once

#include "contours.hpp"
#include "colors.hpp"

#include <opencv2/core.hpp>

namespace FullSeg
{
    objectsContours execute(cv::Mat preProcessedImg, pinguim::app::subsystems::input::vision_impl::Colors colors);
}