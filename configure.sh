#!/usr/bin/env sh

meson setup builddir -Dsdl2:warning_level=0 -Dprotobuf:warning_level=0 "$@"
