#!/usr/bin/env sh

. $(pwd)/configure.sh

meson test -C builddir "$@"
