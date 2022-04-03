# Does all the configuration work, like setting
# the HAVE_*, detecting compiler configurations, build flags
# and what modules to build.

shush_flag = get_option('shush') ? cxx.get_supported_arguments(['-w', '/w']) : []
enable_nonfree = get_option('enable_nonfree')

conf = {
    'print_optimizations': get_option('print_optimizations'),
    'print_conf': get_option('print_conf'),
    'non_printable_keys': ['gen'],

    'shush': get_option('shush'),
    'shush_flag': shush_flag,

    'enable_nonfree': enable_nonfree,
    'module_build_flags': [] \
        + ['-D__OPENCV_BUILD', '-Wno-deprecated-enum-enum-conversion'] \
        + shush_flag \
        + (enable_nonfree ? ['-DOPENCV_ENABLE_NONFREE'] : []),

    # Equivalent to BUILD_opencv_* in the cmake build.
    # TODO: figure out the logic to select what modules are built.
    'modules_to_build': ['core', 'imgproc', 'videoio']
}

subdir('optimizations')

## Config file generation (cvconfig.h, opencv_modules.hpp, ...)
# Keep in mind that the folder structure is replicated, thats why
# *.hpp.in must be in that separate folder.
subdir('opencv2')

## Printing <conf>
if conf['print_conf']
    foreach k : conf.keys()
        if k not in conf['non_printable_keys']
            message('conf[@0@]: @1@'.format(k, conf[k]))
        else
            message('conf[@0@] <NON_PRINTABLE>'.format(k))
        endif
    endforeach
endif
