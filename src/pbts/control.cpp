#include "control.h"
#include <math.h>


std::tuple<double,double> pbts::Control::generateVels(const fira_message::Robot &robot, const fira_message::Ball &ball) {

    double x, y, orientation, vx, vy, vorientation;
    double ball_x, ball_y, ball_vx, ball_vy;

    x = robot.x() + 0.75;
    y = robot.y() + 0.65;
    orientation = robot.orientation();
    vx = robot.vx();
    vy = robot.vy();
    vorientation = robot.vorientation();

    ball_x = ball.x() + 0.75;
    ball_y = ball.y() + 0.65;
    ball_vx = ball.vx();
    ball_vy = ball.vy();

    double xDif, yDif, ballAngle, hypotenuse;

    xDif = ball_x - x;
    yDif = ball_y - y;

    ballAngle = atan2(yDif, xDif); // Angulo robô -> bola

    hypotenuse = std::sqrt(xDif + yDif); //Distancia linha reta robo -> bola

    double angleError = 0.0, target_angle = 0.0;

    if (x < ball_x) {
        target_angle = ballAngle;
    } else {
        target_angle = M_PI + ballAngle;
    }

    angleError = target_angle - orientation;

    if (angleError > (2 * M_PI)) {
        angleError = fmod(angleError, (2 * M_PI));
    }
    if (angleError < (-2 * M_PI)) {
        angleError = fmod(angleError, (-2 * M_PI));
    }
    if (angleError > M_PI) {
        angleError = -(2 * M_PI - angleError);
    } else if (angleError < (-M_PI)) {
        angleError = +(angleError + 2 * M_PI);
    }

    double robot_angle_error = 0.0, linvel_left = 0.0, linvel_right = 0.0;
    const double kap = 1.0, kad = 0.2, velmax = 5.0, velmin = 0.5, vel = 1.0;

    linvel_left = +kap * angleError - kad * robot_angle_error + vel;

    if (linvel_left > velmax) {
        linvel_left = velmax;
    } else if (linvel_left < velmin) {
        linvel_left = velmin;
    }

    linvel_right = -kap * angleError + kad * robot_angle_error + vel;

    if (linvel_right > velmax) {
        linvel_right = velmax;
    } else if (linvel_right < velmin) {
        linvel_right = velmin;
    }
    
    return {linvel_left, linvel_right};
}
