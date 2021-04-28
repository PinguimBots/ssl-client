#include "pinguim/control.hpp"

#include <cmath>
#include <algorithm>

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

#include <fmt/core.h>

auto pinguim::control::rotate(pinguim::robot robot, double angle) -> pinguim::point
{
    auto angle_error = robot.orientation - angle;
    const double kap = 1.0, kad = 0.2, velmax = 1.0, velmin = -1.0;

    return {angle_error*velmin,
            angle_error*velmax};
}


auto pinguim::control::generate_vels(pinguim::robot robot, pinguim::point target_pos, int rotation) -> pinguim::point
{
    // NOTE: atan  returns the angle value between -pi/2 (-90deg) and pi/2 (90deg)
    // while atan2 returns the angle value between -pi (-180deg) and pi (180deg).

    static const constexpr auto pi = glm::pi<double>();

    auto [x, y] = pinguim::to_pair(robot.position);

    auto orientation = robot.orientation;


    auto [target_x, target_y] = pinguim::to_pair(target_pos);
    auto [xDif, yDif] = pinguim::to_pair(target_pos - robot.position);

    double ballAngle = atan2(yDif, xDif); // Angulo robô -> bola

    double positionError = glm::length(glm::vec2{xDif, yDif}); //Distancia linha reta robo -> bola

    double angleError = 0.0, target_angle = 0.0;

    double epsilon = 1e-18;

    if(x < target_x){
        target_angle = atan((target_y - y)/(target_x - x + epsilon));
    } else {
        target_angle = pi + atan((target_y - y)/(target_x - x + epsilon));
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
    const double velmax = 100.0, velmin = -100.0; //kap = 1.0, kad = 0.2

    double vel_front;


    // Esse ou o outro comentado abaixo
    // Comenta o que não for testar
    if (robot.id == 1) {
        vel_front = std::clamp(100.0 * cos(angleError), velmin, velmax);
    }
    else {
        vel_front = std::clamp(100.0 * (positionError)* cos(angleError), velmin, velmax);
    }

    //vel_front = std::clamp(100.0 * (positionError)* cos(angleError), velmin, velmax);
    double vel_side  = std::clamp(10.0 * sin(angleError), velmin, velmax);

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

/*     fmt::print("Position Error: {}\n \
                Angle Error: {}\n \
                Vel Front: {}\n \
                Vel Side: {}\n \
                Linvel Left: {}\n \
                Linvel Right: {}\n\n", \
                positionError, (angleError*180)/pi, vel_front, vel_side, linvel_left, linvel_right); */

    double vell, velr;
    if(rotation == 0)
    { vell = linvel_left; velr = linvel_right;}
    else if(rotation == 1) {vell = 10*velmin; velr = 9.7*velmax;}
    else if(rotation == 2){vell = 9.7*velmax; velr = 10*velmin;}
    else if(rotation == 3){vell = 0; velr = 0.1*velmin;}
    else if(rotation == 4){vell = 10*velmax; velr = 0;}

    return {vell, velr};
}
