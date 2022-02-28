#include "pinguim/conf.hpp"

#if defined(PINGUIM_CONF_OPENCV_SUPPORT)
#include "pinguim/app/subsystems/input/vision_enabled.inc.cpp"
#else
#include "pinguim/app/subsystems/input/vision_disabled.inc.cpp"
#endif