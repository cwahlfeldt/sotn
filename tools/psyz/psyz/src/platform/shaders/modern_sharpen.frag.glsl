#version 450

// The Modern filter's first pass (sdl3_gpu_filter.h), at the 1x picture's
// size: brings out the detail of the original pixels with contrast adaptive
// sharpening (after AMD FidelityFX CAS, MIT licensed), which pushes each
// pixel away from its neighbours, less where they already differ a lot.
// Working on the 1x pixels keeps them square once they are scaled up.
//
// SPIR-V only for now: there is no MSL version yet.

layout(set = 2, binding = 0) uniform sampler2D src;

layout(set = 3, binding = 0) uniform Params {
    ivec2 size; // of the picture, at the texture's origin
    ivec2 pad;
};

layout(location = 0) out vec4 FragColor;

const float SHARPNESS = 0.0; // 0 subtle, 1 strong

vec3 Px(ivec2 p) {
    return texelFetch(src, clamp(p, ivec2(0), size - 1), 0).rgb;
}

void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    vec3 e = Px(p);
    vec3 b = Px(p + ivec2(0, -1));
    vec3 d = Px(p + ivec2(-1, 0));
    vec3 f = Px(p + ivec2(1, 0));
    vec3 h = Px(p + ivec2(0, 1));
    vec3 mn = min(e, min(min(b, d), min(f, h)));
    vec3 mx = max(e, max(max(b, d), max(f, h)));
    vec3 amp = sqrt(clamp(min(mn, 1.0 - mx) / max(mx, 1e-4), 0.0, 1.0));
    vec3 w = -amp / mix(8.0, 5.0, SHARPNESS);
    vec3 color = clamp((e + (b + d + f + h) * w) / (1.0 + 4.0 * w), 0.0, 1.0);
    FragColor = vec4(color, 1.0);
}
