#include "pinguim/scoped.hpp"

pinguim::scoped::~scoped()
{ if(is_armed) { on_exit(); } }