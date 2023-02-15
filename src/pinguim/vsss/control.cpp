#include "pinguim/vsss/control.hpp"

#include "pinguim/cvt.hpp"

#include <cmath>
#include <algorithm>

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

#include <fmt/core.h>

constexpr auto sign(auto v) { return v >= 0 ? 1 : -1; }

auto pinguim::vsss::control::rotate(pinguim::vsss::robot robot, double angle) -> pinguim::vsss::point
{
    auto angle_error = robot.orientation - angle;
    [[maybe_unused]] const double kap = 1.0, kad = 0.2, velmax = 1.0, velmin = -1.0;

    return {angle_error*velmin,
            angle_error*velmax};
}


auto pinguim::vsss::control::generate_vels(pinguim::vsss::robot robot, pinguim::vsss::point target_pos, int rotation) -> pinguim::vsss::point
{
    const auto velmax = 100.0, velmin = -100.0; //kap = 1.0, kad = 0.2

    // NOTE: atan  returns the angle value between -pi/2 (-90deg) and pi/2 (90deg)
    // while atan2 returns the angle value between -pi (-180deg) and pi (180deg).
    switch(rotation) {
        case 1: return {10.0*velmin, 9.70*velmax};
        case 2: return {9.70*velmax, 10.0*velmin};
        case 3: return {0, 0.1*velmin};
        case 4: return {10*velmax, 0};
        default: break;
    }

    static const constexpr auto pi = glm::pi<double>();

    auto [x, y] = pinguim::vsss::to_pair(robot.position);

    auto orientation = robot.orientation;

    auto [target_x, target_y] = pinguim::vsss::to_pair(target_pos);
    auto [xDif, yDif] = pinguim::vsss::to_pair(target_pos - robot.position);

    double positionError = cvt::toe * glm::length(glm::vec<2, double>{xDif, yDif}); //Distancia linha reta robo -> bola

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

    [[maybe_unused]] double robot_angle_error = 0.0;

    const auto vel_front = std::clamp(100.0 * (robot.id == 1 ? 1 : positionError) * cos(angleError), velmin, velmax);
    const auto vel_side  = std::clamp(10.0 * sin(angleError), velmin, velmax);
    const auto linvel_left  = std::clamp(vel_front - sign(vel_front)*vel_side, velmin, velmax);
    const auto linvel_right = std::clamp(vel_front + sign(vel_front)*vel_side, velmin, velmax);

    return {linvel_left, linvel_right};

}
