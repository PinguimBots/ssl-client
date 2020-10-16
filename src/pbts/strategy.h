#pragma once

namespace pbts {

    class Strategy
    {
        enum class goal_bound_type {
            left,
            right
        };

    public:
        Strategy();
        int pertoBola();

        //auto tick(auto allied_team, goal_bound_type allied_target, auto enemy_team);

    private:
        
    };

}
