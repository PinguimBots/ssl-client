#include "pbts/strategy.hpp"

auto pbts::Strategy::generate_robot_positions(
    const pbts::field_geometry& field,
    const std::vector<pbts::robot>& allied_robots,
    const std::vector<pbts::robot>& enemy_robots,
    const pbts::ball& ball)
    -> std::vector< pbts::point >
{    
    int closest_ball;
    auto [ball_x, ball_y] = pbts::to_pair(ball.position);

    for (int i = 0; i < 3; ++i) {
        //Verificar as condições
    }
    //Situação de ataque ou defesa
    //Usar as demarcações do campo

    return {};
}

#if 0
int pbts::Strategy::pertoBola(const fira_message::Robot &ally_robots, const fira_message::Robot &enemy_robots, const fira_message::Ball &ball) {
   int pbts::strategy::pertoBola(std::vector<pbts::robot>& allied_robots, const std::vector<pbts::robot> &enemy_robots,  const pbts::ball& ball) {
    /*
        Verifica quem está mais perto da bola considerando se tem marcação ou não
        Caso houver marcação, definir critério de desempate 

     */
    /*
        Log de problemas:

        - Nas chamadas de to_pair ta dando um erro que não estou conseguindo resolver.

    */
    double x1,y1,x2,y2,by,bx,xe1,xe2,ye1,ye2;
    double dist_robot_bola1, dist_r1_enemy1, dist_r1_enemy2, dist_r2_enemy1, dist_robot_bola2, dist_r2_enemy2;
    
    [bx, by] = pbts::to_pair(ball.position);
    [x1,y1] = pbts::to_pair(allied_robots[1].position);
    [x2,y2] = pbts::to_pair(allied_robots[2].position);
    [xe1, ye1] = pbts::to_pair(enemy_robots[1].position);    
    [xe2, ye2] = pbts::to_pair(enemy_robots[2].position);

    
    dist_robot_bola1 =  sqrt(pow((x1-bx),2) + pow((y1-by),2));
    dist_r1_enemy1 = sqrt(pow((x1-xe1),2) + pow((y1-ye1),2));
    dist_r1_enemy2 = sqrt(pow((x1-xe2),2) + pow((y1-ye2),2));

    dist_robot_bola2 =  sqrt(pow((x2-bx),2) + pow((y2-by),2));
    dist_r2_enemy1 = sqrt(pow((x2-xe1),2) + pow((y2-ye1),2));
    dist_r2_enemy2 = sqrt(pow((x2-xe2),2) + pow((y2-ye2),2));

    if(dist_robot_bola1 > dist_robot_bola2)
        if ((dist_r1_enemy1 > dist_r2_enemy1) && (dist_r1_enemy2 > dist_r2_enemy2)) 
        {
            return 2;
        }
        else if((dist_r1_enemy1 < dist_r2_enemy1 && dist_r1_enemy1 > dist_r2_enemy2) || (dist_r1_enemy1 < dist_r2_enemy1 && dist_r1_enemy1 > dist_r2_enemy2))
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
#endif

auto pbts::Strategy::wavePlanner(
    const pbts::point goal_position,
    const pbts::robot &allied_robot,
    const std::vector<pbts::robot>& enemy_robots,
    const pbts::ball& ball)
    -> void
{
    int 
    int discreet_field[N][M]  = {-1};
    

}

auto 

auto pbts::Strategy::validNeighbors(int x, int y) -> std::vector<std::tuple<int,int>>
{

}

auto pbts::Strategy::fourNeighborhood(int x, int y) -> std::vector<std::tuple<int,int>>
{
    std::vector<std::tuple<int,int>> possibleMoves = {{x+1,y}, {x-1,y}, {x, y+1}, {x,y-1}};
    std::vector<std::tuple<int,int>> validMoves;

    for (auto& move : possibleMoves) {
        if ((std::get<0>(move) < xT && std::get<0>(move) >= 0) && (std::get<1>(move) < yT && std::get<1>(move) >= 0)) {
            validMoves.push_back(move);
        }
    }

    return validMoves;
}

auto pbts::Strategy::bounds_set() -> bool
{
    //return bounds.has_value();
}

auto pbts::Strategy::set_bounds(pbts::field_geometry newbounds) -> void
{
    //bounds = newbounds;
}

auto pbts::Strategy::create_path() -> pbts::point
{
    return {};
}
