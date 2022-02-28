#pragma once

#include "pinguim/conf.hpp"

#if defined(PINGUIM_CONF_OPENCV_SUPPORT)
#include "pinguim/app/subsystems/input/vision_enabled.inc.hpp"
#else
#include "pinguim/app/subsystems/input/vision_disabled.inc.hpp"
#endif
