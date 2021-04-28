#pragma once

#include <vector>
#include <tuple>
#include <math.h>

#include "pinguim/common.hpp"

#define GOALKEEPER_std_Y 0
#define GOALKEEPER_std_X -0.682

#define GOAL_AREA_MIN -0.350
#define GOAL_AREA_MAX 0.350

#define DEFENDER_std_X -0.4
#define DEFENDER_std_Y 0.260

namespace pinguim
{
    class Strategy
    {
    public:
        auto create_path(
            const pinguim::point goal_position,
            const pinguim::robot &allied_robot,
            const std::vector<pinguim::point> &enemy_robots)
            -> pinguim::point;

        auto actions(
            const pinguim::robot &robot,
            const pinguim::ball &ball,
            const std::vector<pinguim::point> &enemy_robots) -> std::tuple<pinguim::point, int>;

        auto setTeam(bool isYellow) -> void;

        auto setBounds(pinguim::field_geometry bounds) -> void;

    private:
        static const constexpr auto xmin = -1.0;
        static const constexpr auto xmax = +1.0;
        static const constexpr auto xT = xmax - xmin; //m

        static const constexpr auto ymin = -0.75;
        static const constexpr auto ymax = +0.75;
        static const constexpr auto yT = ymax - ymin; //m

        static const constexpr auto dx = 0.04; //m
        static const constexpr auto dy = 0.04; //m

        static const constexpr auto imin = 0;
        static const constexpr auto jmin = 0;

        static const constexpr int imax = std::round(xT / dx) + imin;
        static const constexpr int jmax = std::round(yT / dy) + jmin;

        int border_obstacle[imax][jmax];

        pinguim::point old_point = {0.0, 0.0};

        enum class ActionType
        {
            KICK,
            TOWARDGOAL,
            MOVETOBALL
        };

        pinguim::field_geometry field_bounds;

        bool is_yellow;
        double team;

        //Só pros scanf;
        //char c;


        /* Métodos de manipualção de valores discretos */

        // Verificação de vizinhança
        auto four_neighborhood(pinguim::wpoint point, int radius) -> std::vector<pinguim::wpoint>;
        auto d_neighborhood(pinguim::wpoint point, int radius) -> std::vector<pinguim::wpoint>;
        auto valid_neighbours(pinguim::wpoint point, int ntype, int radius) -> std::vector<pinguim::wpoint>;

        //Wave planner
        auto wave_planner(
            const pinguim::wpoint goal_position,
            const pinguim::wpoint allied_robot,
            const std::vector<pinguim::wpoint> &enemy_robots)
            -> pinguim::wpoint;

        auto wave_path(int (&field)[imax][jmax], const pinguim::wpoint goal, std::vector<std::vector<int>> &cost) -> void;

        // Populando a matriz
        auto generate_obstacle(int (&field)[imax][jmax], const std::vector<pinguim::wpoint> &enemy_robots) -> void;
        auto next_point(const pinguim::wpoint pos_now, const pinguim::wpoint goal, std::vector<std::vector<int>> &cost) -> pinguim::wpoint;
        auto add_clearance(int (&field)[imax][jmax], const pinguim::wpoint goal_position) -> void;
        auto add_shield_ball(int (&field)[imax][jmax], const pinguim::wpoint ball) -> void;

        auto init_border_obstacle_field() -> void;
        auto add_border_field_obstacle() -> void;

        /* Utilitários */
        auto discreet_to_real(pinguim::wpoint wpoint) -> pinguim::point;
        auto real_to_discreet(pinguim::point point) -> pinguim::wpoint;


        /* Métodos de manipualção de valores reais */
        auto rotate(const pinguim::robot &robot, const pinguim::ball &ball) -> std::tuple<pinguim::point, int>;

        auto trackBallYAxix(const pinguim::robot &robot, const pinguim::ball &ball) -> std::tuple<pinguim::point, int>;

        bool isNear(pinguim::point point1, pinguim::point point2, double tol);

        auto kick(const pinguim::robot &robot, const pinguim::ball &ball) -> std::tuple<pinguim::point, int>;

        auto moveBack(const pinguim::robot &robot) -> std::tuple<pinguim::point, int>;
        auto towardGoal(const pinguim::robot &robot) -> std::tuple<pinguim::point, int>;
        auto moveOntoBall(
            const pinguim::robot &robot,
            const pinguim::ball &ball)
            -> std::tuple<pinguim::point, int>;

        auto lin_pred(pinguim::point point1, pinguim::point point2, double x) -> pinguim::point;

        // Actions
        auto attacker_action(
            const pinguim::robot &robot,
            const pinguim::ball &ball,
            const std::vector<pinguim::point> &enemy_robots)
            -> std::tuple<pinguim::point, int>;

        auto goalkeeper_action(
            const pinguim::robot &robot,
            const pinguim::ball &ball)
            -> std::tuple<pinguim::point, int>;

        auto defender_action(
            const pinguim::robot &robot,
            const pinguim::ball &ball)
            -> std::tuple<pinguim::point, int>;
    };
} // namespace pinguim
