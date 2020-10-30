#include "pbts/strategy.hpp"

auto pbts::strategy::generate_robot_positions(
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

auto pbts::strategy::create_path() -> pbts::point
{
    return {};
}
