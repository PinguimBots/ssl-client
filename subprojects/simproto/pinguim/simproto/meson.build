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

generated_only_headers = []

foreach g : generated.to_list()
    if g.full_path().endswith('.h')
        generated_only_headers += g
    endif
endforeach
