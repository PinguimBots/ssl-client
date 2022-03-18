#include "pinguim/vsss/control2.hpp"

#include "pinguim/math.hpp"
#include "pinguim/cvt.hpp"

#include <cmath>
#include <algorithm>
#include <numbers>

auto pinguim::vsss::control::goto_point(
    pinguim::vsss::robot robot,
    pinguim::geo::fpoint target,
    special_action action
) -> command
{
    constexpr auto tof = cvt::to<float>;
    constexpr auto tod = cvt::to<double>;

    auto const velmax = 1.0f;
    auto const velmin = -1.0f;

    switch(action) {
        case special_action::rotate_anti_clockwise:
            return {.team_id=0, .robot_id=0, .left_motor = velmin, .right_motor = 0.97f*velmax};
        case special_action::rotate_clockwise:
            return {.team_id=0, .robot_id=0, .left_motor = 0.97f*velmax, .right_motor = velmin};
        //case 3: return {0, 0.1*velmin};
        //case 4: return {10*velmax, 0};
        default: break;
    }

    constexpr auto pi = std::numbers::pi_v<float>;

    auto const [x, y] = robot.location;
    auto const [target_x, target_y] = target;
    auto const position_error = (robot.location - target).length();

    // NOTE: atan  returns the angle value between -pi/2 (-90deg) and pi/2 (90deg)
    // while atan2 returns the angle value between -pi (-180deg) and pi (180deg).
    auto const epsilon = 1e-18f;
    auto const target_angle = x < target_x
        ?      atan((target_y - y)/(target_x - x + epsilon) * tod) * tof
        : pi + atan((target_y - y)/(target_x - x + epsilon) * tod) * tof;

    auto angle_error = target_angle - tof * robot.rotation;

    if (angle_error > (2.f * pi))
    { angle_error = fmod(angle_error*tod, (2.f * pi)*tod) * tof; }

    if (angle_error < (-2.f * pi))
    { angle_error = fmod(angle_error*tod, (-2.f * pi)*tod) * tof; }

    if (angle_error > pi)
    { angle_error = -(2.f * pi - angle_error); }
    else if (angle_error < (-pi))
    { angle_error = +(angle_error + 2.f * pi); }

    const auto vel_front = std::clamp(position_error * tof(cos(angle_error*tod)), velmin, velmax);
    const auto vel_side  = std::clamp(0.2f * tof(sin(angle_error*tod)), velmin, velmax);
    const auto linvel_left  = std::clamp(vel_front - pb::m::sign<float>(vel_front) * vel_side, velmin, velmax);
    const auto linvel_right = std::clamp(vel_front + pb::m::sign<float>(vel_front) * vel_side, velmin, velmax);

    return {.team_id=0, .robot_id=0, .left_motor = linvel_left, .right_motor = linvel_right};

}
