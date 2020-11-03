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

#if 0
int pbts::Strategy::pertoBola(const fira_message::Robot &ally_robots, const fira_message::Robot &enemy_robots, const fira_message::Ball &ball) {
    /*
        Verifica quem está mais perto da bola considerando se tem marcação ou não
        Caso houver marcação, definir critério de desempate 
     */
    double x,y,by,bx;
    /*robot1*/
    
    x = Robot.x(); y = Robot.y();
    by = Ball.y(); bx = Ball.x();
    
    dist_robot_bola =  sqrt(pow((x-bx),2) + pow((y-by),2));
    dist_robota_robote = sqrt(pow((x-),2) + pow((y-),2));

    
    return Robot.robot_id();
}
#endif

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

    recursive_wave((int**)discreet_field, neighboors, 1); 

    return {};
}

auto pbts::Strategy::recursive_wave(int **field, const std::vector<pbts::wpoint> points, int prev_cost) -> void
{

    for (const auto point : points) {
        auto [x, y] = pbts::to_pair(point);

        if(field[y][x] == -1) {
            std::vector<pbts::wpoint> neighboors = valid_neighboors(point);
            field[y][x] = prev_cost + 1;

            recursive_wave((int**)field, neighboors, field[y][x]);
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
    std::vector<pbts::wpoint> wenemy_robots ;



    return {};
}
