#pragma once

#include <opencv2/imgproc.hpp>

namespace PreProcess
{
<<<<<<< HEAD
    inline void execute(cv::InputArray src, cv::OutputArray dst) 
=======
    inline cv::Mat execute(cv::Mat src) 
>>>>>>> e74e739 (refator(input/vision): reorganize main func)
    {
        auto dst = src.clone();
        cv::cvtColor(src, dst, cv::COLOR_BGR2HSV);
        return dst;
    }
}