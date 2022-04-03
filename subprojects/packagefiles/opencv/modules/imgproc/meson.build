mod_imgproc_sources = []

# Add the sources proper
mod_imgproc_sources += files(fs.read('sources.txt').split('\n'))
mod_imgproc_sources += files(fs.read('internal_headers.txt').split('\n'))

# Add the dispatched files to sources
mod_imgproc_sources += custom_target(
    'mod_imgproc_dispatched',
    command: [
        import('python').find_installation('python3'),
        make_dispatched_files,
        meson.current_source_dir() / 'src',
        '@OUTDIR@',
        conf['cpu_dispatch_final'],
        'precomp.hpp',
        'accum SSE4_1 AVX AVX2',
        'bilateral_filter SSE2 AVX2',
        'box_filter SSE2 SSE4_1 AVX2',
        'filter SSE2 SSE4_1 AVX2',
        'color_hsv SSE2 SSE4_1 AVX2',
        'color_rgb SSE2 SSE4_1 AVX2',
        'color_yuv SSE2 SSE4_1 AVX2',
        'median_blur SSE2 SSE4_1 AVX2',
        'morph SSE2 SSE4_1 AVX2',
        'smooth SSE2 SSE4_1 AVX2',
        'sumpixels SSE2 AVX2 AVX512_SKX',
    ],
    output: [
        'accum.simd_declarations.hpp', 'accum.sse4_1.cpp', 'accum.avx.cpp', 'accum.avx2.cpp',
        'bilateral_filter.simd_declarations.hpp', 'bilateral_filter.sse2.cpp', 'bilateral_filter.avx2.cpp',
        'box_filter.simd_declarations.hpp', 'box_filter.sse2.cpp', 'box_filter.sse4_1.cpp', 'box_filter.avx2.cpp',
        'filter.simd_declarations.hpp', 'filter.sse2.cpp', 'filter.sse4_1.cpp', 'filter.avx2.cpp',
        'color_hsv.simd_declarations.hpp', 'color_hsv.sse2.cpp', 'color_hsv.sse4_1.cpp', 'color_hsv.avx2.cpp',
        'color_rgb.simd_declarations.hpp', 'color_rgb.sse2.cpp', 'color_rgb.sse4_1.cpp', 'color_rgb.avx2.cpp',
        'color_yuv.simd_declarations.hpp', 'color_yuv.sse2.cpp', 'color_yuv.sse4_1.cpp', 'color_yuv.avx2.cpp',
        'median_blur.simd_declarations.hpp', 'median_blur.sse2.cpp', 'median_blur.sse4_1.cpp', 'median_blur.avx2.cpp',
        'morph.simd_declarations.hpp', 'morph.sse2.cpp', 'morph.sse4_1.cpp', 'morph.avx2.cpp',
        'smooth.simd_declarations.hpp', 'smooth.sse2.cpp', 'smooth.sse4_1.cpp', 'smooth.avx2.cpp',
        'sumpixels.simd_declarations.hpp', 'sumpixels.sse2.cpp', 'sumpixels.avx2.cpp', 'sumpixels.avx512_skx.cpp',
    ],
)

# Add the opencl kernel generated using cl2cpp.
mod_imgproc_sources += custom_target(
    'mod_imgproc_generated_opencl_kernels',
    command: [
        import('python').find_installation('python3'),
        cl2cpp,
        '@OUTDIR@',
        'opencl_kernels_imgproc',
        files(fs.read('opencl_kernels.txt').split('\n'))
    ],
    output: ['opencl_kernels_imgproc.cpp', 'opencl_kernels_imgproc.hpp'],
)

mod_imgproc = {
    'lib': library(
        'opencv_imgproc',
        mod_imgproc_sources,
        include_directories: include_directories('include', 'src'),
        cpp_args: conf['module_build_flags'],
        dependencies: mod_core_dep,
    ),
    'exported_includes': include_directories('include'),
}

modules_built += {'imgproc': mod_imgproc}