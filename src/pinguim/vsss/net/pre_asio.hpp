// Include this before asio. Defines
// some stuff so asio doens't complain
// on windows.
#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <SDKDDKVer.h>
#endif
