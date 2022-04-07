#!/usr/bin/env sh

. ./configure.sh

meson compile -C builddir && ./builddir/pbssl "$@"
