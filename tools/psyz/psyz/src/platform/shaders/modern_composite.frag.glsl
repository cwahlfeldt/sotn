#version 450

// Modern + CRT's composite video pass (sdl3_gpu_filter.h), at the size the
// picture has in the window, on Modern's picture: like an NTSC composite
// signal it keeps the brightness (Y) fairly sharp but smears the colour (I
// and Q) sideways over a few of the game's pixels, which also blends the
// PlayStation's dithering. Works on the gamma-encoded signal, as a TV did;
// no colour fringing or dot crawl.
//
// SPIR-V only for now: there is no MSL version yet.

layout(set = 2, binding = 0) uniform sampler2D picture; // Modern's output

layout(set = 3, binding = 0) uniform Params {
    vec2 outSize;    // output size in pixels, the picture's too
    vec2 srcSize;    // the game's 1x picture, for sizes in its pixels
    vec2 pictureTex; // the picture's texture size
    vec2 pad;
};

layout(location = 0) out vec4 FragColor;

// blur widths (Gaussian sigmas), in the game's pixels
const float LUMA_SIGMA = 0.4;
const float I_SIGMA = 1.1;
const float Q_SIGMA = 1.6;
const int TAPS = 8; // on each side, half a game pixel apart

// column-major: Y = 0.299 R + 0.587 G + 0.114 B, and so on
const mat3 RGB_TO_YIQ =
    mat3(0.299, 0.596, 0.211, 0.587, -0.274, -0.523, 0.114, -0.322, 0.312);
const mat3 YIQ_TO_RGB =
    mat3(1.0, 1.0, 1.0, 0.956, -0.272, -1.106, 0.621, -0.647, 1.703);

// the picture at `pos`, in output pixels, filtered linearly
vec3 Px(vec2 pos) {
    return texture(picture, clamp(pos, vec2(0.5), outSize - 0.5) / pictureTex)
        .rgb;
}

void main() {
    const vec3 sigma = vec3(LUMA_SIGMA, I_SIGMA, Q_SIGMA);
    float step = 0.5 * outSize.x / srcSize.x; // half a game pixel
    vec3 sum = vec3(0.0);
    vec3 wsum = vec3(0.0);
    for (int k = -TAPS; k <= TAPS; k++) {
        float x = 0.5 * float(k); // in game pixels
        vec3 w = exp(-x * x / (2.0 * sigma * sigma));
        vec3 yiq = RGB_TO_YIQ * Px(gl_FragCoord.xy + vec2(float(k) * step, 0.0));
        sum += yiq * w;
        wsum += w;
    }
    FragColor = vec4(clamp(YIQ_TO_RGB * (sum / wsum), 0.0, 1.0), 1.0);
}
