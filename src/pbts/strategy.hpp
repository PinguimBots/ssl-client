#pragma once

#include <vector>
#include <tuple>
#include <math.h>

#include "pbts/common.hpp"

#define GOALKEEPER_std_Y 0
#define GOALKEEPER_std_X -0.682

#define GOAL_AREA_MIN -0.350
#define GOAL_AREA_MAX 0.350

#define DEFENDER_std_X -0.4
#define DEFENDER_std_Y 0.260

namespace pbts
{
    class Strategy
    {
    public:
        auto create_path(
            const pbts::point goal_position,
            const pbts::robot &allied_robot,
            const std::vector<pbts::point> &enemy_robots)
            -> pbts::point;

        auto actions(
            const pbts::field_geometry &field,
            const pbts::robot &robot,
            const pbts::ball &ball,
            const std::vector<pbts::point> &enemy_robots) -> std::tuple<pbts::point, int>;

        auto setTeam(bool isYellow) -> void;

        auto setBounds(pbts::field_geometry bounds) -> void;

    private:
        static const constexpr auto xmin = -1.0;
        static const constexpr auto xmax = +1.0;
        static const constexpr auto xT = xmax - xmin; //m

        static const constexpr auto ymin = -0.75;
        static const constexpr auto ymax = +0.75;
        static const constexpr auto yT = ymax - ymin; //m

        static const constexpr auto dx = 0.04d; //m
        static const constexpr auto dy = 0.04d; //m

        static const constexpr auto imin = 0;
        static const constexpr auto jmin = 0;

        static const constexpr int imax = std::round(xT / dx) + imin;
        static const constexpr int jmax = std::round(yT / dy) + jmin;

        pbts::point old_point = {0.0, 0.0};

        enum class ActionType
        {
            KICK,
            TOWARDGOAL,
            MOVETOBALL
        };

        pbts::field_geometry field_bounds;

        bool is_yellow;
        double team;

        //Só pros scanf;
        // char c;

        bool isNear(pbts::point point1, pbts::point point2, double tol);

        auto kick(const pbts::robot &robot, const pbts::ball &ball) -> std::tuple<pbts::point, int>;

        // Verificação de vizinhança
        auto four_neighborhood(pbts::wpoint point, int radius) -> std::vector<pbts::wpoint>;
        auto d_neighborhood(pbts::wpoint point, int radius) -> std::vector<pbts::wpoint>;
        auto valid_neighbours(pbts::wpoint point, int ntype, int radius) -> std::vector<pbts::wpoint>;

        //Wave planner
        auto wave_planner(
            const pbts::wpoint goal_position,
            const pbts::wpoint allied_robot,
            const std::vector<pbts::wpoint> &enemy_robots)
            -> pbts::wpoint;

        auto wave_path(int (&field)[imax][jmax], const pbts::wpoint goal, std::vector<std::vector<int>> &cost) -> void;

        // Populando a matriz
        auto generate_obstacle(int (&field)[imax][jmax], const std::vector<pbts::wpoint> &enemy_robots) -> void;
        auto next_point(const pbts::wpoint pos_now, const pbts::wpoint goal, std::vector<std::vector<int>> &cost) -> pbts::wpoint;
        auto add_clearance(int (&field)[imax][jmax], const pbts::wpoint goal_position) -> void;
        auto add_shield_ball(int (&field)[imax][jmax], const pbts::wpoint ball) -> void;

        auto discreet_to_real(pbts::wpoint wpoint) -> pbts::point;
        auto real_to_discreet(pbts::point point) -> pbts::wpoint;

        auto rotate(const pbts::robot &robot, const pbts::ball &ball) -> std::tuple<pbts::point, int>;

        auto trackBallYAxix(const pbts::robot &robot, const pbts::ball &ball) -> std::tuple<pbts::point, int>;

        auto moveBack(const pbts::robot &robot) -> std::tuple<pbts::point, int>;
        auto towardGoal(const pbts::robot &robot) -> std::tuple<pbts::point, int>;
        auto moveOntoBall(
            const pbts::robot &robot,
            const pbts::ball &ball) 
            -> std::tuple<pbts::point, int>;
            
        auto lin_pred(pbts::point point1, pbts::point point2, double x) -> pbts::point;

        // Actions
        auto attacker_action(
            const pbts::robot &robot,
            const pbts::ball &ball,
            const std::vector<pbts::point> &enemy_robots)
            -> std::tuple<pbts::point, int>;

        auto goalkeeper_action(
            const pbts::robot &robot,
            const pbts::ball &ball)
            -> std::tuple<pbts::point, int>;

        auto defender_action(
            const pbts::robot &robot,
            const pbts::ball &ball)
            -> std::tuple<pbts::point, int>;
    };
} // namespace pbts
