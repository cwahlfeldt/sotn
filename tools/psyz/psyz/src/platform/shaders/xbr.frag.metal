// Hyllian's xBR-lv2 (libretro slang-shaders, edge-smoothing/xbr/shaders/
// xbr-lv2-standalone.slang) in MSL: one pass from the 1x picture straight to
// the output size, with the default parameters (colour distinction
// threshold 0.32, smoothness 0.30, anti-aliased blending, corner C).
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
// this reads the same texels with read(), clamped to the picture.

#include <metal_stdlib>
using namespace metal;

#define XBR_EQ_THRESHOLD 0.32
#define XBR_LV2_COEFFICIENT 0.30
#define lv2_cf (XBR_LV2_COEFFICIENT + 2.0)

struct Params {
    int2 origin;   // the 1x picture in the source texture
    int2 size;
    float2 output; // output size in pixels
    float2 pad;
};

constant float4 Ao = float4(1.0, -1.0, -1.0, 1.0);
constant float4 Bo = float4(1.0, 1.0, -1.0, -1.0);
constant float4 Co = float4(1.5, 0.5, -0.5, 0.5);
constant float4 Ax = float4(1.0, -1.0, -1.0, 1.0);
constant float4 Bx = float4(0.5, 2.0, -0.5, -2.0);
constant float4 Cx = float4(1.0, 1.0, -0.5, 0.0);
constant float4 Ay = float4(1.0, -1.0, -1.0, 1.0);
constant float4 By = float4(2.0, 0.5, -2.0, -0.5);
constant float4 Cy = float4(2.0, 0.0, -1.0, 0.5);
constant float4 Ci = float4(0.25, 0.25, 0.25, 0.25);

constant float3 v2f = float3(65536, 256, 1); // vec to float encode
constant float3 Y = float3(0.2627, 0.6780, 0.0593);

static inline float4 LTE(float4 A, float4 B) { return step(A, B); }
static inline float4 LT(float4 A, float4 B) {
    return select(float4(0.0), float4(1.0), A < B);
}
static inline float4 NOT(float4 A) { return float4(1.0) - A; }
static inline float4 diff(float4 A, float4 B) {
    return select(float4(0.0), float4(1.0), A != B);
}
static inline float dist(float3 A, float3 B) { return dot(abs(A - B), Y); }
static inline float4 dist4(float4x3 A, float4x3 B) {
    return float4(dist(A[0], B[0]), dist(A[1], B[1]), dist(A[2], B[2]),
                  dist(A[3], B[3]));
}
static inline float4 eq(float4x3 A, float4x3 B) {
    return step(dist4(A, B), float4(XBR_EQ_THRESHOLD));
}
static inline float4 neq(float4x3 A, float4x3 B) {
    return float4(1.0) - eq(A, B);
}
static inline float4 weighted_distance(float4x3 a, float4x3 b, float4x3 c,
                                       float4x3 d, float4x3 e, float4x3 f,
                                       float4x3 g, float4x3 h) {
    return dist4(a, b) + dist4(a, c) + dist4(d, e) + dist4(d, f) +
           4.0 * dist4(g, h);
}

static inline float3
Px(texture2d<float> t, constant Params& p, int2 e, int dx, int dy) {
    int2 c = clamp(e + int2(dx, dy), int2(0), p.size - 1) + p.origin;
    return t.read(uint2(c)).rgb;
}

struct main0_out {
    float4 color [[color(0)]];
};

fragment main0_out main0(float4 coord [[position]],
                         constant Params& p [[buffer(0)]],
                         texture2d<float> src [[texture(0)]],
                         sampler srcSmplr [[sampler(0)]]) {
    const float2 size = float2(p.size);
    const float2 pos = coord.xy / p.output * size; // in source texels
    const int2 e = int2(floor(pos));
    const float2 fp = fract(pos);
    const float aa_factor = 2.0 * size.x / p.output.x;

    //    A1 B1 C1
    // A0  A  B  C C4
    // D0  D  E  F F4
    // G0  G  H  I I4
    //    G5 H5 I5
#define N(dx, dy) Px(src, p, e, dx, dy)
    const float3 A1 = N(-1, -2), B1 = N(0, -2), C1 = N(1, -2);
    const float3 A = N(-1, -1), B = N(0, -1), C = N(1, -1);
    const float3 D = N(-1, 0), E = N(0, 0), F = N(1, 0);
    const float3 G = N(-1, 1), H = N(0, 1), I = N(1, 1);
    const float3 G5 = N(-1, 2), H5 = N(0, 2), I5 = N(1, 2);
    const float3 A0 = N(-2, -1), D0 = N(-2, 0), G0 = N(-2, 1);
    const float3 C4 = N(2, -1), F4 = N(2, 0), I4 = N(2, 1);
#undef N

    const float4x3 b = float4x3(B, D, H, F);
    const float4x3 c = float4x3(C, A, G, I);
    const float4x3 d = float4x3(D, H, F, B);
    const float4x3 e3 = float4x3(E, E, E, E);
    const float4x3 f = float4x3(F, B, D, H);
    const float4x3 g = float4x3(G, I, C, A);
    const float4x3 h = float4x3(H, F, B, D);
    const float4x3 i = float4x3(I, C, A, G);

    const float4x3 i4 = float4x3(I4, C1, A0, G5);
    const float4x3 i5 = float4x3(I5, C4, A1, G0);
    const float4x3 h5 = float4x3(H5, F4, B1, D0);
    const float4x3 f4 = float4x3(F4, B1, D0, H5);

    const float4 b_ = v2f * b;
    const float4 c_ = v2f * c;
    const float4 d_ = b_.yzwx;
    const float4 e_ = v2f * e3;
    const float4 f_ = b_.wxyz;
    const float4 g_ = c_.zwxy;
    const float4 h_ = b_.zwxy;

    // These inequations define the line below which interpolation occurs.
    const float4 fx = (Ao * fp.y + Bo * fp.x);
    const float4 fx_l = (Ax * fp.y + Bx * fp.x);
    const float4 fx_u = (Ay * fp.y + By * fp.x);

    const float4 irlv0 = diff(e_, f_) * diff(e_, h_);
    // corner C
    const float4 irlv1 = saturate(
        irlv0 * (neq(f, b) * neq(f, c) + neq(h, d) * neq(h, g) +
                 eq(e3, i) * (neq(f, f4) * neq(f, i4) + neq(h, h5) * neq(h, i5)) +
                 eq(e3, g) + eq(e3, c)));

    const float4 irlv2l = diff(e_, g_) * diff(d_, g_);
    const float4 irlv2u = diff(e_, c_) * diff(b_, c_);

    // blending (anti-aliased)
    const float4 delta = float4(aa_factor);
    const float4 deltaL = float4(0.5, 1.0, 0.5, 1.0) * aa_factor;
    const float4 deltaU = deltaL.yxwz;
    float4 fx45i = saturate(0.5 + (fx - Co - Ci) / delta);
    float4 fx45 = saturate(0.5 + (fx - Co) / delta);
    float4 fx30 = saturate(0.5 + (fx_l - Cx) / deltaL);
    float4 fx60 = saturate(0.5 + (fx_u - Cy) / deltaU);

    const float4 wd1 = weighted_distance(e3, c, g, i, h5, f4, h, f);
    const float4 wd2 = weighted_distance(h, d, i5, f, i4, b, e3, i);

    const float4 d_fg = dist4(f, g);
    const float4 d_hc = dist4(h, c);

    const float4 edri = LTE(wd1, wd2) * irlv0;
    const float4 edr = LT(wd1, wd2) * irlv1 * NOT(edri.yzwx * edri.wxyz);
    const float4 edr_l = LTE(lv2_cf * d_fg, d_hc) * irlv2l * edr *
                         (NOT(edri.yzwx) * eq(e3, c));
    const float4 edr_u = LTE(lv2_cf * d_hc, d_fg) * irlv2u * edr *
                         (NOT(edri.wxyz) * eq(e3, g));

    fx45i = edri * fx45i;
    fx45 = edr * fx45;
    fx30 = edr_l * fx30;
    fx60 = edr_u * fx60;

    const float4 px = LTE(dist4(e3, f), dist4(e3, h));

    const float4 maximos = max(max(fx30, fx60), max(fx45, fx45i));

    float3 res1 = mix(E, mix(H, F, px.x), maximos.x);
    float3 res2 = mix(E, mix(B, D, px.z), maximos.z);
    const float3 res1a = mix(res1, res2, step(dist(E, res1), dist(E, res2)));

    res1 = mix(E, mix(F, B, px.y), maximos.y);
    res2 = mix(E, mix(D, H, px.w), maximos.w);
    const float3 res1b = mix(res1, res2, step(dist(E, res1), dist(E, res2)));

    const float3 res =
        mix(res1a, res1b, step(dist(E, res1a), dist(E, res1b)));

    main0_out out = {};
    out.color = float4(res, 1.0);
    return out;
}
