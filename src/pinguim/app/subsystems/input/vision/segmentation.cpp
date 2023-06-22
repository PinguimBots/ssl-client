#include "segmentation.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <omp.h>

#include "pinguim/imgui/img.hpp"
#include <imgui.h>
#include <GL/glew.h>

namespace
{
    int MIN_DIRECT_AREA = 70;
    int MAX_DIRECT_AREA = 450;
    int MIN_BALL_AREA = 200;
    int MAX_BALL_AREA = 500;
    int MIN_OBJECT_AREA = 700;
    int MAX_OBJECT_AREA = 900;

    void segmentTeam(cv::Mat preProcessedImg, std::vector<std::vector<cv::Point>> *teamContours, pinguim::app::subsystems::input::vision_impl::Colors colors)
    {
        ImGui::Begin("Segmentation params");
        ImGui::SliderInt("MIN_DIRECT_AREA", &MIN_DIRECT_AREA, 0, 1000);
        ImGui::SliderInt("MAX_DIRECT_AREA", &MAX_DIRECT_AREA, 0, 1000);
        ImGui::SliderInt("MIN_BALL_AREA",   &MIN_BALL_AREA,   0, 1000);
        ImGui::SliderInt("MAX_BALL_AREA",   &MAX_BALL_AREA,   0, 1000);
        ImGui::SliderInt("MIN_OBJECT_AREA", &MIN_OBJECT_AREA, 0, 1000);
        ImGui::SliderInt("MAX_OBJECT_AREA", &MAX_OBJECT_AREA, 0, 1000);
        ImGui::End();

        std::vector<std::vector<cv::Point>> tempContours;
        cv::Mat teamThreshold;
        cv::Scalar teamMin, teamMax;

        teamMin = colors.allyHSV.first().to_cv_hsv();
        teamMax = colors.allyHSV.second().to_cv_hsv();

        cv::inRange(preProcessedImg, teamMin, teamMax, teamThreshold);
        ImGui::Begin("Team threshold");
        pb::ImGui::Image({teamThreshold, GL_LUMINANCE, GL_LUMINANCE});
        ImGui::End();

        cv::findContours(teamThreshold, tempContours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < tempContours.size(); ++i)
        {
            cv::Moments teamMoments;
            double teamArea;

            teamMoments = cv::moments(tempContours[i]);

            teamArea = teamMoments.m00;

            if (teamArea >= MIN_OBJECT_AREA && teamArea <= MAX_OBJECT_AREA)
            {
                teamContours->push_back(tempContours[i]);
            }
        }
    }

    void segmentPlayers(cv::Mat preProcessedImg, std::vector<std::vector<std::vector<cv::Point>>> *allPlayersContours, pinguim::app::subsystems::input::vision_impl::Colors colors)
    {
        // #pragma omp parallel for
        for (int i = 0; i < 3; ++i)
        {
            cv::Scalar playerColorMin, playerColorMax;
            cv::Mat thresholdPlayer;
            std::vector<std::vector<cv::Point>> playersContours, playerContours;

            playerColorMin = colors.robotHSV[i].first().to_cv_hsv();
            playerColorMax = colors.robotHSV[i].second().to_cv_hsv();

            cv::inRange(preProcessedImg, playerColorMin, playerColorMax, thresholdPlayer);

            cv::findContours(thresholdPlayer, playerContours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

            if (playerContours.size() >= 1)
            {
                for (auto j = 0ul; j < playerContours.size(); j++)
                {

                    cv::Moments playerMoment;
                    double playerArea;

                    playerMoment = cv::moments(playerContours[j]);

                    playerArea = playerMoment.m00;

                    if (playerArea >= MIN_DIRECT_AREA && playerArea <= MAX_DIRECT_AREA)
                    {
                        playersContours.push_back(playerContours[j]);
                    }
                }
            }

            (*allPlayersContours)[i] = playersContours;
        }
    }

    void segmentBall(cv::Mat preProcessedImg, std::vector<std::vector<cv::Point>> *ballContour, pinguim::app::subsystems::input::vision_impl::Colors colors)
    {
        std::vector<std::vector<cv::Point>> contours;
        cv::Mat ballThreshold;
        cv::Scalar ballMin, ballMax;

        ballMin = colors.ballHSV.first().to_cv_hsv();
        ballMax = colors.ballHSV.second().to_cv_hsv();

        cv::inRange(preProcessedImg, ballMin, ballMax, ballThreshold);
        ImGui::Begin("Ball threshold");
        pb::ImGui::Image({ballThreshold, GL_LUMINANCE, GL_LUMINANCE});
        ImGui::End();

        cv::findContours(ballThreshold, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < contours.size(); ++i)
        {
            cv::Moments ballMoment;
            double ballArea;

            ballMoment = cv::moments(contours[i]);

            ballArea = ballMoment.m00;

            if (ballArea >= MIN_BALL_AREA && ballArea <= MAX_BALL_AREA)
            {
                ballContour->push_back(contours[i]);
            }
        }
    }

    void segmentEnemy(cv::Mat preProcessedImg, std::vector<std::vector<cv::Point>> *enemyContours, pinguim::app::subsystems::input::vision_impl::Colors colors)
    {
        std::vector<std::vector<cv::Point>> tempContours;
        cv::Mat enemyThreshold;
        cv::Scalar enemyMin, enemyMax;

        enemyMin = colors.enemyHSV.first().to_cv_hsv();
        enemyMax = colors.enemyHSV.second().to_cv_hsv();

        cv::inRange(preProcessedImg, enemyMin, enemyMax, enemyThreshold);
        ImGui::Begin("Enemy threshold");
        pb::ImGui::Image({enemyThreshold, GL_LUMINANCE, GL_LUMINANCE});
        ImGui::End();

        cv::findContours(enemyThreshold, tempContours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < tempContours.size(); ++i)
        {
            cv::Moments enemyMoments;
            double enemyArea;

            enemyMoments = cv::moments(tempContours[i]);

            enemyArea = enemyMoments.m00;

            if (enemyArea >= MIN_OBJECT_AREA && enemyArea <= MAX_OBJECT_AREA)
            {
                enemyContours->push_back(tempContours[i]);
            }
        }
    }

}

objectsContours FullSeg::execute(cv::Mat preProcessedImg, pinguim::app::subsystems::input::vision_impl::Colors colors)
{
    std::vector<std::vector<std::vector<cv::Point>>> allPlayersContours(3, std::vector<std::vector<cv::Point>>());
    std::vector<std::vector<cv::Point>> teamContours, enemyContours;
    std::vector<std::vector<cv::Point>> ballContour;

    // #pragma omp task
    segmentBall(preProcessedImg, &ballContour, colors);

    // #pragma omp task
    segmentTeam(preProcessedImg, &teamContours, colors);

    // #pragma omp task
    segmentPlayers(preProcessedImg, &allPlayersContours, colors);

    // #pragma omp task
    segmentEnemy(preProcessedImg, &enemyContours, colors);

    // #pragma omp taskwait
    objectsContours oc;
    oc.rolesContour = allPlayersContours;
    oc.teamContours = teamContours;
    oc.ballContour  = ballContour;
    oc.enemyContours = enemyContours;

    return oc;
}