#pragma once

#include <tuple>

#include "pb/command.pb.h"
#include "pb/common.pb.h"
#include "pb/packet.pb.h"
#include "pb/replacement.pb.h"

class Control
{
public:
    Control();

    std::tuple<double,double> generateVels(const fira_message::Robot &robot, const fira_message::Ball &ball);
};
