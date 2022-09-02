conf += { 'depend': {
    'requested_modules': get_option('modules'),

    # A dependency map to resolve what modules to build.
    # Taken from https://github.com/shimat/opencv_dependency.
    # NOTE: it's up to the module itself to handle optional dependencies (cudev for core, flann for features2d, etc).
    'dependencies': {
        'core':       ['cudev'],
        'flann':      ['cudev', 'core'],
        'imgproc':    ['cudev', 'core'],
        'ml':         ['cudev', 'core'],
        'dnn':        ['cudev', 'core', 'imgproc'],
        'imgcodecs':  ['cudev', 'core', 'imgproc'],
        'features2d': ['cudev', 'core', 'imgproc', 'flann'],
        'videoio':    ['cudev', 'core', 'imgproc', 'imgcodecs'],
        'calib3d':    ['cudev', 'core', 'imgproc', 'flann', 'features2d'],
        'highgui':    ['cudev', 'core', 'imgproc', 'imgcodecs', 'videoio'],
        'photo':      ['cudev', 'core', 'imgproc', 'cudaarithm', 'cudaimgproc'],
        'objdetect':  ['cudev', 'core', 'imgproc', 'flann', 'features2d', 'calib3d'],
        'ts':         ['cudev', 'core', 'imgproc', 'imgcodecs', 'videoio', 'highgui'],
        'video':      ['cudev', 'core', 'imgproc', 'flann', 'features2d', 'calib3d', 'dnn'],
        'stitching':  ['cudev', 'core', 'imgproc', 'flann', 'features2d', 'calib3d', 'cudaarithm', 'cudawarping', 'cudafeatures2d', 'cudalegacy', 'cudaimgproc'],
        'gapi':       ['cudev', 'core', 'imgproc', 'flann', 'features2d', 'calib3d', 'dnn', 'video'],
    },

    # The absolute build order for the modules, we intersect this list
    # with the resolved module list to figure out the order (to avoid visiting
    # dependands before dependencies).
    'modules_ordered': [
        # 'cudev (where is this)
        # Where is all the cuda stuff ?
        'core',
        'flann',
        'imgproc',
        'ml',
        'features2d',
        'dnn',
        'imgcodecs',
        'calib3d',
        'videoio',
        'stitching',
        'photo',
        'video',
        'objdetect',
        'highgui',
        'gapi',
        'ts',
        'world',
        'java',
        'js',
        'objc',
        'python',
    ],
}}

module_list = []
foreach module : conf['depend']['requested_modules']
    module_list += conf['depend']['dependencies'][module]
endforeach

resolved_module_list = []
foreach module : conf['depend']['modules_ordered']
    if module in module_list
        resolved_module_list += module
    endif
endforeach

conf += {
    # The full module tree, ordered and with all dependencies. Equivalent to BUILD_opencv_* in the cmake build.
    'resolved_module_list': resolved_module_list
}