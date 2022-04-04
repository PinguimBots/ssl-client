#!/usr/bin/env sh

# TODO: Commented out until the script is fully fleshed out.
#cd tooling && ./setup_tooling.sh && cd ..
#. $(pwd)/tooling/tooling.sh

if [ ! -d "builddir" ]; then meson setup builddir "$@"; fi
