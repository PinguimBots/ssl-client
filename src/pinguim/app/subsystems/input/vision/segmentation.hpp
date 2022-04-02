#pragma once

#include <opencv2/core.hpp>

namespace pinguim::app::subsystems::input
{
    const int MIN_DIRECT_AREA = 70;
    const int MAX_DIRECT_AREA = 450;

    const int MIN_BALL_AREA = 200;
    const int MAX_BALL_AREA = 500;

    const int MIN_OBJECT_AREA = 700;
    const int MAX_OBJECT_AREA = 900;

    void exec_segmentation();

    /* ------------------------ */

    void limiarization(cv::Mat preProcCurrFrame);

    /* ------------------------ */

    void extractPlayer(std::vector<std::vector<std::vector<cv::Point>>> rolesContour, std::vector<std::vector<cv::Point>> teamContour);
    void extractBall(std::vector<cv::Point> ballContour);
    void extractEnemy(std::vector<std::vector<cv::Point>> enemyContours);

    void extraction();
}