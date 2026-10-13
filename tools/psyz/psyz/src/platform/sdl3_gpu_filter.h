// Pixel-art output filters (Psyz_VideoSetFilter) for the SDL_GPU backend,
// included by sdl3_gpu.c. The game renders at 1x; before the blit to the
// swapchain the display area is copied to a texture of its own and
// magnified:
// - MMPX (shaders/mmpx.frag.metal) in 2x passes, as many as fit the window's
//   height (up to 8x); the blit stretches the rest of the way linearly.
// - xBR-lv2 (shaders/xbr.frag.metal) in one pass straight to the size the
//   picture has in the window, 4:3 stretch included; the blit copies it.

#if defined(__APPLE__)
#include "shaders/mmpx_vert_msl.h"
#include "shaders/mmpx_frag_msl.h"
#include "shaders/xbr_frag_msl.h"
#define FILTERS_SUPPORTED 1
#else
#define FILTERS_SUPPORTED 0 // the shaders exist in MSL only
#endif

#define MMPX_MAX_PASSES 3

static PsyzFilterMode filter_mode = PSYZ_FILTER_NONE;
// the internal resolution asked for while a filter keeps it at 1x
static unsigned filter_saved_res = 1;
static SDL_GPUGraphicsPipeline* pipe_filter[3]; // by PsyzFilterMode
static bool filter_failed[3];
// [0]: a copy of the 1x input, [i]: the result of pass i
static SDL_GPUTexture* filter_tex[MMPX_MAX_PASSES + 1];
static Posi filter_tex_size[MMPX_MAX_PASSES + 1];
static Posi filter_pass_size[MMPX_MAX_PASSES + 1]; // of the last Filter_Run
static int filter_last_passes = 0;

static bool Filter_Init(PsyzFilterMode mode) {
#if FILTERS_SUPPORTED
    if (pipe_filter[mode] || filter_failed[mode]) {
        return pipe_filter[mode] != NULL;
    }
    const unsigned char* frag = mode == PSYZ_FILTER_XBR ? xbr_frag_msl
                                                         : mmpx_frag_msl;
    const unsigned int frag_len = mode == PSYZ_FILTER_XBR ? xbr_frag_msl_len
                                                           : mmpx_frag_msl_len;
    SDL_GPUShader* vs = CreateShader(
        mmpx_vert_msl, mmpx_vert_msl_len, SDL_GPU_SHADERSTAGE_VERTEX, 0, 0);
    SDL_GPUShader* fs =
        CreateShader(frag, frag_len, SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 1);
    if (vs && fs) {
        const SDL_GPUColorTargetDescription target = {
            .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        };
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
        pipe_filter[mode] = SDL_CreateGPUGraphicsPipeline(device, &info);
        if (!pipe_filter[mode]) {
            ERRORF("SDL_CreateGPUGraphicsPipeline: %s", SDL_GetError());
        }
    }
    if (vs) {
        SDL_ReleaseGPUShader(device, vs);
    }
    if (fs) {
        SDL_ReleaseGPUShader(device, fs);
    }
    if (!pipe_filter[mode]) {
        WARNF("%s unavailable, presenting without it",
              mode == PSYZ_FILTER_XBR ? "xBR" : "MMPX");
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
    for (int i = 0; i <= MMPX_MAX_PASSES; i++) {
        if (filter_tex[i]) {
            SDL_ReleaseGPUTexture(device, filter_tex[i]);
            filter_tex[i] = NULL;
        }
    }
    filter_last_passes = 0;
}

static bool Filter_EnsureTexture(int i, int w, int h) {
    if (filter_tex[i] && filter_tex_size[i].x >= w &&
        filter_tex_size[i].y >= h) {
        return true;
    }
    if (filter_tex[i]) {
        SDL_ReleaseGPUTexture(device, filter_tex[i]);
    }
    const SDL_GPUTextureCreateInfo info = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage =
            SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = (Uint32)w,
        .height = (Uint32)h,
        .layer_count_or_depth = 1,
        .num_levels = 1,
    };
    filter_tex[i] = SDL_CreateGPUTexture(device, &info);
    if (!filter_tex[i]) {
        ERRORF("SDL_CreateGPUTexture: %s", SDL_GetError());
        return false;
    }
    filter_tex_size[i] = (Posi){w, h};
    return true;
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

// draw filter_tex[i] (w x h) from filter_tex[i - 1] with `pipe`
static void Filter_Pass(SDL_GPUCommandBuffer* cmd,
                        SDL_GPUGraphicsPipeline* pipe, int i, int w, int h,
                        const void* params, Uint32 params_size) {
    const SDL_GPUColorTargetInfo target = {
        .texture = filter_tex[i],
        .load_op = SDL_GPU_LOADOP_DONT_CARE,
        .store_op = SDL_GPU_STOREOP_STORE,
    };
    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &target, 1, NULL);
    const SDL_GPUViewport viewport = {
        .w = (float)w, .h = (float)h, .max_depth = 1.0f};
    SDL_SetGPUViewport(pass, &viewport);
    const SDL_Rect scissor = {0, 0, w, h};
    SDL_SetGPUScissor(pass, &scissor);
    SDL_BindGPUGraphicsPipeline(pass, pipe);
    const SDL_GPUTextureSamplerBinding binding = {
        .texture = filter_tex[i - 1], .sampler = vram_sampler};
    SDL_BindGPUFragmentSamplers(pass, 0, &binding, 1);
    SDL_PushGPUFragmentUniformData(cmd, 0, params, params_size);
    SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(pass);
    filter_pass_size[i] = (Posi){w, h};
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
    const int passes = filter_mode == PSYZ_FILTER_XBR
                           ? 1
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

    if (filter_mode == PSYZ_FILTER_XBR) {
        if (!Filter_EnsureTexture(1, dst.w, dst.h)) {
            return NULL;
        }
        // Params in xbr.frag.metal
        const struct {
            Sint32 origin[2], size[2];
            float output[2], pad[2];
        } params = {{0, 0}, {rect.w, rect.h}, {(float)dst.w, (float)dst.h}};
        Filter_Pass(cmd, pipe_filter[PSYZ_FILTER_XBR], 1, dst.w, dst.h,
                    &params, sizeof(params));
        *blit_filter = SDL_GPU_FILTER_NEAREST; // already the window's size
    } else {
        for (int i = 1; i <= passes; i++) {
            const int w = rect.w << i, h = rect.h << i;
            if (!Filter_EnsureTexture(i, w, h)) {
                return NULL;
            }
            // Params in mmpx.frag.metal: the source rectangle
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
    if (mode != PSYZ_FILTER_NONE && mode != PSYZ_FILTER_MMPX &&
        mode != PSYZ_FILTER_XBR) {
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
    static const char* const names[] = {"none", "MMPX", "xBR"};
    INFOF("output filter: %s, internal resolution %ux", names[mode],
          set_internal_res);
    return 0;
}

PsyzFilterMode Psyz_VideoGetFilter(void) { return filter_mode; }
