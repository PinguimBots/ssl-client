#include "pbts/strategy.hpp"
#include <queue>
#include <iostream>
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

auto pbts::Strategy::pertoBola(std::vector<pbts::robot> &allied_robots, const std::vector<pbts::robot> &enemy_robots, const pbts::ball &ball) -> int
{
    /*
        Verifica quem está mais perto da bola considerando se tem marcação ou não
        Caso houver marcação, definir critério de desempate 

     */

    double dist_robot_bola1, dist_r1_enemy1, dist_r1_enemy2, dist_r2_enemy1, dist_robot_bola2, dist_r2_enemy2;

    auto [bx, by] = pbts::to_pair(ball.position);
    auto [x1, y1] = pbts::to_pair(allied_robots[1].position);
    auto [x2, y2] = pbts::to_pair(allied_robots[2].position);
    auto [xe1, ye1] = pbts::to_pair(enemy_robots[1].position);
    auto [xe2, ye2] = pbts::to_pair(enemy_robots[2].position);

    dist_robot_bola1 = sqrt(pow((x1 - bx), 2) + pow((y1 - by), 2));
    dist_r1_enemy1 = sqrt(pow((x1 - xe1), 2) + pow((y1 - ye1), 2));
    dist_r1_enemy2 = sqrt(pow((x1 - xe2), 2) + pow((y1 - ye2), 2));

    dist_robot_bola2 = sqrt(pow((x2 - bx), 2) + pow((y2 - by), 2));
    dist_r2_enemy1 = sqrt(pow((x2 - xe1), 2) + pow((y2 - ye1), 2));
    dist_r2_enemy2 = sqrt(pow((x2 - xe2), 2) + pow((y2 - ye2), 2));

    if (dist_robot_bola1 > dist_robot_bola2)
    {
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
    }
    else
        return 1;
}


auto pbts::Strategy::actions(
    const pbts::field_geometry& field,
    const pbts::robot& robot,
    const pbts::ball& ball,
    const std::vector<pbts::point> &enemy_robots,
    double team
) -> std::tuple<pbts::point, int>
{
    /*
    * Each role has limited actions 
    * -> goalkeeper: stays restricted to goal bounds, following the y-axis of the ball
    * -> defenser: moves along the first half of the field, stay quiet if the attacker holds the ball
    * -> attacker: moves onto the ball and carries it toward the goal
    */
    pbts::wpoint wgoal_position;
    pbts::wpoint wallied_robot, wnew_position;
    std::vector<pbts::wpoint> wenemy_robots;

    std::tuple<pbts::point, int> action;
    printf("%d\n ", robot.id);

    if(robot.id == pbts::GOALKEEPER)
    {
        pbts::Strategy::isNear(robot.position, ball.position, 8.0e-2) 
        ? action = pbts::Strategy::kick(robot, ball)
        : action = pbts::Strategy::trackBallYAxix(robot, ball, team );
       
    }
    else if(robot.id == pbts::DEFENDER)
    {
       pbts::point point = (team*DEFENDER_std_X, team*DEFENDER_std_Y);
       pbts::Strategy::isNear(robot.position, ball.position , 8.0e-2) 
       ? action = pbts::Strategy::kick(robot, ball)
       : action = pbts::Strategy::trackBallYAxix(robot, ball, team);

    }
    else if(robot.id == pbts::ATTACKER)
    {
        
        pbts::Strategy::isNear(robot.position, ball.position, 7e-2) 
        ? action = pbts::Strategy::kick(robot, ball)
        : action = pbts::Strategy::moveOntoBall(robot, ball);

    }
    
    auto [new_point, flag] = action;

    wgoal_position = real_to_discreet(new_point);
    wallied_robot = real_to_discreet(robot.position);

    for (auto enemy_robot : enemy_robots)
    {
        wenemy_robots.push_back(real_to_discreet(enemy_robot));
    }

    //return action;

    wnew_position = wave_planner(wgoal_position, wallied_robot, wenemy_robots);

    return {discreet_to_real(wnew_position), flag};
}

auto pbts::Strategy::rotate(const pbts::robot& robot, const pbts::ball& ball) -> std::tuple<pbts::point, int>
{
    // auto angle_error = robot.orientation - angle;
    // auto point = pbts::point(cos(angle_error)*6.0e-1 + robot.position.real(), 
    //                          sin(angle_error)*6.0e-1 + robot.position.imag());
    
    if(ball.position.imag() < 0)   return {ball.position, 1};
    else  return {ball.position, 2};


}


auto pbts::Strategy::kick(const pbts::robot& robot, const pbts::ball& ball) -> std::tuple<pbts::point, int>
{
    printf("%d kicking\n", robot.id);
    return pbts::Strategy::rotate(robot, ball);
}

bool pbts::Strategy::isNear(pbts::point point1, pbts::point point2, double tol)
{
    printf("%lf \n ", std::abs((point2 - point1)));
    return (std::abs((point2 - point1)) < tol);
    
}

auto pbts::Strategy::trackBallYAxix(const pbts::robot& rbt, const pbts::ball& ball, double team) -> std::tuple<pbts::point, int>
{
    /*
    *Need to limit the goalkeeper's area
    */
    printf("%d Tracking\n", rbt.id);
    auto position = rbt.id == pbts::GOALKEEPER
                    ? pbts::point(team*GOALKEEPER_std_X, std::clamp(ball.position.imag(), GOAL_AREA_MIN, GOAL_AREA_MAX))
                    : pbts::point(team*DEFENDER_std_X, ball.position.imag());
    
    // auto ball_to_robot_angle = ball.position - rbt.position;
    //pbts::control::generate_vels(rbt, position);
    return {position,
            0};

}
auto pbts::Strategy::moveBack(const pbts::robot& rbt) -> std::tuple<pbts::point, int>
{
    printf("%d Moving back\n", rbt.id);
    //  pbts::control::generate_vels(rbt, pbts::point(pbts::DEFENDER_std_X, pbts::DEFENDER_std_Y));
    return {(DEFENDER_std_X, DEFENDER_std_Y),
             0};
}

auto pbts::Strategy::towardGoal(const pbts::robot& rbt, const pbts::field_geometry& field, double team) -> std::tuple<pbts::point, int>
{
    auto point = [](pbts::rect bound) {return (bound[0]+bound[1]+bound[2]+bound[3])/4.;};
     
    auto points = team == 1.0 
    ? point(field.right_goal_bounds)
    : point(field.left_goal_bounds);

    // pbts::control::generate_vels(rbt, points);
    return {points,
            0};
}
auto pbts::Strategy::moveOntoBall(const pbts::robot& rbt, const pbts::ball& ball) -> std::tuple<pbts::point, int>
{
    // double ball_angle = std::arg(ball.velocity);


    // pbts::control::generate_vels(rbt, ball.position);
    return {ball.position,
            0};
}



auto pbts::Strategy::wave_planner(
    const pbts::wpoint goal_position,
    const pbts::wpoint allied_robot,
    const std::vector<pbts::wpoint> &enemy_robots)
    -> pbts::wpoint
{
    int discreet_field[N][M];
    auto [goal_x, goal_y] = pbts::to_pair(goal_position);
    auto [robot_x, robot_y] = pbts::to_pair(allied_robot);

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            discreet_field[i][j] = -1;
        }
    }

    generate_obstacle(discreet_field, enemy_robots);

 /*    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            printf("%d", discreet_field[i][j]);
        }
        printf("\n");
    }
    printf("\n\n"); */

    discreet_field[goal_y][goal_x] = 1;

    wave_path(discreet_field, goal_position);

/*     for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            printf("[%d]", discreet_field[i][j]);
        }
        printf("\\\n");
    }
    printf("\n\n");

    exit(1); */

    int cost = discreet_field[robot_y][robot_x];

    for (int i = 0; i < 10; i++)
    {
        auto neighboors = valid_neighbours({robot_x, robot_y}, 0);

        for (auto neighboor : neighboors)
        {
            auto [nx, ny] = pbts::to_pair(neighboor);

            if (auto curr_val = discreet_field[ny][nx];
                curr_val < cost && curr_val != 0)
            {
                cost = curr_val;
                robot_x = nx;
                robot_y = ny;

                if (curr_val == 1) goto end;
            }
        }
    }

  end:  return {robot_x, robot_y};
}

auto pbts::Strategy::wave_path(int (&field)[N][M], const pbts::wpoint goal) -> void
{

    auto open_queue = std::queue<pbts::wpoint>();
    open_queue.push(goal);

    while(!open_queue.empty()) {
        auto current_pos = open_queue.front();
        auto [curr_x, curr_y] = pbts::to_pair(current_pos);
        auto curr_val = field[curr_y][curr_x];
        auto neighbours = valid_neighbours(current_pos, 0);

        for (const auto& neighbour : neighbours) {
            auto [looking_x, looking_y] = pbts::to_pair(neighbour);

            if (auto old_val = field[looking_y][looking_x];
                old_val == -1 || curr_val + 1 < old_val) {
                    field[looking_y][looking_x] = curr_val + 1;
                    open_queue.push(neighbour);
            }
        }

        open_queue.pop();
    }

}

auto pbts::Strategy::generate_obstacle(int (&field)[N][M], const std::vector<pbts::wpoint> &enemy_robots) -> void
{

    for (const auto &robot : enemy_robots)
    {
        auto neighboors = valid_neighbours(robot, 1);

        for (const auto &neighboor : neighboors) {
            auto [rx, ry] = pbts::to_pair(neighboor);

            field[ry][rx] = 0;
        }
    }
}

auto pbts::Strategy::valid_neighbours(pbts::wpoint point, int ntype) -> std::vector<pbts::wpoint>
{
    std::vector<pbts::wpoint> fourNB;
    std::vector<pbts::wpoint> dNB;

    fourNB = four_neighborhood(point);

    if (ntype == 1) {
        dNB = d_neighborhood(point);
        fourNB.insert(fourNB.end(), dNB.begin(), dNB.end());
    }


    return fourNB;
}

auto pbts::Strategy::four_neighborhood(pbts::wpoint point) -> std::vector<pbts::wpoint>
{
    auto [x, y] = pbts::to_pair(point);

    std::vector<pbts::wpoint> possibleMoves = {{x + 1, y}, {x - 1, y}, {x, y + 1}, {x, y - 1}};
    std::vector<pbts::wpoint> validMoves;

    for (auto move : possibleMoves)
    {
        auto [mx, my] = pbts::to_pair(move);

        if ((mx < M && mx >= 0) && (my < N && my >= 0))
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

    for (auto move : possibleMoves)
    {
        auto [mx, my] = pbts::to_pair(move);

        if ((mx < M && mx >= 0) && (my < N && my >= 0))
        {
            validMoves.push_back(move);
        }
    }

    return validMoves;
}

auto pbts::Strategy::create_path(
    const pbts::point goal_position,
    const pbts::robot &allied_robot,
    const std::vector<pbts::point> &enemy_robots)
    -> pbts::point
{
    pbts::wpoint wgoal_position;
    pbts::wpoint wallied_robot, wnew_position;
    std::vector<pbts::wpoint> wenemy_robots;

    wgoal_position = real_to_discreet(goal_position);
    wallied_robot = real_to_discreet(allied_robot.position);

    for (auto enemy_robot : enemy_robots)
    {
        wenemy_robots.push_back(real_to_discreet(enemy_robot));
    }

    wnew_position = wave_planner(wgoal_position, wallied_robot, wenemy_robots);

    return {discreet_to_real(wnew_position)};
}

auto pbts::Strategy::discreet_to_real(pbts::wpoint wpoint) -> pbts::point
{

    auto [iin, jin] = pbts::to_pair(wpoint);

    double xout = iin * dx - dx + xmin;
    double yout = jin * dy - dy + ymin;

    if (xout > xmax)
    {
        xout = xmax;
    }
    else if (xout < xmin)
    {
        xout = xmin;
    }

    if (yout > ymax)
    {
        yout = ymax;
    }
    else if (yout < ymin)
    {
        xout = ymin;
    }

    return {xout, yout};
}

auto pbts::Strategy::real_to_discreet(pbts::point point) -> pbts::wpoint
{
    auto [xin, yin] = pbts::to_pair(point);

    int iout = std::round((M-1)*((xin-xmin)/xT));
    int jout = std::round((N-1)*((yin-ymin)/yT));

    if (iout < 0)
    {
        iout = 0;
    }
    else if (iout > M)
    {
        iout = M;
    }

    if (jout < 0)
    {
        jout = 0;
    }
    else if (jout > N)
    {
        jout = N;
    }

    return {iout, jout};
}