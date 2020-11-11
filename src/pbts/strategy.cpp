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
       pbts::Strategy::isNear(robot.position, ball.position , 8.0e-2) 
       ? action = pbts::Strategy::kick(robot, ball)
       : action = pbts::Strategy::trackBallYAxix(robot, ball, team);

    }
    else if(robot.id == pbts::ATTACKER)
    {
        auto point = [](pbts::rect bound) {return (bound[0]+bound[1]+bound[2]+bound[3])/4.;};

        actionType acType;

         if(!is_yellow)
        {
            if(robot.position.real() > ball.position.real())
                action = {pbts::point(DEFENDER_std_X + .225, robot.position.imag()), 0};
            else
            {
                 pbts::Strategy::isNear(robot.position, ball.position, 8e-2) 
                    ? pbts::Strategy::isNear(robot.position, is_yellow
                                                            ? point(field.left_goal_bounds) 
                                                            : point(field.right_goal_bounds), 4e-2) 
                    ? acType = actionType::KICK 
                    : acType = actionType::TOWARDGOAL
                    : acType = actionType::MOVETOBALL;

                    if (acType == actionType::TOWARDGOAL) {
                        return pbts::Strategy::towardGoal(robot, field, team);
                    }
                    else if (acType == actionType::KICK) {
                        action = pbts::Strategy::kick(robot, ball);
                    }
                    else if (acType == actionType::MOVETOBALL) {
                        action = pbts::Strategy::moveOntoBall(robot, ball, field, team);
                    }

                    auto [new_point, flag] = action;
                    
                   
                    new_point.real() < DEFENDER_std_X
                    ? new_point.real(DEFENDER_std_X + .225) 
                    : new_point.real(new_point.real());
                

                    action = {pbts::Strategy::create_path(new_point, robot, enemy_robots), flag};
            }
        }
        else    
        {
            if(robot.position.real() < ball.position.real())
            { action = {pbts::point(-DEFENDER_std_X - .225, robot.position.imag()), 0};}
            else 
            {
                 pbts::Strategy::isNear(robot.position, ball.position, 8e-2) 
                ? pbts::Strategy::isNear(robot.position, is_yellow
                                                        ? point(field.left_goal_bounds) 
                                                        : point(field.right_goal_bounds), 4e-2) 
                ? acType = actionType::KICK 
                : acType = actionType::TOWARDGOAL
                : acType = actionType::MOVETOBALL;

                if (acType == actionType::TOWARDGOAL) {
                    return pbts::Strategy::towardGoal(robot, field, team);
                }
                else if (acType == actionType::KICK) {
                    action = pbts::Strategy::kick(robot, ball);
                }
                else if (acType == actionType::MOVETOBALL) {
                    action = pbts::Strategy::moveOntoBall(robot, ball, field, team);
                }

                auto [new_point, flag] = action;
                
                
                new_point.real() > -DEFENDER_std_X
                ? new_point.real(-DEFENDER_std_X - .225) 
                : new_point.real(new_point.real());
                
                


                action = {pbts::Strategy::create_path(new_point, robot, enemy_robots), flag};
            }
        }


       
   
    }
    
    return action;
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

auto pbts::Strategy::towardGoal(const pbts::robot& rbt, 
    const pbts::field_geometry& field, 
    double team) -> std::tuple<pbts::point, int>
{
    auto point = [](pbts::rect bound) {return (bound[0]+bound[1]+bound[2]+bound[3])/4.;};
    pbts::point new_point;

    printf("%d Toward Goal\n", rbt.id);
    team == 1.0
    ? new_point = point(field.right_goal_bounds)
    : new_point = point(field.left_goal_bounds);
 
    
    return {new_point, 0};

}
auto pbts::Strategy::moveOntoBall(
    const pbts::robot& rbt,
    const pbts::ball& ball,
    const pbts::field_geometry& field,
    double team) -> std::tuple<pbts::point, int>
{
    // double ball_angle = std::arg(ball.velocity);

    // pbts::point new_point;

    // if(rbt.position.real() < ball.position.real()  || std::abs(rbt.position - ball.position) > 1.5e-1)
    //     return {ball.position, 0};
    // else if(rbt.position.real() >= ball.position.real())
    // {
    //     if(rbt.position.imag() >= ball.position.imag())
    //     {
    //         new_point.imag(std::clamp(ball.position.imag()+.1, -field.field_bounds[0].imag() ,field.field_bounds[0].imag()));
    //         new_point.real(std::clamp(ball.position.real()-.05*team, team*field.field_bounds[0].real(), team*-field.field_bounds[0].imag()));
    //     }
    //     else
    //     {
    //         new_point.imag(std::clamp(ball.position.imag()-.1, -field.field_bounds[0].imag(), field.field_bounds[0].imag()));
    //         new_point.real(std::clamp(ball.position.real()-.05*team, team*field.field_bounds[0].real(), team * -field.field_bounds[0].imag()));
    //     }
        
    // }

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
    std::vector<std::vector<int>> cost(imax, std::vector<int> (jmax, 0));
    //auto [goal_x, goal_y] = pbts::to_pair(goal_position);
    //auto [robot_x, robot_y] = pbts::to_pair(allied_robot);

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
            printf("%d", discreet_field[i][j]);
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

    //int increment = is_yellow ? -5 : 5;

    auto [goal_i, goal_j] = pbts::to_pair(goal_position);

    //if (isNear(discreet_to_real(allied_robot), discreet_to_real(goal_position), 8.0e-2))

    return next_point(allied_robot, {goal_i, goal_j}, cost);
}

auto pbts::Strategy::add_clearance(int (&field)[imax][jmax], const pbts::wpoint goal_position) -> void
{
    //auto [icle, jcle] = pbts::to_pair(goal_position);

    int theta = 0;
    const int raio = 2;
    const int step = 10;

    auto [hx, hy] = pbts::to_pair(goal_position);

    while (theta <= 360) {
        int x = round(hx + raio * cos(theta));
        int y = round(hy + raio * sin(theta));

        field[x][y] = 0;
        theta += step;
    }

    theta = 0;

}

auto pbts::Strategy::next_point(const pbts::wpoint pos_now, const pbts::wpoint goal, std::vector<std::vector<int>> &cost) -> pbts::wpoint
{
    // Onde custo = 0 -> custo = 10mil
    for (int i = imin; i < imax; i++) {
        for (int j = jmin; j < jmax; j++) {
            if (cost[i][j] == 0) cost[i][j] = 10000;
        }
    }


    ///dads/scanf("Enter pra continuar ...");
    // APENAS PRINT
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
    printf("\n\n"); */

    // APENAS PRINT

    //c = getchar();


    auto [i_goal, j_goal] = pbts::to_pair(goal);
    auto [i_now, j_now] = pbts::to_pair(pos_now);

    int cost_ij = cost[i_now][j_now];

    // printf("COST IJ %d\n", cost_ij);

    int i_next = i_now;
    int j_next = j_now;
    int i = 0;

    while (i < 5) {

        auto neighbours = valid_neighbours({i_next,j_next}, 1, 1);

        for (const auto &neighbour : neighbours) {
            auto [new_i, new_j] = pbts::to_pair(neighbour);

            if (cost[new_i][new_j] < cost_ij) {
                i_next = new_i;
                j_next = new_j;
                cost_ij = cost[new_i][new_j];
            }

            /* if (i_next == i_goal && j_next == j_goal) {
                goto end;
            } */
        }

        ++i;
    }   

    /*end:*/ return {i_next, j_next};
}

auto pbts::Strategy::wave_path(int (&field)[imax][jmax], const pbts::wpoint goal, std::vector<std::vector<int>> &cost) -> void
{
    
    int occ[imax][jmax];

    for (int i = 0; i < imax; i++) {
        for (int j = 0; j < jmax; j++) {
            occ[i][j] = field[i][j];
        }
    }

    auto [igoal, jgoal] = pbts::to_pair(goal);

    cost[igoal][jgoal] = 1;

    auto open = std::queue<pbts::wpoint>();
    open.push({igoal, jgoal});

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

}

auto pbts::Strategy::generate_obstacle(int (&field)[imax][jmax], const std::vector<pbts::wpoint> &enemy_robots) -> void
{

    /* for (const auto &robot : enemy_robots)
    {
        auto [i_now, j_now] = pbts::to_pair(robot);
        std::vector<pbts::wpoint> neighboors = {{i_now, j_now+2},{i_now+1,j_now+2},{i_now+2,j_now+2},{i_now+2,j_now+1},{i_now+2,j_now},{i_now+2,j_now-1},{i_now+2,j_now-2},{i_now+1,j_now-2},{i_now,j_now-2},{i_now-1,j_now-2},{i_now-2,j_now-2},{i_now-2,j_now-1},{i_now-2,j_now},{i_now-2,j_now+1},{i_now-2,j_now+2},{i_now-1,j_now+2}};
        
        // Opções para teste
        //valid_neighbours({i_now, j_now}, 1, 1);
        
        //{{i_now, j_now+2},{i_now+1,j_now+2},{i_now+2,j_now+2},{i_now+2,j_now+1},{i_now+2,j_now},{i_now+2,j_now-1},{i_now+2,j_now-2},{i_now+1,j_now-2},{i_now,j_now-2},{i_now-1,j_now-2},{i_now-2,j_now-2},{i_now-2,j_now-1},{i_now-2,j_now},{i_now-2,j_now+1},{i_now-2,j_now+2},{i_now-1,j_now+2}};

        for (const auto &neighboor : neighboors) {
            auto [ri, rj] = pbts::to_pair(neighboor);

            field[ri][rj] = 1;
        }
    } */

    int theta = 0;
    const int raio = 2;
    const int step = 10;

    for (const auto &robot : enemy_robots) {
        auto [hx, hy] = pbts::to_pair(robot);

        while (theta <= 360) {
            int x = round(hx + raio * cos(theta));
            int y = round(hy + raio * sin(theta));

            field[x][y] = 1;
            theta += step;
        }

        theta = 0;
    }
}

auto pbts::Strategy::add_shield_ball(int (&field)[imax][jmax], const pbts::wpoint ball) -> void
{
    auto [i_ball, j_ball] = pbts::to_pair(ball);

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
    //Correspondestes Discretas
    pbts::wpoint wgoal_position;
    pbts::wpoint wallied_robot, wnew_position;
    std::vector<pbts::wpoint> wenemy_robots;

    // Transformação
    wgoal_position = real_to_discreet(goal_position);
    wallied_robot = real_to_discreet(allied_robot.position);

    for (auto enemy_robot : enemy_robots)
    {
        wenemy_robots.push_back(real_to_discreet(enemy_robot));
    }

    // Geração da nova posição
    wnew_position = wave_planner(wgoal_position, wallied_robot, wenemy_robots);


    /* auto [currx, curry] = pbts::to_pair(allied_robot.position);
    printf("Real Robot Current: Pos[x] = %f | Pos[y] = %f\n", currx, curry);

    auto [wcurri, wcurrj] = pbts::to_pair(wallied_robot);
    printf("Discreet Robot Current: Pos[i] = %d | Pos[j] = %d\n\n", wcurri, wcurrj);


    auto [newx, newy] = pbts::to_pair(discreet_to_real(wnew_position));
    printf("Real Robot New: Pos[x] = %f | Pos[y] = %f\n", newx, newy);

    auto [wni, wnj] = pbts::to_pair(wnew_position);
    printf("Discreet Robot New: Pos[i] = %d | Pos[j] = %d\n\n", wni, wnj);


    auto [goal_x, goal_y] = pbts::to_pair(goal_position);
    printf("Real Goal: Pos[x] = %f | Pos[y] = %f\n", goal_x, goal_y);

    auto [wgi, wgj] = pbts::to_pair(wgoal_position);
    printf("Discreet Goal: Pos[i] = %d | Pos[j] = %d\n\n", wgi, wgj); */


    //c = getchar();

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

auto pbts::Strategy::setTeam(bool is_yellow) -> void
{
    this->is_yellow = is_yellow;
}

auto pbts::Strategy::setBounds(pbts::field_geometry bounds) -> void
{
    field_bounds = bounds;
}