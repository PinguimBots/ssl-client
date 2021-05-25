generated = custom_target(
    'simproto generated',
    command: [
        find_program('protoc'),
        '@INPUT@',
        '--proto_path=@CURRENT_SOURCE_DIR@',
        '--cpp_out=@OUTDIR@'
    ],
    input:  protofiles_in,
    output: protofiles_out
)
