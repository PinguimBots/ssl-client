project('imgui', 'cpp',
    version: '1.87',
    license: 'MIT',
)

add_project_arguments('-DIMGUI_IMPL_OPENGL_LOADER_CUSTOM', language: 'cpp')

include_dirs = include_directories('.')

sources = [
    'imgui_demo.cpp',
    'imgui_draw.cpp',
    'imgui_tables.cpp',
    'imgui_widgets.cpp',
    'imgui.cpp',
    'backends/imgui_impl_sdl.cpp',
    'backends/imgui_impl_opengl3.cpp'
]

imgui = static_library('imgui',
    sources,
    dependencies: [
        host_machine.system() == 'windows' ? meson.get_compiler('c').find_library('opengl32') : dependency('gl'),
        dependency('glew'),
        dependency('sdl2'),
    ],
    cpp_args: \
        ['-DGLEW_NO_GLU', get_option('default_library') == 'static' ? '-DGLEW_STATIC' : '-DGLEW_BUILD'] + \
        meson.get_compiler('cpp').get_supported_arguments(['/W', '-w']),
    include_directories: include_dirs,
)

binary_to_compressed_c = executable('binary_to_compressed_c', 'misc/fonts/binary_to_compressed_c.cpp')
meson.override_find_program('binary_to_compressed_c', binary_to_compressed_c)

imgui_dep = declare_dependency(include_directories: include_dirs, link_with: imgui)
