// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"
#include <scratchpad.h>

#include "../stained_glass_recurse.h"

extern void StainedGlassBlendPalette(RECT* arg0, u16 srcPalIdx, u16 destPalIdx, s32 steps, CVECTOR* color);
extern EInit g_EInitInteractable;
extern SVECTOR D_us_80180B1C;
extern SVECTOR D_us_80180B24;
extern SVECTOR D_us_80180B2C;
extern SVECTOR D_us_80180B34;
extern SVECTOR D_us_80180B3C;
extern SVECTOR D_us_80180B44;
extern SVECTOR D_us_80180B4C;
extern SVECTOR D_us_80180B54;
extern VECTOR D_us_80180B5C[];
extern s16 D_us_80180C1C[];
extern CVECTOR D_us_80180C34[];
extern SVECTOR D_us_80180C64;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_327A4", func_us_801B2C28);
#else
// DAI's EntityStainedGlass upside down: the windows and the light falling
// through the transparent ones
void func_us_801B2C28(Entity* self) {
    SVECTOR rotVector;
    VECTOR transVector;
    MATRIX mtx;
    s32 p;
    s32 flag;
    s32 primIndex;
    s32 iterations;
    s32 count;
    u8 transparent;
    s16 midpointX;
    s16 midpointY;
    VECTOR* paramsPtr;
    CVECTOR* colorPtr;
    s16* yValsPtr;
    Primitive* glassPrim;
    Primitive* lightPrim;
    Primitive* tempPrim;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 60);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        glassPrim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.stainedGlass.glassPrim = glassPrim;
        self->flags |= FLAG_HAS_PRIMS;
        for (count = 0; count < 12; count++) {
            PGREY(glassPrim, 0) = 0x60;
            glassPrim->priority = 0x56;
            glassPrim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
            glassPrim->tpage = 0xF;
            glassPrim->clut = 0xE;
            glassPrim->u0 = glassPrim->u2 = 4;
            glassPrim->u1 = glassPrim->u3 = 0x1C;
            glassPrim->v0 = glassPrim->v1 = 1;
            glassPrim->v2 = glassPrim->v3 = 0xAE;
            LOW(glassPrim->r1) = LOW(glassPrim->r0);
            LOW(glassPrim->r2) = LOW(glassPrim->r0);
            LOW(glassPrim->r3) = LOW(glassPrim->r0);
            glassPrim = glassPrim->next;
        }
        for (self->ext.stainedGlass.lightPrim = glassPrim; glassPrim != NULL;
             glassPrim = glassPrim->next) {
            glassPrim->drawMode = DRAW_HIDE;
        }
        break;
    case 1:
        SetGeomScreen(0x400);
        SetGeomOffset(0x80, 0xA0);
        paramsPtr = D_us_80180B5C;
        colorPtr = D_us_80180C34;
        yValsPtr = D_us_80180C1C;
        glassPrim = self->ext.stainedGlass.glassPrim;
        lightPrim = self->ext.stainedGlass.lightPrim;
        for (count = 0; count < 12; count++) {
            rotVector.vx = 0;
            rotVector.vy = *yValsPtr;
            rotVector.vz = 0;
            RotMatrix(&rotVector, &mtx);
            RotMatrixZ(0x800, &mtx);
            transVector = *paramsPtr;
            if (transVector.vx > 0) {
                transparent = true;
                glassPrim->drawMode =
                    DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            } else {
                transparent = false;
                glassPrim->drawMode = DRAW_COLORS;
            }
            iterations = paramsPtr->pad;
            transVector.vx += self->posX.i.hi - 0x80;
            transVector.vz += 0x400;
            transVector.vy += self->posY.i.hi - 0xA0;
            TransMatrix(&mtx, &transVector);
            SetRotMatrix(&mtx);
            SetTransMatrix(&mtx);
            CVEC(glassPrim->r0) = *colorPtr;
            glassPrim->type = PRIM_GT4;
            LOW(glassPrim->r1) = LOW(glassPrim->r0);
            LOW(glassPrim->r2) = LOW(glassPrim->r0);
            LOW(glassPrim->r3) = LOW(glassPrim->r0);
            if (iterations) {
                lightPrim = StainedGlassRecurse(
                    &D_us_80180B1C, &D_us_80180B24, &D_us_80180B2C,
                    &D_us_80180B34, glassPrim, iterations, lightPrim,
                    (u8*)SP(0));
                glassPrim->drawMode = DRAW_HIDE;
            }
            RotTransPers4(&D_us_80180B1C, &D_us_80180B24, &D_us_80180B2C,
                          &D_us_80180B34, (s32*)&glassPrim->x0,
                          (s32*)&glassPrim->x1, (s32*)&glassPrim->x2,
                          (s32*)&glassPrim->x3, &p, &flag);
            if (transparent) {
                tempPrim = lightPrim->next;
                *lightPrim = *glassPrim;
                lightPrim->next = tempPrim;
                lightPrim->clut = PAL_FILL_WHITE;
                lightPrim->drawMode = DRAW_COLORS;
                lightPrim->priority = glassPrim->priority - 1;
                lightPrim->r0 = lightPrim->g0 = lightPrim->b0 = colorPtr->cd;
                LOW(lightPrim->r1) = LOW(lightPrim->r0);
                lightPrim->r2 = lightPrim->g2 = lightPrim->b2 =
                    colorPtr->cd >> 1;
                LOW(lightPrim->r3) = LOW(lightPrim->r2);

                lightPrim = lightPrim->next;
                tempPrim = lightPrim->next;
                *lightPrim = *glassPrim;
                lightPrim->next = tempPrim;
                rotVector.vx = 0;
                rotVector.vy = *yValsPtr;
                rotVector.vz = 0x400;
                RotMatrix(&D_us_80180C64, &mtx);
                RotMatrixY(rotVector.vy, &mtx);
                RotMatrixZ(rotVector.vz, &mtx);
                RotMatrixZ(0x800, &mtx);
                transVector = *paramsPtr;
                transVector.vx = self->posX.i.hi - 0x20;
                transVector.vz += 0x400;
                transVector.vy = self->posY.i.hi - 0x1A0;
                TransMatrix(&mtx, &transVector);
                SetRotMatrix(&mtx);
                SetTransMatrix(&mtx);
                CVEC(lightPrim->r0) = *colorPtr;
                lightPrim->type = PRIM_GT4;
                LOW(lightPrim->r1) = LOW(lightPrim->r0);
                LOW(lightPrim->r2) = LOW(lightPrim->r0);
                LOW(lightPrim->r3) = LOW(lightPrim->r0);
                RotTransPers4(&D_us_80180B3C, &D_us_80180B44, &D_us_80180B4C,
                              &D_us_80180B54, (s32*)&lightPrim->x0,
                              (s32*)&lightPrim->x1, (s32*)&lightPrim->x2,
                              (s32*)&lightPrim->x3, &p, &flag);
                lightPrim->clut = 0x200;
                lightPrim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                                      DRAW_COLORS | DRAW_TRANSP;
                midpointX = (lightPrim->x0 + lightPrim->x2) / 2;
                midpointY = (lightPrim->y0 + lightPrim->y1) / 2;

                lightPrim = lightPrim->next;
                tempPrim = lightPrim->next;
                *lightPrim = *glassPrim;
                lightPrim->next = tempPrim;
                lightPrim->clut = PAL_FILL_WHITE;
                lightPrim->drawMode =
                    DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
                lightPrim->r0 = 0x18;
                lightPrim->g0 = 8;
                lightPrim->b0 = 0x20;
                lightPrim->r2 = 8;
                lightPrim->g2 = 0x18;
                lightPrim->b2 = 8;
                lightPrim->r3 = lightPrim->g3 = lightPrim->b3 = 0;
                lightPrim->priority = glassPrim->priority + 1;
                LOW(lightPrim->r1) = LOW(lightPrim->r0);
                LOW(lightPrim->x0) = LOW(glassPrim->x1);
                LOW(lightPrim->x1) = LOW(glassPrim->x0);
                lightPrim->x3 = midpointX;
                lightPrim->y3 = midpointY;
                LOW(lightPrim->x2) = LOW(glassPrim->x3);
                lightPrim = lightPrim->next;
            }
            colorPtr++;
            yValsPtr++;
            glassPrim = glassPrim->next;
            paramsPtr++;
        }
        for (count = 0; lightPrim != NULL; lightPrim = lightPrim->next) {
            lightPrim->drawMode = DRAW_HIDE;
            count++;
        }
        FntPrint("rest %x\n", count);
        break;
    }
}
#endif

extern SVECTOR D_us_80180C6C;
extern SVECTOR D_us_80180C74;
extern SVECTOR D_us_80180C7C;
extern SVECTOR D_us_80180C84;
extern SVECTOR D_us_80180C8C;
extern VECTOR D_us_80180C94[];
extern SVECTOR D_us_80180D24;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_327A4", func_us_801B3368);
#else
// DAI's EntityStainedGlassBackground upside down: the wall and arches
// behind the windows
void func_us_801B3368(Entity* self) {
    MATRIX mtx;
    VECTOR transVector;
    RECT rect;
    CVECTOR color;
    s32 sxy[4];
    s32 p;
    s32 flag;
    s32 primIndex;
    s32 mid01;
    s32 mid23;
    s32 idx;
    u16 clut;
    s16 x;
    s16 y;
    VECTOR* transVectorPtr;
    Primitive* prim;

    g_GpuBuffers[0].draw.r0 = 8;
    g_GpuBuffers[0].draw.g0 = 0;
    g_GpuBuffers[0].draw.b0 = 0x38;
    g_GpuBuffers[1].draw.r0 = 8;
    g_GpuBuffers[1].draw.g0 = 0;
    g_GpuBuffers[1].draw.b0 = 0x38;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 19);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.stainedGlass.glassPrim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        for (; prim != NULL; prim = prim->next) {
            PGREY(prim, 0) = 0x80;
            prim->tpage = 0xF;
            prim->clut = 0xF;
            prim->u0 = prim->u2 = 0x23;
            prim->u1 = prim->u3 = 0x5C;
            prim->v0 = prim->v1 = 1;
            prim->v2 = prim->v3 = 0x3E;
            prim->priority = 0x54;
            prim->drawMode = DRAW_DEFAULT;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
        }
        for (prim = self->ext.stainedGlass.glassPrim, idx = 0; idx < 4;
             prim = prim->next, idx++) {
            prim->clut = 5;
            prim->u0 = prim->u2 = 0x81;
            prim->u1 = prim->u3 = 0xAE;
            prim->v0 = prim->v1 = 1;
            prim->v2 = prim->v3 = 0x7E;
            prim->priority = 0x50;
        }
        prim = self->ext.stainedGlass.glassPrim;
        PGREY(prim, 0) = 0x20;
        PGREY(prim, 1) = 0x30;
        PGREY(prim, 2) = 0x40;
        PGREY(prim, 3) = 0x30;
        prim = prim->next;
        prim->r0 = 0x10;
        prim->g0 = 0;
        prim->b0 = 0x38;
        prim->clut = PAL_FILL_WHITE;
        prim->priority = 0x51;
        prim->drawMode = DRAW_HIDE;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        prim = prim->next;
        PGREY(prim, 0) = 0x40;
        PGREY(prim, 1) = 0x30;
        PGREY(prim, 2) = 0x50;
        PGREY(prim, 3) = 0x30;
        prim = prim->next;
        prim->r0 = 0x10;
        prim->g0 = 0;
        prim->b0 = 0x38;
        prim->clut = PAL_FILL_WHITE;
        prim->priority = 0x51;
        prim->drawMode = DRAW_HIDE;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        prim = prim->next;
        prim->type = PRIM_G4;
        prim->r0 = 0x40;
        prim->g0 = 0x18;
        prim->b0 = 0x48;
        prim->g2 = 0x18;
        prim->r2 = 0xC8;
        prim->b2 = 0x68;
        prim->priority = 0x52;
        prim->drawMode = DRAW_COLORS | DRAW_UNK02;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r3);
        rect.x = 0;
        rect.y = 0x100;
        rect.w = 0x10;
        rect.h = 1;
        color.r = 8;
        color.g = 0;
        color.b = 0x38;
        StainedGlassBlendPalette(&rect, 0xF, 0x201, 0x10, &color);
        break;
    case 1:
        SetGeomScreen(0x400);
        SetGeomOffset(0x80, 0xA0);
        prim = self->ext.stainedGlass.glassPrim;
        transVectorPtr = D_us_80180C94;
        clut = 0x201;
        for (idx = 0; idx < 8; idx++, transVectorPtr++, clut += 2) {
            RotMatrix(&D_us_80180D24, &mtx);
            RotMatrixZ(0x800, &mtx);
            transVector = *transVectorPtr;
            transVector.vx += self->posX.i.hi - 0x80;
            transVector.vz += 0x400;
            transVector.vy += self->posY.i.hi - 0xA0;
            TransMatrix(&mtx, &transVector);
            SetRotMatrix(&mtx);
            SetTransMatrix(&mtx);
            if (idx == 0) {
                RotTransPers(&D_us_80180C8C, &sxy[0], &p, &flag);
                prim->drawMode = DRAW_COLORS;
                x = sxy[0];
                y = sxy[0] >> 16;
                prim->x0 = prim->x2 = x - 0x2D;
                prim->x1 = prim->x3 = x;
                prim->y0 = prim->y1 = y + 0x3E;
                prim->y2 = prim->y3 = y - 0x3E;
                prim = prim->next;
                prim->x0 = prim->x2 = x - 0x2D;
                prim->x1 = prim->x3 = x;
                prim->y0 = prim->y1 = y + 0x3E;
                prim->y2 = prim->y3 = y - 0x3E;
                prim->drawMode = DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
                prim = prim->next;
                prim->x0 = prim->x2 = x + 0x2D;
                prim->x1 = prim->x3 = x;
                prim->y0 = prim->y1 = y + 0x3E;
                prim->y2 = prim->y3 = y - 0x3E;
                prim->drawMode = DRAW_COLORS;
                prim = prim->next;
                prim->x0 = prim->x2 = x + 0x2D;
                prim->x1 = prim->x3 = x;
                prim->y0 = prim->y1 = y + 0x3E;
                prim->y2 = prim->y3 = y - 0x3E;
                prim->drawMode = DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
                prim = prim->next;
                prim->x0 = x - 8;
                prim->x1 = x + 8;
                prim->x2 = self->posX.i.hi - 0x18;
                prim->y0 = prim->y1 = y - 0x38;
                prim->x3 = self->posX.i.hi + 0x18;
                prim->drawMode = DRAW_COLORS;
                prim->y2 = prim->y3 = self->posY.i.hi - 0xC2;
                prim = prim->next;
                continue;
            }
            RotTransPers4(&D_us_80180C6C, &D_us_80180C74, &D_us_80180C7C,
                          &D_us_80180C84, &sxy[0], &sxy[1], &sxy[2], &sxy[3],
                          &p, &flag);
            if ((s16)(sxy[2] >> 16) < 0xF1) {
                prim->clut = clut;
                prim->drawMode = DRAW_DEFAULT;
                mid01 = ((sxy[0] + sxy[1]) / 2) & 0xFFFF0000;
                mid01 |= ((s16)sxy[0] + (s16)sxy[1]) >> 1;
                LOW(prim->x0) = sxy[0];
                LOW(prim->x1) = mid01;
                LOW(prim->x2) = sxy[2];
                mid23 = ((sxy[2] + sxy[3]) / 2) & 0xFFFF0000;
                mid23 |= ((s16)sxy[2] + (s16)sxy[3]) >> 1;
                LOW(prim->x3) = mid23;
                prim = prim->next;
                prim->clut = clut;
                LOW(prim->x0) = sxy[1];
                LOW(prim->x1) = mid01;
                LOW(prim->x2) = sxy[3];
                LOW(prim->x3) = mid23;
                prim->drawMode = DRAW_DEFAULT;
                prim = prim->next;
            }
        }
        for (; prim != NULL; prim = prim->next) {
            prim->drawMode = DRAW_HIDE;
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s16 g_StainedGlassRecurseIndices[0x110] = {
    0,
    2,
    3,
    5,
    3,
    5,
    6,
    8,
    -0xD0,
    -0xC0,
    0,
    0,
    0xD0,
    -0xC0,
    0,
    0,
    -0xD0,
    0xC0,
    0,
    0,
    0xD0,
    0xC0,
    0,
    0,
    -0xD0,
    -0x60,
    0,
    0,
    0xD0,
    -0x60,
    0,
    0,
    -0xD0,
    0x60,
    0,
    0,
    0xD0,
    0x60,
    0,
    0,
    0x100,
    0,
    0,
    0,
    0x200,
    0,
    3,
    0,
    0x100,
    0,
    0,
    0,
    0x480,
    0,
    2,
    0,
    0x100,
    0,
    0,
    0,
    0x700,
    0,
    2,
    0,
    0x100,
    0,
    0,
    0,
    0x980,
    0,
    0,
    0,
    0x100,
    0,
    0,
    0,
    0xC00,
    0,
    0,
    0,
    0x100,
    0,
    0,
    0,
    0xE80,
    0,
    0,
    0,
    -0x100,
    -1,
    0,
    0,
    0x200,
    0,
    3,
    0,
    -0x100,
    -1,
    0,
    0,
    0x480,
    0,
    2,
    0,
    -0x100,
    -1,
    0,
    0,
    0x700,
    0,
    2,
    0,
    -0x100,
    -1,
    0,
    0,
    0x980,
    0,
    0,
    0,
    -0x100,
    -1,
    0,
    0,
    0xC00,
    0,
    0,
    0,
    -0x100,
    -1,
    0,
    0,
    0xE80,
    0,
    0,
    0,
    0x400,
    0x400,
    0x400,
    0x400,
    0x400,
    0x400,
    -0x400,
    -0x400,
    -0x400,
    -0x400,
    -0x400,
    -0x400,
    0x8080,
    0x2080,
    0x7878,
    0x2478,
    0x7070,
    0x2870,
    0x6868,
    0x3068,
    0x6060,
    0x3860,
    0x5858,
    0x4058,
    0x6060,
    0x460,
    0x5858,
    0x458,
    0x5050,
    0x450,
    0x4848,
    0x448,
    0x4040,
    0x440,
    0x3838,
    0x438,
    0,
    0,
    0,
    0,
    -0xC0,
    -0x40,
    0,
    0,
    0xC0,
    -0x40,
    0,
    0,
    -0xC0,
    0x40,
    0,
    0,
    0xC0,
    0x40,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x1000,
    0,
    0,
    0,
    0,
    0,
    0xF0,
    0,
    0x200,
    0,
    0,
    0,
    0,
    0,
    0xF0,
    0,
    0x400,
    0,
    0,
    0,
    0,
    0,
    0xF0,
    0,
    0x600,
    0,
    0,
    0,
    0,
    0,
    0xF0,
    0,
    0x800,
    0,
    0,
    0,
    0,
    0,
    0xF0,
    0,
    0xA00,
    0,
    0,
    0,
    0,
    0,
    0xF0,
    0,
    0xC00,
    0,
    0,
    0,
    0,
    0,
    0xF0,
    0,
    0xE00,
    0,
    0,
    0,
    0,
    0,
    0xF0,
    0,
    0x1000,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
SVECTOR D_us_80180B1C = { -0xD0, -0xC0, 0, 0 };
SVECTOR D_us_80180B24 = { 0xD0, -0xC0, 0, 0 };
SVECTOR D_us_80180B2C = { -0xD0, 0xC0, 0, 0 };
SVECTOR D_us_80180B34 = { 0xD0, 0xC0, 0, 0 };
SVECTOR D_us_80180B3C = { -0xD0, -0x60, 0, 0 };
SVECTOR D_us_80180B44 = { 0xD0, -0x60, 0, 0 };
SVECTOR D_us_80180B4C = { -0xD0, 0x60, 0, 0 };
SVECTOR D_us_80180B54 = { 0xD0, 0x60, 0, 0 };
VECTOR D_us_80180B5C[0xC] = {
    { 0x100, 0, 0x200, 3 },
    { 0x100, 0, 0x480, 2 },
    { 0x100, 0, 0x700, 2 },
    { 0x100, 0, 0x980, 0 },
    { 0x100, 0, 0xC00, 0 },
    { 0x100, 0, 0xE80, 0 },
    { -0x100, 0, 0x200, 3 },
    { -0x100, 0, 0x480, 2 },
    { -0x100, 0, 0x700, 2 },
    { -0x100, 0, 0x980, 0 },
    { -0x100, 0, 0xC00, 0 },
    { -0x100, 0, 0xE80, 0 },
};
s16 D_us_80180C1C[0xC] = {
    0x400,
    0x400,
    0x400,
    0x400,
    0x400,
    0x400,
    -0x400,
    -0x400,
    -0x400,
    -0x400,
    -0x400,
    -0x400,
};
CVECTOR D_us_80180C34[0xC] = {
    { 0x80, 0x80, 0x80, 0x20 },
    { 0x78, 0x78, 0x78, 0x24 },
    { 0x70, 0x70, 0x70, 0x28 },
    { 0x68, 0x68, 0x68, 0x30 },
    { 0x60, 0x60, 0x60, 0x38 },
    { 0x58, 0x58, 0x58, 0x40 },
    { 0x60, 0x60, 0x60, 4 },
    { 0x58, 0x58, 0x58, 4 },
    { 0x50, 0x50, 0x50, 4 },
    { 0x48, 0x48, 0x48, 4 },
    { 0x40, 0x40, 0x40, 4 },
    { 0x38, 0x38, 0x38, 4 },
};
SVECTOR D_us_80180C64 = { 0, 0, 0, 0 };
SVECTOR D_us_80180C6C = { -0xC0, -0x40, 0, 0 };
SVECTOR D_us_80180C74 = { 0xC0, -0x40, 0, 0 };
SVECTOR D_us_80180C7C = { -0xC0, 0x40, 0, 0 };
SVECTOR D_us_80180C84 = { 0xC0, 0x40, 0, 0 };
SVECTOR D_us_80180C8C = { 0, 0, 0, 0 };
VECTOR D_us_80180C94[9] = {
    { 0, 0, 0x1000, 0 },
    { 0, 0xF0, 0x200, 0 },
    { 0, 0xF0, 0x400, 0 },
    { 0, 0xF0, 0x600, 0 },
    { 0, 0xF0, 0x800, 0 },
    { 0, 0xF0, 0xA00, 0 },
    { 0, 0xF0, 0xC00, 0 },
    { 0, 0xF0, 0xE00, 0 },
    { 0, 0xF0, 0x1000, 0 },
};
SVECTOR D_us_80180D24 = { 0, 0, 0, 0 };
// end pc_import_data.py
// clang-format on
#endif
