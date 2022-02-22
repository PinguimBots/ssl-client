#include "pinguim/vsss/strategy.hpp"

#include <algorithm> // For std::clamp.
#include <iostream>
#include <queue>
#include <omp.h>

#include "pinguim/cvt.hpp"

using pinguim::cvt::tou;
using pinguim::cvt::to_expected;

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

auto pinguim::vsss::Strategy::wave_planner(
    const pinguim::vsss::wpoint goal_position,
    const pinguim::vsss::wpoint allied_robot,
    const std::vector<pinguim::vsss::wpoint> &enemy_robots)
    -> pinguim::vsss::wpoint
{
    int discreet_field[imax][jmax];
    std::vector<std::vector<int>> cost(imax, std::vector<int> (jmax, 0));
    //auto [goal_x, goal_y] = pinguim::vsss::to_pair(goal_position);
    //auto [robot_x, robot_y] = pinguim::vsss::to_pair(allied_robot);

    #pragma omp parallel for
    for (int i = 0; i < imax; i++)
    {
        for (int j = 0; j < jmax; j++)
        {
            discreet_field[i][j] = 0;
        }
    }

    generate_obstacle(discreet_field, enemy_robots);

    /* printf("FIELD\n");

    for (int i = 0; i < imax; i++)
    {
        for (int j = 0; j < jmax; j++)
        {
            printf("%d", border_obstacle[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    c = getchar(); */

    add_clearance(discreet_field, goal_position);

    add_shield_ball(discreet_field, goal_position);

    /* printf("FIELD\n");

    for (int i = 0; i < imax; i++)
    {
        for (int j = 0; j < jmax; j++)
        {
            printf("%d", discreet_field[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    c = getchar(); */

    wave_path(discreet_field, goal_position, cost);

    /* for (int i = imin; i < imax; i++) {
        for (int j = jmin; j < jmax; j++) {
            if (cost[i][j] == 10000) {
                printf("[X ] ");
            }
            else if (cost[i][j] < 10)
                printf("[%d ] ", cost[i][j]);

            else printf("[%d] ", cost[i][j]);
        }
        printf("\n");
    }

    printf("\n\n");

    c = getchar(); */

    auto [goal_i, goal_j] = pinguim::vsss::to_pair(goal_position);

    return next_point(allied_robot, {goal_i, goal_j}, cost);
}

auto pinguim::vsss::Strategy::add_clearance(int (&field)[imax][jmax], const pinguim::vsss::wpoint goal_position) -> void
{
    //auto [icle, jcle] = pinguim::vsss::to_pair(goal_position);

    int theta = 0;
    const int raio = 2;
    const int step = 10;

    auto [hx, hy] = pinguim::vsss::to_pair(goal_position);

    while (theta <= 360) {
        int x = to_expected << round(hx + raio * cos(theta));
        int y = to_expected << round(hy + raio * sin(theta));

        field[x][y] = 0;
        theta += step;
    }

    theta = 0;

}

auto pinguim::vsss::Strategy::next_point(const pinguim::vsss::wpoint pos_now, const pinguim::vsss::wpoint goal, std::vector<std::vector<int>> &cost) -> pinguim::vsss::wpoint
{
    // Onde custo = 0 -> custo = 10mil
    #pragma omp parallel for
    for (int i = imin; i < imax; i++) {
        for (int j = jmin; j < jmax; j++) {
            if (cost[to_expected << i][to_expected << j] == 0) cost[to_expected << i][to_expected << j] = 10000;
        }
    }

    [[maybe_unused]] auto [i_goal, j_goal] = pinguim::vsss::to_pair(goal);
    auto [i_now, j_now] = pinguim::vsss::to_pair(pos_now);

    int cost_ij = cost[to_expected << i_now][to_expected << j_now];

    // printf("COST IJ %d\n", cost_ij);

    int i_next = i_now;
    int j_next = j_now;
    int i = 0;

    while (i < 4) {

        auto neighbours = valid_neighbours({i_next,j_next}, 1, 1);

        for (const auto &neighbour : neighbours) {
            auto [new_i, new_j] = pinguim::vsss::to_pair(neighbour);

            if (cost[to_expected << new_i][to_expected << new_j] < cost_ij) {
                i_next = new_i;
                j_next = new_j;
                cost_ij = cost[to_expected << new_i][to_expected << new_j];
            }

            /* if (i_next == i_goal && j_next == j_goal) {
                goto end;
            } */
        }

        ++i;
    }

    /*end:*/ return {i_next, j_next};
}

auto pinguim::vsss::Strategy::wave_path(int (&field)[imax][jmax], const pinguim::vsss::wpoint goal, std::vector<std::vector<int>> &cost) -> void
{

    int occ[imax][jmax];

    #pragma omp parallel for
    for (int i = 0; i < imax; i++) {
        for (int j = 0; j < jmax; j++) {
            occ[i][j] = field[i][j];
        }
    }

    auto [igoal, jgoal] = pinguim::vsss::to_pair(goal);

    cost[to_expected << igoal][to_expected << jgoal] = 1;

    auto open = std::queue<pinguim::vsss::wpoint>();
    open.push({igoal, jgoal});

    while(!open.empty()) {
        auto current_pos = open.front();
        auto [curr_x, curr_y] = pinguim::vsss::to_pair(current_pos);

        auto neighbours = valid_neighbours(current_pos, 0, 1);

        for (const auto& neighbour : neighbours) {
            auto [looking_x, looking_y] = pinguim::vsss::to_pair(neighbour);

            if (occ[looking_x][looking_y] == 1) continue;

            if (cost[to_expected << looking_x][to_expected << looking_y] != 0) continue;

            cost[to_expected << looking_x][to_expected << looking_y] = cost[to_expected << curr_x][to_expected << curr_y] + 1;

            open.push(neighbour);

        }

        open.pop();
    }

}

auto pinguim::vsss::Strategy::generate_obstacle(int (&field)[imax][jmax], const std::vector<pinguim::vsss::wpoint> &enemy_robots) -> void
{

    int theta = 0;
    const int raio = 2;
    const int step = 10;

    for (const auto &robot : enemy_robots) {
        auto [hx, hy] = pinguim::vsss::to_pair(robot);

        while (theta <= 360) {
            int x = to_expected << round(hx + raio * cos(theta));
            int y = to_expected << round(hy + raio * sin(theta));

            field[x][y] = 1;
            theta += step;
        }

        theta = 0;
    }
}

auto pinguim::vsss::Strategy::add_shield_ball(int (&field)[imax][jmax], const pinguim::vsss::wpoint ball) -> void
{
    auto [i_ball, j_ball] = pinguim::vsss::to_pair(ball);

    /* for (int i = iini; i <= ifin; i++){
        for (int j = jini; j <= jfin; j++){
            field[i][j] = 1;
        }
    } */

    if (!is_yellow) {
        field[i_ball-1][j_ball+1] = 1;
        field[i_ball][j_ball+1] = 1;
        field[i_ball+1][j_ball+1] = 1;
        field[i_ball+1][j_ball] = 1;
        field[i_ball+1][j_ball-1] = 1;
        field[i_ball][j_ball-1] = 1;
        field[i_ball-1][j_ball-1] = 1;

        field[i_ball-1][j_ball+2] = 1;
        field[i_ball][j_ball+2] = 1;
        field[i_ball+1][j_ball+2] = 1;
        field[i_ball+2][j_ball+1] = 1;
        field[i_ball+2][j_ball] = 1;
        field[i_ball+2][j_ball-1] = 1;
        field[i_ball+1][j_ball-2] = 1;
        field[i_ball][j_ball-2] = 1;
        field[i_ball-1][j_ball-2] = 1;

    }
    else {
        field[i_ball+1][j_ball+1] = 1;
        field[i_ball][j_ball+1] = 1;
        field[i_ball-1][j_ball+1] = 1;
        field[i_ball-1][j_ball] = 1;
        field[i_ball-1][j_ball-1] = 1;
        field[i_ball][j_ball-1] = 1;
        field[i_ball+1][j_ball-1] = 1;

        field[i_ball+1][j_ball+2] = 1;
        field[i_ball][j_ball+2] = 1;
        field[i_ball-1][j_ball+2] = 1;
        field[i_ball-2][j_ball+1] = 1;
        field[i_ball-2][j_ball] = 1;
        field[i_ball-2][j_ball-1] = 1;
        field[i_ball-1][j_ball-2] = 1;
        field[i_ball][j_ball-2] = 1;
        field[i_ball+1][j_ball-2] = 1;
    }
}

auto pinguim::vsss::Strategy::valid_neighbours(pinguim::vsss::wpoint point, int ntype, int radius) -> std::vector<pinguim::vsss::wpoint>
{
    std::vector<pinguim::vsss::wpoint> fourNB;
    std::vector<pinguim::vsss::wpoint> dNB;

    fourNB = four_neighborhood(point, radius);

    if (ntype == 1) {
        dNB = d_neighborhood(point, radius);
        fourNB.insert(fourNB.end(), dNB.begin(), dNB.end());
    }


    return fourNB;
}

auto pinguim::vsss::Strategy::four_neighborhood(pinguim::vsss::wpoint point, int radius) -> std::vector<pinguim::vsss::wpoint>
{
    auto [x, y] = pinguim::vsss::to_pair(point);

    std::vector<pinguim::vsss::wpoint> possibleMoves = {{x + radius, y}, {x - radius, y}, {x, y + radius}, {x, y - radius}};
    std::vector<pinguim::vsss::wpoint> validMoves;

    for (auto move : possibleMoves)
    {
        auto [mx, my] = pinguim::vsss::to_pair(move);

        if ((mx < imax && mx >= imin) && (my < jmax && my >= jmin))
        {
            if (border_obstacle[mx][my] == 0)
                validMoves.push_back(move);
        }
    }

    return validMoves;
}

auto pinguim::vsss::Strategy::d_neighborhood(pinguim::vsss::wpoint point, int radius) -> std::vector<pinguim::vsss::wpoint>
{
    auto [x, y] = pinguim::vsss::to_pair(point);

    std::vector<pinguim::vsss::wpoint> possibleMoves = {{x + radius, y + radius}, {x + radius, y - radius}, {x - radius, y + radius}, {x - radius, y - radius}};
    std::vector<pinguim::vsss::wpoint> validMoves;

    for (auto move : possibleMoves)
    {
        auto [mx, my] = pinguim::vsss::to_pair(move);

        if ((mx < imax && mx >= imin) && (my < jmax && my >= jmin))
        {
            if (border_obstacle[mx][my] == 0)
                validMoves.push_back(move);
        }
    }

    return validMoves;
}

auto pinguim::vsss::Strategy::create_path(
    const pinguim::vsss::point goal_position,
    const pinguim::vsss::robot &allied_robot,
    const std::vector<pinguim::vsss::point> &enemy_robots)
    -> pinguim::vsss::point
{
    //Correspondestes Discretas
    pinguim::vsss::wpoint wgoal_position;
    pinguim::vsss::wpoint wallied_robot, wnew_position;
    std::vector<pinguim::vsss::wpoint> wenemy_robots;

    // Transformação
    wgoal_position = real_to_discreet(goal_position);
    wallied_robot = real_to_discreet(allied_robot.position);


    for (auto enemy_robot : enemy_robots)
    {
        wenemy_robots.push_back(real_to_discreet(enemy_robot));
    }

    // Geração da nova posição
    wnew_position = wave_planner(wgoal_position, wallied_robot, wenemy_robots);


    /* auto [currx, curry] = pinguim::vsss::to_pair(allied_robot.position);
    printf("Real Robot Current: Pos[x] = %f | Pos[y] = %f\n", currx, curry);

    auto [wcurri, wcurrj] = pinguim::vsss::to_pair(wallied_robot);
    printf("Discreet Robot Current: Pos[i] = %d | Pos[j] = %d\n\n", wcurri, wcurrj);


    auto [newx, newy] = pinguim::vsss::to_pair(discreet_to_real(wnew_position));
    printf("Real Robot New: Pos[x] = %f | Pos[y] = %f\n", newx, newy);

    auto [wni, wnj] = pinguim::vsss::to_pair(wnew_position);
    printf("Discreet Robot New: Pos[i] = %d | Pos[j] = %d\n\n", wni, wnj);


    auto [goal_x, goal_y] = pinguim::vsss::to_pair(goal_position);
    printf("Real Goal: Pos[x] = %f | Pos[y] = %f\n", goal_x, goal_y);

    auto [wgi, wgj] = pinguim::vsss::to_pair(wgoal_position);
    printf("Discreet Goal: Pos[i] = %d | Pos[j] = %d\n\n", wgi, wgj); */


    //c = getchar();

    return {discreet_to_real(wnew_position)};
}

auto pinguim::vsss::Strategy::init_border_obstacle_field() -> void
{
    #pragma omp parallel for
    for (int i = imin; i < imax; i++)
    {
        for (int j = jmin; j < jmin; j++)
        {
            border_obstacle[i][j] = 0;
        }
    }
}

auto pinguim::vsss::Strategy::add_border_field_obstacle() -> void
{

    pinguim::vsss::point enemy_point1, enemy_point2;

    if (is_yellow) {
        enemy_point1 = field_bounds.left_goal_bounds[1];
        enemy_point2 = field_bounds.left_goal_bounds[2];
    } else {
        enemy_point1 = field_bounds.right_goal_bounds[0];
        enemy_point2 = field_bounds.right_goal_bounds[3];
    }

    pinguim::vsss::wpoint wenemy_point1, wenemy_point2;

    wenemy_point1 = real_to_discreet(enemy_point1);
    wenemy_point2 = real_to_discreet(enemy_point2);

    auto [e_1_i, e_1_j] = pinguim::vsss::to_pair(wenemy_point1);
    auto [e_2_i, e_2_j] = pinguim::vsss::to_pair(wenemy_point2);

    auto [f_0_i, f_0_j] = pinguim::vsss::to_pair(real_to_discreet(field_bounds.field_bounds[0]));
    auto [f_1_i, f_1_j] = pinguim::vsss::to_pair(real_to_discreet(field_bounds.field_bounds[1]));
    auto [f_2_i, f_2_j] = pinguim::vsss::to_pair(real_to_discreet(field_bounds.field_bounds[2]));
    auto [f_3_i, f_3_j] = pinguim::vsss::to_pair(real_to_discreet(field_bounds.field_bounds[3]));

    //#pragma omp parallel for
    for (int i = 0; i < imax; i++)
    {
        for (int j = 0; j < jmax; j++)
        {
            if (j == f_0_j && i >= f_0_i && i <= f_1_i) border_obstacle[i][j] = 1;
            else if (i == f_1_i && j <= f_1_j && j >= f_2_j) border_obstacle[i][j] = 1;
            else if (j == f_2_j && i <= f_2_i && i >= f_3_i) border_obstacle[i][j] = 1;
            else if (i == f_3_i && j >= f_3_j && j <= f_0_j) border_obstacle[i][j] = 1;
        }
    }

    for (int j = e_1_j; j <= e_2_i; j++) {

        int i = e_1_i;

        border_obstacle[i][j] = 0;
    }
}

auto pinguim::vsss::Strategy::discreet_to_real(pinguim::vsss::wpoint wpoint) -> pinguim::vsss::point
{
    const auto [iin, jin] = pinguim::vsss::to_pair(wpoint);

    const auto xout = std::clamp(iin * dx - imin * dx + xmin, xmin, xmax);
    const auto yout = std::clamp(jin * dy - jmin * dy + ymin, ymin, ymax);

    return {xout, yout};
}

auto pinguim::vsss::Strategy::real_to_discreet(pinguim::vsss::point point) -> pinguim::vsss::wpoint
{
    const auto [xin, yin] = pinguim::vsss::to_pair(point);

    int iout = cvt::to_expected << std::round((imax-1)*((xin-xmin)/xT)) + imin;
    int jout = cvt::to_expected << std::round((jmax-1)*((yin-ymin)/yT)) + jmin;

    iout = std::clamp(iout, imin, imax);
    jout = std::clamp(jout, jmin, jmax);

    return {iout, jout};
}
