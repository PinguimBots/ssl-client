#include "control.h"
#include <math.h>

#include <glm/gtc/constants.hpp>

#include <fmt/core.h>

#include <algorithm>

const constexpr auto pi = glm::pi<double>();

std::tuple<double,double> pbts::Control::generateVels(const fira_message::Robot &robot, const fira_message::Ball &ball) {

    double x, y, orientation, vx, vy, vorientation;
    double ball_x, ball_y, ball_vx, ball_vy;

    x = robot.x();
    y = robot.y();
    orientation = robot.orientation();
    vx = robot.vx();
    vy = robot.vy();
    vorientation = robot.vorientation();

    ball_x = ball.x();
    ball_y = ball.y();
    ball_vx = ball.vx();
    ball_vy = ball.vy();

    double xDif, yDif, ballAngle;

    xDif = ball_x - x;
    yDif = ball_y - y;

    ballAngle = atan2(yDif, xDif); // Angulo robô -> bola

    auto hypotenuse = std::sqrt(xDif + yDif); //Distancia linha reta robo -> bola

    double angleError = 0.0, target_angle = 0.0;

    if (x < ball_x) {
        target_angle = ballAngle;
    } else {
        target_angle = pi + ballAngle;
    }

    angleError = target_angle - orientation;

    fmt::print("Angle Error: {} \n\n", angleError);

    if (angleError > (2 * pi)) {
        angleError = fmod(angleError, (2 * pi));
    }
    if (angleError < (-2 * pi)) {
        angleError = fmod(angleError, (-2 * pi));
    }
    if (angleError > pi) {
        angleError = -(2 * pi - angleError);
    } else if (angleError < (-pi)) {
        angleError = +(angleError + 2 * pi);
    }

    double robot_angle_error = 0.0, linvel_left = 0.0, linvel_right = 0.0;
    const double kap = 1.0, kad = 0.2, velmax = 5.0, velmin = -5.0, vel = 5.0;

    double positionError = vel; // sqrt(pow(ball_x - x,2) + pow(ball_y - y,2));

    double vel_front = std::clamp(1.0 * positionError * cos(angleError), velmin, velmax);
    double vel_side  = std::clamp(0.5 * sin(angleError),                velmin, velmax);

    if(vel_front > 0) {
        linvel_left = vel_front - vel_side;
        linvel_right = vel_front + vel_side;
    }
    else {
        linvel_left = vel_front + vel_side;
        linvel_right = vel_front - vel_side;
    }

    if(linvel_left > velmax) {
        linvel_left = velmax;
    }
    else if(linvel_left < velmin) {  
        linvel_left = velmin;
    }
    if(linvel_right > velmax) {
        linvel_right = velmax;
    }
    else if(linvel_right < velmin) {    
        linvel_right = velmin;
    }

/*     linvel_left = +kap * angleError - kad * robot_angle_error + vel;

    if (linvel_left > velmax) {
        linvel_left = velmax;
    } else if (linvel_left < velmin) {
        linvel_left = velmin;
    }

    linvel_right = -kap * angleError + kad * robot_angle_error + vel;

    if (linvel_right > velmax) {
        linvel_right = velmax;
    } else if (linvel_right < velmin) {
        linvel_right = velmin;
    } */
    
    return {linvel_left, linvel_right};
}
