#pragma once

#include <vector>
#include <tuple>
#include <math.h>

#include "pbts/common.hpp"


#define GOALKEEPER_std_Y 0
#define GOALKEEPER_std_X -0.67

#define GOAL_AREA_MIN -0.350
#define GOAL_AREA_MAX 0.350

#define DEFENDER_std_X -0.4
#define DEFENDER_std_Y 0.260

namespace pbts
{
    enum roles {GOALKEEPER, DEFENDER, ATTACKER};

    class Strategy
    {
    public:
        // Generates the final position for each allied robot.
        auto generate_robot_positions(
            const pbts::field_geometry &field,
            const std::vector<pbts::robot> &allied_robots,
            const std::vector<pbts::robot> &enemy_robots,
            const pbts::ball &ball /*,
        const pbts::game_info& game_state*/
            )
            -> std::vector<pbts::point>;

        //static const constexpr auto dt = 50e-3;
        static const constexpr auto tmax = 10;
        static const constexpr auto telap = 0;

        static const constexpr auto xmin = -0.0;
        static const constexpr auto xmax = +2.0;
        static const constexpr auto xT = xmax - xmin; //cm

        static const constexpr auto ymin = -0.0;
        static const constexpr auto ymax = +2.0;
        static const constexpr auto yT = ymax - ymin; //cm

        static const constexpr auto dx = 0.05d;   //cm
        static const constexpr auto dy = 0.05d;   //cm

        static const constexpr int M = std::round(xT/dx);
        static const constexpr int N = std::round(yT/dy);

        auto create_path(
            const pbts::point goal_position,
            const pbts::robot &allied_robot,
            const std::vector<pbts::point> &enemy_robots)
            -> pbts::point;
        
        auto actions(
            const pbts::field_geometry& field,
            const pbts::robot& robot,
            const pbts::ball& ball,
            const std::vector<pbts::point> &enemy_robots,
            double team
        ) -> std::tuple<pbts::point, int>;

        auto kick(const pbts::robot& robot, const pbts::ball& ball) -> std::tuple<pbts::point, int>;

    private:
        bool isNear(pbts::point point1, pbts::point point2, double tol);
        auto four_neighborhood(pbts::wpoint point) -> std::vector<pbts::wpoint>;
        auto d_neighborhood(pbts::wpoint point) -> std::vector<pbts::wpoint>;
        auto valid_neighbours(pbts::wpoint point, int ntype) -> std::vector<pbts::wpoint>;
        auto wave_planner(
            const pbts::wpoint goal_position,
            const pbts::wpoint allied_robot,
            const std::vector<pbts::wpoint> &enemy_robots)
            -> pbts::wpoint;
        auto generate_obstacle(int (&field)[N][M], const std::vector<pbts::wpoint> &enemy_robots) -> void;
        auto wave_path(int (&field)[N][M], const pbts::wpoint goal) -> void;
        auto pertoBola(std::vector<pbts::robot>& allied_robots, const std::vector<pbts::robot> &enemy_robots,  const pbts::ball& ball) -> int;
        auto discreet_to_real(pbts::wpoint wpoint) -> pbts::point;
        auto real_to_discreet(pbts::point point) -> pbts::wpoint;
        auto rotate(const pbts::robot& robot, const pbts::ball& ball) -> std::tuple<pbts::point, int>;
       
        auto trackBallYAxix(const pbts::robot& rbt, const pbts::ball& ball, double team) -> std::tuple<pbts::point, int>;

        auto moveBack(const pbts::robot& rbt) -> std::tuple<pbts::point, int>;
        auto towardGoal(const pbts::robot& rbt, const pbts::field_geometry& field, double team) -> std::tuple<pbts::point, int>;
        auto moveOntoBall(const pbts::robot& rbt, const pbts::ball& ball) -> std::tuple<pbts::point, int>;


    };
} // namespace pbts
