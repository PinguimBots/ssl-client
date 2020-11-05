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

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            printf("%d ", discreet_field[i][j]);
        }
        printf("\n");
    }

    discreet_field[goal_y][goal_x] = 1;

    std::vector<pbts::wpoint> neighboors = valid_neighboors(goal_position);

    recursive_wave((int **)discreet_field, neighboors, 1);

    int cost = discreet_field[robot_y][robot_x];

    for (int i = 0; i < 6; i++)
    {
        neighboors = valid_neighboors({robot_x, robot_y});

        for (auto neighboor : neighboors)
        {
            auto [nx, ny] = pbts::to_pair(neighboor);

            if (discreet_field[ny][nx] < cost)
            {
                cost = discreet_field[ny][nx];
                robot_x = nx;
                robot_y = ny;
                break;
            }
        }
    }

    return {robot_x, robot_y};
}

auto pbts::Strategy::recursive_wave(int **field, const std::vector<pbts::wpoint> points, int prev_cost) -> void
{

    for (const auto point : points)
    {
        auto [x, y] = pbts::to_pair(point);

        printf("X = %d | Y = %d\n", x, y);
        printf("FIELD[Y][X] = %d\n", field[y][x]);

        if (field[y][x] == -1)
        {
            std::vector<pbts::wpoint> neighboors = valid_neighboors(point);
            field[y][x] = prev_cost + 1;

            recursive_wave(field, neighboors, field[y][x]);
        }
    }
}

auto pbts::Strategy::generate_obstacle(int **field, const std::vector<pbts::wpoint> &enemy_robots) -> void
{
    int r = 1, theta = 0;
    int step = 7;

    printf("ENTROU\n");

    for (const auto &robot : enemy_robots)
    {
        auto [r_x, r_y] = pbts::to_pair(robot);
        int h = r_x + (xT / 2);
        int k = r_y + (yT / 2);

        while (theta <= 360)
        {
            int x = h + r * std::cos(theta);
            int y = k + r * std::sin(theta);

            if ((x < M && x >= 0) && (y < N && y >= 0))
            {
                field[y][x] = 0;
            }
            theta += step;
        }

        theta = 0;
    }
    printf("SAIU\n");
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

    auto [wx, wy] = pbts::to_pair(wpoint);

    double xout = wx * dx - xbias;
    double yout = wy * dy - ybias;

    if (xout > (xT / 2))
    {
        xout = xT / 2;
    }
    else if (xout < (-xT / 2))
    {
        xout = -xT / 2;
    }

    if (yout > (yT / 2))
    {
        yout = yT / 2;
    }
    else if (yout < (-yT / 2))
    {
        xout = -yT / 2;
    }

    return {xout, yout};
}

auto pbts::Strategy::real_to_discreet(pbts::point point) -> pbts::wpoint
{
    auto [xin, yin] = pbts::to_pair(point);

    auto xnew = xin + xbias;
    auto ynew = yin + ybias;

    int iout = round(M * xnew / xT);
    int jout = round(N * ynew / yT);

    if (iout < 1)
    {
        iout = 1;
    }
    else if (iout > M)
    {
        iout = M;
    }

    if (jout < 1)
    {
        jout = 1;
    }
    else if (jout > N)
    {
        jout = N;
    }

    return {iout, jout};
}
/*
// Variáveis globais necessárias para discretização

    dx = 0.025; // resolução ao longo do eixo x - comprimento do campo [m]
    dy = 0.025; // resolução ao longo do eixo y - largura do campo [m]
    xtot = 2.0; // comprimento total do campo (extrapolado) [m]
    ytot = 1.5; // largura total do campo (extrapolada) [m]
    
    xbias = xtot/2; // compensação para deslocamento de origem
    ybias = ytot/2; // compensação para deslocamento de origem
    
    Nx = round(xtot/dx); // Número de posições discretas ao longo do eixo x
    Ny = round(ytot/dy); // Número de posições discretas ao longo do eixo y



//              saídas                              entradas
    function [xout, yout] = coords_discrete_to_real(iin, jin)
    
        xout = iin*dx - xbias;
        yout = jin*dy - ybias;
        
        if(xout > (xtot/2))
            xout = xtot/2;
        elseif(xout < (-xtot/2))
            xout = -xtot/2;
        end
        
        if(yout > (ytot/2))
            yout = ytot/2;
        elseif(yout < (-ytot/2))
            xout = -ytot/2;
        end
    end


    function [iout, jout] = coords_real_to_discrete(xin, yin)

        xnew = xin + xbias;
        ynew = yin + ybias;
        
        iout = round(Nx*xnew/xtot);
        jout = round(Ny*ynew/ytot);
            
        if(iout < 1)
            iout = 1;
        elseif(iout > Nx)
            iout = Nx;
        end
            
        if(jout < 1)
            jout = 1;
        elseif(jout > Ny)
            jout = Ny;
        end
    end

//  Precisa ser aplicada antes do cálculo do custo
    function [map] = add_obstacle_to_map(xobs, yobs, map)

        [iobs, jobs] = coords_real_to_discrete(xobs, yobs);
        
        iii = iobs - 1;
        iff = iobs + 1;
        jii = jobs - 1;
        jff = jobs + 1;
        
        if(iii < 1)
            iii = 1;
        elseif(iff > Nx)
            iff = Nx;
        end
        
        if(jii < 1)
            jii = 1;
        elseif(jff > Ny)
            jff = Ny;
        end
            
        for i = iii:iff
            for j = jii:jff
                map(i,j) = 1;
            end
        end
        
    end

    function [i_next, j_next] = calc_next_point(i_now, j_now, cost)

        cost(cost == 0) = 10000; // Zeros se tornam 10 mil.
        cost_ij = cost(i_now,j_now); // Custo na atual posição.
        
        i_next = i_now;
        j_next = j_now;
        
        if((i_now > 1) && (i_now < Nx))        
            if(cost(i_now-1,j_now) < cost_ij)
                i_next = i_now-1;
            elseif(cost(i_now+1,j_now) < cost_ij)
                i_next = i_now+1;
            end
        end

        if((j_now > 1) && (j_now < Ny))
            if(cost(i_now,j_now-1) < cost_ij)
                j_next = j_now-1;
            elseif(cost(i_now,j_now+1) < cost_ij)
                j_next = j_now+1;
            end
        end
    end


    function [cost] = calculate_cost(goal, xaxis, yaxis, logitMap)

    //  Create a cost map
        cost = zeros(size(logitMap));

    //  Find index of goal and current pos:
        [x, goali] = min(abs(xaxis - goal(1)));
        [y, goalj] = min(abs(yaxis - goal(2)));


        open = [goali goalj];
        cost(goali, goalj) = 1;
        adjacent = [ 1 0; 0 1; -1 0; 0 -1];  // immediate adjacents
        
        while size(open,1) ~= 0
        
            //  Iterate through cells adjacent to the cell at the top of the open queue:
            for k=1:size(adjacent,1)
            
                // Calculate index for current adjacent cell:
                adj = open(1,:)+adjacent(k,:);
                
                // Make sure adjacent cell is in the map
                if(min(adj) < 1)
                    continue
                end

                if(adj(1) > length(xaxis))
                    continue
                end

                if (adj(2) > length(yaxis))
                    continue
                end

                // Make sure the adjacent cell is not an obstacle 
                if(occ(adj(1), adj(2))) == 1)
                    continue
                end

                // Make sure the adjacent cell is not closed:
                if(cost(adj(1), adj(2)) ~= 0)
                    continue
                end

                // Set the cost and add the adjacent to the open set
                cost(adj(1), adj(2)) = cost(open(1,1), open(1,2)) + 1;
                open(size(open,1)+1,:) = adj;
            end

            //  Pop the top open cell from the queue
            open = open(2:end,:);
        end
    end


*/