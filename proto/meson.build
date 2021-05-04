protobuf_dep = dependency('protobuf', version: '>= 3.0.0', required: false, include_type: 'system')
if not protobuf_dep.found()
    protobuf_subproj = subproject('protobuf')
    protobuf_dep     = protobuf_subproj.get_variable('protobuf_dep').as_system('system')
    meson.override_find_program('protoc', protobuf_subproj.get_variable('protoc'))
endif

proto_in = [
    'src/proto/command.proto',
    'src/proto/common.proto',
    'src/proto/packet.proto',
    'src/proto/replacement.proto',
    'src/proto/vssref_command.proto',
    'src/proto/vssref_common.proto',
    'src/proto/vssref_placement.proto',
]

proto_out = []
foreach file : proto_in
    file = file.substring(0, -6).substring(10)
    proto_out += file + '.pb.cc'
    proto_out += file + '.pb.h'
endforeach

proto_gen_target = custom_target(
    'protobuf_generated_sources',
    command: [
        find_program('protoc'), '@INPUT@',
        '--proto_path=@CURRENT_SOURCE_DIR@'/'src',
        '--cpp_out=@BUILD_ROOT@',
    ],
    input:  files(proto_in),
    output: proto_out,
)

proto_gen_dep = declare_dependency(
    dependencies: protobuf_dep,
    link_with: library('proto_gen', proto_gen_target, include_directories: include_directories('..'), dependencies: protobuf_dep),
    include_directories: include_directories('.'),
).as_system('system')
