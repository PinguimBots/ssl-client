#!/usr/bin/env sh

set -e

cd tools; sh setup_toolchain.sh; cd ..
. ./tools/toolchain.sourceme.sh

if [ ! -d "builddir" ]; then meson setup builddir "$@"; fi
