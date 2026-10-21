#version 450

// ScaleFX pass 0 for the Modern filter (sdl3_gpu_filter.h), at 1x: the
// colour distances from each pixel to four of its neighbours, the metric the
// next passes work from. Written to a 16-bit float texture.
//
// ScaleFX by Sp00kyFox, ported from libretro's slang-shaders
// (edge-smoothing/scalefx/shaders/scalefx-pass0.slang) to SDL_GPU's SPIR-V
// bindings, with texel reads clamped to the picture. SPIR-V only for now:
// there is no MSL version yet.
//
// ScaleFX is an edge interpolation algorithm specialized in pixel art. It was
// originally intended as an improvement upon Scale3x but became a new filter
// in its own right. ScaleFX interpolates edges up to level 6 and makes smooth
// transitions between different slopes. The filtered picture will only
// consist of colours present in the original.
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

layout(set = 2, binding = 0) uniform sampler2D src; // the picture

layout(set = 3, binding = 0) uniform Params {
    ivec2 size; // of the picture, at the texture's origin
    ivec2 pad;
};

layout(location = 0) out vec4 FragColor;

// Reference: http://www.compuphase.com/cmetric.htm
float dist(vec3 A, vec3 B) {
    float r = 0.5 * (A.r + B.r);
    vec3 d = A - B;
    vec3 c = vec3(2.0 + r, 4.0, 3.0 - r);
    return sqrt(dot(c * d, d)) / 3.0;
}

void main() {
    //  grid    metric
    //
    //  A B C   x y z
    //    E F     o w
    ivec2 p = ivec2(gl_FragCoord.xy);
#define TEX(x, y)                                                              \
    texelFetch(src, clamp(p + ivec2(x, y), ivec2(0), size - 1), 0).rgb

    vec3 A = TEX(-1, -1);
    vec3 B = TEX(0, -1);
    vec3 C = TEX(1, -1);
    vec3 E = TEX(0, 0);
    vec3 F = TEX(1, 0);

    FragColor = vec4(dist(E, A), dist(E, B), dist(E, C), dist(E, F));
}
