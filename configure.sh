#!/usr/bin/env sh

meson setup builddir "$@" -Dopencv:print_conf=true -Dopencv:shush=false -Dopencv:modules=core,imgproc,videoio
