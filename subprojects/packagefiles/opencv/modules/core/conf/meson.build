mod_core_sources += configure_file(
    configuration: {'OPENCV_DATA_CONFIG_HPP_INCLUDED': '''

#define OPENCV_BUILD_DIR "@0@"

#define OPENCV_DATA_BUILD_DIR_SEARCH_PATHS "@1@"

/* TODO: Missing some stuff. e.g:
#define OPENCV_INSTALL_PREFIX "/usr/local"

#define OPENCV_DATA_INSTALL_PATH "share/opencv4"

#define OPENCV_INSTALL_DATA_DIR_RELATIVE "../share/opencv4"
*/
'''.format(
        meson.project_build_root(),
        meson.project_source_root()
    )},
    output: 'opencv_data_config.hpp'
)
