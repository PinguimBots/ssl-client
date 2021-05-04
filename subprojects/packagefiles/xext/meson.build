project('xext', 'c')

sources = files(
    'src/DPMS.c',
    'src/MITMisc.c',
    'src/XAppgroup.c',
    'src/XEVI.c',
    'src/XLbx.c',
    'src/XMultibuf.c',
    'src/XSecurity.c',
    'src/XShape.c',
    'src/XShm.c',
    'src/XSync.c',
    'src/XTestExt1.c',
    'src/Xcup.c',
    'src/Xdbe.c',
    'src/Xge.c',
    'src/extutil.c',
    'src/globals.c',
)

libxext = library(
    'xext',
    sources,
    dependencies: [dependency('x11')],
    include_directories: include_directories('src', 'include'),
)

xext_dep = declare_dependency(
    link_with: libxext,
    include_directories: include_directories('include'),
)
