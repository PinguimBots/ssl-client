#pragma once

#include <opencv2/imgproc.hpp>

namespace PreProcess
{
    inline void execute(cv::Mat src, cv::Mat &dst) 
    {
        cv::cvtColor(src, dst, cv::COLOR_BGR2HSV);
    }
}