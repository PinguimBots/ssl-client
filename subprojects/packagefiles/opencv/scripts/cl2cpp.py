# Python implementation of cmake/cv2cpp.cmake

import hashlib
import pathlib
import sys
import os
import re

def main():
    outdir = sys.argv[1]
    outname = sys.argv[2]
    sources = sys.argv[3:]

    str_cpp = '// This file is auto-generated. Do not edit!\n\n' \
        + '#include "opencv2/core.hpp"\n' \
        + '#include "cvconfig.h"\n' \
        + f'#include "{outname}.hpp"\n\n' \
        + '#ifdef HAVE_OPENCL\n\n' \
        + 'namespace cv\n{\n' \
        + 'namespace ocl\n{\n' \
        + f'namespace {outname}\n{{\n\n' \
        + f'static const char* const moduleName = "${outname}";\n\n'

    str_hpp = '// This file is auto-generated. Do not edit!\n\n' \
        + '#include "opencv2/core/ocl.hpp"\n' \
        + '#include "opencv2/core/ocl_genbase.hpp"\n' \
        + '#include "opencv2/core/opencl/ocl_defs.hpp"\n\n' \
        + '#ifdef HAVE_OPENCL\n' \
        + 'namespace cv\n{\n' \
        + 'namespace ocl\n{\n' \
        + f'namespace {outname}\n{{\n\n'

    for source in sources:
        with open(source, mode='r') as f: lines = f.read()
        lines = lines.replace('\r', '').replace('\t', ' ')
        lines = re.sub('/\\*([^*]/|\\*[^/]|[^*/])*\\*/', '',   lines) # multiline comments
        lines = re.sub('/\\*([^\n])*\\*/',               '',   lines) # single-line comments
        lines = re.sub('[ ]*//[^\n]*\n',                 '\n', lines) # single-line comments
        lines = re.sub('\n[ ]*(\n[ ]*)*',                '\n', lines) # empty lines & leading whitespace
        lines = re.sub('^\n',                            '',   lines) # leading new line
        lines = lines.replace('\\', '\\\\').replace('\"', '\\\"').replace('\n', '\\n"\n"')
        lines = re.sub('\"$', '', lines) # unneeded " at the eof

        md5 = hashlib.md5(lines.encode('utf-8')).hexdigest()

        cl_filename = pathlib.Path(source).stem

        str_cpp += f'struct cv::ocl::internal::ProgramEntry {cl_filename}_oclsrc={{moduleName, "{cl_filename}",\n"{lines}, "{md5}", NULL}};\n'
        str_hpp += f'extern struct cv::ocl::internal::ProgramEntry {cl_filename}_oclsrc;\n'

    str_cpp += '\n}}}\n#endif\n'
    str_hpp += '\n}}}\n#endif\n'

    output_cpp = f'{outdir}/{outname}.cpp'
    output_hpp = f'{outdir}/{outname}.hpp'

    if not os.path.exists(outdir): os.makedirs(outdir)

    with open(output_cpp, 'w') as out: out.write(str_cpp)
    with open(output_hpp, 'w') as out: out.write(str_hpp)

if __name__ == '__main__': main()