conf_data = configuration_data()

# Keep in mind that the folder structure is replicated, thats why
# conf.h.in must be in that separate folder.

# For OpenCV, we can check for stuff in <opencv2/cvconfig.h> at any time,
# but we'd rather do this here than in the app, since doing so pollutes the
# global namespace with a bunch of macros.
if get_option('force-opencv-without-opengl')
    message('Assuming OpenCV was built WITHOUT OPENGL SUPPORT since -Dforce-opencv-without-opengl=true')
    has_opencv_with_opengl = false
else
    has_opencv_with_opengl = meson.get_compiler('cpp').run(
        files('detect_opencv_with_opengl.cpp'),
        name: 'has_opencv_with_opengl',
        dependencies: [opencv_dep],
    ).returncode() == 0
endif
conf_data.set10('opencv_with_opengl', has_opencv_with_opengl)

if get_option('fixed-team-size-of') > 0
    conf_data.set10('team_size_fixed', true)
    conf_data.set('fixed_team_size', get_option('fixed-team-size-of').to_string() )
else
    conf_data.set10('team_size_fixed', false)
    conf_data.set('fixed_team_size', 0)
endif

# For now, meson doesn't allow path separators in configure_file(output: 'file_here'),
# which is why we need this wonky folder structure.
subdir('pinguim')

conf_inc = include_directories('.')