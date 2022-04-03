mod_videoio_sources = []

mod_videoio_plugins = get_option('videoio_plugin_list')
if 'default' in mod_videoio_plugins
    mod_videoio_plugins = os in ['windows', 'darwin'] ? [] : ['all']
endif
if 'all' in mod_videoio_plugins
    mod_videoio_plugins = ['ffmpeg', 'gstreamer', 'mfx', 'msmf']
endif

mod_videoio = {
    'lib': library(
        'opencv_videoio',
        mod_videoio_sources,
        include_directories: include_directories('include', 'src'),
        cpp_args: conf['module_build_flags'],
    ),
    'exported_includes': include_directories('include'),
}

modules_built += {'videoio': mod_videoio}
