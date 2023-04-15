#include "pinguim/vsss/game_info.hpp"
#include "extraction.hpp"
#include "contours.hpp"

#include <math.h>
#include <iostream>
#include <opencv2/imgproc.hpp>

namespace
{
    void extractPlayer(
        pinguim::vsss::game_info& gi,
        std::vector<std::vector<std::vector<cv::Point>>> rolesContour,
        std::vector<std::vector<cv::Point>> teamContour)
    {
        std::vector<int> alreadyUsed;

        alreadyUsed = std::vector<int>(teamContour.size(), 0);

        // #pragma omp parallel for shared(alreadyUsed)
        for (int i = 0; i < rolesContour.size(); ++i)
        {

            bool found = false;

            auto& r = gi.allied_team[i];

            std::vector<std::vector<cv::Point>> roleContour = rolesContour[i];

            // std::cout << "Num roles for robot [" << i << "] = " << roleContour.size() << std::endl;

            for (int k = 0; k < roleContour.size(); ++k)
            {
                cv::Moments roleMoments;
                double roleArea;

                roleMoments = cv::moments(roleContour[k]);

                roleArea = roleMoments.m00;

                int playerX = static_cast<int>(roleMoments.m10 / roleArea);
                int playerY = static_cast<int>(roleMoments.m01 / roleArea);

                for (int j = 0; j < teamContour.size(); ++j)
                {

                    if (!alreadyUsed[j])
                    {
                        cv::Moments teamMoments = cv::moments(teamContour[j]);
                        double teamArea = teamMoments.m00;

                        // std::cout << "Team area = " << teamArea << std::endl;

                        int teamX = static_cast<int>(teamMoments.m10 / teamArea);
                        int teamY = static_cast<int>(teamMoments.m01 / teamArea);

                        double xDif, yDif, dist;

                        xDif = playerX - teamX;
                        yDif = playerY - teamY;

                        dist = std::sqrt(std::pow(xDif, 2.0) + std::pow(yDif, 2.0));

                        //std::cout << "Role [" << k << "] | Time [" << j << "] | Distancia = " << dist << std::endl;

                        if (dist > 11.0 && dist < 16.0)
                        {
                            // #pragma omp critical
                            alreadyUsed[j] = 1;

                            int robot_x = static_cast<int>((playerX + teamX) / 2);
                            int robot_y = static_cast<int>((playerY + teamY) / 2);

                            double robotAngle = std::atan2(yDif, xDif) - (M_PI / 4);

                            r.rotation = robotAngle;
                            r.location = {robot_x, robot_y};

                            found = true;

                            break;
                        }
                    }
                }

                if (found) break;
            }
        }
    }

    void extractBall(pinguim::vsss::game_info& gi, std::vector<cv::Point> ballContour)
    {
        cv::Moments ballMoments = cv::moments(ballContour);
        double ballArea = ballMoments.m00;

        auto& b = gi.ball_info;

        int ball_x = static_cast<int>(ballMoments.m10 / ballArea);
        int ball_y = static_cast<int>(ballMoments.m01 / ballArea);

        b.location = {ball_x, ball_y};
    }

    void extractEnemy(pinguim::vsss::game_info& gi, std::vector<std::vector<cv::Point>> enemyContours)
    {
        // #pragma omp parallel for
        std::vector<int> alreadyUsed = std::vector<int>(gi.enemy_team.size(), 0);
        
        for (int i = 0; i < enemyContours.size(); ++i) 
        {
            auto& enemy = gi.enemy_team[i];

            for (int j = 0; j < enemy.size(); ++j) 
            {

                if (!alreadyUsed[j])
                {
                    auto currEnemyLocation = enemy[j].location;

                    cv::Moments enemyMoments = cv::moments(enemyContours[i]);
                    double enemyArea = enemyMoments.m00;

                    int enemy_x = static_cast<int>(enemyMoments.m10 / enemyArea);
                    int enemy_y = static_cast<int>(enemyMoments.m01 / enemyArea);

                    double xDif, yDif, dist;

                    xDif = enemy_x - currEnemyLocation.location.x;
                    yDif = enemy_y - currEnemyLocation.location.y;

                    dist = std::sqrt(std::pow(xDif, 2.0) + std::pow(yDif, 2.0));

                    if (dist > 11.0 && dist < 16.0)
                    {
                        // #pragma omp critical
                        alreadyUsed[j] = 1;

                        enemy[j].location = {enemy_x, enemy_y}

                        break;
                    }

                }

            }
            
        }
    }
}

void Extraction::execute(pinguim::vsss::game_info& gi, objectsContours c)
{
    // std::cout << "Size All roles [Tem que ser 3 ou menos] = " << rolesContour.size() << " | " << "Size team = " << teamContour.size() << std::endl;
    // #pragma omp task
    extractPlayer(gi, c.rolesContour, c.teamContours);

    // #pragma omp task
    extractBall(gi, c.ballContour);

    // #pragma omp task
    extractEnemy(gi, c.enemyContours);
}

