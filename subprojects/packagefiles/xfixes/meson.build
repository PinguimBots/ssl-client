project('xfixes', 'c')

sources = files(
    'src/Cursor.c',
    'src/Region.c',
    'src/SaveSet.c',
    'src/Selection.c',
    'src/Xfixes.c',
    'src/Xfixesint.h',
)

libxfixes = library(
    'xfixes',
    sources,
    dependencies: [dependency('x11')],
    include_directories: include_directories('src', 'include/X11/extensions'),
)

xfixes_dep = declare_dependency(
    link_with: libxfixes,
    include_directories: include_directories('include'),
)
