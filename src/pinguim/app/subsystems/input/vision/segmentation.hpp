#pragma once

#include "contours.hpp"
#include "colors.hpp"

#include <opencv2/core.hpp>

namespace FullSeg
{
    objectsContours execute(cv::InputArray preProcessedImg, Colors colors);
}