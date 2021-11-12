#pragma once

#include <vector>
#include <tuple>
#include <math.h>

#include "pinguim/vsss/common.hpp"
#include "pinguim/cvt.hpp"

#define GOALKEEPER_std_Y 0
#define GOALKEEPER_std_X -0.682

#define GOAL_AREA_MIN -0.350
#define GOAL_AREA_MAX 0.350

#define DEFENDER_std_X -0.4
#define DEFENDER_std_Y 0.260

namespace pinguim::vsss
{
    class Strategy
    {
    public:
        auto create_path(
            const pinguim::vsss::point goal_position,
            const pinguim::vsss::robot &allied_robot,
            const std::vector<pinguim::vsss::point> &enemy_robots)
            -> pinguim::vsss::point;

        auto actions(
            const pinguim::vsss::robot &robot,
            const pinguim::vsss::ball &ball,
            const std::vector<pinguim::vsss::point> &enemy_robots) -> std::tuple<pinguim::vsss::point, int>;

        auto setTeam(bool isYellow) -> void;

        auto setBounds(pinguim::vsss::field_geometry bounds) -> void;

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

        static const int imax = 50;// Hardcoded for now. old => cvt::to<int> << std::round(xT / dx) + imin;
        static const int jmax = 38;// Hardcoded for now. old => cvt::to<int> << std::round(yT / dy) + jmin;

        int border_obstacle[imax][jmax];

        pinguim::vsss::point old_point = {0.0, 0.0};

        enum class ActionType
        {
            KICK,
            TOWARDGOAL,
            MOVETOBALL
        };

        pinguim::vsss::field_geometry field_bounds;

        bool is_yellow;
        double team;

        //Só pros scanf;
        //char c;


        /* Métodos de manipualção de valores discretos */

        // Verificação de vizinhança
        auto four_neighborhood(pinguim::vsss::wpoint point, int radius) -> std::vector<pinguim::vsss::wpoint>;
        auto d_neighborhood(pinguim::vsss::wpoint point, int radius) -> std::vector<pinguim::vsss::wpoint>;
        auto valid_neighbours(pinguim::vsss::wpoint point, int ntype, int radius) -> std::vector<pinguim::vsss::wpoint>;

        //Wave planner
        auto wave_planner(
            const pinguim::vsss::wpoint goal_position,
            const pinguim::vsss::wpoint allied_robot,
            const std::vector<pinguim::vsss::wpoint> &enemy_robots)
            -> pinguim::vsss::wpoint;

        auto wave_path(int (&field)[imax][jmax], const pinguim::vsss::wpoint goal, std::vector<std::vector<int>> &cost) -> void;

        // Populando a matriz
        auto generate_obstacle(int (&field)[imax][jmax], const std::vector<pinguim::vsss::wpoint> &enemy_robots) -> void;
        auto next_point(const pinguim::vsss::wpoint pos_now, const pinguim::vsss::wpoint goal, std::vector<std::vector<int>> &cost) -> pinguim::vsss::wpoint;
        auto add_clearance(int (&field)[imax][jmax], const pinguim::vsss::wpoint goal_position) -> void;
        auto add_shield_ball(int (&field)[imax][jmax], const pinguim::vsss::wpoint ball) -> void;

        auto init_border_obstacle_field() -> void;
        auto add_border_field_obstacle() -> void;

        /* Utilitários */
        auto discreet_to_real(pinguim::vsss::wpoint wpoint) -> pinguim::vsss::point;
        auto real_to_discreet(pinguim::vsss::point point) -> pinguim::vsss::wpoint;


        /* Métodos de manipualção de valores reais */
        auto rotate(const pinguim::vsss::robot &robot, const pinguim::vsss::ball &ball) -> std::tuple<pinguim::vsss::point, int>;

        auto trackBallYAxix(const pinguim::vsss::robot &robot, const pinguim::vsss::ball &ball) -> std::tuple<pinguim::vsss::point, int>;

        bool isNear(pinguim::vsss::point point1, pinguim::vsss::point point2, double tol);

        auto kick(const pinguim::vsss::robot &robot, const pinguim::vsss::ball &ball) -> std::tuple<pinguim::vsss::point, int>;

        auto moveBack(const pinguim::vsss::robot &robot) -> std::tuple<pinguim::vsss::point, int>;
        auto towardGoal(const pinguim::vsss::robot &robot) -> std::tuple<pinguim::vsss::point, int>;
        auto moveOntoBall(
            const pinguim::vsss::robot &robot,
            const pinguim::vsss::ball &ball)
            -> std::tuple<pinguim::vsss::point, int>;

        auto lin_pred(pinguim::vsss::point point1, pinguim::vsss::point point2, double x) -> pinguim::vsss::point;

        // Actions
        auto attacker_action(
            const pinguim::vsss::robot &robot,
            const pinguim::vsss::ball &ball,
            const std::vector<pinguim::vsss::point> &enemy_robots)
            -> std::tuple<pinguim::vsss::point, int>;

        auto goalkeeper_action(
            const pinguim::vsss::robot &robot,
            const pinguim::vsss::ball &ball)
            -> std::tuple<pinguim::vsss::point, int>;

        auto defender_action(
            const pinguim::vsss::robot &robot,
            const pinguim::vsss::ball &ball)
            -> std::tuple<pinguim::vsss::point, int>;
    };
} // namespace pinguim
