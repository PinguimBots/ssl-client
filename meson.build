project('pbssl', 'cpp',
    version: '0.0.0',
    default_options : [
        'cpp_std=c++17',
        'buildtype=debug',
    ],
)

incdirs = include_directories('include', 'src')

sources = files(
    'src/main.cpp',

    'src/net/grSim_client.cpp',
    'src/net/netraw.cpp',
    'src/net/robocup_ssl_client.cpp',
    'src/net/robocup_ssl_server.cpp',

    'src/pb/command.pb.cc',
    'src/pb/common.pb.cc',
    'src/pb/messages_robocup_ssl_detection.pb.cc',
    'src/pb/messages_robocup_ssl_geometry_legacy.pb.cc',
    'src/pb/messages_robocup_ssl_geometry.pb.cc',
    'src/pb/messages_robocup_ssl_refbox_log.pb.cc',
    'src/pb/messages_robocup_ssl_wrapper_legacy.pb.cc',
    'src/pb/messages_robocup_ssl_wrapper.pb.cc',
    'src/pb/packet.pb.cc',
    'src/pb/replacement.pb.cc',

    'src/pbts/control.cpp',
    'src/pbts/strategy.cpp',
)

deps = [
    dependency('qt5', modules: ['Core', 'Network']),
    dependency('protobuf', fallback: ['protobuf', 'protobuf_dep']),
    dependency('fmt', fallback: ['fmt', 'fmt_dep']),
    dependency('docopt', fallback: ['docopt', 'docopt_dep'])
]

qt5 = import('qt5')
moc_files = qt5.preprocess(
    moc_headers: files('src/net/grSim_client.h'),
    include_directories: incdirs,
    dependencies: deps
)
sources += moc_files

warnings = [
    '-Wall', # reasonable and standard
    '-Wextra', # reasonable and standard
    '-Wshadow', # warn the user if a variable declaration shadows one from a parent context
    '-Wnon-virtual-dtor', # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    '-Wold-style-cast', # warn for c-style casts
    '-Wcast-align', # warn for potential performance problem casts
    '-Wunused', # warn on anything being unused
    '-Woverloaded-virtual', # warn if you overload (not override) a virtual function
    '-Wpedantic', # (all versions of GCC, Clang >= 3.2) warn if non-standard C++ is used
    '-Wconversion', # warn on type conversions that may lose data
    '-Wsign-conversion', # (Clang all versions, GCC >= 4.3) warn on sign conversions
    '-Wmisleading-indentation', # (only in GCC >= 6.0) warn if indentation implies blocks where blocks do not exist
    '-Wduplicated-cond', # (only in GCC >= 6.0) warn if if / else chain has duplicated conditions
    '-Wduplicated-branches', # (only in GCC >= 7.0) warn if if / else branches have duplicated code
    '-Wlogical-op', # (only in GCC) warn about logical operations being used where bitwise were probably wanted
    '-Wnull-dereference', # (only in GCC >= 6.0) warn if a null dereference is detected
    '-Wuseless-cast', # (only in GCC >= 4.8) warn if you perform a cast to the same type
    '-Wdouble-promotion', # (GCC >= 4.6, Clang >= 3.8) warn if float is implicit promoted to double
    '-Wformat=2', # warn on security issues around functions that format output (ie printf)
]

pbssl_exe = executable(
    'pbssl',
    sources,
    include_directories: incdirs,
    dependencies: deps,
    link_args: warnings,
    cpp_pch: 'src/pch.h',
    cpp_args: [
        '-DUSE_ARRAY_IN_FRAME_STRUCTURED_BINDINGS',
        '-DARRAY_SIZE_IN_FRAME_STRUCTURED_BINDINGS=3',
    ]
)
