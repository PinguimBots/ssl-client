mod_core_sources = []

# Add extra configure files needed by mod_core.
subdir('conf')

# Add the sources proper.
mod_core_sources += files(fs.read('sources.txt').split('\n'))
mod_core_sources += files(fs.read('internal_headers.txt').split('\n'))

# TODO: port this from cmake, remove all the optimizations below if those are set
# if(CV_DISABLE_OPTIMIZATION OR NOT CV_ENABLE_INTRINSICS)
#    set(__optimizations "")
# endif()
# Add the dispatched files to sources
mod_core_sources += custom_target(
    'mod_core_dispatched',
    command: [
        import('python').find_installation('python3'),
        make_dispatched_files,
        meson.current_source_dir() / 'src',
        '@OUTDIR@',
        conf['cpu_dispatch_final'],
        'precomp.hpp',
        'mathfuncs_core SSE2 AVX AVX2',
        'stat SSE4_2 AVX2',
        'arithm SSE2 SSE4_1 AVX2 VSX3',
        'convert SSE2 AVX2 VSX3',
        'convert_scale SSE2 AVX2',
        'count_non_zero SSE2 AVX2',
        'matmul SSE2 SSE4_1 AVX2 AVX512_SKX',
        'mean SSE2 AVX2',
        'merge SSE2 AVX2',
        'split SSE2 AVX2',
        'sum SSE2 AVX2',
    ],
    output: [
        'mathfuncs_core.sse2',                                 'mathfuncs_core.avx', 'mathfuncs_core.avx2',                                       'mathfuncs_core.simd_declarations',
                                                'stat.sse4_2',                       'stat.avx2',                                                 'stat.simd_declarations',
        'arithm.sse2',         'arithm.sse4_1',                                      'arithm.avx2',          'arithm.vsx3',                       'arithm.simd_declarations',
        'convert.sse2',                                                              'convert.avx2',         'convert.vsx3',                      'convert.simd_declarations',
        'convert_scale.sse2',                                                        'convert_scale.avx2',                                        'convert_scale.simd_declarations',
        'count_non_zero.sse2',                                                       'count_non_zero.avx2',                                       'count_non_zero.simd_declarations',
        'matmul.sse2',         'matmul.sse4_1',                                      'matmul.avx2',                          'matmul.avx512_skx', 'matmul.simd_declarations',
        'mean.sse2',                                                                 'mean.avx2',                                                 'mean.simd_declarations',
        'merge.sse2',                                                                'merge.avx2',                                                'merge.simd_declarations',
        'split.sse2',                                                                'split.avx2',                                                'split.simd_declarations',
        'sum.sse2',                                                                  'sum.avx2',                                                  'sum.simd_declarations',
    ],
)

# Add the version_string.inc generated include.
mod_core_sources += custom_target(
    'mod_core_version_string_inc',
    command: [import('python').find_installation('python3'), make_build_string_inc],
    capture: true,
    output: 'version_string.inc',
)

# Add the opencl kernel generated using cl2cpp.
mod_core_sources += custom_target(
    'mod_core_generated_opencl_kernels',
    command: [
        import('python').find_installation('python3'),
        cl2cpp,
        '@OUTDIR@',
        'opencl_kernels_core',
        files(fs.read('opencl_kernels.txt').split('\n'))
    ],
    output: ['opencl_kernels_core.cpp', 'opencl_kernels_core.hpp'],
)

mod_core = {
    'lib': library(
        'opencv_core',
        mod_core_sources,
        include_directories: [include_directories('include', 'src', 'conf'), conf['gen']['includes']],
        cpp_args: conf['module_build_flags'],
        dependencies: dependency('zlib', fallback: ['zlib', 'zlib_dep']),
    ),
    'exported_includes': [
        include_directories('include'),
        conf['gen']['includes'],  # Since we need users to have 'opencv2/opencv_modules.hpp' findable.
    ],
}

modules_built += {'core': mod_core}
