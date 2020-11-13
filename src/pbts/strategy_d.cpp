#include <pbts/strategy.hpp>
#include <queue>

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

    auto [goal_i, goal_j] = pbts::to_pair(goal_position);

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

    auto [i_goal, j_goal] = pbts::to_pair(goal);
    auto [i_now, j_now] = pbts::to_pair(pos_now);

    int cost_ij = cost[i_now][j_now];

    // printf("COST IJ %d\n", cost_ij);

    int i_next = i_now;
    int j_next = j_now;
    int i = 0;

    while (i < 4) {

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