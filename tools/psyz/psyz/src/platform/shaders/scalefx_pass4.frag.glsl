#version 450

// ScaleFX pass 4 for the Modern filter (sdl3_gpu_filter.h), at 3x: each
// pixel becomes 3x3 subpixels, each a copy of the pixel or of the neighbour
// pass 3 tagged for it, so the picture keeps only its own colours. The
// colours come from `colors` (the sharpened picture), the tags from the
// original's edges. SFX_STRENGTH (not in the original) blends each subpixel
// back towards its own pixel, which turns the smoothing down: at 0 the
// picture is plain 3x pixels, at 1 full ScaleFX.
//
// ScaleFX by Sp00kyFox, ported from libretro's slang-shaders
// (edge-smoothing/scalefx/shaders/scalefx-pass4.slang) to SDL_GPU's SPIR-V
// bindings, with texel reads clamped to the picture. SPIR-V only for now:
// there is no MSL version yet.
//
// Copyright (c) 2016 Sp00kyFox - ScaleFX@web.de
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

layout(set = 2, binding = 0) uniform sampler2D tags;   // pass 3's output
layout(set = 2, binding = 1) uniform sampler2D colors; // at 1x

layout(set = 3, binding = 0) uniform Params {
    ivec2 size; // of the 1x picture, at the texture's origin
    ivec2 pad;
};

layout(location = 0) out vec4 FragColor;

const float SFX_STRENGTH = 0.35; // 0 plain pixels, 1 full ScaleFX

// extract corners
vec4 loadCrn(vec4 x) { return floor(mod(x * 80.0 + 0.5, 9.0)); }

// extract mids
vec4 loadMid(vec4 x) { return floor(mod(x * 8.888888 + 0.055555, 9.0)); }

void main() {
    //  grid    corners  mids
    //
    //    B     x   y     x
    //  D E F           w   y
    //    H     w   z     z
    ivec2 o = ivec2(gl_FragCoord.xy);
    ivec2 p = o / 3;
    vec2 fp = vec2(o - p * 3); // the subpixel, 0-2 each way

    // read data
    vec4 E = texelFetch(tags, clamp(p, ivec2(0), size - 1), 0);

    // extract data
    vec4 crn = loadCrn(E);
    vec4 mid = loadMid(E);

    // determine subpixel
    float sp = fp.y == 0.0 ? (fp.x == 0.0   ? crn.x
                              : fp.x == 1.0 ? mid.x
                                            : crn.y)
                           : (fp.y == 1.0 ? (fp.x == 0.0   ? mid.w
                                             : fp.x == 1.0 ? 0.0
                                                           : mid.y)
                                          : (fp.x == 0.0   ? crn.w
                                             : fp.x == 1.0 ? mid.z
                                                           : crn.z));

    // output coordinate - 0 = E, 1 = D, 2 = D0, 3 = F, 4 = F0, 5 = B, 6 = B0,
    // 7 = H, 8 = H0
    ivec2 res = sp == 0.0   ? ivec2(0, 0)
                : sp == 1.0 ? ivec2(-1, 0)
                : sp == 2.0 ? ivec2(-2, 0)
                : sp == 3.0 ? ivec2(1, 0)
                : sp == 4.0 ? ivec2(2, 0)
                : sp == 5.0 ? ivec2(0, -1)
                : sp == 6.0 ? ivec2(0, -2)
                : sp == 7.0 ? ivec2(0, 1)
                            : ivec2(0, 2);

    vec4 own = texelFetch(colors, clamp(p, ivec2(0), size - 1), 0);
    vec4 sub = texelFetch(colors, clamp(p + res, ivec2(0), size - 1), 0);
    FragColor = mix(own, sub, SFX_STRENGTH);
}
