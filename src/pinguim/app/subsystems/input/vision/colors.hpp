#pragma once

#include <opencv2/core/types.hpp> // For cv::Scalar.
#include <vector>

namespace pinguim::app::subsystems::input::vision_impl
{
    struct Color
    {
        constexpr Color(float _a = 0, float _b = 0, float _c = 0) : r{_a}, g{_b}, b{_c} {}

        union
        {
            float raw[3];
            struct { float r, g, b; };
        };

        operator cv::Scalar() const { return { raw[0], raw[1], raw[2] }; }
        auto to_cv_hsv()      const { return cv::Scalar(r * 180, g * 255, b * 255); }
    };

    struct Colors
    {
        Color allyHSVMin  = {0,   0,   0};
        Color allyHSVMax  = {179.f/255, 255.f/255, 255.f/255};
        Color enemyHSVMin = {0,   0,   0};
        Color enemyHSVMax = {179.f/255, 255.f/255, 255.f/255};
        Color ballHSVMin  = {0,   147, 0};
        Color ballHSVMax  = {23.f/255,  255.f/255, 255.f/255};
        std::vector<Color> robotHSVMin = { {0,         0,         0},         {0,         0,         0},         {0,         0,         0} };
        std::vector<Color> robotHSVMax = { {179.f/255, 255.f/255, 255.f/255}, {179.f/255, 255.f/255, 255.f/255}, (179.f/255, 255.f/255, 255.f/255) };
    };
}