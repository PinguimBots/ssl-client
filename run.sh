#!/usr/bin/env sh

. $(pwd)/configure.sh

meson compile -C builddir && ./builddir/pbssl "$@"
