project('pbssl', 'cpp',
    version: '0.0.0',
    default_options : [
        'cpp_std=c++17',
        'buildtype=debug',
    ],
    # TODO: add license
)

deps = [
    dependency('qt5', modules: ['Core', 'Network']),
    dependency('fmt', fallback: ['fmt', 'fmt_dep']),
    dependency('docopt', fallback: ['docopt', 'docopt_dep'])
]
# protobuf is special since we need protoc to compile .proto files
protobuf_dep = dependency('protobuf', required: false)
if protobuf_dep.found()
    protoc = find_program('protoc', required: true)
else
    protobuf_proj = subproject('protobuf')
    protobuf_dep = protobuf_proj.get_variable('protobuf_dep')
    protoc = protobuf_proj.get_variable('protoc')
endif
deps += protobuf_dep

includes = include_directories('src')

sources = files(
    'src/main.cpp',

    'src/net/grSim_client.cpp',
    'src/net/netraw.cpp',
    'src/net/robocup_ssl_client.cpp',
    'src/net/robocup_ssl_server.cpp',

    'src/pbts/control.cpp',
    'src/pbts/strategy.cpp',
)
sources += import('qt5').preprocess(
    moc_headers: files('src/net/grSim_client.h'),
    include_directories: includes,
    dependencies: deps
)

protobuf_generator = generator(protoc,
    output: ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
    arguments: ['--proto_path=@CURRENT_SOURCE_DIR@/src', '--cpp_out=@BUILD_DIR@', '@INPUT@'],
)
# preserve_path_from is important because since we set proto_path to be the source tree root
# it will generate the .pb.cc and pb.h files mirroring the original folder structure.
# If we dont preserve_path_from '/src' meson won't find these files since it will look in the
# build directory instead of trying to mirror the tree.
protobuf_generated = protobuf_generator.process(
    files(
        'src/pb/command.proto',
        'src/pb/common.proto',
        'src/pb/messages_robocup_ssl_detection.proto',
        'src/pb/messages_robocup_ssl_geometry.proto',
        'src/pb/messages_robocup_ssl_geometry_legacy.proto',
        'src/pb/messages_robocup_ssl_refbox_log.proto',
        'src/pb/messages_robocup_ssl_wrapper.proto',
        'src/pb/messages_robocup_ssl_wrapper_legacy.proto',
        'src/pb/packet.proto',
        'src/pb/replacement.proto',
    ),
    preserve_path_from : meson.current_source_dir() / 'src',
)

# I'm looking at you MSVC, stop it with that /flag nonsense.
# TODO: add MSVC flags.
cxx = meson.get_compiler('cpp')
warnings = cxx.get_supported_arguments([
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
])

pbssl_exe = executable(
    'pbssl',
    sources,
    protobuf_generated,
    include_directories: includes,
    dependencies: deps,
    link_args: warnings,
    cpp_pch: 'src/pch.h',
    cpp_args: [
        # Return the robots as an array when destructuring
        # fira_messa::Frame instead of using a vector.
        #'-DUSE_ARRAY_IN_FRAME_STRUCTURED_BINDINGS',
        '-DARRAY_SIZE_IN_FRAME_STRUCTURED_BINDINGS=3',
    ]
)
