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

# # MSVC not supported yet.
# # Needed by SDL2 in msvc, since i don't want to rewrite
# # the wrapfile i'll just add it here instead.
# add_global_arguments('-DHAVE_LIBC', language: 'c')

os = host_machine.system()

# include_type: 'system' just means if we include a header from that dependency it will
# not emit any warnings (which would be a nuisance since we have such a high warning level).

opencv_dep = dependency('opencv4', version: '>= 4.0.0', include_type: 'system')
# Gui stuff
opengl_dep = os == 'windows' ? meson.get_compiler('c').find_library('opengl32') : dependency('gl', include_type: 'system')
sdl2_intrinsic_deps = os in ['windows', 'darwin'] ? [] : [
    dependency('xext', fallback: ['xext', 'xext_dep'], include_type: 'system'),
    dependency('xi',   fallback: ['xi',   'xi_dep'],   include_type: 'system'),
]
glew_dep  = dependency('glew',  version: '>= 2.0.0', fallback: ['glew', 'glew_dep'],   include_type: 'system')
sdl2_dep  = dependency('sdl2',  version: '>= 2.0.9', fallback: ['sdl2', 'sdl2_dep'],   include_type: 'system')
imgui_dep = dependency('imgui', version: '>= 1.7.6', fallback: ['imgui', 'imgui_dep'], include_type: 'system')
sdl2_deps = [sdl2_intrinsic_deps, sdl2_dep]
gui_deps  = [opengl_dep, glew_dep, sdl2_deps, imgui_dep]
# Utils
qt_dep  =    dependency('qt5',    version: '>= 5.9',   modules: ['Core', 'Network'],       include_type: 'system')
fmt_dep =    dependency('fmt',    version: '>= 7.0.0', fallback: ['fmt', 'fmt_dep'],       include_type: 'system')
docopt_dep = dependency('docopt', version: '>= 0.6.0', fallback: ['docopt', 'docopt_dep'], include_type: 'system')
glm_dep =    dependency('glm',    version: '>= 0.9.8', fallback: ['glm', 'glm_dep'],       include_type: 'system')
openmp_dep = dependency('openmp')
utils_deps = [qt_dep, fmt_dep, docopt_dep, glm_dep, openmp_dep]

subdir('proto')
subdir('tests')
subdir('conf')

sources = files(
    'src/main.cpp',

    'src/pinguim/control.cpp',
    'src/pinguim/strategy_r.cpp',
    'src/pinguim/strategy_d.cpp',
    'src/pinguim/utility.cpp',
    'src/pinguim/simulator_connection.cpp',
    'src/pinguim/common.cpp',
    'src/pinguim/parse.cpp',
)

pbssl_exe = executable(
    'pbssl',
    sources,
    include_directories: [include_directories('src'), conf_inc],
    dependencies: [gui_deps, utils_deps, proto_gen_dep],
    cpp_args: cpp_warnings,
    cpp_pch: 'src/pch.hpp',
)
