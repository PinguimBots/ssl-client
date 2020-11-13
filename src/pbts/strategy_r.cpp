#include "pbts/strategy.hpp"
#include <iostream>

/* auto pbts::Strategy::generate_robot_positions(
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
} */

/* auto pbts::Strategy::pertoBola(std::vector<pbts::robot> &allied_robots, const std::vector<pbts::robot> &enemy_robots, const pbts::ball &ball) -> int
{
    
        // Verifica quem está mais perto da bola considerando se tem marcação ou não
        // Caso houver marcação, definir critério de desempate 



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
} */


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
    //printf("\n\nRobot id: %d  |  Orientation: %f\n\n ", robot.id, robot.orientation);

    if(robot.id == pbts::GOALKEEPER)
    {
        
        pbts::Strategy::isNear(robot.position, ball.position, 7.0e-2) 
        ? action = pbts::Strategy::kick(robot, ball)
        : action = pbts::Strategy::trackBallYAxix(robot, ball, team );

        
       
    }
    else if(robot.id == pbts::DEFENDER)
    {
       pbts::Strategy::isNear(robot.position, ball.position , 7.0e-2) 
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
                 pbts::Strategy::isNear(robot.position, ball.position, 6e-2) 
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
                 pbts::Strategy::isNear(robot.position, ball.position, 6e-2) 
                ? pbts::Strategy::isNear(robot.position, is_yellow
                                                        ? point(field.left_goal_bounds) 
                                                        : point(field.right_goal_bounds), 3e-1) 
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


        //action = {pbts::Strategy::create_path(new_point, robot, enemy_robots), flag};
       
   
    }
    
    return action;
}


auto pbts::Strategy::lin_pred(pbts::point point1, pbts::point point2, double x) -> pbts::point
{
    auto [x1, y1] = pbts::to_pair(point1);
    auto [x2, y2] = pbts::to_pair(point2);

    double k, y;

    k =  (x - x1)/(x2 - x1 + 1e-15);
    y = y1 + k*(y2-y1);

    return {x, y};
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
    //printf("%d kicking\n", robot.id);
    return pbts::Strategy::rotate(robot, ball);
}

bool pbts::Strategy::isNear(pbts::point point1, pbts::point point2, double tol)
{
    //printf("%lf \n ", std::abs((point2 - point1)));
    return (std::abs((point2 - point1)) < tol);
    
}

auto pbts::Strategy::trackBallYAxix(const pbts::robot& rbt, const pbts::ball& ball, double team) -> std::tuple<pbts::point, int>
{
    /*
    *Need to limit the goalkeeper's area
    */
    //printf("%d Tracking\n", rbt.id);

    pbts::point position;
    pbts::point predicted_position = lin_pred(old_point, 
                                     ball.position,
                                     rbt.id == pbts::DEFENDER
                                     ? team*DEFENDER_std_X
                                     : team*GOALKEEPER_std_X);

    if((predicted_position.imag() > GOAL_AREA_MIN
        && predicted_position.imag() <= GOAL_AREA_MAX)
        && rbt.id == pbts::GOALKEEPER)

    {position = predicted_position;}

    else
    {
        position = rbt.id == pbts::DEFENDER
            ? predicted_position //pbts:`:point(team*DEFENDER_std_X, new_y)
            : pbts::point(team*GOALKEEPER_std_X, std::clamp(ball.position.imag(), GOAL_AREA_MIN, GOAL_AREA_MAX));
    }

    old_point = ball.position;

    return {position,
            0};

}

auto pbts::Strategy::towardGoal(const pbts::robot& rbt, 
    const pbts::field_geometry& field, 
    double team) -> std::tuple<pbts::point, int>
{
    auto point = [](pbts::rect bound) {return (bound[0]+bound[1]+bound[2]+bound[3])/4.;};
    pbts::point new_point;

    //printf("%d Toward Goal\n", rbt.id);
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
            
    auto ball_diff = ball.position - old_point; 
    double dt_ahead = 3;
    pbts::point new_position;

    if (ball_diff.real() < 0)
    {
        new_position = lin_pred(old_point, ball.position, dt_ahead*ball_diff.real());
    }
    else new_position = ball.position - point(team*.075, 0.0);

    return {ball.position,
            0};
}

auto pbts::Strategy::setTeam(bool is_yellow) -> void
{
    this->is_yellow = is_yellow;
}

auto pbts::Strategy::setBounds(pbts::field_geometry bounds) -> void
{
    field_bounds = bounds;
}