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
    fmt::print("Robot_id: {}\nX: {} || Y: {}\n", robot.robot_id(), x, y);
    
    orientation = robot.orientation();
    //vx = robot.vx();
    //vy = robot.vy();
    //vorientation = robot.vorientation();

    ball_x = ball.x();
    ball_y = ball.y();
    //ball_vx = ball.vx();
    //ball_vy = ball.vy();

    double xDif, yDif, ballAngle;

    xDif = ball_x - x;
    yDif = ball_y - y;

    ballAngle = atan2(yDif, xDif); // Angulo robô -> bola

    double positionError = std::sqrt(pow(xDif, 2) + pow(yDif, 2)); //Distancia linha reta robo -> bola

    double angleError = 0.0, target_angle = 0.0;

    if(x < ball_x){
        target_angle = atan((ball_y - y)/(ball_x - x));
    } else {
        target_angle = pi + atan((ball_y - y)/(ball_x - x));
    }

    angleError = target_angle - orientation;

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
    const double kap = 1.0, kad = 0.2, velmax = 20.0, velmin = -20.0;

    double vel_front = std::clamp(100.0 * positionError * cos(angleError), velmin, velmax);
    double vel_side  = std::clamp(10.0 * sin(angleError),                velmin, velmax);

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

    fmt::print("Position Error: {}\n \
                Angle Error: {}\n \
                Vel Front: {}\n \
                Vel Side: {}\n \
                Linvel Left: {}\n \
                Linvel Right: {}\n\n", \
                positionError, (angleError*180)/pi, vel_front, vel_side, linvel_left, linvel_right);
    
    return {linvel_left, linvel_right};
}
