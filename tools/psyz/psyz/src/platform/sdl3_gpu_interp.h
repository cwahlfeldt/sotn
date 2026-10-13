// Frame interpolation for the SDL_GPU backend, included by sdl3_gpu.c.
//
// With PSYZ_INTERP_AUTO on a display refreshing at k times the game rate,
// ConfigureVSync syncs to the display and every Psyz_VideoVSync(0) presents k
// refreshes (PlatformBackend_PresentFrames). The extra refreshes show the
// newest game frame redrawn from a log of what it drew, with each primitive
// the game tagged (Psyz_GpuTagPrim) moved part of the way back to where its
// group was in the frame before.
//
// The log covers what was drawn between two VSyncs: every Draw_FlushBuffer
// batch (vertices, indices, draw offset, scissor), every clear, and for each
// primitive its index range and tag. Once the frame's drawing is over, before
// the game uploads the next frame's sprites, VRAM is copied, and the redraw
// samples its textures from that copy.
//
// Which display area the redraw is for: with double buffering (the usual
// case) the game draws into the area that was displayed the VSync before
// while the other one is on screen, so a VSync presents the displayed frame
// and then the in-between frames towards the one just drawn. A game drawing
// into the displayed area gets the in-between frames first.

#define INTERP_MAX_VERTICES 0x20000
#define INTERP_MAX_INDICES (INTERP_MAX_VERTICES / 4 * 6)
#define INTERP_MAX_PRIMS 0x8000
#define INTERP_MAX_OPS 0x800
#define INTERP_MAX_WRITES 0x200
#define INTERP_MAX_STEP 48 // pixels a group may move in one frame
#define INTERP_POS_SLOTS 0x2000 // power of two, more than the keys per frame

typedef struct {
    int first_index; // in InterpLog.indices
    int n_indices;
    PsyzPrimTag tag;
    short dx, dy; // previous position minus this one, 0 when not moving
} InterpPrim;

enum { INTERP_OP_DRAW, INTERP_OP_CLEAR };
typedef struct {
    int kind;
    SDL_Rect rect;    // DRAW: the scissor; CLEAR: the cleared area
    int base_vertex;  // DRAW: the batch's first vertex; CLEAR: the quad's
    int first_prim;   // DRAW
    int n_prims;      // DRAW
    Posi offset;      // DRAW: draw offset
    float grid_scale; // DRAW: GetDrawGridXScale()
} InterpOp;

typedef struct {
    Vertex vertices[INTERP_MAX_VERTICES];
    u16 indices[INTERP_MAX_INDICES];
    InterpPrim prims[INTERP_MAX_PRIMS];
    InterpOp ops[INTERP_MAX_OPS];
    SDL_Rect writes[INTERP_MAX_WRITES]; // VRAM uploads and moves
    int n_vertices, n_indices, n_prims, n_ops, n_writes;
    int n_drawn_ops;  // ops up to the VRAM copy: the frame's drawing
    bool draws;       // DRAW ops recorded since the VRAM copy
    bool any_draw;    // DRAW ops recorded at all
    bool overflow;
} InterpLog;

typedef struct {
    unsigned int key[INTERP_POS_SLOTS];
    short x[INTERP_POS_SLOTS], y[INTERP_POS_SLOTS];
    unsigned int gen[INTERP_POS_SLOTS];
    unsigned int cur_gen;
} InterpPosTable;

static InterpLog* interp_rec = NULL;    // being recorded
static InterpLog* interp_replay = NULL; // the newest frame
static InterpPosTable* interp_pos_prev = NULL;
static InterpPosTable* interp_pos_cur = NULL;

static SDL_GPUTexture* interp_target = NULL;   // VRAM sized, at internal_res
static unsigned interp_target_res = 0;
static SDL_GPUTexture* interp_vram_copy = NULL; // native VRAM, for textures
static SDL_GPUBuffer* interp_vbuf = NULL;
static SDL_GPUBuffer* interp_ibuf = NULL;
static SDL_GPUTransferBuffer* interp_transfer = NULL;

static struct {
    bool valid;    // interp_replay can be redrawn
    bool before;   // the frame is displayed already: in-between frames first
    bool uploaded; // interp_replay's vertices are in interp_vbuf
    Posi area;     // VRAM origin of the display area the frame is drawn in
    Posi shown;    // display_area when the plan was made
    unsigned res;  // internal_res of the VRAM copy
    int step;      // next refresh of the frame
    int steps;     // refreshes the frame lasts
} interp_plan;
static Posi interp_prev_display = {-1, -1};
static bool interp_drawn = false; // the last VSync presented an in-between

// primitives of the batch being filled, indices relative to it
static InterpPrim batch_prims[MAX_INDEX_COUNT / 3];
static int n_batch_prims = 0;
static unsigned int prim_serial = 0; // bumped by Draw_PushPrim
static unsigned int batch_prim_serial = 0;

static bool Interp_Recording(void) {
    if (interp_subframes <= 1) {
        return false;
    }
    if (!interp_rec) {
        interp_rec = calloc(1, sizeof(InterpLog));
        interp_replay = calloc(1, sizeof(InterpLog));
        interp_pos_prev = calloc(1, sizeof(InterpPosTable));
        interp_pos_cur = calloc(1, sizeof(InterpPosTable));
        if (!interp_rec || !interp_replay || !interp_pos_prev ||
            !interp_pos_cur) {
            ERRORF("out of memory for frame interpolation");
            interp_subframes = 1;
            return false;
        }
        interp_pos_prev->cur_gen = interp_pos_cur->cur_gen = 1;
    }
    return true;
}

static void Interp_ResetLog(InterpLog* log) {
    log->n_vertices = log->n_indices = log->n_prims = log->n_ops = 0;
    log->n_writes = log->n_drawn_ops = 0;
    log->draws = log->any_draw = log->overflow = false;
}

static unsigned int Interp_PosHash(unsigned int key) {
    key ^= key >> 16;
    key *= 0x45D9F3Bu;
    key ^= key >> 16;
    return key & (INTERP_POS_SLOTS - 1);
}

static void Interp_PosClear(InterpPosTable* t) {
    if (++t->cur_gen == 0) {
        memset(t->gen, 0, sizeof(t->gen));
        t->cur_gen = 1;
    }
}

static void Interp_PosSet(InterpPosTable* t, PsyzPrimTag tag) {
    unsigned int i = Interp_PosHash(tag.key);
    for (int n = 0; n < INTERP_POS_SLOTS; n++) {
        if (t->gen[i] != t->cur_gen) {
            t->gen[i] = t->cur_gen;
            t->key[i] = tag.key;
            t->x[i] = tag.x;
            t->y[i] = tag.y;
            return;
        }
        if (t->key[i] == tag.key) {
            return; // the group's first primitive gave its position
        }
        i = (i + 1) & (INTERP_POS_SLOTS - 1);
    }
}

static bool Interp_PosGet(InterpPosTable* t, unsigned int key, int* x, int* y) {
    unsigned int i = Interp_PosHash(key);
    for (int n = 0; n < INTERP_POS_SLOTS; n++) {
        if (t->gen[i] != t->cur_gen) {
            return false;
        }
        if (t->key[i] == key) {
            *x = t->x[i];
            *y = t->y[i];
            return true;
        }
        i = (i + 1) & (INTERP_POS_SLOTS - 1);
    }
    return false;
}

static bool Interp_RectsIntersect(SDL_Rect a, SDL_Rect b) {
    return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h &&
           b.y < a.y + a.h;
}

// copy VRAM as the frame's drawing left it: the redraw takes textures from it
static void Interp_CopyVram(void) {
    InterpLog* log = interp_rec;
    SDL_GPUCommandBuffer* cmd = AcquireCmd();
    if (!cmd) {
        return;
    }
    if (!interp_vram_copy) {
        const SDL_GPUTextureCreateInfo info = {
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
            .width = VRAM_W,
            .height = VRAM_H,
            .layer_count_or_depth = 1,
            .num_levels = 1,
        };
        interp_vram_copy = SDL_CreateGPUTexture(device, &info);
        if (!interp_vram_copy) {
            ERRORF("SDL_CreateGPUTexture: %s", SDL_GetError());
            log->overflow = true;
            return;
        }
    }
    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
    const SDL_GPUTextureLocation src = {.texture = vram_render};
    const SDL_GPUTextureLocation dst = {.texture = interp_vram_copy};
    SDL_CopyGPUTextureToTexture(copy, &src, &dst, VRAM_W, VRAM_H, 1, true);
    SDL_EndGPUCopyPass(copy);
    interp_plan.res = internal_res;
    log->n_drawn_ops = log->n_ops;
    log->draws = false;
}

// VRAM is about to change outside of a draw: end the frame's drawing first
static void Interp_BeforeVramWrite(void) {
    if (Interp_Recording() && interp_rec->draws) {
        Interp_CopyVram();
    }
}

static void Interp_RecordWrite(int x, int y, int w, int h) {
    if (!Interp_Recording()) {
        return;
    }
    InterpLog* log = interp_rec;
    if (log->n_writes >= INTERP_MAX_WRITES) {
        log->overflow = true;
        return;
    }
    log->writes[log->n_writes++] = (SDL_Rect){x, y, w, h};
}

static void Interp_RecordClear(const Vertex quad[4], SDL_Rect rect) {
    if (!Interp_Recording()) {
        return;
    }
    InterpLog* log = interp_rec;
    if (log->n_ops >= INTERP_MAX_OPS ||
        log->n_vertices + 4 > INTERP_MAX_VERTICES) {
        log->overflow = true;
        return;
    }
    memcpy(&log->vertices[log->n_vertices], quad, 4 * sizeof(Vertex));
    log->ops[log->n_ops++] = (InterpOp){
        .kind = INTERP_OP_CLEAR,
        .rect = rect,
        .base_vertex = log->n_vertices,
    };
    log->n_vertices += 4;
}

static void PlatformBackend_OnEnqueue(int indices) {
    if (interp_subframes <= 1) {
        return;
    }
    if (n_batch_prims > 0 && batch_prim_serial == prim_serial) {
        InterpPrim* p = &batch_prims[n_batch_prims - 1];
        if (p->first_index + p->n_indices == n_indices) {
            p->n_indices += indices; // the next quad of a line
            return;
        }
    }
    if (n_batch_prims >= LEN(batch_prims)) {
        return;
    }
    batch_prims[n_batch_prims++] = (InterpPrim){
        .first_index = n_indices,
        .n_indices = indices,
        .tag = psyz_cur_prim_tag,
    };
    batch_prim_serial = prim_serial;
}

// Draw_FlushBuffer is drawing the batch in vertex_buf/index_buf
static void Interp_RecordBatch(void) {
    if (!Interp_Recording()) {
        return;
    }
    InterpLog* log = interp_rec;
    if (log->n_ops >= INTERP_MAX_OPS ||
        log->n_vertices + n_vertices > INTERP_MAX_VERTICES ||
        log->n_indices + n_indices > INTERP_MAX_INDICES ||
        log->n_prims + n_batch_prims > INTERP_MAX_PRIMS) {
        log->overflow = true;
        return;
    }
    InterpOp* op = &log->ops[log->n_ops++];
    *op = (InterpOp){
        .kind = INTERP_OP_DRAW,
        .rect = scissor_rect,
        .base_vertex = log->n_vertices,
        .first_prim = log->n_prims,
        .n_prims = n_batch_prims,
        .offset = draw_offset,
        .grid_scale = GetDrawGridXScale(),
    };
    for (int i = 0; i < n_batch_prims; i++) {
        InterpPrim* p = &log->prims[log->n_prims++];
        *p = batch_prims[i];
        p->first_index += log->n_indices;
    }
    memcpy(&log->vertices[log->n_vertices], vertex_buf,
           n_vertices * sizeof(Vertex));
    memcpy(&log->indices[log->n_indices], index_buf, n_indices * sizeof(u16));
    log->n_vertices += n_vertices;
    log->n_indices += n_indices;
    log->draws = true;
    log->any_draw = true;
}

// the VRAM pages a textured primitive samples from
static SDL_Rect Interp_TexturePage(const Vertex* v) {
    const int bpp = (v->t >> 7) & 3; // 0: 4-bit, 1: 8-bit, 2: 15-bit
    return (SDL_Rect){
        (v->t & 0xF) * 64, ((v->t >> 4) & 1) * 256, bpp == 0 ? 64 : bpp * 128,
        256};
}

// is any clear in the frame's drawing the draw area inside `area`?
static bool Interp_ClearsArea(const InterpLog* log, Posi area) {
    const SDL_Rect r = {area.x, area.y, display_size.x, display_size.y};
    for (int i = 0; i < log->n_drawn_ops; i++) {
        const InterpOp* op = &log->ops[i];
        if (op->kind != INTERP_OP_CLEAR) {
            continue;
        }
        if (op->rect.x >= r.x && op->rect.y >= r.y &&
            op->rect.x + op->rect.w <= r.x + r.w &&
            op->rect.y + op->rect.h <= r.y + r.h &&
            op->rect.w * op->rect.h * 2 >= r.w * r.h) {
            return true;
        }
    }
    return false;
}

static const char* interp_skip_reason = NULL;

// interp_replay holds a new frame: can it be redrawn, and how do its groups
// move?
static bool Interp_PlanFrame(void) {
    InterpLog* log = interp_replay;
    interp_skip_reason = NULL;
    if (log->overflow) {
        interp_skip_reason = "log overflow";
        return false;
    }
    if (interp_plan.res != internal_res) {
        interp_skip_reason = "internal resolution changed";
        return false;
    }
    if (interp_prev_display.x != display_area.x ||
        interp_prev_display.y != display_area.y) {
        if (Interp_ClearsArea(log, interp_prev_display)) {
            interp_plan.area = interp_prev_display;
            interp_plan.before = false;
        } else if (Interp_ClearsArea(log, display_area)) {
            interp_plan.area = display_area;
            interp_plan.before = true;
        } else {
            interp_skip_reason = "no clear of a display area";
            return false;
        }
    } else if (Interp_ClearsArea(log, display_area)) {
        interp_plan.area = display_area;
        interp_plan.before = true;
    } else {
        interp_skip_reason = "no clear of the display area";
        return false;
    }

    const SDL_Rect r = {
        interp_plan.area.x, interp_plan.area.y, display_size.x, display_size.y};
    for (int i = 0; i < log->n_writes; i++) {
        if (Interp_RectsIntersect(log->writes[i], r)) {
            interp_skip_reason = "VRAM transfer in the display area";
            return false;
        }
    }

    for (int i = 0; i < log->n_drawn_ops; i++) {
        const InterpOp* op = &log->ops[i];
        if (op->kind != INTERP_OP_DRAW || !Interp_RectsIntersect(op->rect, r)) {
            continue;
        }
        for (int j = 0; j < op->n_prims; j++) {
            InterpPrim* p = &log->prims[op->first_prim + j];
            const Vertex* v =
                &log->vertices[op->base_vertex + log->indices[p->first_index]];
            if (!(v->t & TPAGE_NOTEXTURE) &&
                Interp_RectsIntersect(Interp_TexturePage(v), r)) {
                interp_skip_reason = "a primitive samples the frame buffer";
                return false;
            }
            p->dx = p->dy = 0;
            int px, py;
            if (!p->tag.key ||
                !Interp_PosGet(interp_pos_prev, p->tag.key, &px, &py)) {
                continue;
            }
            const int dx = px - p->tag.x;
            const int dy = py - p->tag.y;
            if (abs(dx) > INTERP_MAX_STEP || abs(dy) > INTERP_MAX_STEP) {
                if (p->tag.key & PSYZ_PRIM_KEY_CAMERA) {
                    interp_skip_reason = "camera cut";
                    return false;
                }
                continue;
            }
            p->dx = (short)dx;
            p->dy = (short)dy;
        }
    }
    return true;
}

static bool Interp_EnsureResources(void) {
    const unsigned n = internal_res;
    if (interp_target && interp_target_res != n) {
        SDL_ReleaseGPUTexture(device, interp_target);
        interp_target = NULL;
    }
    if (!interp_target) {
        const SDL_GPUTextureCreateInfo info = {
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            .usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET |
                     SDL_GPU_TEXTUREUSAGE_SAMPLER,
            .width = VRAM_W * n,
            .height = VRAM_H * n,
            .layer_count_or_depth = 1,
            .num_levels = 1,
        };
        interp_target = SDL_CreateGPUTexture(device, &info);
        if (!interp_target) {
            ERRORF("SDL_CreateGPUTexture: %s", SDL_GetError());
            return false;
        }
        interp_target_res = n;
    }
    if (!interp_vbuf) {
        const SDL_GPUBufferCreateInfo vinfo = {
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = INTERP_MAX_VERTICES * sizeof(Vertex),
        };
        const SDL_GPUBufferCreateInfo iinfo = {
            .usage = SDL_GPU_BUFFERUSAGE_INDEX,
            .size = INTERP_MAX_INDICES * sizeof(u16),
        };
        const SDL_GPUTransferBufferCreateInfo tinfo = {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = INTERP_MAX_VERTICES * sizeof(Vertex) +
                    INTERP_MAX_INDICES * sizeof(u16),
        };
        interp_vbuf = SDL_CreateGPUBuffer(device, &vinfo);
        interp_ibuf = SDL_CreateGPUBuffer(device, &iinfo);
        interp_transfer = SDL_CreateGPUTransferBuffer(device, &tinfo);
        if (!interp_vbuf || !interp_ibuf || !interp_transfer) {
            ERRORF("SDL_CreateGPU*Buffer: %s", SDL_GetError());
            return false;
        }
    }
    return true;
}

static void Interp_Release(void) {
    if (interp_target) {
        SDL_ReleaseGPUTexture(device, interp_target);
        interp_target = NULL;
    }
    if (interp_vram_copy) {
        SDL_ReleaseGPUTexture(device, interp_vram_copy);
        interp_vram_copy = NULL;
    }
    if (interp_vbuf) {
        SDL_ReleaseGPUBuffer(device, interp_vbuf);
        interp_vbuf = NULL;
    }
    if (interp_ibuf) {
        SDL_ReleaseGPUBuffer(device, interp_ibuf);
        interp_ibuf = NULL;
    }
    if (interp_transfer) {
        SDL_ReleaseGPUTransferBuffer(device, interp_transfer);
        interp_transfer = NULL;
    }
    interp_plan.valid = false;
    interp_drawn = false;
}

// round an offset to the internal resolution's pixel grid
static float Interp_Snap(float v) {
    const float n = (float)internal_res;
    return SDL_roundf(v * n) / n;
}

// draw interp_replay into interp_target, with the groups moved to fraction
// `t` of the way from the previous frame (0) to this one (1)
static bool Interp_Draw(float t) {
    InterpLog* log = interp_replay;
    if (!Interp_EnsureResources()) {
        return false;
    }
    SDL_GPUCommandBuffer* cmd = AcquireCmd();
    if (!cmd) {
        return false;
    }
    const unsigned n = internal_res;
    const SDL_Rect area = {
        interp_plan.area.x, interp_plan.area.y, display_size.x, display_size.y};

    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
    if (!interp_plan.uploaded) {
        u8* map = SDL_MapGPUTransferBuffer(device, interp_transfer, true);
        if (!map) {
            SDL_EndGPUCopyPass(copy);
            return false;
        }
        const Uint32 vsize = (Uint32)(log->n_vertices * sizeof(Vertex));
        const Uint32 isize = (Uint32)(log->n_indices * sizeof(u16));
        const Uint32 ioffset = INTERP_MAX_VERTICES * sizeof(Vertex);
        memcpy(map, log->vertices, vsize);
        memcpy(map + ioffset, log->indices, isize);
        SDL_UnmapGPUTransferBuffer(device, interp_transfer);
        const SDL_GPUTransferBufferLocation vsrc = {
            .transfer_buffer = interp_transfer};
        const SDL_GPUBufferRegion vdst = {.buffer = interp_vbuf, .size = vsize};
        SDL_UploadToGPUBuffer(copy, &vsrc, &vdst, true);
        if (isize) {
            const SDL_GPUTransferBufferLocation isrc = {
                .transfer_buffer = interp_transfer, .offset = ioffset};
            const SDL_GPUBufferRegion idst = {
                .buffer = interp_ibuf, .size = isize};
            SDL_UploadToGPUBuffer(copy, &isrc, &idst, true);
        }
        interp_plan.uploaded = true;
    }
    // what the frame does not draw over (the letterbox) as it is now
    const SDL_GPUTextureLocation base_src = {
        .texture = GetRenderTarget(),
        .x = (Uint32)(area.x * n),
        .y = (Uint32)(area.y * n)};
    const SDL_GPUTextureLocation base_dst = {
        .texture = interp_target,
        .x = (Uint32)(area.x * n),
        .y = (Uint32)(area.y * n)};
    SDL_CopyGPUTextureToTexture(copy, &base_src, &base_dst,
                                (Uint32)(area.w * n), (Uint32)(area.h * n), 1,
                                false);
    SDL_EndGPUCopyPass(copy);

    const SDL_GPUColorTargetInfo target = {
        .texture = interp_target,
        .load_op = SDL_GPU_LOADOP_LOAD,
        .store_op = SDL_GPU_STOREOP_STORE,
    };
    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &target, 1, NULL);
    const SDL_GPUBufferBinding vb = {.buffer = interp_vbuf};
    SDL_BindGPUVertexBuffers(pass, 0, &vb, 1);
    const SDL_GPUBufferBinding ib = {.buffer = interp_ibuf};
    SDL_BindGPUIndexBuffer(pass, &ib, SDL_GPU_INDEXELEMENTSIZE_16BIT);
    const SDL_GPUTextureSamplerBinding sampler = {
        .texture = interp_vram_copy, .sampler = vram_sampler};
    const SDL_GPUViewport full_viewport = {
        .w = (float)(VRAM_W * n), .h = (float)(VRAM_H * n), .max_depth = 1.0f};
    const SDL_Rect full_scissor = {0, 0, VRAM_W * (int)n, VRAM_H * (int)n};
    const float back = 1.0f - t;
    SDL_GPUGraphicsPipeline* bound = NULL;

    for (int i = 0; i < log->n_drawn_ops; i++) {
        const InterpOp* op = &log->ops[i];
        if (!Interp_RectsIntersect(op->rect, area)) {
            continue;
        }
        if (op->kind == INTERP_OP_CLEAR) {
            SDL_SetGPUViewport(pass, &full_viewport);
            SDL_SetGPUScissor(pass, &full_scissor);
            if (bound != pipe_clear) {
                SDL_BindGPUGraphicsPipeline(pass, pipe_clear);
                bound = pipe_clear;
            }
            SDL_DrawGPUPrimitives(pass, 4, 1, (Uint32)op->base_vertex, 0);
            continue;
        }
        const SDL_GPUViewport grid_viewport = {
            .x = (float)op->offset.x * n * (1.0f - op->grid_scale),
            .w = (float)(VRAM_W * n) * op->grid_scale,
            .h = (float)(VRAM_H * n),
            .max_depth = 1.0f,
        };
        SDL_SetGPUViewport(pass, &grid_viewport);
        const SDL_Rect scissor = {
            op->rect.x * (int)n, op->rect.y * (int)n, op->rect.w * (int)n,
            op->rect.h * (int)n};
        SDL_SetGPUScissor(pass, &scissor);

        // draw runs of primitives that share an offset and a blend equation
        int j = 0;
        while (j < op->n_prims) {
            const InterpPrim* p = &log->prims[op->first_prim + j];
            const Vertex* v =
                &log->vertices[op->base_vertex + log->indices[p->first_index]];
            const bool subtract = is_subtract_abr(v);
            const short dx = p->dx, dy = p->dy;
            const int first = p->first_index;
            int count = p->n_indices;
            for (j++; j < op->n_prims; j++) {
                const InterpPrim* q = &log->prims[op->first_prim + j];
                const int qv = op->base_vertex + log->indices[q->first_index];
                if (q->dx != dx || q->dy != dy ||
                    is_subtract_abr(&log->vertices[qv]) != subtract ||
                    q->first_index != first + count) {
                    break;
                }
                count += q->n_indices;
            }
            SDL_GPUGraphicsPipeline* pipe =
                subtract ? pipe_tri_sub : pipe_tri_add;
            if (bound != pipe) {
                SDL_BindGPUGraphicsPipeline(pass, pipe);
                SDL_BindGPUFragmentSamplers(pass, 0, &sampler, 1);
                bound = pipe;
            }
            const float offset_ubo[4] = {
                (float)op->offset.x + Interp_Snap(dx * back),
                (float)op->offset.y + Interp_Snap(dy * back), 0, 0};
            SDL_PushGPUVertexUniformData(
                cmd, 0, offset_ubo, sizeof(offset_ubo));
            SDL_DrawGPUIndexedPrimitives(
                pass, (Uint32)count, 1, (Uint32)first, op->base_vertex, 0);
        }
    }
    SDL_EndGPURenderPass(pass);
    interp_drawn = true;
    return true;
}

// one PlatformBackend_PresentFrames refresh: the in-between frame at `t`, or
// false for the frame in the display area
static bool Interp_NextRefresh(float* t) {
    if (!interp_plan.valid) {
        return false;
    }
    const int step = interp_plan.step++;
    const int steps = interp_plan.steps;
    if (interp_plan.before) {
        if (step + 1 >= steps) {
            return false; // the frame itself
        }
        *t = (float)(step + 1) / (float)steps;
        return true;
    }
    if (step == 0) {
        return false; // the displayed frame
    }
    // past the frame's span (a slow frame) keep showing the newest frame
    *t = step >= steps ? 1.0f : (float)step / (float)steps;
    return true;
}

static void PresentTexture(SDL_GPUTexture* src, Posi area);

// PSYZ_INTERP_STATS=1 logs, every 600 game frames, how many in-between
// frames were drawn and how evenly the refreshes were presented
static unsigned long long interp_stats_frames = 0;
static unsigned long long interp_stats_drawn = 0;
static unsigned long long interp_stats_skipped = 0;
static const char* interp_stats_skip = "none";
static Uint64 interp_stats_last_ns = 0;
static double interp_stats_min_ms = 1e9, interp_stats_max_ms = 0;
static double interp_stats_sum_ms = 0;
static int interp_stats_presents = 0;
static int interp_stats_late = 0; // presents more than 1.5 refreshes apart

static void Interp_StatsPresent(void) {
    const Uint64 now = SDL_GetTicksNS();
    if (interp_stats_last_ns) {
        const double ms = (double)(now - interp_stats_last_ns) / 1e6;
        if (ms < interp_stats_min_ms) {
            interp_stats_min_ms = ms;
        }
        if (ms > interp_stats_max_ms) {
            interp_stats_max_ms = ms;
        }
        interp_stats_sum_ms += ms;
        interp_stats_presents++;
        if (ms > 12.5) {
            interp_stats_late++;
        }
    }
    interp_stats_last_ns = now;
}

static void Interp_StatsFrame(void) {
    static int enabled = -1;
    if (enabled < 0) {
        enabled = SDL_getenv("PSYZ_INTERP_STATS") != NULL;
    }
    if (!enabled || interp_stats_frames % 600 || !interp_stats_presents) {
        return;
    }
    INFOF("interp: %llu frames, %llu in-between drawn, %llu frames skipped "
          "(last: %s); present every %.2f ms (%.2f-%.2f), %d late",
          interp_stats_frames, interp_stats_drawn, interp_stats_skipped,
          interp_stats_skip, interp_stats_sum_ms / interp_stats_presents,
          interp_stats_min_ms, interp_stats_max_ms, interp_stats_late);
    interp_stats_min_ms = 1e9;
    interp_stats_max_ms = interp_stats_sum_ms = 0;
    interp_stats_presents = 0;
    interp_stats_late = 0;
}

static void PlatformBackend_PresentFrames(int count) {
    if (!Interp_Recording()) {
        for (int i = 0; i < count; i++) {
            PlatformBackend_Present();
        }
        return;
    }
    InterpLog* log = interp_rec;
    interp_drawn = false;
    if (log->any_draw) {
        // a new frame
        if (log->draws) {
            Interp_CopyVram();
        }
        interp_rec = interp_replay;
        interp_replay = log;
        Interp_ResetLog(interp_rec);
        InterpPosTable* tmp = interp_pos_prev;
        interp_pos_prev = interp_pos_cur;
        interp_pos_cur = tmp;
        Interp_PosClear(interp_pos_cur);
        for (int i = 0; i < log->n_prims; i++) {
            if (log->prims[i].tag.key) {
                Interp_PosSet(interp_pos_cur, log->prims[i].tag);
            }
        }
        interp_plan.valid = Interp_PlanFrame();
        interp_plan.uploaded = false;
        interp_plan.shown = display_area;
        interp_plan.step = 0;
        interp_plan.steps = count * interp_frame_span;
        interp_frame_span = 1;
        interp_stats_frames++;
        if (!interp_plan.valid) {
            interp_stats_skipped++;
            interp_stats_skip = interp_skip_reason ? interp_skip_reason : "?";
        }
        Interp_StatsFrame();
    } else {
        // no new frame (the second VSync of a 30 Hz frame, loading): keep
        // going while nothing else changed what is displayed
        const SDL_Rect shown = {
            display_area.x, display_area.y, display_size.x, display_size.y};
        const SDL_Rect area = {
            interp_plan.area.x, interp_plan.area.y, display_size.x,
            display_size.y};
        if (display_area.x != interp_plan.shown.x ||
            display_area.y != interp_plan.shown.y || log->overflow) {
            interp_plan.valid = false;
        }
        for (int i = 0; i < log->n_writes && interp_plan.valid; i++) {
            if (Interp_RectsIntersect(log->writes[i], shown) ||
                Interp_RectsIntersect(log->writes[i], area)) {
                interp_plan.valid = false;
            }
        }
        for (int i = 0; i < log->n_ops && interp_plan.valid; i++) {
            if (Interp_RectsIntersect(log->ops[i].rect, shown) ||
                Interp_RectsIntersect(log->ops[i].rect, area)) {
                interp_plan.valid = false;
            }
        }
        Interp_ResetLog(log);
    }
    interp_prev_display = display_area;

    for (int i = 0; i < count; i++) {
        float t;
        if (Interp_NextRefresh(&t) && Interp_Draw(t)) {
            PresentTexture(interp_target, interp_plan.area);
            interp_stats_drawn++;
        } else {
            PlatformBackend_Present();
        }
        Interp_StatsPresent();
    }
}

// download `r` of a texture as RGB888 (slow: waits for the GPU)
static unsigned char* AllocRgbFromTexture(SDL_GPUTexture* tex, SDL_Rect r) {
    const SDL_GPUTransferBufferCreateInfo info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD,
        .size = (Uint32)(r.w * r.h * 4),
    };
    SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &info);
    SDL_GPUCommandBuffer* cmd = AcquireCmd();
    if (!tb || !cmd) {
        if (tb) {
            SDL_ReleaseGPUTransferBuffer(device, tb);
        }
        return NULL;
    }
    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
    const SDL_GPUTextureRegion region = {
        .texture = tex,
        .x = (Uint32)r.x,
        .y = (Uint32)r.y,
        .w = (Uint32)r.w,
        .h = (Uint32)r.h,
        .d = 1,
    };
    const SDL_GPUTextureTransferInfo transfer = {.transfer_buffer = tb};
    SDL_DownloadFromGPUTexture(copy, &region, &transfer);
    SDL_EndGPUCopyPass(copy);
    SubmitCmdAndWait();
    unsigned char* pixels = malloc((size_t)r.w * r.h * 3);
    const u8* map = SDL_MapGPUTransferBuffer(device, tb, false);
    if (pixels && map) {
        for (int i = 0; i < r.w * r.h; i++) {
            pixels[i * 3 + 0] = map[i * 4 + 0];
            pixels[i * 3 + 1] = map[i * 4 + 1];
            pixels[i * 3 + 2] = map[i * 4 + 2];
        }
    }
    if (map) {
        SDL_UnmapGPUTransferBuffer(device, tb);
    }
    SDL_ReleaseGPUTransferBuffer(device, tb);
    if (!map) {
        free(pixels);
        return NULL;
    }
    return pixels;
}

unsigned char* Psyz_VideoAllocCapturedInterpFrame(int* w, int* h) {
    *w = *h = 0;
    if (!interp_drawn || !interp_target) {
        return NULL;
    }
    const unsigned n = interp_target_res;
    const SDL_Rect r = {
        interp_plan.area.x * (int)n, interp_plan.area.y * (int)n,
        display_size.x * (int)n, display_size.y * (int)n};
    unsigned char* rgb = AllocRgbFromTexture(interp_target, r);
    if (rgb) {
        *w = r.w;
        *h = r.h;
    }
    return rgb;
}
