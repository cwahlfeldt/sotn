// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rlib.h"

// func_us_801AFA80_from_lib declares these inside its body; the PC data
// import needs them at file scope
extern Point16 D_us_801809CC[];
extern SVECTOR D_us_80180944;
extern SVECTOR D_us_801809DC;
extern u8* D_us_801809BC[];

extern EInit g_EInitBreakable;
extern AnimationFrame* g_RlibBreakableAnimations[];
extern u8 g_RlibBreakableHitboxHeights[];
extern u8 g_RlibBreakableExplosionTypes[];
extern u16 g_RlibBreakablePalettes[];
extern u16 g_RlibBreakableAnimSets[];
extern u8 g_RlibBreakableBlendModes[];
extern u16 g_RlibBreakableUnk5A[];
extern u16 g_RlibBreakableHitboxOffsetsY[];

#ifdef VERSION_PSP
extern s32 E_ID(UNK_1B);
extern s32 E_ID(UNK_28);
#endif

void EntityBreakable(Entity* self) {
    Entity* entity;
    u16 breakableType;

    breakableType = self->params >> 12;
    if (!self->step) {
        InitializeEntity(g_EInitBreakable);
        self->zPriority = g_unkGraphicsStruct.g_zEntityCenter - 20;
        if (breakableType < 5) {
            entity = self + 1;
            CreateEntityFromEntity(E_ID(UNK_1B), self, entity);
            entity->params = breakableType;
            entity->zPriority = self->zPriority;
        }
        self->blendMode = g_RlibBreakableBlendModes[breakableType];
        self->hitboxHeight = g_RlibBreakableHitboxHeights[breakableType];
        self->animSet = g_RlibBreakableAnimSets[breakableType];
        self->unk5A = g_RlibBreakableUnk5A[breakableType];
        self->palette = g_RlibBreakablePalettes[breakableType];
        self->hitboxOffY = g_RlibBreakableHitboxOffsetsY[breakableType];
    }
    AnimateEntity(g_RlibBreakableAnimations[breakableType], self);
    if (self->hitParams) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromCurrentEntity(E_EXPLOSION, entity);
            entity->params = g_RlibBreakableExplosionTypes[breakableType];
        }
        switch (breakableType) {
        case 2:
        case 3:
            PlaySfxPositional(SFX_GLASS_BREAK_E);
            break;
        case 9:
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_ID(UNK_28), entity);
                entity->params = 0x100;
            }
            g_api.PlaySfx(SFX_GLASS_BREAK_E);
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_PERSISTENT_ITEM_DROP, self, entity);
                entity->params = self->params & 0x1FF;
            }
            PreventEntityFromRespawning(self);
            DestroyEntity(self);
            return;
        case 7:
            g_api.PlaySfx(SFX_GLASS_BREAK_E);
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_PERSISTENT_ITEM_DROP, self, entity);
                entity->params = self->params & 0x1FF;
            }
            PreventEntityFromRespawning(self);
            DestroyEntity(self);
            return;
        case 8:
            g_api.PlaySfx(SFX_GLASS_BREAK_E);
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_PERSISTENT_ITEM_DROP, self, entity);
                entity->params = 3;
            }
            PreventEntityFromRespawning(self);
            DestroyEntity(self);
            return;
        default:
            g_api.PlaySfx(SFX_GLASS_BREAK_C);
            break;
        }
        ReplaceBreakableWithItemDrop(self);
    }
}

extern EInit g_EInitInteractable;
extern u8 D_us_80180824[];
extern u8 D_us_8018082C[];

void func_us_801AE7AC_from_lib(Entity* self) {
    Entity* breakable;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        self->animSet = ANIMSET_OVL(2);
    }

    if (!self->params) {
        AnimateEntity(D_us_80180824, self);
    } else {
        AnimateEntity(D_us_8018082C, self);
    }

    breakable = self - 1;
    if (breakable->entityId != E_BREAKABLE) {
        if (!self->params) {
            self->animCurFrame = 0xD;
        } else {
            self->animCurFrame = 0xE;
        }
    }
}

extern EInit g_EInitParticle;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_20AE8", func_us_801AE84C_from_lib);
#else
// LIB's EntityBreakableDebris with RLIB's animset and palette
void func_us_801AE84C_from_lib(Entity* self) {
    switch (self->step) {
    case 0:
        if (self->params & 0x100) {
            InitializeEntity(g_EInitInteractable);
            self->animSet = ANIMSET_OVL(7);
            self->unk5A = 91;
            self->palette = 0x213;
            self->animCurFrame = 21;
            self->zPriority = 106;
            self->step = 0x100;
        } else {
            InitializeEntity(g_EInitParticle);
        }
        break;
    }
}
#endif

extern SVECTOR D_us_80180914;
extern SVECTOR D_us_8018091C;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_20AE8", func_us_801AF280_from_lib);
#else
// LIB's EntityBackgroundBookcase, upside down
void func_us_801AF280_from_lib(Entity* self) {
    Primitive* prim;
    s32 primIndex;
    long p, flag;
    s16 x;
    long sxy;
    VECTOR vector;
    MATRIX matrix;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 4);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.prim = prim;
        while (prim != NULL) {
            prim->tpage = 0xF;
            prim->clut = 0x35;
            prim->u0 = prim->u2 = 130;
            prim->u1 = prim->u3 = 253;
            prim->v0 = prim->v1 = 50;
            prim->v2 = prim->v3 = 125;
            prim->priority = 0x40;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        }
        g_GpuBuffers[0].draw.r0 = 16;
        g_GpuBuffers[0].draw.g0 = 16;
        g_GpuBuffers[0].draw.b0 = 16;
        g_GpuBuffers[1].draw.r0 = 16;
        g_GpuBuffers[1].draw.g0 = 16;
        g_GpuBuffers[1].draw.b0 = 16;
        // fallthrough
    case 1:
        SetGeomScreen(1024);
        SetGeomOffset(128, self->posY.i.hi);
        RotMatrix(&D_us_8018091C, &matrix);
        vector.vx = self->posX.i.hi - 128;
        vector.vy = 0;
        vector.vz = 1024;
        TransMatrix(&matrix, &vector);
        SetRotMatrix(&matrix);
        SetTransMatrix(&matrix);
        RotTransPers(&D_us_80180914, (s32*)&sxy, (s32*)&p, (s32*)&flag);

        x = sxy & 0xFFFF;
        if (x > 0) {
            while (x > -64) {
                x -= 124;
            }
        } else {
            while (x < -128) {
                x += 124;
            }
        }

        prim = self->ext.prim;
        while (prim != NULL) {
            prim->x0 = prim->x2 = x;
            x += 124;
            prim->x1 = prim->x3 = x;
            prim->y0 = prim->y1 = self->posY.i.hi + 76;
            prim->y2 = prim->y3 = self->posY.i.hi;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
            if (x > 256) {
                break;
            }
        }
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        break;
    }
}
#endif

extern EInit g_EInitInteractable;
extern SVECTOR D_us_80180924;
extern SVECTOR D_us_8018092C;

void func_us_801AF538_from_lib(Entity* self) {
    s32 primIndex;
    s16 x;
#ifdef VERSION_PSP
    struct {
        MATRIX matrix;
        VECTOR trans;
        long screenX;
        long screenZ;
        long screenY;
    } geometry;
#else
    struct {
        s32 pad[2];
        VECTOR trans;
        MATRIX matrix;
        s32 screenX;
        s32 screenY;
        s32 screenZ;
    } geometry;
    MATRIX* matrixPtr;
#endif
    Primitive* prim;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 4);
        if (primIndex != -1) {
            self->flags |= FLAG_HAS_PRIMS;
            self->primIndex = primIndex;
            prim = &g_PrimBuf[primIndex];
            self->ext.prim = prim;

            while (prim != NULL) {
                prim->tpage = 0xF;
                prim->clut = 0x34;
                prim->u0 = prim->u2 = 0x80;
                prim->u1 = prim->u3 = 0xF8;
                prim->v0 = prim->v1 = 0;
                prim->v2 = prim->v3 = 0x1C;
                prim->priority = 0x50;
                prim->drawMode = DRAW_UNK02;
                prim = prim->next;
            }
        } else {
            DestroyEntity(self);
            return;
        }

    case 1:
        SetGeomScreen(0x400);
        SetGeomOffset(0x80, self->posY.i.hi);
#ifdef VERSION_PSP
        RotMatrix(&D_us_8018092C, &geometry.matrix);
        geometry.trans.vx = self->posX.i.hi - 0x80;
        geometry.trans.vy = 0;
        geometry.trans.vz = 0x400;
        TransMatrix(&geometry.matrix, &geometry.trans);
        SetRotMatrix(&geometry.matrix);
        SetTransMatrix(&geometry.matrix);
        RotTransPers(&D_us_80180924, &geometry.screenX, &geometry.screenY,
                     &geometry.screenZ);
        x = (s16)geometry.screenX;
#else
        matrixPtr = &geometry.matrix;
        RotMatrix(&D_us_8018092C, matrixPtr);
        geometry.trans.vx = self->posX.i.hi - 0x80;
        geometry.trans.vy = 0;
        geometry.trans.vz = 0x400;
        TransMatrix(matrixPtr, &geometry.trans);
        SetRotMatrix(matrixPtr);
        SetTransMatrix(matrixPtr);
        RotTransPers(&D_us_80180924, &geometry.screenX, &geometry.screenY,
                     &geometry.screenZ);
        x = (s16)geometry.screenX;
#endif
        if (x > 0) {
            while (x >= -0x3F) {
                x -= 0x78;
            }
        } else {
            while (x < -0x80) {
                x += 0x78;
            }
        }

        prim = self->ext.prim;
        while (prim != NULL) {
            prim->x0 = prim->x2 = x;
            x += 0x78;
            prim->x1 = prim->x3 = x;
            prim->y0 = prim->y1 = self->posY.i.hi + 0x1C;
            prim->y2 = prim->y3 = self->posY.i.hi;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
            if (x >= 0x101) {
                break;
            }
        }

        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        return;

    default:
        return;
    }
}

extern EInit g_EInitInteractable;
extern SVECTOR D_us_80180934;
extern SVECTOR D_us_8018093C;

void func_us_801AF7B8_from_lib(Entity* self) {
    Primitive* prim;
    Primitive* last;
    s32 primIndex;
    s16 x;
#ifdef VERSION_PSP
    long sz;
    long flag;
    long sxy;
    VECTOR trans;
    MATRIX matrix;
#else
    struct {
        s32 pad[2];
        VECTOR value;
    } transStorage;
    MATRIX matrix;
    long sxy;
    long sz;
    long flag;
#endif

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api_func_800EDB58(0x11, 0xA);
        if (primIndex != -1) {
            self->flags |= FLAG_HAS_PRIMS;
            self->primIndex = primIndex;
            prim = &g_PrimBuf[primIndex];
            self->ext.et_801AF774.prim1 = prim;

            while (prim != NULL) {
                prim->u0 = 0x18;
                prim->v0 = 0x40;
                prim->r0 = prim->g0 = prim->b0 = 0;
                prim->priority = 0x60;
                prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
                last = prim;
                prim = prim->next;
            }

            prim = last;
            self->ext.et_801AF774.prim2 = prim;
            prim->u0 = 0xFF;
            prim->v0 = 0x30;
            prim->x0 = 0;
            prim->y0 = self->posY.i.hi + 0x70;
            prim->r0 = prim->g0 = prim->b0 = 0;
            prim->priority = 0x60;
            prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
        } else {
            DestroyEntity(self);
            return;
        }
        /* fall through */
    case 1:
        SetGeomScreen(0x400);
        SetGeomOffset(0x80, self->posY.i.hi);
        RotMatrix(&D_us_8018093C, &matrix);
#ifdef VERSION_PSP
        trans.vx = self->posX.i.hi - 0x80;
        trans.vy = 0;
        trans.vz = 0x400;
        TransMatrix(&matrix, &trans);
#else
        transStorage.value.vx = self->posX.i.hi - 0x80;
        transStorage.value.vy = 0;
        transStorage.value.vz = 0x400;
        TransMatrix(&matrix, &transStorage.value);
#endif
        SetRotMatrix(&matrix);
        SetTransMatrix(&matrix);
        RotTransPers(&D_us_80180934, &sxy, &sz, &flag);
        x = (s16)sxy;

        if (x > 0) {
            while (x >= -0x3F) {
                x -= 0x38;
            }
        } else {
            while (x < -0x80) {
                x += 0x38;
            }
        }

        prim = self->ext.et_801AF774.prim1;
        while (prim != NULL) {
            prim->x0 = x;
            x += 0x38;
            prim->y0 = self->posY.i.hi;
            prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            prim = prim->next;
            if (x >= 0x101) {
                break;
            }
        }

        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }

        prim = self->ext.et_801AF774.prim2;
        prim->u0 = 0xFF;
        prim->v0 = 0x30;
        prim->x0 = 0;
        prim->y0 = self->posY.i.hi + 0x40;
        prim->r0 = prim->g0 = prim->b0 = 0;
        prim->priority = 0x60;
        prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
    }
}

void func_us_801AFA80_from_lib(Entity* self) {
    extern u16 g_EInitInteractable[];
    extern Point16 D_us_801809CC[];
    extern SVECTOR D_us_80180944;
    extern SVECTOR D_us_801809DC;
    extern u8* D_us_801809BC[];

    long p, flag;
    u8 pad[4];
    long sxy;
    VECTOR trans;
    MATRIX m;
    Primitive* prim;
    s32 primIndex;
    s32 i, j;
    u8* ptr;
    s16 posX, posY;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->posX.i.hi =
            D_us_801809CC[self->params].x - g_Tilemap.scrollX.i.hi;
        self->posY.i.hi =
            D_us_801809CC[self->params].y - g_Tilemap.scrollY.i.hi;
        primIndex = g_api.func_800EDB58(PRIM_GT4, 0x10);
        if (primIndex != -1) {
            self->flags |= FLAG_HAS_PRIMS;
            self->primIndex = primIndex;
            prim = &g_PrimBuf[primIndex];
            self->ext.prim = prim;
            for (i = 0; prim != NULL; i++) {
                prim->tpage = 0xF;
                prim->clut = 0x2F;
                if (i % 2) {
                    prim->u0 = prim->u2 = 0x88;
                    prim->u1 = prim->u3 = 0xFE;
                } else {
                    prim->u0 = prim->u2 = 0xFE;
                    prim->u1 = prim->u3 = 0x88;
                }
                prim->v0 = prim->v1 = 0xFE;
                prim->v2 = prim->v3 = 0x80;
                prim->priority = 0x20;
                prim->drawMode = DRAW_UNK02;
                prim = prim->next;
            }
        } else {
            DestroyEntity(self);
            return;
        }
        g_GpuBuffers[0].draw.r0 = 0x40;
        g_GpuBuffers[0].draw.g0 = 0x38;
        g_GpuBuffers[0].draw.b0 = 0x28;
        g_GpuBuffers[1].draw.r0 = 0x40;
        g_GpuBuffers[1].draw.g0 = 0x38;
        g_GpuBuffers[1].draw.b0 = 0x28;

    case 1:
        SetGeomScreen(0x400);
        SetGeomOffset(0x80, 0x80);
        RotMatrix(&D_us_801809DC, &m);
        trans.vx = self->posX.i.hi - 0x80;
        trans.vy = self->posY.i.hi - 0x80;
        trans.vz = 0x400;
        TransMatrix(&m, &trans);
        SetRotMatrix(&m);
        SetTransMatrix(&m);
        RotTransPers(&D_us_80180944, &sxy, &p, &flag);
        posX = sxy & 0xFFFF;
        posY = sxy >> 0x10;
        prim = self->ext.prim;
        ptr = D_us_801809BC[self->params];
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                prim->drawMode = DRAW_DITHERING | DRAW_COLORS;
                prim->x0 = prim->x2 = posX + (j * 0x78) - 0xF0;
                if (prim->x0 > 0x100) {
                    prim->drawMode = DRAW_HIDE;
                }
                prim->x1 = prim->x3 = posX + (j * 0x78) - 0x78;
                if (prim->x1 < 0) {
                    prim->drawMode = DRAW_HIDE;
                }
                prim->y0 = prim->y1 = posY + (i * 0x80) - 0x100;
                if (prim->y0 > 0x100) {
                    prim->drawMode = DRAW_HIDE;
                }
                prim->y2 = prim->y3 = posY + (i * 0x80) - 0x80;
                if (prim->y2 < 0) {
                    prim->drawMode = DRAW_HIDE;
                }
                PGREY(prim, 0) = *(ptr + i * 5 + j + 0);
                PGREY(prim, 1) = *(ptr + i * 5 + j + 1);
                PGREY(prim, 2) = *(ptr + (i + 1) * 5 + j + 0);
                PGREY(prim, 3) = *(ptr + (i + 1) * 5 + j + 1);
                prim = prim->next;
            }
        }
    }
}

extern SVECTOR* D_us_80180A24[];
extern VECTOR* D_us_80180AC4[];
extern s32 D_us_80180AE4[];
extern MATRIX D_us_80180B24;
extern s16 D_us_80180B44[];
extern s16 D_us_80180B4C[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_20AE8", func_us_801AE8E8_from_lib);
#else
#include <scratchpad.h>
Primitive* UnkRecursivePrimFunc2(
    Primitive* srcPrim, s32 iterations, Primitive* dstPrim, u8* dataPtr);
// LIB's rotating bookshelf (func_us_801AE8E8) upside down: the front and
// back faces swap, Alucard pushes the other edges, and it turns the other
// way
void func_us_801AE8E8_from_lib(Entity* self) {
    s32 primIndex;
    s32 p, flag;
    SVECTOR rot;
    VECTOR trans;
    MATRIX m;
    MATRIX lightMatrix;
    CVECTOR color;
    SVECTOR normal;
    s32 iterations;
    Primitive* prim;
    Primitive* prim2;
    SVECTOR** vertices;
    VECTOR** normals;
    s16 angle;
    s32 i;
    s16 dx;
    s32 clipper;
    Entity* player = &PLAYER;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->zPriority = 0x70;
        self->hitboxWidth = 0xC;
        self->hitboxHeight = 0x30;
        self->hitboxOffY = -0x30;
        self->ext.et_801AE8E8.unk80 = -0xC0;
        self->ext.et_801AE8E8.unk8C = 0x40;
        self->ext.et_801AE8E8.unk84 = 0;
        self->ext.et_801AE8E8.unk88 = 0;
        self->ext.et_801AE8E8.unk82 = 0x60;
        if (self->params) {
            self->step = 4;
            self->ext.et_801AE8E8.unk80 = D_us_80180B44[self->params];
        }
        self->ext.et_801AE8E8.unk90 = D_us_80180B4C[self->params];
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x25);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.et_801AE8E8.unk7C = prim;
        self->flags |= FLAG_HAS_PRIMS;
        for (i = 0; i < 2; i++) {
            prim->r0 = prim->g0 = prim->b0 = 0x80;
            prim->tpage = 0xF;
            prim->clut = 0x32;
            prim->u0 = prim->u2 = 4;
            prim->u1 = prim->u3 = 0x1C;
            prim->v0 = prim->v1 = 0x84;
            prim->v2 = prim->v3 = 0xE4;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim->drawMode = DRAW_DITHERING | DRAW_COLORS | DRAW_UNK02;
            prim->priority = self->zPriority - self->params;
            prim = prim->next;
        }
        for (i = 0; i < 2; i++) {
            prim->r0 = prim->g0 = prim->b0 = 0x80;
            prim->tpage = 0xF;
            prim->clut = 0x33;
            prim->u0 = prim->u2 = 0x24;
            prim->u1 = prim->u3 = 0x64;
            prim->v0 = prim->v1 = 0x84;
            prim->v2 = prim->v3 = 0xE4;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim->drawMode = DRAW_DITHERING | DRAW_COLORS | DRAW_UNK02;
            prim->priority = self->zPriority - self->params;
            prim = prim->next;
        }
        if (self->params) {
            prim->drawMode = DRAW_HIDE;
        } else {
            self->ext.et_801AE8E8.unk94 = prim;
            prim->type = PRIM_G4;
            prim->x1 = prim->x3 = 0x100;
            prim->x0 = prim->x2 = self->posX.i.hi;
            prim->y0 = self->posY.i.hi - 0x58;
            prim->y1 = self->posY.i.hi - 0x40;
            prim->y2 = prim->y3 = self->posY.i.hi;
            prim->drawMode =
                DRAW_DITHERING | DRAW_UNK_40 | DRAW_TPAGE | DRAW_TRANSP;
            prim->priority = self->zPriority - 1;
        }
        prim = prim->next;
        self->ext.et_801AE8E8.unk98 = prim;
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        break;

    case 1:
        prim = NULL;
        angle = self->ext.et_801AE8E8.unk80 & 0xFFF;
        self->ext.et_801AE8E8.unk88 = 0;
        if (angle < 0x380 || angle > 0xC80) {
            prim = self->ext.et_801AE8E8.unk7C->next;
        }
        if (angle > 0x480 && angle < 0xB80) {
            prim = self->ext.et_801AE8E8.unk7C;
        }
        if (prim == NULL) {
            break;
        }
        if (player->posX.i.hi < prim->x0) {
            self->step_s = 0;
        } else {
            self->step_s = 1;
        }
        switch (self->step_s) {
        case 0:
            dx = player->posX.i.hi - (prim->x1 - 10);
            if (dx >= 0) {
                player->posX.i.hi -= dx;
                g_unkGraphicsStruct.shoveX.i.hi -= dx;
                g_Player.vram_flag |= VRAM_FLAG_UNK40 | TOUCHING_R_WALL;
                if (!g_CastleFlags[LIB_BOOKSHELF_SECRET]) {
                    if (--self->ext.et_801AE8E8.unk82) {
                        break;
                    }
                    g_CastleFlags[LIB_BOOKSHELF_SECRET] = 1;
                }
                if (player->velocityX != 0) {
                    if ((g_Timer & 0xF) == 0) {
                        PlaySfxPositional(SFX_STONE_MOVE_B);
                    }
                    self->ext.et_801AE8E8.unk88 = 0x600;
                }
            }
            break;
        case 1:
            dx = player->posX.i.hi - (prim->x0 + 10);
            if (dx <= 0) {
                player->posX.i.hi -= dx;
                g_unkGraphicsStruct.shoveX.i.hi -= dx;
                g_Player.vram_flag |= VRAM_FLAG_UNK40 | TOUCHING_L_WALL;
                if (!g_CastleFlags[LIB_BOOKSHELF_SECRET]) {
                    if (--self->ext.et_801AE8E8.unk82) {
                        break;
                    }
                    g_CastleFlags[LIB_BOOKSHELF_SECRET] = 1;
                }
                if (player->velocityX != 0) {
                    self->ext.et_801AE8E8.unk88 = -0x600;
                    if ((g_Timer & 0xF) == 0) {
                        PlaySfxPositional(SFX_STONE_MOVE_B);
                    }
                }
            }
            break;
        }
        if (labs(self->ext.et_801AE8E8.unk84) > 0x1000) {
            self->ext.et_801AE8E8.unk8C = 2;
        }
        break;
    }

    self->ext.et_801AE8E8.unk84 += self->ext.et_801AE8E8.unk88;
    if (self->ext.et_801AE8E8.unk84 != 0) {
        self->ext.et_801AE8E8.unk84 -=
            (self->ext.et_801AE8E8.unk88 / self->ext.et_801AE8E8.unk84);
    }
    self->ext.et_801AE8E8.unk84 -=
        (self->ext.et_801AE8E8.unk84 * self->ext.et_801AE8E8.unk8C) / 0x100;
    self->ext.et_801AE8E8.unk80 += self->ext.et_801AE8E8.unk84 / 0x1000;
    prim = self->ext.et_801AE8E8.unk7C;
    SetGeomScreen(0x200);
    rot.vx = 0x800;
    rot.vy = self->ext.et_801AE8E8.unk80;
    rot.vz = 0;
    RotMatrix(&rot, &m);
    trans.vx = 0;
    trans.vy = 0;
    trans.vz = 0x228;
    if (self->params) {
        trans.vz += self->ext.et_801AE8E8.unk90;
    }
    TransMatrix(&m, &trans);
    SetRotMatrix(&m);
    SetTransMatrix(&m);
    color.r = 0x90;
    color.g = 0x90;
    color.b = 0x90;
    color.cd = prim->type;
    RotMatrix(&rot, &lightMatrix);
    SetBackColor(0x58, 0x58, 0x58);
    SetLightMatrix(&lightMatrix);
    SetColorMatrix(&D_us_80180B24);
    SetFarColor(0x40, 0x40, 0x40);
    SetFogNear(0x218, 0x200);
    SetGeomOffset(self->posX.i.hi, self->posY.i.hi);
    vertices = D_us_80180A24;
    normals = D_us_80180AC4;
    prim2 = self->ext.et_801AE8E8.unk98;
    for (i = 0; i < 8; i++) {
        RotTransPers(*vertices, &SPAD(0)[i], &p, &flag);
        VectorNormalS(*normals, &normal);
        NormalColorDpq(&normal, &color, p, (CVECTOR*)&SPAD(8)[i]);
        vertices++;
        normals++;
    }
    if (!self->params) {
        iterations = 2;
    } else {
        iterations = 1;
    }
    prim = self->ext.et_801AE8E8.unk7C;
    for (i = 0; i < 4; i++) {
        LOW(prim->x0) = SPAD(0)[D_us_80180AE4[i * 4 + 0]];
        LOW(prim->x1) = SPAD(0)[D_us_80180AE4[i * 4 + 1]];
        LOW(prim->x2) = SPAD(0)[D_us_80180AE4[i * 4 + 2]];
        LOW(prim->x3) = SPAD(0)[D_us_80180AE4[i * 4 + 3]];
        LOW(prim->r0) = SPAD(8)[D_us_80180AE4[i * 4 + 0]];
        LOW(prim->r1) = SPAD(8)[D_us_80180AE4[i * 4 + 1]];
        LOW(prim->r2) = SPAD(8)[D_us_80180AE4[i * 4 + 2]];
        LOW(prim->r3) = SPAD(8)[D_us_80180AE4[i * 4 + 3]];
        clipper = NormalClip(LOW(prim->x0), LOW(prim->x1), LOW(prim->x2));
        prim->drawMode &= ~DRAW_HIDE;
        if (clipper <= 0) {
            prim->drawMode |= DRAW_HIDE;
        } else {
            prim2 = UnkRecursivePrimFunc2(prim, iterations, prim2, (u8*)SPAD(16));
            prim->drawMode |= DRAW_HIDE;
        }
        prim = prim->next;
    }
    while (prim2 != NULL) {
        prim2->drawMode = DRAW_HIDE;
        prim2 = prim2->next;
    }
    if (!self->params) {
        prim = self->ext.et_801AE8E8.unk94;
        prim->r0 = abs(rcos(self->ext.et_801AE8E8.unk80)) * 0x28 / 0x1000;
        prim->g0 = prim->b0 = prim->r0;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_8018094C[];
extern u8 D_us_80180968[];
extern u8 D_us_80180984[];
extern u8 D_us_801809A0[];
u8 D_us_8018094C[] ALIGNED4 = {0x00, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0xC0, 0xF0, 0xC0, 0x00, 0x00, 0x40, 0x80, 0x40, 0x00, 0x00, 0x20, 0x20, 0x20, 0x00, 0x00, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180968[] ALIGNED4 = {0x40, 0x20, 0x60, 0xA0, 0x20, 0x50, 0x30, 0x70, 0xB0, 0x30, 0x60, 0x40, 0x80, 0xC0, 0x40, 0x70, 0x50, 0x90, 0xD0, 0x50, 0x80, 0x60, 0xA0, 0xE0, 0x60, 0x00, 0x00, 0x00};
u8 D_us_80180984[] ALIGNED4 = {0x40, 0x20, 0x60, 0xA0, 0x20, 0x50, 0x30, 0x70, 0xB0, 0x30, 0x60, 0x40, 0x80, 0xC0, 0x40, 0x70, 0x50, 0x90, 0xD0, 0x50, 0x80, 0x60, 0xA0, 0xE0, 0x60, 0x00, 0x00, 0x00};
u8 D_us_801809A0[] ALIGNED4 = {0x00, 0xC0, 0xF0, 0xC0, 0x00, 0x00, 0x60, 0x60, 0x60, 0x00, 0x00, 0x40, 0x80, 0x40, 0x00, 0x00, 0x20, 0x20, 0x20, 0x00, 0x00, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00};
extern u8 D_us_801809E4[];
extern u8 D_us_801809EC[];
extern u8 D_us_801809F4[];
extern u8 D_us_801809FC[];
extern u8 D_us_80180A04[];
extern u8 D_us_80180A0C[];
extern u8 D_us_80180A14[];
extern u8 D_us_80180A1C[];
u8 D_us_801809E4[] ALIGNED4 = {0xF4, 0xFF, 0xA0, 0xFF, 0xE0, 0xFF, 0x00, 0x00};
u8 D_us_801809EC[] ALIGNED4 = {0x0C, 0x00, 0xA0, 0xFF, 0xE0, 0xFF, 0x00, 0x00};
u8 D_us_801809F4[] ALIGNED4 = {0xF4, 0xFF, 0x00, 0x00, 0xE0, 0xFF, 0x00, 0x00};
u8 D_us_801809FC[] ALIGNED4 = {0x0C, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x00, 0x00};
u8 D_us_80180A04[] ALIGNED4 = {0x0C, 0x00, 0xA0, 0xFF, 0x20, 0x00, 0x00, 0x00};
u8 D_us_80180A0C[] ALIGNED4 = {0xF4, 0xFF, 0xA0, 0xFF, 0x20, 0x00, 0x00, 0x00};
u8 D_us_80180A14[] ALIGNED4 = {0x0C, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00};
u8 D_us_80180A1C[] ALIGNED4 = {0xF4, 0xFF, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00};
extern u8 D_us_80180834[];
extern u8 D_us_80180840[];
extern u8 D_us_8018084C[];
extern u8 D_us_80180858[];
extern u8 D_us_80180860[];
extern u8 D_us_80180864[];
extern u8 D_us_80180868[];
extern u8 D_us_80180A44[];
extern u8 D_us_80180A54[];
extern u8 D_us_80180A64[];
extern u8 D_us_80180A74[];
extern u8 D_us_80180A84[];
extern u8 D_us_80180A94[];
extern u8 D_us_80180AA4[];
extern u8 D_us_80180AB4[];
u8 D_us_80180834[] ALIGNED4 = {0x06, 0x08, 0x06, 0x09, 0x06, 0x0A, 0x06, 0x0B, 0x06, 0x0C, 0x00, 0x00};
u8 D_us_80180840[] ALIGNED4 = {0x05, 0x09, 0x05, 0x0A, 0x05, 0x0B, 0x05, 0x0C, 0x00, 0x00, 0x00, 0x00};
u8 D_us_8018084C[] ALIGNED4 = {0x05, 0x0D, 0x05, 0x0E, 0x05, 0x0F, 0x05, 0x10, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180858[] ALIGNED4 = {0x05, 0x11, 0x05, 0x12, 0x05, 0x13, 0x00, 0x00};
u8 D_us_80180860[] ALIGNED4 = {0x05, 0x17, 0x00, 0x00};
u8 D_us_80180864[] ALIGNED4 = {0x05, 0x16, 0x00, 0x00};
u8 D_us_80180868[] ALIGNED4 = {0x05, 0x14, 0xFF, 0xFF, 0x05, 0x15, 0x05, 0x15, 0xFF, 0x00, 0x00, 0x00};
u8 D_us_80180A44[] ALIGNED4 = {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180A54[] ALIGNED4 = {0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180A64[] ALIGNED4 = {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180A74[] ALIGNED4 = {0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180A84[] ALIGNED4 = {0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180A94[] ALIGNED4 = {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180AA4[] ALIGNED4 = {0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180AB4[] ALIGNED4 = {0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180824[8] = { 3, 4, 3, 5, 0, 0, 0, 0 };
u8 D_us_8018082C[8] = { 3, 6, 3, 7, 0, 0, 0, 0 };
SVECTOR D_us_80180914 = { 0, 0, 0x200, 0 };
SVECTOR D_us_8018091C = { 0, 0, 0, 0 };
SVECTOR D_us_80180924 = { 0, 0, 0x100, 0 };
SVECTOR D_us_8018092C = { 0, 0, 0, 0 };
SVECTOR D_us_80180934 = { 0, 0, 0x1C0, 0 };
SVECTOR D_us_8018093C = { 0, 0, 0, 0 };
VECTOR* D_us_80180AC4[8] = {
    &D_us_80180A44,
    &D_us_80180A54,
    &D_us_80180A64,
    &D_us_80180A74,
    &D_us_80180A84,
    &D_us_80180A94,
    &D_us_80180AA4,
    &D_us_80180AB4,
};
s32 D_us_80180AE4[0x10] = { 0, 1, 2, 3, 4, 5, 6, 7, 1, 4, 3, 6, 5, 0, 7, 2 };
MATRIX D_us_80180B24 = {
    { { 0x1000, 0, 0x800 }, { 0x1000, 0, 0x800 }, { 0x1000, 0, 0x800 } },
    { 0, 0, 0 },
};
s16 D_us_80180B44[4] = { 0, -0x200, 0x240, 0x400 };
s16 D_us_80180B4C[8] = { 0, 0x3C, 0x40, 0x58, 0, 0, 0, 0 };
AnimationFrame* g_RlibBreakableAnimations[0xA] = {
    D_us_80180834,
    D_us_80180834,
    (AnimationFrame *) D_us_80180824,
    (AnimationFrame *) D_us_8018082C,
    D_us_80180840,
    D_us_8018084C,
    D_us_80180858,
    &D_us_80180860,
    &D_us_80180864,
    D_us_80180868,
};
u16 g_RlibBreakableAnimSets[0xA] = {
    0x8002,
    0x8002,
    0x8002,
    0x8002,
    0x8007,
    0x8007,
    0x8007,
    0x8007,
    0x8007,
    0x8007,
};
u8 g_RlibBreakableBlendModes[0xC] = { 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0, 0 };
u8 g_RlibBreakableExplosionTypes[0xC] = { 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0 };
u8 g_RlibBreakableHitboxHeights[0xC] = { 8, 8, 8, 8, 0x10, 0x10, 8, 8, 8, 8, 8, 0 };
u16 g_RlibBreakableHitboxOffsetsY[0xC] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
u16 g_RlibBreakablePalettes[0xA] = { 0, 0, 0, 0, 0x213, 0x213, 0x213, 0x213, 0x213, 0x213 };
u16 g_RlibBreakableUnk5A[0xA] = { 0, 0, 0, 0, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B };
SVECTOR* D_us_80180A24[8] = {
    &D_us_801809E4,
    &D_us_801809EC,
    &D_us_801809F4,
    &D_us_801809FC,
    &D_us_80180A04,
    &D_us_80180A0C,
    &D_us_80180A14,
    &D_us_80180A1C,
};
SVECTOR D_us_80180944 = { 0, 0, 0x400, 0 };
u8* D_us_801809BC[4] = { D_us_8018094C, D_us_80180968, D_us_80180984, D_us_801809A0 };
Point16 D_us_801809CC[4] = { { 0, 0 }, { 0x420, 0x1E0 }, { 0xB0, 0x280 }, { 0x80, 0x1A0 } };
SVECTOR D_us_801809DC = { 0, 0, 0, 0 };
// end pc_import_data.py
// clang-format on
#endif
