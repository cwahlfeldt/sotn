// MMPX style-preserving pixel-art magnification, one 2x pass. Every output
// pixel is one quarter of a source pixel E: J (top left), K (top right),
// L (bottom left) or M (bottom right).
//
// The algorithm and its reference implementation are by Morgan McGuire and
// Mara Gagiu, "MMPX Style-Preserving Pixel-Art Magnification", Journal of
// Computer Graphics Techniques 10(2), 2021, under this license:
//
// Copyright 2020 Morgan McGuire & Mara Gagiu
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// The neighbourhood of E:
//
//           P
//        A  B  C
//     Q  D  E  F  R
//        G  H  I
//           S
//
// Colours compare on RGB only: psyz keeps the PlayStation mask bit in alpha.
// Written by hand in MSL (no GLSL toolchain on the machine it was made on).

#include <metal_stdlib>
using namespace metal;

struct Params {
    int2 origin; // source rectangle in the texture
    int2 size;
};

static inline uint
Px(texture2d<float> t, constant Params& p, int2 e, int dx, int dy) {
    int2 c = clamp(e + int2(dx, dy), int2(0), p.size - 1) + p.origin;
    uint3 u = uint3(t.read(uint2(c)).rgb * 255.0 + 0.5);
    return u.r | (u.g << 8) | (u.b << 16);
}

static inline int Luma(uint c) {
    return int(c & 0xFFu) + int((c >> 8) & 0xFFu) + int((c >> 16) & 0xFFu) +
           1;
}

static inline bool AllEq2(uint b, uint a0, uint a1) {
    return b == a0 && b == a1;
}
static inline bool AllEq3(uint b, uint a0, uint a1, uint a2) {
    return b == a0 && b == a1 && b == a2;
}
static inline bool AllEq4(uint b, uint a0, uint a1, uint a2, uint a3) {
    return b == a0 && b == a1 && b == a2 && b == a3;
}
static inline bool AnyEq3(uint b, uint a0, uint a1, uint a2) {
    return b == a0 || b == a1 || b == a2;
}
static inline bool NoneEq2(uint b, uint a0, uint a1) {
    return b != a0 && b != a1;
}
static inline bool NoneEq4(uint b, uint a0, uint a1, uint a2, uint a3) {
    return b != a0 && b != a1 && b != a2 && b != a3;
}

struct main0_out {
    float4 color [[color(0)]];
};

fragment main0_out main0(float4 coord [[position]],
                         constant Params& p [[buffer(0)]],
                         texture2d<float> src [[texture(0)]],
                         sampler srcSmplr [[sampler(0)]]) {
    const int2 o = int2(coord.xy);
    const int2 e = o >> 1;
#define N(dx, dy) Px(src, p, e, dx, dy)
    const uint A = N(-1, -1), B = N(0, -1), C = N(1, -1);
    const uint D = N(-1, 0), E = N(0, 0), F = N(1, 0);
    const uint G = N(-1, 1), H = N(0, 1), I = N(1, 1);
    uint J = E, K = E, L = E, M = E;

    if (A != E || B != E || C != E || D != E || F != E || G != E || H != E ||
        I != E) {
        const uint P = N(0, -2), Q = N(-2, 0), R = N(2, 0), S = N(0, 2);
        const int Bl = Luma(B), Dl = Luma(D), El = Luma(E), Fl = Luma(F),
                  Hl = Luma(H);

        // 1:1 slope rules
        if ((D == B && D != H && D != F) && (El >= Dl || E == A) &&
            AnyEq3(E, A, C, G) && ((El < Dl) || A != D || E != P || E != Q)) {
            J = D;
        }
        if ((B == F && B != D && B != H) && (El >= Bl || E == C) &&
            AnyEq3(E, A, C, I) && ((El < Bl) || C != B || E != P || E != R)) {
            K = B;
        }
        if ((H == D && H != F && H != B) && (El >= Hl || E == G) &&
            AnyEq3(E, A, G, I) && ((El < Hl) || G != H || E != S || E != Q)) {
            L = H;
        }
        if ((F == H && F != B && F != D) && (El >= Fl || E == I) &&
            AnyEq3(E, C, G, I) && ((El < Fl) || I != H || E != R || E != S)) {
            M = F;
        }

        // intersection rules
        if ((E != F && AllEq4(E, C, I, D, Q) && AllEq2(F, B, H)) &&
            (F != N(3, 0))) {
            K = M = F;
        }
        if ((E != D && AllEq4(E, A, G, F, R) && AllEq2(D, B, H)) &&
            (D != N(-3, 0))) {
            J = L = D;
        }
        if ((E != H && AllEq4(E, G, I, B, P) && AllEq2(H, D, F)) &&
            (H != N(0, 3))) {
            L = M = H;
        }
        if ((E != B && AllEq4(E, A, C, H, S) && AllEq2(B, D, F)) &&
            (B != N(0, -3))) {
            J = K = B;
        }

        // triangle tip rules
        if (Bl < El && AllEq4(E, G, H, I, S) && NoneEq4(E, A, D, C, F)) {
            J = K = B;
        }
        if (Hl < El && AllEq4(E, A, B, C, P) && NoneEq4(E, D, G, I, F)) {
            L = M = H;
        }
        if (Fl < El && AllEq4(E, A, D, G, Q) && NoneEq4(E, B, C, I, H)) {
            K = M = F;
        }
        if (Dl < El && AllEq4(E, C, F, I, R) && NoneEq4(E, B, A, G, H)) {
            J = L = D;
        }

        // 2:1 slope rules
        if (H != B) {
            if (H != A && H != E && H != C) {
                if (AllEq3(H, G, F, R) && NoneEq2(H, D, N(2, -1))) {
                    L = M;
                }
                if (AllEq3(H, I, D, Q) && NoneEq2(H, F, N(-2, -1))) {
                    M = L;
                }
            }
            if (B != I && B != G && B != E) {
                if (AllEq3(B, A, F, R) && NoneEq2(B, D, N(2, 1))) {
                    J = K;
                }
                if (AllEq3(B, C, D, Q) && NoneEq2(B, F, N(-2, 1))) {
                    K = J;
                }
            }
        }
        if (F != D) {
            if (D != I && D != E && D != C) {
                if (AllEq3(D, A, H, S) && NoneEq2(D, B, N(1, 2))) {
                    J = L;
                }
                if (AllEq3(D, G, B, P) && NoneEq2(D, H, N(1, -2))) {
                    L = J;
                }
            }
            if (F != E && F != A && F != G) {
                if (AllEq3(F, C, H, S) && NoneEq2(F, B, N(-1, 2))) {
                    K = M;
                }
                if (AllEq3(F, I, B, P) && NoneEq2(F, H, N(-1, -2))) {
                    M = K;
                }
            }
        }
    }
#undef N

    const uint c = (o.y & 1) ? ((o.x & 1) ? M : L) : ((o.x & 1) ? K : J);
    main0_out out = {};
    out.color = float4(float(c & 0xFFu), float((c >> 8) & 0xFFu),
                       float((c >> 16) & 0xFFu), 255.0) /
                255.0;
    return out;
}
