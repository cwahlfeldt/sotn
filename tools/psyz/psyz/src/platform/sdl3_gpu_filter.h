// Output filters (Psyz_VideoSetFilter) for the SDL_GPU backend, included by
// sdl3_gpu.c. The game renders at 1x; before the blit to the swapchain the
// display area is copied to a texture of its own and magnified:
// - MMPX (shaders/mmpx.frag.metal, .glsl) in 2x passes, as many as fit the
//   window's height (up to 8x); the blit stretches the rest of the way
//   linearly.
// - xBR-lv2 (shaders/xbr.frag.metal, .glsl) in one pass straight to the size
//   the picture has in the window, 4:3 stretch included; the blit copies it.
// - CRT (shaders/crt.frag.glsl; Vulkan only, there is no MSL version yet)
//   the same way as xBR: scanlines, bloom and an aperture grille.
// - Modern (shaders/modern*.frag.glsl, scalefx_pass*.frag.glsl; Vulkan
//   only) keeps the pixel art's look: it sharpens the 1x picture, smooths its
//   staircase edges to 3x with ScaleFX (four passes at 1x, one at 3x, using
//   only the picture's own colours), blurs its bright parts into a bloom
//   (two passes) and then, in a last pass at the window's size, scales the
//   3x subpixels the rest of the way as crisp blocks, adds the bloom and
//   grades the colour.
// - Modern + CRT (shaders/modern_composite.frag.glsl, modern_crt.frag.glsl;
//   Vulkan only) puts Modern's picture through NTSC-style composite video
//   (colour smeared sideways) and then onto a CRT's scanlines and aperture
//   grille, two more passes at the window's size.

#if defined(_WIN32)
#define FILTERS_SUPPORTED 0 // the shaders exist in MSL and SPIR-V only
#elif defined(__APPLE__)
#include "shaders/mmpx_vert_msl.h"
#include "shaders/mmpx_frag_msl.h"
#include "shaders/xbr_frag_msl.h"
#define mmpx_vert mmpx_vert_msl
#define mmpx_vert_len mmpx_vert_msl_len
#define mmpx_frag mmpx_frag_msl
#define mmpx_frag_len mmpx_frag_msl_len
#define xbr_frag xbr_frag_msl
#define xbr_frag_len xbr_frag_msl_len
#define crt_frag NULL // no MSL version yet: CRT is unavailable
#define crt_frag_len 0
#define modern_sharpen_frag NULL // nor Modern
#define modern_sharpen_frag_len 0
#define scalefx_pass0_frag NULL
#define scalefx_pass0_frag_len 0
#define scalefx_pass1_frag NULL
#define scalefx_pass1_frag_len 0
#define scalefx_pass2_frag NULL
#define scalefx_pass2_frag_len 0
#define scalefx_pass3_frag NULL
#define scalefx_pass3_frag_len 0
#define scalefx_pass4_frag NULL
#define scalefx_pass4_frag_len 0
#define modern_bloom_frag NULL
#define modern_bloom_frag_len 0
#define modern_frag NULL
#define modern_composite_frag NULL
#define modern_composite_frag_len 0
#define modern_crt_frag NULL
#define modern_crt_frag_len 0
#define modern_frag_len 0
#define FILTERS_SUPPORTED 1
#else
#include "shaders/mmpx_vert_spv.h"
#include "shaders/mmpx_frag_spv.h"
#include "shaders/xbr_frag_spv.h"
#include "shaders/crt_frag_spv.h"
#include "shaders/modern_sharpen_frag_spv.h"
#include "shaders/scalefx_pass0_frag_spv.h"
#include "shaders/scalefx_pass1_frag_spv.h"
#include "shaders/scalefx_pass2_frag_spv.h"
#include "shaders/scalefx_pass3_frag_spv.h"
#include "shaders/scalefx_pass4_frag_spv.h"
#include "shaders/modern_bloom_frag_spv.h"
#include "shaders/modern_frag_spv.h"
#include "shaders/modern_composite_frag_spv.h"
#include "shaders/modern_crt_frag_spv.h"
#define mmpx_vert mmpx_vert_spv
#define mmpx_vert_len mmpx_vert_spv_len
#define mmpx_frag mmpx_frag_spv
#define mmpx_frag_len mmpx_frag_spv_len
#define xbr_frag xbr_frag_spv
#define xbr_frag_len xbr_frag_spv_len
#define crt_frag crt_frag_spv
#define crt_frag_len crt_frag_spv_len
#define modern_sharpen_frag modern_sharpen_frag_spv
#define modern_sharpen_frag_len modern_sharpen_frag_spv_len
#define scalefx_pass0_frag scalefx_pass0_frag_spv
#define scalefx_pass0_frag_len scalefx_pass0_frag_spv_len
#define scalefx_pass1_frag scalefx_pass1_frag_spv
#define scalefx_pass1_frag_len scalefx_pass1_frag_spv_len
#define scalefx_pass2_frag scalefx_pass2_frag_spv
#define scalefx_pass2_frag_len scalefx_pass2_frag_spv_len
#define scalefx_pass3_frag scalefx_pass3_frag_spv
#define scalefx_pass3_frag_len scalefx_pass3_frag_spv_len
#define scalefx_pass4_frag scalefx_pass4_frag_spv
#define scalefx_pass4_frag_len scalefx_pass4_frag_spv_len
#define modern_bloom_frag modern_bloom_frag_spv
#define modern_bloom_frag_len modern_bloom_frag_spv_len
#define modern_frag modern_frag_spv
#define modern_composite_frag modern_composite_frag_spv
#define modern_composite_frag_len modern_composite_frag_spv_len
#define modern_crt_frag modern_crt_frag_spv
#define modern_crt_frag_len modern_crt_frag_spv_len
#define modern_frag_len modern_frag_spv_len
#define FILTERS_SUPPORTED 1
#endif

#define MMPX_MAX_PASSES 3
#define FILTER_MAX_PASSES 4 // Modern + CRT
#define FILTER_FORMAT SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM
#define FLOAT_FORMAT SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT

static PsyzFilterMode filter_mode = PSYZ_FILTER_NONE;
// the internal resolution asked for while a filter keeps it at 1x
static unsigned filter_saved_res = 1;
// by PsyzFilterMode
static const char* const filter_names[] = {
    "none", "MMPX", "xBR", "CRT", "Modern", "Modern + CRT"};
static SDL_GPUGraphicsPipeline* pipe_filter[LEN(filter_names)];
static bool filter_failed[LEN(filter_names)];
// [0]: a copy of the 1x input, [i]: the result of pass i
static SDL_GPUTexture* filter_tex[FILTER_MAX_PASSES + 1];
static Posi filter_tex_size[FILTER_MAX_PASSES + 1];
static Posi filter_pass_size[FILTER_MAX_PASSES + 1]; // of the last Filter_Run
static int filter_last_passes = 0;
// Modern's passes before its last one (pipe_filter[PSYZ_FILTER_MODERN])
enum {
    MP_SHARPEN,
    MP_SFX0, // ScaleFX: colour metric
    MP_SFX1, // corner strengths
    MP_SFX2, // junctions
    MP_SFX3, // edge levels
    MP_SFX4, // the 3x subpixels, into filter_tex[1]
    MP_BLOOM,
    MP_COUNT
};
static SDL_GPUGraphicsPipeline* pipe_modern[MP_COUNT];
// the 1x textures they draw, and their formats
enum {
    MT_SHARP,
    MT_SFX0,
    MT_SFX1,
    MT_SFX2,
    MT_SFX3,
    MT_BLOOM_H,
    MT_BLOOM_V,
    MT_COUNT
};
static const SDL_GPUTextureFormat modern_tex_format[MT_COUNT] = {
    FILTER_FORMAT, FLOAT_FORMAT, FLOAT_FORMAT, FILTER_FORMAT,
    FILTER_FORMAT, FLOAT_FORMAT, FLOAT_FORMAT,
};
static SDL_GPUTexture* modern_tex[MT_COUNT];
static Posi modern_tex_size[MT_COUNT];
static SDL_GPUSampler* linear_sampler; // for Modern's last pass
// Modern + CRT's composite pass, before its last one
static SDL_GPUGraphicsPipeline* pipe_composite;

// a full-screen pass drawing `format` with `frag`, which samples n_samplers
// textures; NULL when `frag` is (no version for this backend) or on errors
static SDL_GPUGraphicsPipeline* Filter_CreatePipeline(
    const unsigned char* frag, unsigned int frag_len, Uint32 n_samplers,
    SDL_GPUTextureFormat format) {
    SDL_GPUGraphicsPipeline* pipe = NULL;
    if (!frag) {
        return NULL;
    }
    SDL_GPUShader* vs = CreateShader(
        mmpx_vert, mmpx_vert_len, SDL_GPU_SHADERSTAGE_VERTEX, 0, 0);
    SDL_GPUShader* fs = CreateShader(
        frag, frag_len, SDL_GPU_SHADERSTAGE_FRAGMENT, n_samplers, 1);
    if (vs && fs) {
        const SDL_GPUColorTargetDescription target = {.format = format};
        const SDL_GPUGraphicsPipelineCreateInfo info = {
            .vertex_shader = vs,
            .fragment_shader = fs,
            .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
            .target_info =
                {
                    .color_target_descriptions = &target,
                    .num_color_targets = 1,
                },
        };
        pipe = SDL_CreateGPUGraphicsPipeline(device, &info);
        if (!pipe) {
            ERRORF("SDL_CreateGPUGraphicsPipeline: %s", SDL_GetError());
        }
    }
    if (vs) {
        SDL_ReleaseGPUShader(device, vs);
    }
    if (fs) {
        SDL_ReleaseGPUShader(device, fs);
    }
    return pipe;
}

static bool Filter_Init(PsyzFilterMode mode) {
#if FILTERS_SUPPORTED
    if (pipe_filter[mode] || filter_failed[mode]) {
        return pipe_filter[mode] != NULL;
    }
    if (mode == PSYZ_FILTER_MODERN) {
        // the passes before the last one (pipe_filter[mode]): their shader,
        // how many textures it samples and the format it draws
        const struct {
            const unsigned char* frag;
            unsigned int len;
            Uint32 n_samplers;
            SDL_GPUTextureFormat format;
        } passes[MP_COUNT] = {
            [MP_SHARPEN] = {modern_sharpen_frag, modern_sharpen_frag_len, 1,
                            FILTER_FORMAT},
            [MP_SFX0] = {scalefx_pass0_frag, scalefx_pass0_frag_len, 1,
                         FLOAT_FORMAT},
            [MP_SFX1] = {scalefx_pass1_frag, scalefx_pass1_frag_len, 1,
                         FLOAT_FORMAT},
            [MP_SFX2] = {scalefx_pass2_frag, scalefx_pass2_frag_len, 2,
                         FILTER_FORMAT},
            [MP_SFX3] = {scalefx_pass3_frag, scalefx_pass3_frag_len, 1,
                         FILTER_FORMAT},
            [MP_SFX4] = {scalefx_pass4_frag, scalefx_pass4_frag_len, 2,
                         FILTER_FORMAT},
            [MP_BLOOM] = {modern_bloom_frag, modern_bloom_frag_len, 1,
                          FLOAT_FORMAT},
        };
        bool ok = true;
        for (int i = 0; i < MP_COUNT && ok; i++) {
            if (!pipe_modern[i]) {
                pipe_modern[i] = Filter_CreatePipeline(
                    passes[i].frag, passes[i].len, passes[i].n_samplers,
                    passes[i].format);
            }
            ok = pipe_modern[i] != NULL;
        }
        if (ok && !linear_sampler) {
            const SDL_GPUSamplerCreateInfo info = {
                .min_filter = SDL_GPU_FILTER_LINEAR,
                .mag_filter = SDL_GPU_FILTER_LINEAR,
                .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
                .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
                .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
            };
            linear_sampler = SDL_CreateGPUSampler(device, &info);
            if (!linear_sampler) {
                ERRORF("SDL_CreateGPUSampler: %s", SDL_GetError());
            }
        }
        if (ok && linear_sampler) {
            pipe_filter[mode] = Filter_CreatePipeline(
                modern_frag, modern_frag_len, 2, FILTER_FORMAT);
        }
    } else if (mode == PSYZ_FILTER_MODERN_CRT) {
        // Modern, then the composite pass and the last one (pipe_filter[mode])
        if (Filter_Init(PSYZ_FILTER_MODERN) && !pipe_composite) {
            pipe_composite = Filter_CreatePipeline(
                modern_composite_frag, modern_composite_frag_len, 1,
                FILTER_FORMAT);
        }
        if (pipe_composite) {
            pipe_filter[mode] = Filter_CreatePipeline(
                modern_crt_frag, modern_crt_frag_len, 1, FILTER_FORMAT);
        }
    } else {
        const unsigned char* frag =
            mode == PSYZ_FILTER_XBR   ? xbr_frag
            : mode == PSYZ_FILTER_CRT ? crt_frag
                                      : mmpx_frag;
        const unsigned int frag_len =
            mode == PSYZ_FILTER_XBR   ? xbr_frag_len
            : mode == PSYZ_FILTER_CRT ? crt_frag_len
                                      : mmpx_frag_len;
        pipe_filter[mode] =
            Filter_CreatePipeline(frag, frag_len, 1, FILTER_FORMAT);
    }
    if (!pipe_filter[mode]) {
        WARNF("%s unavailable, presenting without it", filter_names[mode]);
        filter_failed[mode] = true;
    }
    return pipe_filter[mode] != NULL;
#else
    (void)mode;
    return false;
#endif
}

static void Filter_Release(void) {
    for (int i = 0; i < LEN(pipe_filter); i++) {
        if (pipe_filter[i]) {
            SDL_ReleaseGPUGraphicsPipeline(device, pipe_filter[i]);
            pipe_filter[i] = NULL;
        }
        filter_failed[i] = false;
    }
    for (int i = 0; i < MP_COUNT; i++) {
        if (pipe_modern[i]) {
            SDL_ReleaseGPUGraphicsPipeline(device, pipe_modern[i]);
            pipe_modern[i] = NULL;
        }
    }
    if (pipe_composite) {
        SDL_ReleaseGPUGraphicsPipeline(device, pipe_composite);
        pipe_composite = NULL;
    }
    if (linear_sampler) {
        SDL_ReleaseGPUSampler(device, linear_sampler);
        linear_sampler = NULL;
    }
    for (int i = 0; i < LEN(filter_tex); i++) {
        if (filter_tex[i]) {
            SDL_ReleaseGPUTexture(device, filter_tex[i]);
            filter_tex[i] = NULL;
        }
    }
    for (int i = 0; i < MT_COUNT; i++) {
        if (modern_tex[i]) {
            SDL_ReleaseGPUTexture(device, modern_tex[i]);
            modern_tex[i] = NULL;
        }
    }
    filter_last_passes = 0;
}

// make *tex at least w x h; it only ever grows, so shaders clamp their reads
// to the part they were told about
static bool EnsureTexture(SDL_GPUTexture** tex, Posi* size, int w, int h,
                          SDL_GPUTextureFormat format) {
    if (*tex && size->x >= w && size->y >= h) {
        return true;
    }
    if (*tex) {
        SDL_ReleaseGPUTexture(device, *tex);
    }
    const SDL_GPUTextureCreateInfo info = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = format,
        .usage =
            SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = (Uint32)w,
        .height = (Uint32)h,
        .layer_count_or_depth = 1,
        .num_levels = 1,
    };
    *tex = SDL_CreateGPUTexture(device, &info);
    if (!*tex) {
        ERRORF("SDL_CreateGPUTexture: %s", SDL_GetError());
        return false;
    }
    *size = (Posi){w, h};
    return true;
}

static bool Filter_EnsureTexture(int i, int w, int h) {
    return EnsureTexture(
        &filter_tex[i], &filter_tex_size[i], w, h, FILTER_FORMAT);
}

// doublings of a `src` sized image whose height fits in `dst`; the width of
// the wide modes (512 or 640 for 240 lines) may then come out up to twice as
// wide as `dst`, which the blit narrows
static int Mmpx_Passes(Posi src, SDL_Rect dst) {
    int n = 0;
    while (n < MMPX_MAX_PASSES && (src.y << (n + 1)) <= dst.h &&
           (src.x << (n + 1)) <= dst.w * 2) {
        n++;
    }
    return n;
}

// draw the top left w x h of `target` with `pipe`, which samples `inputs`
// through `sampler`
static void Filter_Draw(
    SDL_GPUCommandBuffer* cmd, SDL_GPUGraphicsPipeline* pipe,
    SDL_GPUTexture* target, int w, int h, SDL_GPUTexture* const* inputs,
    int n_inputs, SDL_GPUSampler* sampler, const void* params,
    Uint32 params_size) {
    const SDL_GPUColorTargetInfo target_info = {
        .texture = target,
        .load_op = SDL_GPU_LOADOP_DONT_CARE,
        .store_op = SDL_GPU_STOREOP_STORE,
    };
    SDL_GPURenderPass* pass =
        SDL_BeginGPURenderPass(cmd, &target_info, 1, NULL);
    const SDL_GPUViewport viewport = {
        .w = (float)w, .h = (float)h, .max_depth = 1.0f};
    SDL_SetGPUViewport(pass, &viewport);
    const SDL_Rect scissor = {0, 0, w, h};
    SDL_SetGPUScissor(pass, &scissor);
    SDL_BindGPUGraphicsPipeline(pass, pipe);
    SDL_GPUTextureSamplerBinding bindings[2];
    for (int i = 0; i < n_inputs && i < LEN(bindings); i++) {
        bindings[i] = (SDL_GPUTextureSamplerBinding){
            .texture = inputs[i], .sampler = sampler};
    }
    SDL_BindGPUFragmentSamplers(pass, 0, bindings, (Uint32)n_inputs);
    SDL_PushGPUFragmentUniformData(cmd, 0, params, params_size);
    SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(pass);
}

// draw filter_tex[i] (w x h) from filter_tex[i - 1] with `pipe`
static void Filter_Pass(SDL_GPUCommandBuffer* cmd,
                        SDL_GPUGraphicsPipeline* pipe, int i, int w, int h,
                        const void* params, Uint32 params_size) {
    Filter_Draw(cmd, pipe, filter_tex[i], w, h, &filter_tex[i - 1], 1,
                vram_sampler, params, params_size);
    filter_pass_size[i] = (Posi){w, h};
}

// draw modern_tex[target] (1x, `src` sized) with Modern's pass `pass`
static void Modern_Draw(SDL_GPUCommandBuffer* cmd, int pass, int target,
                        Posi src, SDL_GPUTexture* const* inputs, int n_inputs,
                        const void* params, Uint32 params_size) {
    Filter_Draw(cmd, pipe_modern[pass], modern_tex[target], src.x, src.y,
                inputs, n_inputs, vram_sampler, params, params_size);
}

// Modern from the 1x picture in filter_tex[0] (`src` sized): sharpened,
// smoothed to 3x by ScaleFX into filter_tex[1], its bloom, then the last
// pass into filter_tex[2]
static bool Modern_Run(SDL_GPUCommandBuffer* cmd, Posi src, SDL_Rect dst) {
    const Posi up = {src.x * 3, src.y * 3};
    if (!Filter_EnsureTexture(1, up.x, up.y) ||
        !Filter_EnsureTexture(2, dst.w, dst.h)) {
        return false;
    }
    for (int i = 0; i < MT_COUNT; i++) {
        if (!EnsureTexture(&modern_tex[i], &modern_tex_size[i], src.x, src.y,
                           modern_tex_format[i])) {
            return false;
        }
    }
    // Params in modern_sharpen.frag.glsl and scalefx_pass*.frag.glsl: the
    // picture's size
    const Sint32 size[4] = {src.x, src.y};
    Modern_Draw(
        cmd, MP_SHARPEN, MT_SHARP, src, &filter_tex[0], 1, size, sizeof(size));
    // ScaleFX finds the edges in the original pixels; its 3x subpixels then
    // take the sharpened colours
    Modern_Draw(
        cmd, MP_SFX0, MT_SFX0, src, &filter_tex[0], 1, size, sizeof(size));
    Modern_Draw(cmd, MP_SFX1, MT_SFX1, src, &modern_tex[MT_SFX0], 1, size,
                sizeof(size));
    SDL_GPUTexture* const sfx2_inputs[2] = {
        modern_tex[MT_SFX1], modern_tex[MT_SFX0]};
    Modern_Draw(cmd, MP_SFX2, MT_SFX2, src, sfx2_inputs, 2, size, sizeof(size));
    Modern_Draw(cmd, MP_SFX3, MT_SFX3, src, &modern_tex[MT_SFX2], 1, size,
                sizeof(size));
    SDL_GPUTexture* const sfx4_inputs[2] = {
        modern_tex[MT_SFX3], modern_tex[MT_SHARP]};
    Filter_Draw(cmd, pipe_modern[MP_SFX4], filter_tex[1], up.x, up.y,
                sfx4_inputs, 2, vram_sampler, size, sizeof(size));
    filter_pass_size[1] = up;
    // Params in modern_bloom.frag.glsl
    struct {
        Sint32 size[2], dir[2], bright_pass, pad[3];
    } bloom = {{src.x, src.y}, {1, 0}, 1};
    Modern_Draw(cmd, MP_BLOOM, MT_BLOOM_H, src, &filter_tex[0], 1, &bloom,
                sizeof(bloom));
    bloom.dir[0] = 0;
    bloom.dir[1] = 1;
    bloom.bright_pass = 0;
    Modern_Draw(cmd, MP_BLOOM, MT_BLOOM_V, src, &modern_tex[MT_BLOOM_H], 1,
                &bloom, sizeof(bloom));
    // Params in modern.frag.glsl: the sizes of the output, of ScaleFX's
    // picture and its texture, and of the bloom and its texture
    const float params[12] = {
        (float)dst.w,
        (float)dst.h,
        (float)up.x,
        (float)up.y,
        (float)filter_tex_size[1].x,
        (float)filter_tex_size[1].y,
        (float)src.x,
        (float)src.y,
        (float)modern_tex_size[MT_BLOOM_V].x,
        (float)modern_tex_size[MT_BLOOM_V].y,
    };
    SDL_GPUTexture* const inputs[2] = {filter_tex[1], modern_tex[MT_BLOOM_V]};
    Filter_Draw(cmd, pipe_filter[PSYZ_FILTER_MODERN], filter_tex[2], dst.w,
                dst.h, inputs, 2, linear_sampler, params, sizeof(params));
    filter_pass_size[2] = (Posi){dst.w, dst.h};
    return true;
}

// Modern + CRT once Modern has drawn filter_tex[2]: composite video into
// filter_tex[3], then the CRT into filter_tex[4]
static bool ModernCrt_Run(SDL_GPUCommandBuffer* cmd, Posi src, SDL_Rect dst) {
    if (!Filter_EnsureTexture(3, dst.w, dst.h) ||
        !Filter_EnsureTexture(4, dst.w, dst.h)) {
        return false;
    }
    // Params in modern_composite.frag.glsl and modern_crt.frag.glsl: the
    // output's size, the 1x picture's, and the input's texture size
    float params[8] = {
        (float)dst.w,
        (float)dst.h,
        (float)src.x,
        (float)src.y,
        (float)filter_tex_size[2].x,
        (float)filter_tex_size[2].y,
    };
    Filter_Draw(cmd, pipe_composite, filter_tex[3], dst.w, dst.h,
                &filter_tex[2], 1, linear_sampler, params, sizeof(params));
    filter_pass_size[3] = (Posi){dst.w, dst.h};
    params[4] = (float)filter_tex_size[3].x;
    params[5] = (float)filter_tex_size[3].y;
    Filter_Draw(
        cmd, pipe_filter[PSYZ_FILTER_MODERN_CRT], filter_tex[4], dst.w, dst.h,
        &filter_tex[3], 1, linear_sampler, params, sizeof(params));
    filter_pass_size[4] = (Posi){dst.w, dst.h};
    return true;
}

// magnify `rect` of `src` (at 1x) for a picture drawn at `dst` in the window;
// returns the texture holding the result at its origin, sized `*out`, and
// how the blit should scale it the rest of the way
static SDL_GPUTexture* Filter_Run(SDL_GPUCommandBuffer* cmd, SDL_GPUTexture* src,
                                  SDL_Rect rect, SDL_Rect dst, Posi* out,
                                  SDL_GPUFilter* blit_filter) {
    if (!Filter_Init(filter_mode)) {
        return NULL;
    }
    // xBR and CRT draw straight to the size the picture has in the window
    const bool one_pass =
        filter_mode == PSYZ_FILTER_XBR || filter_mode == PSYZ_FILTER_CRT;
    const int passes =
        filter_mode == PSYZ_FILTER_MODERN_CRT ? 4
        : filter_mode == PSYZ_FILTER_MODERN   ? 2
        : one_pass ? 1
                   : Mmpx_Passes((Posi){rect.w, rect.h}, dst);
    if (!passes || !Filter_EnsureTexture(0, rect.w, rect.h)) {
        return NULL;
    }
    // every pass reads its input at the origin of a texture of its own
    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
    const SDL_GPUTextureLocation in_src = {
        .texture = src, .x = (Uint32)rect.x, .y = (Uint32)rect.y};
    const SDL_GPUTextureLocation in_dst = {.texture = filter_tex[0]};
    SDL_CopyGPUTextureToTexture(
        copy, &in_src, &in_dst, (Uint32)rect.w, (Uint32)rect.h, 1, false);
    SDL_EndGPUCopyPass(copy);
    filter_pass_size[0] = (Posi){rect.w, rect.h};

    if (filter_mode == PSYZ_FILTER_MODERN ||
        filter_mode == PSYZ_FILTER_MODERN_CRT) {
        const Posi size = {rect.w, rect.h};
        if (!Modern_Run(cmd, size, dst) ||
            (filter_mode == PSYZ_FILTER_MODERN_CRT &&
             !ModernCrt_Run(cmd, size, dst))) {
            return NULL;
        }
        *blit_filter = SDL_GPU_FILTER_NEAREST; // already the window's size
    } else if (one_pass) {
        if (!Filter_EnsureTexture(1, dst.w, dst.h)) {
            return NULL;
        }
        // Params in xbr.frag.metal and .glsl, crt.frag.glsl
        const struct {
            Sint32 origin[2], size[2];
            float output[2], pad[2];
        } params = {{0, 0}, {rect.w, rect.h}, {(float)dst.w, (float)dst.h}};
        Filter_Pass(cmd, pipe_filter[filter_mode], 1, dst.w, dst.h, &params,
                    sizeof(params));
        *blit_filter = SDL_GPU_FILTER_NEAREST; // already the window's size
    } else {
        for (int i = 1; i <= passes; i++) {
            const int w = rect.w << i, h = rect.h << i;
            if (!Filter_EnsureTexture(i, w, h)) {
                return NULL;
            }
            // Params in mmpx.frag.metal and .glsl: the source rectangle
            const Sint32 params[4] = {0, 0, w / 2, h / 2};
            Filter_Pass(cmd, pipe_filter[PSYZ_FILTER_MMPX], i, w, h, params,
                        sizeof(params));
        }
        // the rest of the way (the 4:3 stretch) smoothly, so no column of
        // MMPX's diagonals gets doubled
        *blit_filter = SDL_GPU_FILTER_LINEAR;
    }
    filter_last_passes = passes;
    *out = filter_pass_size[passes];
    return filter_tex[passes];
}

static unsigned char* AllocRgbFromTexture(SDL_GPUTexture* tex, SDL_Rect r);

unsigned char* Psyz_VideoAllocCapturedFilterPass(int pass, int* w, int* h) {
    *w = *h = 0;
    if (pass < 0 || pass > filter_last_passes || !filter_tex[pass]) {
        return NULL;
    }
    const SDL_Rect r = {
        0, 0, filter_pass_size[pass].x, filter_pass_size[pass].y};
    unsigned char* rgb = AllocRgbFromTexture(filter_tex[pass], r);
    if (rgb) {
        *w = r.w;
        *h = r.h;
    }
    return rgb;
}

int Psyz_VideoSetFilter(PsyzFilterMode mode) {
    if ((int)mode < 0 || (int)mode >= LEN(filter_names)) {
        return -1;
    }
    if (mode != PSYZ_FILTER_NONE && !FILTERS_SUPPORTED) {
        return -1;
    }
    if (mode == filter_mode) {
        return 0;
    }
    if (filter_mode == PSYZ_FILTER_NONE) {
        filter_saved_res = set_internal_res;
        set_internal_res = 1;
    } else if (mode == PSYZ_FILTER_NONE) {
        set_internal_res = filter_saved_res;
    }
    filter_mode = mode;
    if (sdl3_window && is_platform_init_successful) {
        ApplyPendingInternalRes();
    }
    INFOF("output filter: %s, internal resolution %ux", filter_names[mode],
          set_internal_res);
    return 0;
}

PsyzFilterMode Psyz_VideoGetFilter(void) { return filter_mode; }
