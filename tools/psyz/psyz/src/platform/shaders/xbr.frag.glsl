#version 450

// Hyllian's xBR-lv2 (libretro slang-shaders, edge-smoothing/xbr/shaders/
// xbr-lv2-standalone.slang): one pass from the 1x picture straight to the
// output size, with the default parameters (colour distinction threshold
// 0.32, smoothness 0.30, anti-aliased blending, corner C). The GLSL (SPIR-V)
// version of xbr.frag.metal; keep the two in step.
//
// Copyright (C) 2011-2022 Hyllian - sergiogdb@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Incorporates some of the ideas from SABR shader. Thanks to Joshua Street.
//
// The original samples with normalized coordinates and nearest filtering;
// this reads the same texels with texelFetch(), clamped to the picture.

#define XBR_EQ_THRESHOLD 0.32
#define XBR_LV2_COEFFICIENT 0.30
#define lv2_cf (XBR_LV2_COEFFICIENT + 2.0)

layout(set = 2, binding = 0) uniform sampler2D src;

layout(set = 3, binding = 0) uniform Params {
    ivec2 origin; // the 1x picture in the source texture
    ivec2 size;
    vec2 outSize; // output size in pixels
    vec2 pad;
};

layout(location = 0) out vec4 FragColor;

const vec4 Ao = vec4(1.0, -1.0, -1.0, 1.0);
const vec4 Bo = vec4(1.0, 1.0, -1.0, -1.0);
const vec4 Co = vec4(1.5, 0.5, -0.5, 0.5);
const vec4 Ax = vec4(1.0, -1.0, -1.0, 1.0);
const vec4 Bx = vec4(0.5, 2.0, -0.5, -2.0);
const vec4 Cx = vec4(1.0, 1.0, -0.5, 0.0);
const vec4 Ay = vec4(1.0, -1.0, -1.0, 1.0);
const vec4 By = vec4(2.0, 0.5, -2.0, -0.5);
const vec4 Cy = vec4(2.0, 0.0, -1.0, 0.5);
const vec4 Ci = vec4(0.25, 0.25, 0.25, 0.25);

const vec3 v2f = vec3(65536, 256, 1); // vec to float encode
const vec3 Y = vec3(0.2627, 0.6780, 0.0593);

vec4 LTE(vec4 A, vec4 B) { return step(A, B); }
vec4 LT(vec4 A, vec4 B) { return vec4(lessThan(A, B)); }
vec4 NOT(vec4 A) { return vec4(1.0) - A; }
vec4 diff(vec4 A, vec4 B) { return vec4(notEqual(A, B)); }
float dist(vec3 A, vec3 B) { return dot(abs(A - B), Y); }
vec4 dist4(mat4x3 A, mat4x3 B) {
    return vec4(dist(A[0], B[0]), dist(A[1], B[1]), dist(A[2], B[2]),
                dist(A[3], B[3]));
}
vec4 eq(mat4x3 A, mat4x3 B) {
    return step(dist4(A, B), vec4(XBR_EQ_THRESHOLD));
}
vec4 neq(mat4x3 A, mat4x3 B) { return vec4(1.0) - eq(A, B); }
vec4 weighted_distance(mat4x3 a, mat4x3 b, mat4x3 c, mat4x3 d, mat4x3 e,
                       mat4x3 f, mat4x3 g, mat4x3 h) {
    return dist4(a, b) + dist4(a, c) + dist4(d, e) + dist4(d, f) +
           4.0 * dist4(g, h);
}

vec3 Px(ivec2 e, int dx, int dy) {
    ivec2 c = clamp(e + ivec2(dx, dy), ivec2(0), size - 1) + origin;
    return texelFetch(src, c, 0).rgb;
}

void main() {
    vec2 fsize = vec2(size);
    vec2 pos = gl_FragCoord.xy / outSize * fsize; // in source texels
    ivec2 e = ivec2(floor(pos));
    vec2 fp = fract(pos);
    float aa_factor = 2.0 * fsize.x / outSize.x;

    //    A1 B1 C1
    // A0  A  B  C C4
    // D0  D  E  F F4
    // G0  G  H  I I4
    //    G5 H5 I5
#define N(dx, dy) Px(e, dx, dy)
    vec3 A1 = N(-1, -2), B1 = N(0, -2), C1 = N(1, -2);
    vec3 A = N(-1, -1), B = N(0, -1), C = N(1, -1);
    vec3 D = N(-1, 0), E = N(0, 0), F = N(1, 0);
    vec3 G = N(-1, 1), H = N(0, 1), I = N(1, 1);
    vec3 G5 = N(-1, 2), H5 = N(0, 2), I5 = N(1, 2);
    vec3 A0 = N(-2, -1), D0 = N(-2, 0), G0 = N(-2, 1);
    vec3 C4 = N(2, -1), F4 = N(2, 0), I4 = N(2, 1);
#undef N

    mat4x3 b = mat4x3(B, D, H, F);
    mat4x3 c = mat4x3(C, A, G, I);
    mat4x3 d = mat4x3(D, H, F, B);
    mat4x3 e3 = mat4x3(E, E, E, E);
    mat4x3 f = mat4x3(F, B, D, H);
    mat4x3 g = mat4x3(G, I, C, A);
    mat4x3 h = mat4x3(H, F, B, D);
    mat4x3 i = mat4x3(I, C, A, G);

    mat4x3 i4 = mat4x3(I4, C1, A0, G5);
    mat4x3 i5 = mat4x3(I5, C4, A1, G0);
    mat4x3 h5 = mat4x3(H5, F4, B1, D0);
    mat4x3 f4 = mat4x3(F4, B1, D0, H5);

    vec4 b_ = v2f * b;
    vec4 c_ = v2f * c;
    vec4 d_ = b_.yzwx;
    vec4 e_ = v2f * e3;
    vec4 f_ = b_.wxyz;
    vec4 g_ = c_.zwxy;
    vec4 h_ = b_.zwxy;

    // These inequations define the line below which interpolation occurs.
    vec4 fx = (Ao * fp.y + Bo * fp.x);
    vec4 fx_l = (Ax * fp.y + Bx * fp.x);
    vec4 fx_u = (Ay * fp.y + By * fp.x);

    vec4 irlv0 = diff(e_, f_) * diff(e_, h_);
    // corner C
    vec4 irlv1 = clamp(
        irlv0 * (neq(f, b) * neq(f, c) + neq(h, d) * neq(h, g) +
                 eq(e3, i) * (neq(f, f4) * neq(f, i4) + neq(h, h5) * neq(h, i5)) +
                 eq(e3, g) + eq(e3, c)),
        0.0, 1.0);

    vec4 irlv2l = diff(e_, g_) * diff(d_, g_);
    vec4 irlv2u = diff(e_, c_) * diff(b_, c_);

    // blending (anti-aliased)
    vec4 delta = vec4(aa_factor);
    vec4 deltaL = vec4(0.5, 1.0, 0.5, 1.0) * aa_factor;
    vec4 deltaU = deltaL.yxwz;
    vec4 fx45i = clamp(0.5 + (fx - Co - Ci) / delta, 0.0, 1.0);
    vec4 fx45 = clamp(0.5 + (fx - Co) / delta, 0.0, 1.0);
    vec4 fx30 = clamp(0.5 + (fx_l - Cx) / deltaL, 0.0, 1.0);
    vec4 fx60 = clamp(0.5 + (fx_u - Cy) / deltaU, 0.0, 1.0);

    vec4 wd1 = weighted_distance(e3, c, g, i, h5, f4, h, f);
    vec4 wd2 = weighted_distance(h, d, i5, f, i4, b, e3, i);

    vec4 d_fg = dist4(f, g);
    vec4 d_hc = dist4(h, c);

    vec4 edri = LTE(wd1, wd2) * irlv0;
    vec4 edr = LT(wd1, wd2) * irlv1 * NOT(edri.yzwx * edri.wxyz);
    vec4 edr_l = LTE(lv2_cf * d_fg, d_hc) * irlv2l * edr *
                 (NOT(edri.yzwx) * eq(e3, c));
    vec4 edr_u = LTE(lv2_cf * d_hc, d_fg) * irlv2u * edr *
                 (NOT(edri.wxyz) * eq(e3, g));

    fx45i = edri * fx45i;
    fx45 = edr * fx45;
    fx30 = edr_l * fx30;
    fx60 = edr_u * fx60;

    vec4 px = LTE(dist4(e3, f), dist4(e3, h));

    vec4 maximos = max(max(fx30, fx60), max(fx45, fx45i));

    vec3 res1 = mix(E, mix(H, F, px.x), maximos.x);
    vec3 res2 = mix(E, mix(B, D, px.z), maximos.z);
    vec3 res1a = mix(res1, res2, step(dist(E, res1), dist(E, res2)));

    res1 = mix(E, mix(F, B, px.y), maximos.y);
    res2 = mix(E, mix(D, H, px.w), maximos.w);
    vec3 res1b = mix(res1, res2, step(dist(E, res1), dist(E, res2)));

    vec3 res = mix(res1a, res1b, step(dist(E, res1a), dist(E, res1b)));

    FragColor = vec4(res, 1.0);
}
