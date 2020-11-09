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
    int discreet_field[imax][jmax];
    auto [goal_x, goal_y] = pbts::to_pair(goal_position);
    //auto [robot_x, robot_y] = pbts::to_pair(allied_robot);

    for (int i = 0; i < imax; i++)
    {
        for (int j = 0; j < jmax; j++)
        {
            discreet_field[i][j] = 0;
        }
    }

    generate_obstacle(discreet_field, enemy_robots);

    add_clearance(discreet_field, allied_robot);

 /*    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            printf("%d", discreet_field[i][j]);
        }
        printf("\n");
    }
    printf("\n\n"); */

    discreet_field[goal_x][goal_y] = 1;

    auto cost = wave_path(discreet_field, goal_position);

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
    

    return next_point(allied_robot, cost);
}

auto pbts::Strategy::add_clearance(int (&field)[imax][jmax], const pbts::wpoint robot_position) -> void
{
    //auto [icle, jcle] = pbts::to_pair(robot_position);

    auto neighbours = valid_neighbours(robot_position, 1, 2);

    for (auto const neighbour : neighbours) {
        auto [ni, nj] = pbts::to_pair(neighbour);

        field[ni][nj] = 0;
    }
}

auto pbts::Strategy::next_point(const pbts::wpoint pos_now, std::vector<std::vector<int>> &cost) -> pbts::wpoint
{
    for (int i = imin; i < imax; i++) {
        for (int j = jmin; j < jmax; j++) {
            if (cost[i][j] == 0) cost[i][j] = 10000;
        }
    }

    for (int i = imin; i < imax; i++) {
        for (int j = jmin; j < jmax; j++) {
            printf("%d", cost[i][j]);
        }
    }

    scanf("Enter pra continuar ...");

    auto [i_now, j_now] = pbts::to_pair(pos_now);

    int cost_ij = cost[i_now][j_now];

    int i_next = i_now;
    int j_next = j_now;

    auto neighbours = valid_neighbours(pos_now, 0, 1);

    for (const auto &neighbour : neighbours) {
        auto [new_i, new_j] = pbts::to_pair(neighbour);

        if (cost[new_i][new_j] < cost_ij) {
            i_next = new_i;
            j_next = new_j;
            cost_ij = cost[new_i][new_j];
        }
    }

    /* if ((i_now > imin) && (i_now < imax)) {
        if (cost[i_now][j_now] < cost_ij) {
            i_next = i_now - 5;
        }
        else if (cost[i_now+5][j_now] < cost_ij) {
            i_next = i_now + 5;
        }
    }

    if ((j_now > jmin) && (j_now < jmax)) {
        if (cost[i_now][j_now-5] < cost_ij) {
            j_next = j_now - 5;
        }
        else if (cost[i_now][j_now+5] < cost_ij) {
            j_next = j_now + 5;
        }
    } */

    return {i_next, j_next};
}

auto pbts::Strategy::wave_path(int (&field)[imax][jmax], const pbts::wpoint goal) -> std::vector<std::vector<int>>
{

    int occ[imax][jmax];

    for (int i = 0; i < imax; i++) {
        for (int j = 0; j < jmax; j++) {
            occ[i][j] = field[i][j];
        }
    }

    std::vector<std::vector<int>> cost(imax, std::vector<int> (jmax, 0));

    int xaxis[imax];
    int yaxis[jmax];

    for (int i = imin; i < imax; i++) {
        xaxis[i] = i;
    }

    for (int j = jmin; j < jmax; j++) {
        yaxis[j] = j;
    }

    int goali = 0;
    int goalj = 0;

    auto [igoal, jgoal] = pbts::to_pair(goal);

    for (int i = 0; i < imax; i++) {
        if (abs(xaxis[i] - igoal) < abs(xaxis[goali] - igoal)) {
            goali = i;
        }
    }

    for (int i = 0; i < jmax; i++) {
        if (abs(yaxis[i] - jgoal) < abs(yaxis[goalj] - igoal)) {
            goalj = i;
        }
    }

    cost[goali][goalj] = 1;

    auto open = std::queue<pbts::wpoint>();
    open.push({goali, goalj});

    while(!open.empty()) {
        auto current_pos = open.front();
        auto [curr_x, curr_y] = pbts::to_pair(current_pos);

        auto neighbours = valid_neighbours(current_pos, 0, 1);

        for (const auto& neighbour : neighbours) {
            auto [looking_x, looking_y] = pbts::to_pair(neighbour);

            if (occ[looking_x][looking_y] == 1) continue;

            if (cost[looking_x][looking_y] != 0) continue;

            cost[looking_x][looking_y] = cost[curr_x][curr_y] + 1;

            open.push(neighbour);

        }

        open.pop();
    }

    return cost;

}

auto pbts::Strategy::generate_obstacle(int (&field)[imax][jmax], const std::vector<pbts::wpoint> &enemy_robots) -> void
{

    for (const auto &robot : enemy_robots)
    {
        auto neighboors = valid_neighbours(robot, 1, 2);

        for (const auto &neighboor : neighboors) {
            auto [ri, rj] = pbts::to_pair(neighboor);

            field[ri][rj] = 1;
        }
    }
}

auto pbts::Strategy::valid_neighbours(pbts::wpoint point, int ntype, int radius) -> std::vector<pbts::wpoint>
{
    std::vector<pbts::wpoint> fourNB;
    std::vector<pbts::wpoint> dNB;

    fourNB = four_neighborhood(point, radius);

    if (ntype == 1) {
        dNB = d_neighborhood(point, radius);
        fourNB.insert(fourNB.end(), dNB.begin(), dNB.end());
    }


    return fourNB;
}

auto pbts::Strategy::four_neighborhood(pbts::wpoint point, int radius) -> std::vector<pbts::wpoint>
{
    auto [x, y] = pbts::to_pair(point);

    std::vector<pbts::wpoint> possibleMoves = {{x + radius, y}, {x - radius, y}, {x, y + radius}, {x, y - radius}};
    std::vector<pbts::wpoint> validMoves;

    for (auto move : possibleMoves)
    {
        auto [mx, my] = pbts::to_pair(move);

        if ((mx < imax && mx >= imin) && (my < jmax && my >= jmin))
        {
            validMoves.push_back(move);
        }
    }

    return validMoves;
}

auto pbts::Strategy::d_neighborhood(pbts::wpoint point, int radius) -> std::vector<pbts::wpoint>
{
    auto [x, y] = pbts::to_pair(point);

    std::vector<pbts::wpoint> possibleMoves = {{x + radius, y + radius}, {x + radius, y - radius}, {x - radius, y + radius}, {x - radius, y - radius}};
    std::vector<pbts::wpoint> validMoves;

    for (auto move : possibleMoves)
    {
        auto [mx, my] = pbts::to_pair(move);

        if ((mx < imax && mx >= imin) && (my < jmax && my >= jmin))
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

    double xout = iin * dx - imin * dx + xmin;
    double yout = jin * dy - jmin * dy + ymin;

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

    int iout = std::round((imax-1)*((xin-xmin)/xT)) + imin;
    int jout = std::round((jmax-1)*((yin-ymin)/yT)) + jmin;

    if (iout < imin)
    {
        iout = imin;
    }
    else if (iout > imax)
    {
        iout = imax;
    }

    if (jout < jmin)
    {
        jout = jmin;
    }
    else if (jout > jmax)
    {
        jout = jmax;
    }

    return {iout, jout};
}