#!/usr/bin/env sh

. ./configure.sh

meson test -C builddir "$@"
