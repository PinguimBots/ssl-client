#pragma once

#include <tuple>

#include "pb/command.pb.h"
#include "pb/common.pb.h"
#include "pb/packet.pb.h"
#include "pb/replacement.pb.h"

namespace pbts {

    class Control
    {
    public:
        std::tuple<double,double> generateVels(const fira_message::Robot &robot, const fira_message::Ball &ball);
    };

}
