project('pbssl', ['cpp', 'c'],
    version: '0.0.0',
    default_options : [
        'buildtype=debugoptimized',
        'warning_level=0',
        'cpp_std=c++20',
    ],
    meson_version: '>= 0.60',
    #license: ['???'],
)

os = host_machine.system()
cxx = meson.get_compiler('cpp')
cxxid = cxx.get_id()

if os == 'windows'
    ## Needed by SDL2.
    add_global_arguments('-DHAVE_LIBC', language: 'c')
    add_global_arguments('-DSDL_VIDEO_RENDER_OGL_ES2=0', language: 'c')
    add_global_arguments('-DSDL_VIDEO_RENDER_D3D=0', language: 'c')
    add_global_arguments('-DSDL_VIDEO_RENDER_OGL_ES=0', language: 'c')
    add_global_arguments('-DSDL_VIDEO_OPENGL_EGL=0', language: 'c')

    ## Needed by asio (see https://think-async.com/Asio/asio-1.22.1/doc/asio/using.html).
    add_global_arguments('-DBOOST_DATE_TIME_NO_LIB', language: 'cpp')
    add_global_arguments('-DBOOST_REGEX_NO_LIB', language: 'cpp')
endif

if cxxid == 'msvc'
    # Silence SDL2 warning: warning C4828: The file contains a character starting at offset 0x45e that is illegal in the current source character set (codepage 65001).
    add_global_arguments('/wd4828', language: 'c')

    # Silence warnings from headers beyond our control.
    add_project_arguments('/external:W0', language: 'cpp')
    add_project_arguments('/external:anglebrackets', language: 'cpp')
endif

# Clang has a great compile time (not compile-time) flag.
if get_option('time-trace') == true
    add_project_arguments('-ftime-trace', language: 'c')
    add_project_arguments('-ftime-trace', language: 'cpp')
endif

# NOTE: include_type: 'system' just means if we include a header from that dependency it will
# not emit any warnings (which would be a nuisance since we have such a high warning level).

# OpenCV is special since we couldn't sucessfully wrap it using the cmake module and
# wrapping it manually is WAY beyond the scope of this project.
# So it's wise to make the project runnable without it, check out 'pinguim/conf.hpp' and the
# vision input subsystem.
opencv_dep = dependency('opencv4', version: '>= 4.5.1', include_type: 'system', required: false)
# Gui stuff
opengl_dep = os == 'windows' ? meson.get_compiler('c').find_library('opengl32') : dependency('gl').as_system('system')
sdl2_intrinsic_deps = os in ['windows', 'darwin'] ? [] : [
    dependency('xext', fallback: ['xext', 'xext_dep']).as_system('system'),
    dependency('xi',   fallback: ['xi',   'xi_dep']).as_system('system'),
]
glew_dep  = dependency('glew',  version: '>= 2.0.0', fallback: ['glew', 'glew_dep']).as_system('system')
sdl2_dep  = dependency('sdl2',  version: '>= 2.0.9', fallback: ['sdl2', 'sdl2_dep']).as_system('system')
imgui_dep = dependency('imgui', version: '>= 1.7.6', fallback: ['imgui', 'imgui_dep']).as_system('system')
imgui_file_dialog_dep = subproject('imgui_file_dialog').get_variable('imgui_file_dialog_dep').as_system('system')
sdl2_deps = [sdl2_intrinsic_deps, sdl2_dep]
gui_deps  = [opengl_dep, glew_dep, sdl2_deps, imgui_dep, imgui_file_dialog_dep]
# Utils and other stuff.
simproto_dep = subproject('simproto').get_variable('simproto_dep').as_system('system')
protobuf_dep = dependency('protobuf', version: '>= 3.0.0',  fallback: ['protobuf', 'protobuf_dep']).as_system('system')
asio_dep     = dependency('asio',     version: '>= 1.18.1', fallback: ['asio', 'asio_dep']).as_system('system')
fmt_dep      = dependency('fmt',      version: '>= 7.0.0',  fallback: ['fmt', 'fmt_dep']).as_system('system')
glm_dep      = dependency('glm',      version: '>= 0.9.8',  fallback: ['glm', 'glm_dep']).as_system('system')
openmp_dep   = dependency('openmp')
utils_deps   = [simproto_dep, asio_dep, fmt_dep, glm_dep, openmp_dep, protobuf_dep]

app_inc = include_directories('src')

sources = files(
    'src/pinguim/geometry.cpp',

    'src/pinguim/app/subsystems/manager.cpp',
    'src/pinguim/app/subsystems/registrar.cpp',

    'src/pinguim/app/subsystems/input/firasim.cpp',
    'src/pinguim/app/subsystems/input/vision.cpp',
    'src/pinguim/app/subsystems/input/vision/pipeline.cpp',
    'src/pinguim/app/subsystems/input/vision/segmentation.cpp',
    'src/pinguim/app/subsystems/logic/direct_control.cpp',
    'src/pinguim/app/subsystems/logic/strategy.cpp',
    'src/pinguim/app/subsystems/output/firasim.cpp',
    'src/pinguim/app/subsystems/misc/imgui_debug.cpp',

    'src/pinguim/imgui/widgets/toggle_button.cpp',
    'src/pinguim/imgui/widgets/group_panel.cpp',
    'src/pinguim/imgui/fonts/loader.cpp',
    'src/pinguim/imgui/plumber.cpp',
    'src/pinguim/imgui/img.cpp',

    'src/pinguim/imgui/fonts/kenney_game_icons.cpp',

    'src/pinguim/vsss/net/multicast_udp_receiver.cpp',
    'src/pinguim/vsss/net/udp_sender.cpp',

    'src/pinguim/vsss/strategy.cpp',
    'src/pinguim/vsss/control.cpp',
    'src/pinguim/vsss/control2.cpp',
    'src/pinguim/vsss/common.cpp',
)

subdir('conf')
subdir('scripts')
subdir('tests')

pbssl_exe = executable(
    'pbssl',
    sources + ['src/pinguim/app/main.cpp'], # Main is separate to not interfere in tests.
    include_directories: [app_inc, conf_inc],
    dependencies: [opencv_dep, gui_deps, utils_deps],
    cpp_args: cpp_warnings,
)
