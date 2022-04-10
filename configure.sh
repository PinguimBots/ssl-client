#!/usr/bin/env sh

set -e

cd tools
    touch logs/setup_toolchain.txt
    sh setup_toolchain.sh --resume-last-call | tee logs/setup_toolchain.txt
cd ..
. ./tools/toolchain.sourceme.sh

if [ ! -d "builddir" ]; then meson setup builddir "$@"; fi
