#version 450

// A triangle covering the viewport, for full-screen passes (MMPX, xBR). The
// GLSL (SPIR-V) version of mmpx.vert.metal.

void main() {
    vec2 p = vec2(float((gl_VertexIndex << 1) & 2), float(gl_VertexIndex & 2));
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
