conf_data = configuration_data()

# Keep in mind that the folder structure is replicated, thats why
# conf.hpp.in must be in that separate folder.

## Compiler stuff.
cxxid = cxx.get_id()
conf_data.set_quoted('cxx_id', cxx.get_id())
conf_data.set('PINGUIM_CONF_COMPILER_IS_GCC',   cxxid == 'gcc')
conf_data.set('PINGUIM_CONF_COMPILER_IS_CLANG', cxxid == 'clang')
conf_data.set('PINGUIM_CONF_COMPILER_IS_MSVC',  cxxid == 'msvc')
conf_data.set_quoted('cxx_version', cxx.version())

if get_option('fixed-team-size-of') > 0
    conf_data.set10('team_size_fixed', true)
    conf_data.set('fixed_team_size', get_option('fixed-team-size-of').to_string() )
else
    conf_data.set10('team_size_fixed', false)
    conf_data.set('fixed_team_size', 0)
endif

has_opencv_with_opengl = meson.get_compiler('cpp').run(
    files('detect_opencv_with_opengl.cpp'),
    name: 'has_opencv_with_opengl',
    dependencies: [opencv_dep],
).returncode() == 0
conf_data.set10('opencv_with_opengl', has_opencv_with_opengl)

conf_data.set10('opencv_support', opencv_dep.found())
conf_data.set('PINGUIM_CONF_OPENCV_SUPPORT', opencv_dep.found())

# For now, meson doesn't allow path separators in configure_file(output: 'file_here'),
# which is why we need this wonky folder structure.
subdir('pinguim')

conf_inc = include_directories('.')
