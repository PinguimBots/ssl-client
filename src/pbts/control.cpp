#include "pbts/control.hpp"

#include <cmath>
#include <algorithm>

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

#include <fmt/core.h>

auto pbts::control::generate_vels(pbts::robot robot, pbts::point target_pos) -> pbts::point
{
    // NOTE: atan  returns the angle value between -pi/2 (-90deg) and pi/2 (90deg)
    // while atan2 returns the angle value between -pi (-180deg) and pi (180deg).

    static const constexpr auto pi = glm::pi<double>();

    auto [x, y] = pbts::to_pair(robot.position);
    
    auto orientation = robot.orientation;


    auto [target_x, target_y] = pbts::to_pair(target_pos);
    auto [xDif, yDif] = pbts::to_pair(target_pos - robot.position);

    double ballAngle = atan2(yDif, xDif); // Angulo robô -> bola

    double positionError = glm::length(glm::vec2{xDif, yDif}); //Distancia linha reta robo -> bola

    double angleError = 0.0, target_angle = 0.0;

    
    if(x < target_x){
        target_angle = atan((target_y - y)/(target_x - x));
    } else {
        target_angle = pi + atan((target_y - y)/(target_x - x));
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
