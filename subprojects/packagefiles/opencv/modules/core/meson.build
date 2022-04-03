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
        'mathfuncs_core.simd_declarations.hpp', 'mathfuncs_core.sse2.cpp',                                         'mathfuncs_core.avx.cpp', 'mathfuncs_core.avx2.cpp',
        'stat.simd_declarations.hpp',                                                           'stat.sse4_2.cpp',                           'stat.avx2.cpp',
        'arithm.simd_declarations.hpp',         'arithm.sse2.cpp',         'arithm.sse4_1.cpp',                                              'arithm.avx2.cpp',          'arithm.vsx3.cpp',
        'convert.simd_declarations.hpp',        'convert.sse2.cpp',                                                                          'convert.avx2.cpp',         'convert.vsx3.cpp',
        'convert_scale.simd_declarations.hpp',  'convert_scale.sse2.cpp',                                                                    'convert_scale.avx2.cpp',
        'count_non_zero.simd_declarations.hpp', 'count_non_zero.sse2.cpp',                                                                   'count_non_zero.avx2.cpp',
        'matmul.simd_declarations.hpp',         'matmul.sse2.cpp',         'matmul.sse4_1.cpp',                                              'matmul.avx2.cpp',                              'matmul.avx512_skx.cpp',
        'mean.simd_declarations.hpp',           'mean.sse2.cpp',                                                                             'mean.avx2.cpp',
        'merge.simd_declarations.hpp',          'merge.sse2.cpp',                                                                            'merge.avx2.cpp',
        'split.simd_declarations.hpp',          'split.sse2.cpp',                                                                            'split.avx2.cpp',
        'sum.simd_declarations.hpp',            'sum.sse2.cpp',                                                                              'sum.avx2.cpp',
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

mod_core_dep = declare_dependency(
    link_with: mod_core['lib'],
    include_directories: mod_core['exported_includes'],
)

modules_built += {'core': mod_core}
