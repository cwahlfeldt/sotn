#version 450

// Bloom for the Modern filter (sdl3_gpu_filter.h), at the 1x picture's size:
// one direction of a separable Gaussian blur. The first, horizontal pass reads
// the picture and keeps only its bright parts, in linear light; both passes
// write a 16-bit float texture so the glow's falloff doesn't band.
//
// SPIR-V only for now: there is no MSL version yet.

layout(set = 2, binding = 0) uniform sampler2D src;

layout(set = 3, binding = 0) uniform Params {
    ivec2 size; // of the input, at the texture's origin
    ivec2 dir;  // (1, 0) or (0, 1)
    int brightPass; // the input is the picture: keep its bright parts
};

layout(location = 0) out vec4 FragColor;

// by its brightest channel, so saturated light (fire, magic) glows as much
// as white. Pale stone and hair highlights peak around 0.8; light sources
// reach the PlayStation's brightest (248/255, 0.94 linear).
const float THRESHOLD = 0.72; // linear level where the glow starts
const float KNEE = 0.24;      // and how gradually it comes in
const int RADIUS = 9;         // taps on each side
const float SIGMA = 3.5;      // in 1x pixels

vec3 Fetch(ivec2 p) {
    vec3 c = texelFetch(src, clamp(p, ivec2(0), size - 1), 0).rgb;
    if (brightPass != 0) {
        c = pow(c, vec3(2.2));
        float peak = max(c.r, max(c.g, c.b));
        c *= smoothstep(THRESHOLD, THRESHOLD + KNEE, peak);
    }
    return c;
}

void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    vec3 sum = vec3(0.0);
    float wsum = 0.0;
    for (int k = -RADIUS; k <= RADIUS; k++) {
        float w = exp(-float(k * k) / (2.0 * SIGMA * SIGMA));
        sum += Fetch(p + dir * k) * w;
        wsum += w;
    }
    FragColor = vec4(sum / wsum, 1.0);
}
