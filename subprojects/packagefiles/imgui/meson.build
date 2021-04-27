# Taken from wrapdb, removed what i don't use, added project arguments and gl dependency.
project('imgui', 'cpp',
    version: '1.78',
    license: 'MIT',
)

add_project_arguments('-DIMGUI_IMPL_OPENGL_LOADER_GLEW', language: 'cpp')

# Folder `examples` is needed, as it includes
# the implementations for different rendering backends and
# context creation APIs
include_dirs = include_directories('.', 'examples')

sources = [
    'imgui_demo.cpp',
    'imgui_draw.cpp',
    'imgui_widgets.cpp',
    'imgui.cpp',
    'examples/imgui_impl_sdl.cpp',
    'examples/imgui_impl_opengl3.cpp'
]

imgui = static_library('imgui',
    sources,
    dependencies: [
        host_machine.system() == 'windows' ? meson.get_compiler('c').find_library('opengl32') : dependency('gl'),
        dependency('glew'),
        dependency('sdl2'),
    ],
    cpp_args: ['-DGLEW_NO_GLU', get_option('default_library') == 'static' ? '-DGLEW_STATIC' : '-DGLEW_BUILD'],
    include_directories: include_dirs,
)

imgui_dep = declare_dependency(include_directories: include_dirs, link_with: imgui)
