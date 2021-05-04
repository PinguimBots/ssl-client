#include <pinguim/vsss/strategy.hpp>
#include <queue>
#include <omp.h>

#include "pinguim/cvt.hpp"

using pinguim::cvt::to_expected;

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

    #pragma omp parallel for
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