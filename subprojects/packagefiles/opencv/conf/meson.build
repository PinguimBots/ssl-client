# Does all the configuration work, like setting
# the HAVE_* flags, detecting compiler configurations, build flags
# and figure out what modules to build (and in what order).

conf = {}

# The opencv cmake visits each module twice, once to
# explore all the available modules and resolve their
# dependencies, and the second time to actually build them.
# We can only visit a folder once in meson, so we need to resolve
# the dependencies upfront.
subdir('depend')

shush_flag = get_option('shush') ? cxx.get_supported_arguments(['-w', '/w']) : []
enable_nonfree = get_option('enable_nonfree')

conf += {
    'print_optimizations': get_option('print_optimizations'),
    'print_conf': get_option('print_conf'),
    # Internal use, to avoid meson freaking out.
    'non_printable_keys': ['gen'],

    'shush': get_option('shush'),
    'shush_flag': shush_flag,

    'enable_nonfree': enable_nonfree,
    'module_build_flags': [] \
        + ['-D__OPENCV_BUILD', '-Wno-deprecated-enum-enum-conversion'] \
        + shush_flag \
        + (enable_nonfree ? ['-DOPENCV_ENABLE_NONFREE'] : []),
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
