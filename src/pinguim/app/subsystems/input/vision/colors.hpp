#pragma once

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

class Colors
{
public:
    Colors();

    //Getters
    cv::Scalar getAllyMin();
    cv::Scalar getAllyMax();

    cv::Scalar getRobotColorMin(int robot_id);
    cv::Scalar getRobotColorMax(int robot_id);

    cv::Scalar getEnemyMin();
    cv::Scalar getEnemyMax();

    cv::Scalar getBallMin();
    cv::Scalar getBallMax();

    //Setters
    void setAllyMin(cv::Scalar hsv);
    void setAllyMax(cv::Scalar hsv);

    void setRobotColorMin(cv::Scalar hsv, int robot_id);
    void setRobotColorMax(cv::Scalar hsv, int robot_id);

    void setEnemyMin(cv::Scalar hsv);
    void setEnemyMax(cv::Scalar hsv);

    void setBallMin(cv::Scalar hsv);
    void setBallMax(cv::Scalar hsv);

private:
    cv::Scalar allyHSVMin, allyHSVMax;
    cv::Scalar enemyHSVMin, enemyHSVMax;
    cv::Scalar ballHSVMin, ballHSVMax;
    std::vector<cv::Scalar> robotMin, robotMax;
};