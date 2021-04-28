#include "pinguim/vsss/strategy.hpp"

#include <iostream>

#include "pinguim/cvt.hpp"

using pinguim::cvt::tou;

auto pinguim::vsss::Strategy::actions(
    const pinguim::vsss::robot &robot,
    const pinguim::vsss::ball &ball,
    const std::vector<pinguim::vsss::point> &enemy_robots) -> std::tuple<pinguim::vsss::point, int>
{
    /*
    * Each role has limited actions
    * -> goalkeeper: stays restricted to goal bounds, following the y-axis of the ball
    * -> defenser: moves along the first half of the field, stay quiet if the attacker holds the ball
    * -> attacker: moves onto the ball and carries it toward the goal
    */
    std::tuple<pinguim::vsss::point, int> action;
    //printf("\n\nRobot id: %d  |  Orientation: %f\n\n ", robot.id, robot.orientation);

    switch (robot.id)
    {
    case tou << pinguim::vsss::Roles::GOALKEEPER:
        action = goalkeeper_action(robot, ball);
        break;

    case tou << pinguim::vsss::Roles::DEFENDER:
        action = defender_action(robot, ball);
        break;

    case tou << pinguim::vsss::Roles::ATTACKER:
        action = attacker_action(robot, ball, enemy_robots);
        break;
    }

    return action;
}

auto pinguim::vsss::Strategy::defender_action(
    const pinguim::vsss::robot &robot,
    const pinguim::vsss::ball &ball)
    -> std::tuple<pinguim::vsss::point, int>
{
    std::tuple<pinguim::vsss::point, int> action;

    isNear(robot.position, ball.position, 7.0e-2)
        ? action = kick(robot, ball)
        : action = trackBallYAxix(robot, ball);

    return action;
}

auto pinguim::vsss::Strategy::goalkeeper_action(
    const pinguim::vsss::robot &robot,
    const pinguim::vsss::ball &ball)
    -> std::tuple<pinguim::vsss::point, int>
{
    std::tuple<pinguim::vsss::point, int> action;

    isNear(robot.position, ball.position, 7.0e-2)
        ? action = kick(robot, ball)
        : action = trackBallYAxix(robot, ball);

    return action;
}

auto pinguim::vsss::Strategy::attacker_action(
    const pinguim::vsss::robot &robot,
    const pinguim::vsss::ball &ball,
    const std::vector<pinguim::vsss::point> &enemy_robots)
    -> std::tuple<pinguim::vsss::point, int>
{
    std::tuple<pinguim::vsss::point, int> action;

    auto point = [&](pinguim::vsss::rect bound) { return (bound[0] + bound[1] + bound[2] + bound[3]) / 4.; };

    ActionType actionType;

    if (!is_yellow)
    {
        if (robot.position.real() > ball.position.real())
            action = {pinguim::vsss::point(DEFENDER_std_X + .225, robot.position.imag()), 0};
        else
        {
            isNear(robot.position, ball.position, 6e-2)
                ? isNear(robot.position,
                         is_yellow ? point(field_bounds.left_goal_bounds)
                                   : point(field_bounds.right_goal_bounds),
                         4e-2)
                      ? actionType = ActionType::KICK
                      : actionType = ActionType::TOWARDGOAL
                : actionType = ActionType::MOVETOBALL;

            if (actionType == ActionType::TOWARDGOAL)
            {
                //return towardGoal(robot);

                action = towardGoal(robot);
            }
            else if (actionType == ActionType::KICK)
            {
                action = kick(robot, ball);
            }
            else if (actionType == ActionType::MOVETOBALL)
            {
                action = moveOntoBall(robot, ball);
            }

            auto [new_point, flag] = action;

            new_point.real() < DEFENDER_std_X
                ? new_point.real(DEFENDER_std_X + .225)
                : new_point.real(new_point.real());

            action = {create_path(new_point, robot, enemy_robots), flag};
        }
    }
    else
    {
        if (robot.position.real() < ball.position.real())
        {
            action = {pinguim::vsss::point(-DEFENDER_std_X - .225, robot.position.imag()), 0};
        }
        else
        {
            isNear(robot.position, ball.position, 6e-2)
                ? isNear(robot.position, is_yellow ? point(field_bounds.left_goal_bounds) : point(field_bounds.right_goal_bounds), 3e-1)
                      ? actionType = ActionType::KICK
                      : actionType = ActionType::TOWARDGOAL
                : actionType = ActionType::MOVETOBALL;

            if (actionType == ActionType::TOWARDGOAL)
            {
                //return towardGoal(robot);

                action = towardGoal(robot);
            }
            else if (actionType == ActionType::KICK)
            {
                action = kick(robot, ball);
            }
            else if (actionType == ActionType::MOVETOBALL)
            {
                action = moveOntoBall(robot, ball);
            }

            auto [new_point, flag] = action;

            new_point.real() > -DEFENDER_std_X
                ? new_point.real(-DEFENDER_std_X - .225)
                : new_point.real(new_point.real());

            action = {create_path(new_point, robot, enemy_robots), flag};
        }
    }

    return action;
}

auto pinguim::vsss::Strategy::lin_pred(pinguim::vsss::point point1, pinguim::vsss::point point2, double x) -> pinguim::vsss::point
{
    auto [x1, y1] = pinguim::vsss::to_pair(point1);
    auto [x2, y2] = pinguim::vsss::to_pair(point2);

    double k, y;

    k = (x - x1) / (x2 - x1 + 1e-15);
    y = y1 + k * (y2 - y1);

    return {x, y};
}

auto pinguim::vsss::Strategy::rotate([[maybe_unused]] const pinguim::vsss::robot &robot, const pinguim::vsss::ball &ball) -> std::tuple<pinguim::vsss::point, int>
{
    // auto angle_error = robot.orientation - angle;
    // auto point = pinguim::vsss::point(cos(angle_error)*6.0e-1 + robot.position.real(),
    //                          sin(angle_error)*6.0e-1 + robot.position.imag());

    if (ball.position.imag() < 0)
        return {ball.position, 1};
    else
        return {ball.position, 2};
}

auto pinguim::vsss::Strategy::kick(const pinguim::vsss::robot &robot, const pinguim::vsss::ball &ball) -> std::tuple<pinguim::vsss::point, int>
{
    //printf("%d kicking\n", robot.id);
    return rotate(robot, ball);
}

bool pinguim::vsss::Strategy::isNear(pinguim::vsss::point point1, pinguim::vsss::point point2, double tol)
{
    //printf("%lf \n ", std::abs((point2 - point1)));
    return (std::abs((point2 - point1)) < tol);
}

auto pinguim::vsss::Strategy::trackBallYAxix(const pinguim::vsss::robot &robot, const pinguim::vsss::ball &ball) -> std::tuple<pinguim::vsss::point, int>
{
    /*
    *Need to limit the goalkeeper's area
    */
    //printf("%d Tracking\n", robot.id);

    pinguim::vsss::point position;

    pinguim::vsss::point predicted_position = lin_pred(old_point,
                                              ball.position,
                                              robot.id == tou << pinguim::vsss::Roles::DEFENDER
                                                  ? team * DEFENDER_std_X
                                                  : team * GOALKEEPER_std_X);

    if ((predicted_position.imag() > GOAL_AREA_MIN && predicted_position.imag() <= GOAL_AREA_MAX) && robot.id == tou << pinguim::vsss::Roles::GOALKEEPER)

    {
        position = predicted_position;
    }

    else
    {
        position = robot.id == tou << pinguim::vsss::Roles::DEFENDER
                       ? predicted_position //pinguim::vsss::point(team*DEFENDER_std_X, new_y)
                       : pinguim::vsss::point(team * GOALKEEPER_std_X, std::clamp(ball.position.imag(), GOAL_AREA_MIN, GOAL_AREA_MAX));
    }

    old_point = ball.position;

    return {position,
            0};
}

auto pinguim::vsss::Strategy::towardGoal([[maybe_unused]] const pinguim::vsss::robot &robot) -> std::tuple<pinguim::vsss::point, int>
{
    auto point = [&](pinguim::vsss::rect bound) { return (bound[0] + bound[1] + bound[2] + bound[3]) / 4.; };

    pinguim::vsss::point new_point;

    //printf("%d Toward Goal\n", robot.id);
    team == 1.0
        ? new_point = point(field_bounds.right_goal_bounds)
        : new_point = point(field_bounds.left_goal_bounds);

    return {new_point, 0};
}

auto pinguim::vsss::Strategy::moveOntoBall(
    [[maybe_unused]] const pinguim::vsss::robot &robot,
    const pinguim::vsss::ball &ball)
    -> std::tuple<pinguim::vsss::point, int>
{

    auto ball_diff = ball.position - old_point;
    double dt_ahead = 3;
    pinguim::vsss::point new_position;

    if (ball_diff.real() < 0)
    {
        new_position = lin_pred(old_point, ball.position, dt_ahead * ball_diff.real());
    }
    else
        new_position = ball.position - point(team * .075, 0.0);

    return {ball.position,
            0};
}

auto pinguim::vsss::Strategy::setTeam(bool is_yellow_) -> void
{
    this->is_yellow = is_yellow_;

    this->team = is_yellow_ ? -1.0 : 1.0;

    init_border_obstacle_field();
}

auto pinguim::vsss::Strategy::setBounds(pinguim::vsss::field_geometry bounds) -> void
{
    field_bounds = bounds;

    add_border_field_obstacle();
}