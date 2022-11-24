#pragma once

#include <opencv2/imgproc.hpp>

namespace PreProcess
{
    inline cv::Mat execute(cv::Mat const& src) 
    {
        cv::Mat dst;
        cv::cvtColor(src, dst, cv::COLOR_BGR2HSV);
        return dst;
    }
}
