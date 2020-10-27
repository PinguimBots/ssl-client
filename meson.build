project('pbssl', 'cpp',
    version: '0.0.0',
    default_options : [
        'cpp_std=c++17',
        'buildtype=debug',
    ],
    meson_version: '>= 0.55', # Needed by meson.override_find_program()
    # TODO: add license
)

cpp_defines = []

if get_option('fixed-team-size-of') > 0
    cpp_defines += '-DPBTS_FIXED_TEAM_SIZE_OF=' + get_option('fixed-team-size-of').to_string()
endif

# -- Dependencies --
deps = [
    dependency('qt5', version: '>= 5.9', modules: ['Core', 'Network']),
    dependency('fmt', version: '>= 7.0.0', fallback: ['fmt', 'fmt_dep']),
    dependency('docopt', version: '>= 0.6.0', fallback: ['docopt', 'docopt_dep']),
    dependency('glm', version: '>= 0.9.8', fallback: ['glm', 'glm_dep']),
]
# protobuf is special since we need protoc to compile .proto files
protobuf_dep = dependency('protobuf', version: '>= 3.0.0', required: false)
if not protobuf_dep.found()
    protobuf_subproj = subproject('protobuf')
    protobuf_dep     = protobuf_subproj.get_variable('protobuf_dep')
    meson.override_find_program('protoc', protobuf_subproj.get_variable('protoc'))
endif
deps += protobuf_dep

# -- Source files and source generation --
sources = files(
    'src/main.cpp',

    'src/pbts/control.cpp',
    'src/pbts/strategy.cpp',
    'src/pbts/simulator_connection.cpp',
)

protobuf_generator = generator(
    find_program('protoc'),
    output: ['@BASENAME@.pb.cc', '@BASENAME@.pb.h'],
    arguments: ['--proto_path=@CURRENT_SOURCE_DIR@/src', '--cpp_out=@BUILD_DIR@', '@INPUT@'],
)
# preserve_path_from is important because since we set proto_path to be the source tree root
# it will generate the .pb.cc and .pb.h files mirroring the original folder structure.
# If we dont preserve_path_from '/src' meson won't find these files since it will look in the
# build directory instead of trying to mirror the tree.
protobuf_generated = protobuf_generator.process(
    files(
        'src/pb/command.proto',
        'src/pb/common.proto',
        'src/pb/packet.proto',
        'src/pb/replacement.proto',
        'src/pb/vssref_command.proto',
        'src/pb/vssref_common.proto',
        'src/pb/vssref_placement.proto',
    ),
    preserve_path_from : meson.current_source_dir() / 'src',
)

# -- Compile options --
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

    '/W4', # Covers most of our needs for MSVC.
])

pbssl_exe = executable(
    'pbssl',
    sources,
    protobuf_generated,
    include_directories: include_directories('src'),
    dependencies: deps,
    link_args: warnings,
    cpp_pch: 'src/pch.h',
    cpp_args: cpp_defines
)
