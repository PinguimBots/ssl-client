#include "pbts/strategy.hpp"

auto pbts::Strategy::generate_robot_positions(
    const pbts::field_geometry &field,
    const std::vector<pbts::robot> &allied_robots,
    const std::vector<pbts::robot> &enemy_robots,
    const pbts::ball &ball)
    -> std::vector<pbts::point>
{
    int closest_ball;
    auto [ball_x, ball_y] = pbts::to_pair(ball.position);

    for (int i = 0; i < 3; ++i)
    {
        //Verificar as condições
    }
    //Situação de ataque ou defesa
    //Usar as demarcações do campo

    return {};
}

int pbts::Strategy::pertoBola(std::vector<pbts::robot> &allied_robots, const std::vector<pbts::robot> &enemy_robots, const pbts::ball &ball)
{
    /*
        Verifica quem está mais perto da bola considerando se tem marcação ou não
        Caso houver marcação, definir critério de desempate 

     */
    /*
        Log de problemas:

        - Nas chamadas de to_pair ta dando um erro que não estou conseguindo resolver.

    */
    double dist_robot_bola1, dist_r1_enemy1, dist_r1_enemy2, dist_r2_enemy1, dist_robot_bola2, dist_r2_enemy2;

    auto [ bx, by ] = pbts::to_pair(ball.position);
    auto [ x1, y1 ] = pbts::to_pair(allied_robots[1].position);
    auto [ x2, y2 ] = pbts::to_pair(allied_robots[2].position);
    auto [ xe1, ye1 ] = pbts::to_pair(enemy_robots[1].position);
    auto [ xe2, ye2 ] = pbts::to_pair(enemy_robots[2].position);

    dist_robot_bola1 = sqrt(pow((x1 - bx), 2) + pow((y1 - by), 2));
    dist_r1_enemy1 = sqrt(pow((x1 - xe1), 2) + pow((y1 - ye1), 2));
    dist_r1_enemy2 = sqrt(pow((x1 - xe2), 2) + pow((y1 - ye2), 2));

    dist_robot_bola2 = sqrt(pow((x2 - bx), 2) + pow((y2 - by), 2));
    dist_r2_enemy1 = sqrt(pow((x2 - xe1), 2) + pow((y2 - ye1), 2));
    dist_r2_enemy2 = sqrt(pow((x2 - xe2), 2) + pow((y2 - ye2), 2));

    if (dist_robot_bola1 > dist_robot_bola2)
        if ((dist_r1_enemy1 > dist_r2_enemy1) && (dist_r1_enemy2 > dist_r2_enemy2))
        {
            return 2;
        }
        else if ((dist_r1_enemy1 < dist_r2_enemy1 && dist_r1_enemy1 > dist_r2_enemy2) || (dist_r1_enemy1 < dist_r2_enemy1 && dist_r1_enemy1 > dist_r2_enemy2))
        {
            return 2;
        }
        else if (dist_r1_enemy1 < dist_r2_enemy1 && dist_r1_enemy2 < dist_r2_enemy2)
        {
            return 1;
        }
        else
            return 1;
}

auto pbts::Strategy::wave_planner(
    const pbts::wpoint goal_position,
    const pbts::wpoint allied_robot,
    const std::vector<pbts::wpoint> &enemy_robots)
    -> pbts::wpoint
{
    int discreet_field[N][M] = {-1};
    auto [goal_x, goal_y] = pbts::to_pair(goal_position);
    auto [robot_x, robot_y] = pbts::to_pair(allied_robot);

    generate_obstacle((int **)discreet_field, enemy_robots);

    discreet_field[goal_y][goal_x] = 1;

    std::vector<pbts::wpoint> neighboors = valid_neighboors(goal_position);

    recursive_wave((int **)discreet_field, neighboors, 1);

    return {};
}

auto pbts::Strategy::recursive_wave(int **field, const std::vector<pbts::wpoint> points, int prev_cost) -> void
{

    for (const auto point : points)
    {
        auto [x, y] = pbts::to_pair(point);

        if (field[y][x] == -1)
        {
            std::vector<pbts::wpoint> neighboors = valid_neighboors(point);
            field[y][x] = prev_cost + 1;

            recursive_wave((int **)field, neighboors, field[y][x]);
        }
    }
}

auto pbts::Strategy::generate_obstacle(int **field, const std::vector<pbts::wpoint> &enemy_robots) -> void
{
    int r = 1, theta = 0;
    int step = 10;

    for (const auto &robot : enemy_robots)
    {
        auto [r_x, r_y] = pbts::to_pair(robot);
        int h = r_x + (xT / 2);
        int k = r_y + (yT / 2);

        while (theta <= 360)
        {
            int x = h + r * std::cos(theta);
            int y = k + r + std::sin(theta);

            field[y][x] = 0;
            theta += step;
        }

        theta = 0;
    }
}

auto pbts::Strategy::valid_neighboors(pbts::wpoint point) -> std::vector<pbts::wpoint>
{
    std::vector<pbts::wpoint> fourNB;
    std::vector<pbts::wpoint> dNB;

    fourNB = four_neighborhood(point);
    dNB = d_neighborhood(point);

    fourNB.insert(fourNB.end(), dNB.begin(), dNB.end());

    return fourNB;
}

auto pbts::Strategy::four_neighborhood(pbts::wpoint point) -> std::vector<pbts::wpoint>
{
    auto [x, y] = pbts::to_pair(point);

    std::vector<pbts::wpoint> possibleMoves = {{x + 1, y}, {x - 1, y}, {x, y + 1}, {x, y - 1}};
    std::vector<pbts::wpoint> validMoves;

    for (auto &move : possibleMoves)
    {
        auto [mx, my] = pbts::to_pair(move);

        if ((mx < xT && mx >= 0) && (my < yT && my >= 0))
        {
            validMoves.push_back(move);
        }
    }

    return validMoves;
}

auto pbts::Strategy::d_neighborhood(pbts::wpoint point) -> std::vector<pbts::wpoint>
{
    auto [x, y] = pbts::to_pair(point);

    std::vector<pbts::wpoint> possibleMoves = {{x + 1, y + 1}, {x + 1, y - 1}, {x - 1, y + 1}, {x - 1, y - 1}};
    std::vector<pbts::wpoint> validMoves;

    for (auto &move : possibleMoves)
    {
        auto [mx, my] = pbts::to_pair(move);

        if ((mx < xT && mx >= 0) && (my < yT && my >= 0))
        {
            validMoves.push_back(move);
        }
    }

    return validMoves;
}

/* auto pbts::Strategy::bounds_set() -> bool
{
    return bounds.has_value();
}

auto pbts::Strategy::set_bounds(pbts::field_geometry newbounds) -> void
{
    bounds = newbounds;
} */

auto pbts::Strategy::create_path(
    const pbts::point goal_position,
    const pbts::robot &allied_robot,
    const std::vector<pbts::robot> &enemy_robots)
    -> pbts::point
{
    pbts::wpoint wgoal_position;
    pbts::wpoint wallied_robot;
    std::vector<pbts::wpoint> wenemy_robots;

    return {};
}
