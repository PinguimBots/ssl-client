cpp_warnings = meson.get_compiler('cpp').get_supported_arguments([
    # /Wall enables ALL warnings in MSCV
    # while -Wall enables only the reasonable ones in GCC/Clang.
    cxxid != 'msvc' ? '-Wall' : '/W4',
    '-Wpedantic',
    '-Wextra', # reasonable and standard
    '-Wshadow', # warn the user if a variable declaration shadows one from a parent context
    '-Wnon-virtual-dtor', # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    '-Wold-style-cast', # warn for c-style casts
    '-Wcast-align', # warn for potential performance problem casts
    '-Wunused', # warn on anything being unused
    '-Woverloaded-virtual', # warn if you overload (not override) a virtual function
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

test('cvt', executable('cvt', 'cvt.cpp', include_directories: app_inc))
test('renum', executable('renum', 'renum.cpp', include_directories: [app_inc, conf_inc]))
test('math', executable('math', 'math.cpp', include_directories: app_inc))
