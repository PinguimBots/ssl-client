# Python implementation of __ocv_add_dispatched_file @ cmake/OpenCVCompilerOptimizations.cmake

import sys
import os

def main():
    srcdir           = sys.argv[1]
    outdir           = sys.argv[2]
    cpu_dispatch_all = sys.argv[3]
    precomp_hpp      = sys.argv[4] # Just the name, no path.
    dispatched_files = sys.argv[5:]

    cpu_dispatch_all = list( map( lambda x: x.strip(), cpu_dispatch_all.split(' ') ) )

    if not os.path.exists(outdir): os.makedirs(outdir)

    for dispatched_file in dispatched_files:
        parts = dispatched_file.split(' ')

        name          = parts[0].strip()
        optimizations = list( map( lambda x: x.strip(), parts[1:] ) )

        codestr = '' \
            + f'#include "{srcdir}/{precomp_hpp}"\n' \
            + f'#include "{srcdir}/{name}.simd.hpp"\n\n'

        declarations_str = f'#define CV_CPU_SIMD_FILENAME "{srcdir}/{name}.simd.hpp"\n'
        dispatch_modes = "BASELINE"

        for opt in optimizations:
            opt_lower = opt.lower()

            with open(f"{outdir}/{name}.{opt_lower}.cpp", 'w') as f: f.write(codestr)

            if opt not in cpu_dispatch_all: continue

            declarations_str += '' \
                + f'#define CV_CPU_DISPATCH_MODE {opt}\n' \
                + '#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"\n'

            dispatch_modes = f'{opt}, {dispatch_modes}'

        declarations_str += '' \
            + f'#define CV_CPU_DISPATCH_MODES_ALL {dispatch_modes}\n\n' \
            + '#undef CV_CPU_SIMD_FILENAME\n'

        with open(f'{outdir}/{name}.simd_declarations.hpp', 'w') as f: f.write(declarations_str)

if __name__ == '__main__': main()