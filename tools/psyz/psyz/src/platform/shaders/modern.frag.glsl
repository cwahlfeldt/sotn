#version 450

// The Modern filter's last pass (sdl3_gpu_filter.h), at the size the picture
// has in the window: scales ScaleFX's 3x picture (scalefx_pass*.frag.glsl,
// in the sharpened colours of modern_sharpen.frag.glsl) the rest of the way
// keeping its subpixels crisp with anti-aliased edges, adds the bloom
// (modern_bloom.frag.glsl) with a soft shoulder instead of clipping, lifts
// contrast and colour a little and darkens the corners. Both inputs are
// sampled with linear filtering.
//
// SPIR-V only for now: there is no MSL version yet.

layout(set = 2, binding = 0) uniform sampler2D picture; // ScaleFX, at 3x
layout(set = 2, binding = 1) uniform sampler2D bloom;   // linear light

layout(set = 3, binding = 0) uniform Params {
    vec2 outSize;     // output size in pixels
    vec2 pictureSize; // the picture, at its texture's origin
    vec2 pictureTex;  // the size of that texture
    vec2 bloomSize;   // the bloom, the same way
    vec2 bloomTex;
    vec2 pad;
};

layout(location = 0) out vec4 FragColor;

const float BLOOM = 0.1;       // glow strength
const float SHOULDER = 0.8;    // linear level where highlights start to roll off
const float SATURATION = 1.2;
const float CONTRAST = 1.1;    // around PIVOT: the game is mostly dark, so
const float PIVOT = 0.3;       // a mid-grey pivot would only darken it
const float VIGNETTE = 0.12;   // how much darker the corners are

// texture coordinates for `pos`, in pixels of a `size` picture at the origin
// of a `tex` sized texture, kept half a texel inside the picture so linear
// filtering never reads past it (textures are reused, so often larger)
vec2 TexUv(vec2 pos, vec2 size, vec2 tex) {
    return clamp(pos, vec2(0.5), size - 0.5) / tex;
}

// the picture at output pixel `p`, each source pixel a solid block: only the
// last half output pixel at a block's edges blends with its neighbour, so at
// sizes that aren't whole multiples the blocks stay the same width instead
// of some being a pixel wider ("sharp bilinear")
vec3 Picture(vec2 p) {
    vec2 scale = outSize / pictureSize;
    vec2 texel = p / scale;
    vec2 d = fract(texel) - 0.5;
    vec2 range = max(0.5 - 0.5 / scale, 0.0);
    vec2 f = (d - clamp(d, -range, range)) * scale + 0.5;
    vec2 pos = floor(texel) + f;
    return texture(picture, TexUv(pos, pictureSize, pictureTex)).rgb;
}

// leaves x alone up to SHOULDER, then eases it towards 1
vec3 Shoulder(vec3 x) {
    vec3 over = max(x - SHOULDER, 0.0);
    return min(x, SHOULDER) +
           (1.0 - SHOULDER) * (1.0 - exp(-over / (1.0 - SHOULDER)));
}

void main() {
    vec2 uv = gl_FragCoord.xy / outSize;

    vec3 glow = texture(bloom, TexUv(uv * bloomSize, bloomSize, bloomTex)).rgb;
    vec3 color = Picture(gl_FragCoord.xy);
    color = Shoulder(pow(color, vec3(2.2)) + glow * BLOOM);
    color = pow(color, vec3(1.0 / 2.2));

    float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
    color = clamp(mix(vec3(luma), color, SATURATION), 0.0, 1.0);
    color = clamp((color - PIVOT) * CONTRAST + PIVOT, 0.0, 1.0);

    vec2 v = uv * 2.0 - 1.0;
    color *= 1.0 - VIGNETTE * dot(v, v) * 0.5;

    FragColor = vec4(color, 1.0);
}
