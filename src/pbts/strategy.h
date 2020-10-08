#pragma once

#include "pb/command.pb.h"
#include "pb/common.pb.h"
#include "pb/packet.pb.h"
#include "pb/replacement.pb.h"

class Strategy
{
public:
    Strategy(bool isYellow);
    int pertoBola();

private:
    const bool _isYellow;
};
