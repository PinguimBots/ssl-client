#!/usr/bin/env sh

if [ ! -d "builddir" ]
then
    ./configure.sh
fi

meson test -C builddir "$@"
