#include "pipeline.hpp"
#include "preprocess.hpp"
#include "segmentation.hpp"
#include "extraction.hpp"
#include "contours.hpp"

#include "pinguim/vsss/game_info.hpp"

#include "pinguim/imgui/img.hpp"
#include <imgui.h>

#include <opencv2/core.hpp>
#include <vector>

cv::Mat Pipeline::execute(pinguim::vsss::game_info& gi, cv::Mat const& currframe, pinguim::app::subsystems::input::vision_impl::Colors colors)
{
    auto frame = PreProcess::execute(currframe);

    objectsContours contours = FullSeg::execute(frame, colors);

    auto frame2 = frame.clone();
    cv::cvtColor(frame, frame2, cv::COLOR_HSV2BGR);
    if(contours.teamContours.size())  cv::drawContours(frame2, contours.teamContours, -1,  {255, 0, 0}, 3, cv::LINE_AA);
    if(contours.enemyContours.size()) cv::drawContours(frame2, contours.enemyContours, -1, {0, 0, 255}, 3, cv::LINE_AA);
    // cv::drawContour(frame2, contours.ballContour, 1, {0, 0, 255}, 3, cv::LINE_AA);

    ImGui::Begin("Contours");
    pb::ImGui::Image({frame2});
    ImGui::End();

    Extraction::execute(gi, contours);

    return frame;
}