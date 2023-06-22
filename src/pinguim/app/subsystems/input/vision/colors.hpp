#pragma once

#include <opencv2/core/types.hpp> // For cv::Scalar.
#include <algorithm> // For std::clamp.
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

    // Can represent center +- range or just 2 distinct colors.
    struct ColorRange
    {
        union {
            Color center;
            Color color1;
        };
        union {
            Color range;
            Color color2;
        };

        bool is_centered = false; // = true  => final_color[0], final_color[1] = min(), max().
                                  // = false => final_color[0], final_color[1] = color1, color2.
        bool using_full_range = false; // = true  => final_color = center +- range / 2.
                                       // = false => final_color = center +- range[0] / 2.

        constexpr Color min() { return using_full_range ? Color{ std::clamp(center.r - range.r / 2, 0.f, 1.f), std::clamp(center.g - range.g / 2, 0.f, 1.f), std::clamp(center.b - range.b / 2, 0.f, 1.f) } : Color{ std::clamp(center.r - range.r / 2, 0.f, 1.f), std::clamp(center.g - range.r / 2, 0.f, 1.f), std::clamp(center.b - range.r / 2, 0.f, 1.f) }; }
        constexpr Color max() { return using_full_range ? Color{ std::clamp(center.r + range.r / 2, 0.f, 1.f), std::clamp(center.g + range.g / 2, 0.f, 1.f), std::clamp(center.b + range.b / 2, 0.f, 1.f) } : Color{ std::clamp(center.r + range.r / 2, 0.f, 1.f), std::clamp(center.g + range.r / 2, 0.f, 1.f), std::clamp(center.b + range.r / 2, 0.f, 1.f) }; }
        constexpr Color first()  { return is_centered ? min() : color1; }
        constexpr Color second() { return is_centered ? max() : color2; }
    };

    struct Colors
    {
        ColorRange allyHSV  = {Color{0, 0,   0}, Color{179.f/255, 255.f/255, 255.f/255}};
        ColorRange enemyHSV = {Color{0, 0,   0}, Color{179.f/255, 255.f/255, 255.f/255}};
        ColorRange ballHSV  = {Color{0, 147, 0}, Color{23.f/255,  255.f/255, 255.f/255}};
        std::vector<ColorRange> robotHSV = {
            {Color{0, 0, 0}, Color{179.f/255, 255.f/255, 255.f/255}},
            {Color{0, 0, 0}, Color{179.f/255, 255.f/255, 255.f/255}},
            {Color{0, 0, 0}, Color{179.f/255, 255.f/255, 255.f/255}},
        };
    };
}