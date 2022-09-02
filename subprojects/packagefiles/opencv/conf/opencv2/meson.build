cvcpu_confdata = configuration_data()
# Baseline definitions.
cvcpu_confdata_baseline_definitions = ''
cvcpu_confdata_baseline_names = []
foreach opt : conf['cpu_baseline']
    cvcpu_confdata_baseline_definitions += '#define CV_CPU_COMPILE_@0@ 1\n'.format(opt.to_upper())
    cvcpu_confdata_baseline_definitions += '#define CV_CPU_BASELINE_COMPILE_@0@ 1\n\n'.format(opt.to_upper())
    cvcpu_confdata_baseline_names += 'CV_CPU_@0@'.format(opt.to_upper())
endforeach
cvcpu_confdata_baseline_definitions += '#define CV_CPU_BASELINE_FEATURES 0, @0@'.format(', '.join(cvcpu_confdata_baseline_names))
cvcpu_confdata.set('OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE', cvcpu_confdata_baseline_definitions)
# Dispatch definitions
cvcpu_confdata_dispatch_definitions = ''
cvcpu_confdata_dispatch_names = []
foreach opt : conf['cpu_optimizations_supported']
    cvcpu_confdata_dispatch_definitions += '#define CV_CPU_DISPATCH_COMPILE_@0@ 1\n'.format(opt.to_upper())
    cvcpu_confdata_dispatch_names += 'CV_CPU_@0@'.format(opt.to_upper())
endforeach
cvcpu_confdata_dispatch_definitions += '\n#define CV_CPU_DISPATCH_FEATURES 0, @0@'.format(', '.join(cvcpu_confdata_dispatch_names))
cvcpu_confdata.set('OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE', cvcpu_confdata_dispatch_definitions)

modules_confdata = configuration_data()
modules_confdata.set('OPENCV_ENABLE_NONFREE', conf['enable_nonfree'])
modules_confdata_definitions_str = ''
foreach mod : conf['resolved_module_list']
    modules_confdata_definitions_str += '\n#define HAVE_@0@'.format(mod.to_upper())
endforeach
modules_confdata.set('OPENCV_MODULE_DEFINITIONS_CONFIGMAKE', modules_confdata_definitions_str)

# TODO: empty for now.
cvconfig_confdata = configuration_data()
cvconfig_confdata.set_quoted('OPENCV_CUDA_ARCH_BIN', '')
cvconfig_confdata.set_quoted('OPENCV_CUDA_ARCH_FEATURES', '')
cvconfig_confdata.set_quoted('OPENCV_CUDA_ARCH_PTX', '')
cvconfig_confdata.set('CV_ENABLE_INTRINSICS', 1)

conf += {'gen': {
    'includes': include_directories('..', '.'),
    'custom_hal.hpp': configure_file(
        input: 'custom_hal.hpp.in',
        output: 'custom_hal.hpp',
        configuration: {'_hal_includes': ''},
    ),
    'cv_cpu_config.h': configure_file(
        input: 'cv_cpu_config.h.in',
        output: 'cv_cpu_config.h',
        configuration: cvcpu_confdata,
    ),
    'cvconfig.h': configure_file(
        input:  'cvconfig.h.in',
        output: 'cvconfig.h',
        configuration: cvconfig_confdata,
    ),
    'opencv_modules.hpp': configure_file(
        input:  'opencv_modules.hpp.in',
        output: 'opencv_modules.hpp',
        configuration: modules_confdata,
    ),
}}
