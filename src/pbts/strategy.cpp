#include "pbts/strategy.h"

pbts::Strategy::Strategy() {}

void pbts::Strategy::checkGameState(const fira_message::Robot &ally_robots, const fira_message::Robot &enemy_robots, const fira_message::Ball &ball) {
    int closest_ball;
    int ball_x = ball.x(), ball_y = ball.y();

    for (int i = 0; i < 3; ++i) {
        //Verificar as condições
    }
    //Situação de ataque ou defesa
    //Usar as demarcações do campo
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

    dist =  sqrt(pow((x-bx),2) + pow((y-by),2));

    
    return Robot.robot_id();
}
#endif

auto pbts::Strategy::createPath() -> std::tuple<double,double>
{

}

auto pbts::Strategy::wavePlanner() -> void
{

}

auto pbts::Strategy::bounds_set() -> bool
{
    //return bounds.has_value();
}

auto pbts::Strategy::set_bounds(pbts::field_geometry newbounds) -> void
{
    //bounds = newbounds;
}
