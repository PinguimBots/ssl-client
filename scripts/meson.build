# Defined in the glyphmap.txt, but we need to repeat it here.
kenney_font_prefix = 'kenney_game_icons'

binary_to_compressed_c = find_program('binary_to_compressed_c', required: false)
if not binary_to_compressed_c.found()
    binary_to_compressed_c = subproject('imgui').get_variable('binary_to_compressed_c')
endif

# Run build_kenney_font_sources.sh when you want to rebuild this target.
# We keep in on the source-tree and don'tt run in by default since
# there are some dependencies to run the script.
custom_target(
    'kenney_font_sources',

    output: [kenney_font_prefix+'.cpp', kenney_font_prefix+'.hpp'],
    command:
    [
        import('python').find_installation('python3'),
        files('pngs2imguiFont.py'),
        files('kenney_font_glyphmap.txt'),
        '@PRIVATE_DIR@',
        '@OUTDIR@',
        binary_to_compressed_c,
    ],

    build_by_default: false,
    build_always_stale: true,

    install: true,
    install_dir: meson.project_source_root()/'src'/'pinguim'/'imgui'/'fonts',
    install_tag: 'kenney_font_sources'
)
