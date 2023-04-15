#include "segmentation.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <omp.h>

namespace
{
    const int MIN_DIRECT_AREA = 70;
    const int MAX_DIRECT_AREA = 450;

    const int MIN_BALL_AREA = 200;
    const int MAX_BALL_AREA = 500;

    const int MIN_OBJECT_AREA = 700;
    const int MAX_OBJECT_AREA = 900;

    void segmentTeam(cv::Mat preProcessedImg, std::vector<std::vector<cv::Point>> *teamContours, pinguim::app::subsystems::input::vision_impl::Colors colors)
    {
        std::vector<std::vector<cv::Point>> tempContours;
        cv::Mat teamThreshold;
        cv::Scalar teamMin, teamMax;

        teamMin = colors.allyHSVMin;
        teamMax = colors.allyHSVMax;

        cv::inRange(preProcessedImg, teamMin, teamMax, teamThreshold);

        cv::findContours(teamThreshold, *teamContours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

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

            playerColorMin = colors.robotHSVMin[i];
            playerColorMax = colors.robotHSVMax[i];

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

    void segmentBall(cv::Mat preProcessedImg, std::vector<cv::Point> *ballContour, pinguim::app::subsystems::input::vision_impl::Colors colors)
    {
        std::vector<std::vector<cv::Point>> contours;
        cv::Mat ballThreshold;
        cv::Scalar ballMin, ballMax;

        ballMin = colors.ballHSVMin;
        ballMax = colors.ballHSVMax;

        cv::inRange(preProcessedImg, ballMin, ballMax, ballThreshold);

        cv::findContours(ballThreshold, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < contours.size(); ++i)
        {
            cv::Moments ballMoment;
            double ballArea;

            ballMoment = cv::moments(contours[i]);

            ballArea = ballMoment.m00;

            if (ballArea >= MIN_BALL_AREA && ballArea <= MAX_BALL_AREA)
            {
                *ballContour = contours[i];

                break;
            }
        }
    }

    void segmentEnemy(cv::Mat preProcessedImg, std::vector<std::vector<cv::Point>> *enemyContours, pinguim::app::subsystems::input::vision_impl::Colors colors)
    {
        std::vector<std::vector<cv::Point>> tempContours;
        cv::Mat enemyThreshold;
        cv::Scalar enemyMin, enemyMax;

        enemyMin = colors.enemyHSVMin;
        enemyMax = colors.enemyHSVMax;

        cv::inRange(preProcessedImg, enemyMin, enemyMax, enemyThreshold);

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
    std::vector<cv::Point> ballContour;

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