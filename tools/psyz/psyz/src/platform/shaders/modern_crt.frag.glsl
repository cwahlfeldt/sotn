#version 450

// Modern + CRT's last pass (sdl3_gpu_filter.h), at the size the picture has
// in the window, on the composite picture (modern_composite.frag.glsl): each
// of the game's lines becomes a Gaussian beam, its neighbours bleeding into
// the gaps and bright lines blooming a little further (as in crt.frag.glsl,
// after Timothy Lottes' public domain CRT shader), under an aperture grille
// at the window's pixels. Flat screen. Blending happens in linear light.
//
// SPIR-V only for now: there is no MSL version yet.

layout(set = 2, binding = 0) uniform sampler2D picture; // the composite one

layout(set = 3, binding = 0) uniform Params {
    vec2 outSize;    // output size in pixels, the picture's too
    vec2 srcSize;    // the game's 1x picture: its lines are the scanlines
    vec2 pictureTex; // the picture's texture size
    vec2 pad;
};

layout(location = 0) out vec4 FragColor;

const float HARD_SCAN = -8.0;  // beam height: -8 soft, -16 thin
const float HARD_BLOOM_SCAN = -2.0; // bloom spread, vertical
const float BLOOM_AMOUNT = 0.15;
const float MASK_DARK = 0.5;
const float MASK_LIGHT = 1.5;
const float BRIGHTNESS = 1.15; // makes up for the mask and the dark gaps

float Gaus(float pos, float scale) { return exp2(scale * pos * pos); }

// the picture along the middle of game line `line`, at output column x, in
// linear light
vec3 Line(float x, float line) {
    vec2 pos = vec2(x, (line + 0.5) * outSize.y / srcSize.y);
    pos = clamp(pos, vec2(0.5), outSize - 0.5);
    return pow(texture(picture, pos / pictureTex).rgb, vec3(2.2));
}

// vertical red, green and blue phosphor stripes
vec3 Mask(vec2 pix) {
    vec3 mask = vec3(MASK_DARK);
    float x = fract(pix.x / 3.0);
    if (x < 1.0 / 3.0) {
        mask.r = MASK_LIGHT;
    } else if (x < 2.0 / 3.0) {
        mask.g = MASK_LIGHT;
    } else {
        mask.b = MASK_LIGHT;
    }
    return mask;
}

void main() {
    float x = gl_FragCoord.x;
    float pos = gl_FragCoord.y / outSize.y * srcSize.y; // in game lines
    float line = floor(pos);
    float d = 0.5 - fract(pos); // to the line's centre

    // the five nearest lines: the three nearest each weighted by its beam's
    // profile, and all five blooming
    vec3 color = vec3(0.0);
    vec3 bloom = vec3(0.0);
    for (int k = -2; k <= 2; k++) {
        vec3 l = Line(x, line + float(k));
        if (k >= -1 && k <= 1) {
            color += l * Gaus(d + float(k), HARD_SCAN);
        }
        bloom += l * Gaus(d + float(k), HARD_BLOOM_SCAN);
    }
    color += bloom * BLOOM_AMOUNT;

    color *= Mask(floor(gl_FragCoord.xy)) * BRIGHTNESS;
    FragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
