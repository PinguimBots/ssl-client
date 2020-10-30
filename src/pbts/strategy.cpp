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
