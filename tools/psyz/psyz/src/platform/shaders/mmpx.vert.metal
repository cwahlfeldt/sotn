// A triangle covering the viewport, for full-screen passes (MMPX).
#include <metal_stdlib>
using namespace metal;

struct main0_out {
    float4 gl_Position [[position]];
};

vertex main0_out main0(uint vid [[vertex_id]]) {
    main0_out out = {};
    const float2 p = float2(float((vid << 1) & 2u), float(vid & 2u));
    out.gl_Position = float4(p * 2.0 - 1.0, 0.0, 1.0);
    return out;
}
