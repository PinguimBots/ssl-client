#include "pinguim/conf.hpp"

#if defined(PINGUIM_CONF_OPENCV_SUPPORT)
#include "pinguim/app/subsystems/input/vision_enabled.cpp.inc"
#else
#include "pinguim/app/subsystems/input/vision_disabled.cpp.inc"
#endif