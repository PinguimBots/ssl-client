#ifndef CONTROL_H
#define CONTROL_H

#include "pb/command.pb.h"
#include "pb/common.pb.h"
#include "pb/packet.pb.h"
#include "pb/replacement.pb.h"


class Control
{

private:


public:
    Control();

    void generateVels(double &velL, double &velR, fira_message:: Robot &robot, fira_message::Ball ball);
};

#endif // CONTROL_H
