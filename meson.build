project('pbssl', ['cpp', 'c'],
    version: '0.0.0',
    default_options : [
        'buildtype=debugoptimized',
        'warning_level=3',
        'cpp_std=c++2a',
    ],
    meson_version: '>= 0.56',
    #license: ['???'],
)

os = host_machine.system()
cxx = meson.get_compiler('cpp')

## Needed by SDL2 in msvc, since i don't want to rewrite
## the wrapfile i'll just add it here instead.
if os == 'windows'
    #add_global_arguments('-DHAVE_LIBC', language: 'c')
    add_global_arguments('-DSDL_VIDEO_RENDER_OGL_ES2=0', language: 'c')
    add_global_arguments('-DSDL_VIDEO_RENDER_D3D=0', language: 'c')
    add_global_arguments('-DSDL_VIDEO_RENDER_OGL_ES=0', language: 'c')
    add_global_arguments('-DSDL_VIDEO_OPENGL_EGL=0', language: 'c')
endif

# Clang has a great compile time (not compile-time) flag.
if get_option('time-trace') == true
    add_project_arguments('-ftime-trace', language: 'c')
    add_project_arguments('-ftime-trace', language: 'cpp')
endif

# Depenency KWargs.
# include_type: 'system' just means if we include a header from that dependency it will
# not emit any warnings (which would be a nuisance since we have such a high warning level).
dkw = {'include_type': 'system', 'default_options': 'warning_level=0'}

# OpenCV is special since we couldn't sucessfully wrap it using the cmake module and
# wrapping it manually is WAY beyond the scope of this project.
# So it's wise to make the project runnable without it, check out 'pinguim/conf.hpp' and the
# vision input subsystem.
opencv_dep = dependency('opencv4', version: '>= 4.0.0', include_type: 'system', required: false)
# Gui stuff
opengl_dep = os == 'windows' ? meson.get_compiler('c').find_library('opengl32') : dependency('gl', include_type: 'system')
sdl2_intrinsic_deps = os in ['windows', 'darwin'] ? [] : [
    dependency('xext', fallback: ['xext', 'xext_dep'], kwargs: dkw),
    dependency('xi',   fallback: ['xi',   'xi_dep'],   kwargs: dkw),
]
glew_dep  = dependency('glew',  version: '>= 2.0.0', fallback: ['glew', 'glew_dep'],   kwargs: dkw)
sdl2_dep  = dependency('sdl2',  version: '>= 2.0.9', fallback: ['sdl2', 'sdl2_dep'],   kwargs: dkw)
imgui_dep = dependency('imgui', version: '>= 1.7.6', fallback: ['imgui', 'imgui_dep'], kwargs: dkw)
sdl2_deps = [sdl2_intrinsic_deps, sdl2_dep]
gui_deps  = [opengl_dep, glew_dep, sdl2_deps, imgui_dep]
# Utils and other stuff.
simproto_dep = subproject('simproto').get_variable('simproto_dep').as_system('system')
asio_dep     = dependency('asio',   version: '>= 1.18.1', fallback: ['asio', 'asio_dep'],     kwargs: dkw)
fmt_dep      = dependency('fmt',    version: '>= 7.0.0',  fallback: ['fmt', 'fmt_dep'],       kwargs: dkw)
docopt_dep   = dependency('docopt', version: '>= 0.6.0',  fallback: ['docopt', 'docopt_dep'], kwargs: dkw)
glm_dep      = dependency('glm',    version: '>= 0.9.8',  fallback: ['glm', 'glm_dep'],       kwargs: dkw)
openmp_dep   = dependency('openmp')
utils_deps   = [simproto_dep, asio_dep, fmt_dep, docopt_dep, glm_dep, openmp_dep]

app_inc = include_directories('src')

sources = files(
    'src/pinguim/app/subsystems/manager.cpp',
    'src/pinguim/app/subsystems/input/firasim.cpp',
    'src/pinguim/app/subsystems/input/vision.cpp',
    'src/pinguim/app/subsystems/logic/direct_control.cpp',
    'src/pinguim/app/subsystems/logic/strategy.cpp',
    'src/pinguim/app/subsystems/output/firasim.cpp',

    'src/pinguim/imgui/plumber.cpp',
    'src/pinguim/imgui/img.cpp',
    'src/pinguim/imgui/widgets/group_panel.cpp',

    'src/pinguim/vsss/strategy.cpp',
    'src/pinguim/vsss/control.cpp',
    'src/pinguim/vsss/common.cpp',
)

subdir('conf')
subdir('tests')

pbssl_exe = executable(
    'pbssl',
    sources + ['src/pinguim/app/main.cpp'], # Main is separate to not interfere in tests.
    include_directories: [app_inc, conf_inc],
    dependencies: [opencv_dep, gui_deps, utils_deps],
    cpp_args: cpp_warnings,
    #cpp_pch: 'src/pinguim/app/pch.hpp',
)
