import numpy as np
import subprocess
import fontforge
import sys
import os

from PIL import Image

def main():
    # Works more or less like this:
    #     [png, ...] ---(PIL)----------> [pgm, ...]
    #     [pgm, ...] ---(potrace)------> [svg, ...]
    #     [svg, ...] ---(fontforge)----> ttf
    #     ttf        ---(imgui_tool)---> [cpp, hpp]

    glyphmap_file            = sys.argv[1]
    builddir                 = sys.argv[2]
    outdir                   = sys.argv[3]
    imgui_compress_tool_path = sys.argv[4]

    if not os.path.exists(builddir): os.makedirs(builddir)
    if not os.path.exists(outdir):   os.makedirs(outdir)

    glyphmap = read_glyphmap_file(glyphmap_file)

    codepoints = [g[0] for g in glyphmap['glyphs']]
    pngs       = [g[1] for g in glyphmap['glyphs']]
    names      = [g[2] for g in glyphmap['glyphs']]

    ttf_out = f'{builddir}/{glyphmap["prefix"]}.ttf'
    pngs2ttf(pngs, codepoints, builddir, ttf_out)

    cpp_out = f'{outdir}/{glyphmap["prefix"]}.cpp'
    cpp_str = build_cpp(
        imgui_compress_tool_path,
        ttf_out,
        glyphmap['prefix'],
        glyphmap['min_label'],
        glyphmap['max_label']
    )

    hpp_out = f'{outdir}/{glyphmap["prefix"]}.hpp'
    hpp_str = build_hpp(
        codepoints,
        names,
        glyphmap['prefix'],
        glyphmap['min_label'],
        glyphmap['max_label']
    )

    with open(cpp_out, 'w') as f: f.write(cpp_str)
    with open(hpp_out, 'w') as f: f.write(hpp_str)

def read_glyphmap_file(file):
    out = {'min_label': None, 'max_label': None, 'glyphs': [], 'prefix': None}

    with open(file) as f:
        out['prefix'] = f.readline().strip()

        l2 = f.readline().split(',')
        out['min_label'] = l2[0].strip()
        out['max_label'] = l2[1].strip()

        for line in f.readlines():
            glyphs = line.split(',')
            out['glyphs'].append(
                [int(glyphs[0].strip(), 16), glyphs[1].strip(), glyphs[2].strip()]
            )

    return out

def pngs2ttf(pngs, codepoints, builddir, outfile):
    assert(len(pngs) == len(codepoints))

    font = fontforge.font()

    for png, codepoint in zip(pngs, codepoints):
        basename = os.path.splitext(os.path.basename(png))[0]
        pgm_file = f'{builddir}/{basename}.pgm'
        svg_file = f'{builddir}/{basename}.svg'

        png_image = Image.open(png)
        white     = Image.new('RGBA', size = png_image.size, color = (255, 255, 255, 255))
        alpha_composite(png_image, white).convert('L').save(pgm_file)

        subprocess.run(['potrace', '-s', pgm_file, '-o', svg_file])

        glyph = font.createChar(codepoint)
        glyph.importOutlines(svg_file, accuracy=0.01)
        glyph.width = 950

    font.generate(outfile)

# From https://stackoverflow.com/questions/9166400/convert-rgba-png-to-rgb-with-pil.
# Thanks!
def alpha_composite(src, dst):
    '''
    Return the alpha composite of src and dst.

    Parameters:
    src -- PIL RGBA Image object
    dst -- PIL RGBA Image object

    The algorithm comes from http://en.wikipedia.org/wiki/Alpha_compositing
    '''
    # http://stackoverflow.com/a/3375291/190597
    # http://stackoverflow.com/a/9166671/190597
    src = np.asarray(src)
    dst = np.asarray(dst)
    out = np.empty(src.shape, dtype = 'float')
    alpha = np.index_exp[:, :, 3:]
    rgb = np.index_exp[:, :, :3]
    src_a = src[alpha]/255.0
    dst_a = dst[alpha]/255.0
    out[alpha] = src_a+dst_a*(1-src_a)
    old_setting = np.seterr(invalid = 'ignore')
    out[rgb] = (src[rgb]*src_a + dst[rgb]*dst_a*(1-src_a))/out[alpha]
    np.seterr(**old_setting)
    out[alpha] *= 255
    np.clip(out,0,255)
    # astype('uint8') maps np.nan (and np.inf) to 0
    out = out.astype('uint8')
    out = Image.fromarray(out, 'RGBA')
    return out

def build_cpp(compress_tool, ttf, prefix, min_label, max_label):
    out = subprocess.check_output([compress_tool, ttf, prefix]).decode()

    out += f'#include "{prefix}.hpp"\n\n'
    out += f'auto get_{prefix}_compressed_data() -> unsigned const* {{ return {prefix}_compressed_data; }}\n'
    out += f'auto get_{prefix}_compressed_size() -> unsigned        {{ return {prefix}_compressed_size; }}\n'

    return out

def build_hpp(codepoints, names, prefix, min_label, max_label):
    out = '#pragma once\n\n'

    codepoint_min = min(codepoints)
    codepoint_max = max(codepoints)

    out += f'#define {min_label} {hex(codepoint_min)}\n'
    out += f'#define {max_label} {hex(codepoint_max)}\n\n'

    for codepoint, name in zip(codepoints, names):
        out += f'#define {name} "\\u{hex(codepoint)[2:]}"\n'

    out += '\nauto get_kenney_game_icons_compressed_data() -> unsigned const*;\n'
    out += 'auto get_kenney_game_icons_compressed_size() -> unsigned;\n'

    return out

if __name__ == '__main__': main()