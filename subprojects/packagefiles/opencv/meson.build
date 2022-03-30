# Scope: Build opencv on x86_64 linux and windows using
# GCC, Clang or MSVC. Possible performance difference alowed.
project('opencv', ['cpp', 'c'], version: '4.5.5', default_options: 'warning_level=0')

# Used pretty much everywhere.
os         = host_machine.system()
cpu_family = host_machine.cpu_family()
cxx        = meson.get_compiler('cpp')
cxxid      = cxx.get_id()
src_root   = meson.project_source_root()
fs         = import('fs')

# The order here is important, make sure the
# dependencies are built before the dependants,
# refer to here: https://github.com/shimat/opencv_dependency.
modules_ordered = [
    # 'cudev (where is this)
    'core',
    'flann',
    'imgproc',
    'ml',
    'features2d',
    'dnn',
    'imgcodecs',
    'calib3d',
    'videoio',
    'stitching',
    'photo',
    'video',
    'objdetect',
    'highgui',
    'gapi',
    # Disabled until i figure out how to build them.
    #'ts',
    #'world',
    #'java',
    #'js',
    #'objc',
    #'python',
]

subdir('scripts')
subdir('conf')

# Filled in by the visiting each <conf['modules_to_build']>.
modules_built = {}

foreach module : modules_ordered
    if module in conf['modules_to_build']
        subdir('modules' / module)
    endif
endforeach

incdirs = []
libraries = []

foreach name, module : modules_built
    incdirs   += module.get('exported_includes', [])
    libraries += module.get('lib', [])
endforeach

opencv_dep = declare_dependency(
    include_directories: incdirs,
    link_with: libraries,
)
