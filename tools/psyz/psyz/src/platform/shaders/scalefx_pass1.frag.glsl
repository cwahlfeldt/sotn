#version 450

// ScaleFX pass 1 for the Modern filter (sdl3_gpu_filter.h), at 1x: the
// strength of each pixel's four corners as interpolation candidates, from
// pass 0's metric. Written to a 16-bit float texture.
//
// ScaleFX by Sp00kyFox, ported from libretro's slang-shaders
// (edge-smoothing/scalefx/shaders/scalefx-pass1.slang) to SDL_GPU's SPIR-V
// bindings, with texel reads clamped to the picture and the parameters at
// their defaults. SPIR-V only for now: there is no MSL version yet.
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

layout(set = 2, binding = 0) uniform sampler2D src; // pass 0's metric

layout(set = 3, binding = 0) uniform Params {
    ivec2 size; // of the picture, at the texture's origin
    ivec2 pad;
};

layout(location = 0) out vec4 FragColor;

const float SFX_CLR = 0.50; // threshold (0.01-1)
const float SFX_SAA = 1.0;  // filter anti-aliasing (0 or 1)

// corner strength
float str(float d, vec2 a, vec2 b) {
    float diff = a.x - a.y;
    float wght1 = max(SFX_CLR - d, 0.0) / SFX_CLR;
    float wght2 = clamp(
        (1.0 - d) + (min(a.x, b.x) + a.x > min(a.y, b.y) + a.y ? diff : -diff),
        0.0, 1.0);
    return (SFX_SAA == 1.0 || 2.0 * d < a.x + a.y) ? (wght1 * wght2) *
                                                         (a.x * a.y)
                                                   : 0.0;
}

void main() {
    //  grid    metric  pattern
    //
    //  A B     x y z   x y
    //  D E F     o w   w z
    //  G H I
    ivec2 p = ivec2(gl_FragCoord.xy);
#define TEX(x, y) texelFetch(src, clamp(p + ivec2(x, y), ivec2(0), size - 1), 0)

    // metric data
    vec4 A = TEX(-1, -1), B = TEX(0, -1);
    vec4 D = TEX(-1, 0), E = TEX(0, 0), F = TEX(1, 0);
    vec4 G = TEX(-1, 1), H = TEX(0, 1), I = TEX(1, 1);

    // corner strength
    vec4 res;
    res.x = str(D.z, vec2(D.w, E.y), vec2(A.w, D.y));
    res.y = str(F.x, vec2(E.w, E.y), vec2(B.w, F.y));
    res.z = str(H.z, vec2(E.w, H.y), vec2(H.w, I.y));
    res.w = str(H.x, vec2(D.w, H.y), vec2(G.w, G.y));

    FragColor = res;
}
