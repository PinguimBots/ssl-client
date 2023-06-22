#pragma once

#include <opencv2/core.hpp>
#include <vector>

struct objectsContours
{
    using pvec = std::vector<cv::Point>;
    std::vector<std::vector<pvec>> rolesContour;
    std::vector<pvec> teamContours;
    std::vector<pvec> ballContour;
    std::vector<pvec> enemyContours;
};