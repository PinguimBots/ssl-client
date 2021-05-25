# Proto files needed to connect with the *sim (GrSim/FiraSim) simulators
project('simproto', 'cpp', default_options: 'warning_level=0')

protobuf_dep = dependency('protobuf', version: '>= 3.0.0', required: false, include_type: 'system')
if not protobuf_dep.found()
    protobuf_subproj = subproject('protobuf', default_options: 'warning_level=0')
    protobuf_dep     = protobuf_subproj.get_variable('protobuf_dep').as_system('system')
    meson.override_find_program('protoc', protobuf_subproj.get_variable('protoc'))
endif

protofiles = [
    'command',
    'common',
    'packet',
    'replacement',
    'vssref_command',
    'vssref_common',
    'vssref_placement',
]

protofiles_in  = []
protofiles_out = []
foreach file : protofiles
    protofiles_in  += 'simproto' / file + '.proto'
    protofiles_out += file + '.pb.h'
    protofiles_out += file + '.pb.cc'
endforeach
protofiles_in = files(protofiles_in)

incdir = include_directories('.')

# The custom_target() must be defined in that folder
# for folder mirroring reasons.
subdir('simproto')

simproto_lib = library(
    'simproto',
    generated,
    include_directories: incdir,
    dependencies: protobuf_dep,
)

simproto_dep = declare_dependency(
    link_with: simproto_lib,
    include_directories: incdir,
    dependencies: protobuf_dep,
    # Makes sure the headers are built before the dependants, this also exposes the soures,
    # which would be bad if this package has multiple dependants.
    sources: generated,
)
