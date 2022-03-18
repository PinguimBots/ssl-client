# Taken from wrapdb, removed what i don't use, added project arguments and gl dependency.
project('imgui_file_dialog', 'cpp',
    version: '1.78',
    license: 'MIT',
)

# Folder `examples` is needed, as it includes
# the implementations for different rendering backends and
# context creation APIs
include_dirs = include_directories('.')

imgui_dep = dependency('imgui', version: '>= 1.8.7')

imgui_file_dialog = static_library(
    'imgui_file_dialog',
    'ImGuiFileDialog.cpp',
    dependencies: imgui_dep,
    cpp_args: meson.get_compiler('cpp').get_supported_arguments(['/w', '-w']),
    include_directories: include_dirs,
)

imgui_file_dialog_dep = declare_dependency(
    include_directories: include_dirs,
    dependencies: imgui_dep,
    link_with: imgui_file_dialog,
)
