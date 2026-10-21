#version 450

// A CRT look for the 1x picture, in one pass straight to the size the picture
// has in the window (like xbr.frag.glsl): each source line is a Gaussian
// beam, its pixels blur into each other a little, bright lines bloom into
// their neighbours, and an aperture grille covers the output pixels. The
// screen stays flat. Blending happens in linear light.
//
// After Timothy Lottes' "PUBLIC DOMAIN CRT STYLED SCAN-LINE SHADER" (2014):
// "Please take and use, change, or whatever."
//
// SPIR-V only for now: there is no MSL version yet.

layout(set = 2, binding = 0) uniform sampler2D src;

layout(set = 3, binding = 0) uniform Params {
    ivec2 origin; // the 1x picture in the source texture
    ivec2 size;
    vec2 outSize; // output size in pixels
    vec2 pad;
};

layout(location = 0) out vec4 FragColor;

const float HARD_SCAN = -8.0;        // beam height: -8 soft, -16 thin
const float HARD_PIX = -3.0;         // horizontal blur: -2 soft, -4 sharp
const float HARD_BLOOM_SCAN = -2.0;  // bloom spread, vertical
const float HARD_BLOOM_PIX = -1.5;   // bloom spread, horizontal
const float BLOOM_AMOUNT = 0.15;
const float MASK_DARK = 0.5;
const float MASK_LIGHT = 1.5;
const float BRIGHTNESS = 1.15; // makes up for the mask and the dark gaps

// the 7x5 source texels around the output pixel, in linear light
vec3 texels[5][7];

vec3 ToLinear(vec3 c) { return pow(c, vec3(2.2)); }
vec3 ToSrgb(vec3 c) { return pow(c, vec3(1.0 / 2.2)); }

float Gaus(float pos, float scale) { return exp2(scale * pos * pos); }

// the source texel at p, clamped to the picture
vec3 Fetch(ivec2 p) {
    p = clamp(p, ivec2(0), size - 1) + origin;
    return ToLinear(texelFetch(src, p, 0).rgb);
}

// line `row` (-2 to 2) filtered over 2 * taps + 1 texels; dx is the distance
// from the output pixel to the centre of the texel it falls in
vec3 Horz(int row, int taps, float hard, float dx) {
    vec3 sum = vec3(0.0);
    float wsum = 0.0;
    for (int k = -taps; k <= taps; k++) {
        float w = Gaus(float(k) + dx, hard);
        sum += texels[row + 2][k + 3] * w;
        wsum += w;
    }
    return sum / wsum;
}

// an aperture grille: vertical red, green and blue phosphor stripes
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
    vec2 pos = gl_FragCoord.xy / outSize * vec2(size); // in source texels
    ivec2 base = ivec2(floor(pos));
    vec2 d = 0.5 - fract(pos);

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 7; x++) {
            texels[y][x] = Fetch(base + ivec2(x - 3, y - 2));
        }
    }

    // the three nearest lines, each weighted by its beam's profile
    vec3 color = Horz(-1, 1, HARD_PIX, d.x) * Gaus(d.y - 1.0, HARD_SCAN) +
                 Horz(0, 2, HARD_PIX, d.x) * Gaus(d.y, HARD_SCAN) +
                 Horz(1, 1, HARD_PIX, d.x) * Gaus(d.y + 1.0, HARD_SCAN);

    vec3 bloom = vec3(0.0);
    for (int row = -2; row <= 2; row++) {
        int taps = row == -2 || row == 2 ? 2 : 3;
        bloom += Horz(row, taps, HARD_BLOOM_PIX, d.x) *
                 Gaus(d.y + float(row), HARD_BLOOM_SCAN);
    }
    color += bloom * BLOOM_AMOUNT;

    color *= Mask(floor(gl_FragCoord.xy)) * BRIGHTNESS;

    FragColor = vec4(ToSrgb(color), 1.0);
}
