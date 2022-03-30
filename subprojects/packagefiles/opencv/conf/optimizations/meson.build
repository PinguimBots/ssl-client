# Port of cmake/OpenCVCompilerOptimizations.cmake

cpu_opt_implies_ignore = false

cpu_opt_dir = src_root/'cmake'/'checks'

clang_or_gcc = cxxid == 'clang' or cxxid == 'gcc'
gcc_over_5   = cxxid == 'gcc' and cxx.version().version_compare('>= 5.0.0')

if cpu_family == 'x86' or cpu_family == 'x86_64'
    # Nested ternaries not allowed.
    if clang_or_gcc
        sse_flag = '-msse'
    elif cxxid == 'msvc' and cpu_family != 'x86_64'
        sse_flag = '/arch:SSE'
    else
        sse_flag = ''
    endif

    if clang_or_gcc
        sse2_flag = '-msse2'
    elif cxxid == 'msvc' and cpu_family != 'x86_64'
        sse2_flag = '/arch:SSE2'
    else
        sse2_flag = ''
    endif

    if gcc_over_5
        avx512f_flag = '-mavx512f'
    elif cxxid == 'msvc' and cpu_family == 'x86_64'
        avx512f_flag = '/arch:AVX512'
    else
        avx512f_flag = ''
    endif

    cpu_known_optimizations = {
        'SSE': {
            'test': files(cpu_opt_dir/'cpu_sse.cpp'),
            'flags': [sse_flag],
            'implies': [],
        },
        'SSE2': {
            'test': files(cpu_opt_dir/'cpu_sse2.cpp'),
            'flags': [sse2_flag],
            'implies': cpu_opt_implies_ignore ? [] : ['SSE'],
        },
        'SSE3': {
            'test': files(cpu_opt_dir/'cpu_sse3.cpp'),
            'flags': [clang_or_gcc ? '-msse3' : ''],
            'implies': cpu_opt_implies_ignore ? [] : ['SSE2'],
        },
        'SSSE3': {
            'test': files(cpu_opt_dir/'cpu_ssse3.cpp'),
            'flags': [clang_or_gcc ? '-mssse3' : ''],
            'implies': cpu_opt_implies_ignore ? [] : ['SSE3'],
        },
        'SSE4_1': {
            'test': files(cpu_opt_dir/'cpu_sse41.cpp'),
            'flags': [clang_or_gcc ? '-msse4.1' : ''],
            'implies': cpu_opt_implies_ignore ? [] : ['SSE3', 'SSSE3'],
        },
        'SSE4_2': {
            'test': files(cpu_opt_dir/'cpu_sse42.cpp'),
            'flags': [clang_or_gcc ? '-msse4.2' : ''],
            'implies': cpu_opt_implies_ignore ? [] : ['SSE4_1', 'POPCNT'],
        },
        'POPCNT': {
            'test': files(cpu_opt_dir/'cpu_popcnt.cpp'),
            'flags': [clang_or_gcc ? '-mpopcnt' : ''],
            'implies': cpu_opt_implies_ignore ? [] : ['SSE4_1'],
        },
        'FP16': {
            'test': files(cpu_opt_dir/'cpu_fp16.cpp'),
            'flags': [clang_or_gcc ? '-mf16c' : '/arch:AVX'],
            'implies': cpu_opt_implies_ignore ? [] : ['AVX'],
        },
        'FMA3': {
            'test': [],
            'flags': [clang_or_gcc ? '-mfma' : ''],
            'implies': cpu_opt_implies_ignore ? [] : ['AVX2'],
        },
        'AVX': {
            'test': files(cpu_opt_dir/'cpu_avx.cpp'),
            'flags': [clang_or_gcc ? '-mavx' : '/arch:AVX'],
            'implies': cpu_opt_implies_ignore ? [] : ['SSE4_2'],
        },
        'AVX2': {
            'test': files(cpu_opt_dir/'cpu_avx2.cpp'),
            'flags': [clang_or_gcc ? '-mavx2' : '/arch:AVX2'],
            'implies': cpu_opt_implies_ignore ? [] : ['AVX', 'FMA3', 'FP16'],
        },
        'AVX_512F': {
            'test': files(cpu_opt_dir/'cpu_avx512.cpp'),
            'flags': [avx512f_flag],
            'implies': cpu_opt_implies_ignore ? [] : ['AVX', 'FMA3', 'FP16'],
        },
        #'AVX512_COMMON': [],
        #'AVX512_KNL': [],
        #'AVX512_KNM': [],
        #'AVX512_SKX': [],
        #'AVX512_CNL': [],
        #'AVX512_CLX': [],
        #'AVX512_ICL': [],
    }

    cpu_dispatch = cpu_family == 'x86_64' \
        ? ['SSE4_1', 'SSE4_2', 'AVX', 'FP16', 'AVX2', 'AVX512_SKX'] \
        : ['SSE4_1', 'SSE4_2', 'AVX', 'FP16']

    cpu_baseline = cpu_family == 'x86_64' ? ['SSE3'] : ['SSE2']
endif

# TODO: implement group and implies logic from cmake/OpenCVCompilerOptimizations.cmake(451).

cpu_optimizations_supported = []
foreach optname, optinfo : cpu_known_optimizations
    if optinfo['test'].length() == 0 or optinfo['flags'].length() == 0
        cpu_optimizations_supported += optname
        if conf['print_optimizations']
            message('Optimization @0@ is supported (requires no flags or requires no tests)'.format(optname))
        endif
        continue
    endif

    ret = cxx.run(optinfo['test'], args: optinfo['flags'])
    if ret.compiled() and ret.returncode() == 0
        cpu_optimizations_supported += optname
        if conf['print_optimizations']
            message('Optimization @0@ is supported (tested)'.format(optname))
        endif
        continue
    endif

    message('Optimization @0@ is NOT supported')
endforeach

conf += {
    'cpu_dispatch': cpu_dispatch,
    'cpu_baseline': cpu_baseline,
    'cpu_optimizations_supported': cpu_optimizations_supported,
    'cpu_dispatch_final': ' '.join(cpu_optimizations_supported),
}

