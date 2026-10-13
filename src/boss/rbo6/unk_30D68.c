// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo6.h"
#include <scratchpad.h>

// a face of the icosahedron
typedef struct {
    SVECTOR* v[3];
} DraculaCoreFace;

// where a particle (0x30) is spawned and its direction
typedef struct {
    s16 x;
    s16 y;
    s16 rotate;
} DraculaCoreSpark;

extern u16 g_EInitInteractable;
extern u32 g_CutsceneFlags;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_30D68", EntityBackgroundVortex);
#else
// ST0's swirling background, fading in from cutscene flag 0x20
void EntityBackgroundVortex(Entity* self) {
    static SVECTOR D_us_801B3634[0x100];
    static u8 D_us_801B3E34[0x100];
    u8 brightness;
    u8 leftV;
    u8 rightV;
    s32 primIndex;
    s32 scale;
    s32 xSize;
    s32 ySize;
    s32 zSize;
    SVECTOR rotVector;
    VECTOR transVector;
    MATRIX m;
    DRAWENV draw_env;
    DR_ENV* dr_env;
    RECT tw_rect;
    CVECTOR unused_Cvector;
    s32 size;
    s16 angle;
    s32 vectorIndex1;
    s32 vectorIndex2;
    s32 j;
    CVECTOR* scratchColor;
    s32 var_s2;
    s32 i;
    Primitive* prim;

    switch (self->step) {
    case 0:
        InitializeEntity(&g_EInitInteractable);
        self->posX.i.hi = 0x80;
        self->posY.i.hi = 0x1A0;
        self->opacity = 0;
        primIndex = g_api.func_800EDB58(PRIM_GT4, 0x110);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.bgVortex.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->ext.bgVortex.unk9C = 0x5F0;
        self->ext.bgVortex.unk9E = 0xB0;
        self->ext.bgVortex.unkA0 = 0;
        self->ext.bgVortex.unkAE = 0xD0;
        // fallthrough
    case 1:
        size = 0x60;
        brightness = 0x10;
        var_s2 = 0;
        angle = 0;
        for (j = 0; j < 16; j++) {
            scale = SquareRoot0(j * 0x120) * 4;
            for (i = 0; i < 16; i++) {
                xSize = (size * rcos(angle + i * 0x100)) >> 0xC;
                ySize = (size * rsin(angle + i * 0x100)) >> 0xC;
                zSize = scale;
                D_us_801B3634[var_s2].vx = xSize;
                D_us_801B3634[var_s2].vy = ySize;
                D_us_801B3634[var_s2].vz = zSize;
                var_s2++;
            }
            D_us_801B3E34[j] = brightness;
            angle += 0x80;
            size += 0x1C;
            brightness += 0x28;
            if (brightness > 0xA0) {
                brightness = 0xA0;
            }
        }
        self->step++;
        break;
    case 2:
        prim = self->ext.bgVortex.prim;
        while (prim != NULL) {
            prim->tpage = 0x16;
            prim->clut = PAL_UNK_19E;
            prim->u0 = prim->u2 = 0x40;
            prim->u1 = prim->u3 = 0x80;
            prim->v0 = prim->v1 = 0;
            prim->v2 = prim->v3 = 0x40;
            prim->priority = 0x80;
            prim->drawMode = DRAW_DEFAULT;
            prim = prim->next;
        }
        prim = self->ext.bgVortex.prim;
        draw_env = g_CurrentBuffer->draw;
        dr_env = g_api.func_800EDB08((POLY_GT4*)prim);
        if (dr_env == NULL) {
            DestroyEntity(self);
            return;
        }
        prim->type = PRIM_ENV;
        draw_env.isbg = 0;
        draw_env.r0 = 0;
        draw_env.g0 = 0;
        draw_env.b0 = 0;
        draw_env.dtd = 0;
        draw_env.clip.x = 0;
        draw_env.ofs[0] = 0;
        tw_rect.x = 0;
        tw_rect.y = 0;
        tw_rect.w = 0x40;
        tw_rect.h = 0x40;
        draw_env.tw = tw_rect;
        SetDrawEnv(dr_env, &draw_env);
        prim->priority = 2;
        prim->drawMode = DRAW_UNK_1000;
        prim = prim->next;
        dr_env = g_api.func_800EDB08((POLY_GT4*)prim);
        if (dr_env == NULL) {
            DestroyEntity(self);
            return;
        }
        prim->type = PRIM_ENV;
        prim->priority = 0x59;
        prim->drawMode = DRAW_UNK_800;
        self->step++;
        break;
    case 3:
        if ((g_CutsceneFlags & 0x20) && self->opacity < 0x80) {
            self->opacity++;
        }
        switch (self->step_s) {
        case 0:
            self->posY.val -= FIX(0.75);
            if (self->posY.i.hi < 0xA0) {
                self->posY.i.hi = 0xA0;
                self->step_s++;
            }
            break;
        case 1:
            self->ext.bgVortex.unkA4 += 16;
            self->ext.bgVortex.unkA6 += 8;
            self->ext.bgVortex.unkAA += 2;
            self->ext.bgVortex.unkA8 += 32;
            self->ext.bgVortex.unk9C =
                ((rsin(self->ext.bgVortex.unkA4) << 6) >> 0xC) + 0x5F0;
            self->ext.bgVortex.unk9E =
                ((rsin(self->ext.bgVortex.unkA6) << 7) >> 0xC) + 0xB0;
            self->ext.bgVortex.unkA0 = rsin(self->ext.bgVortex.unkA8) >> 8;
            self->ext.bgVortex.unkAE =
                ((rsin(self->ext.bgVortex.unkAA) * 24) >> 0xC) + 0xD0;
            break;
        }
        prim = self->ext.bgVortex.prim;
        prim = prim->next;
        prim = prim->next;
        leftV = prim->v0;
        rightV = prim->v2;
        leftV += 2;
        rightV += 2;
        if (rightV < leftV) {
            leftV = rightV;
            rightV = leftV + 0x40;
        }
        while (prim != NULL) {
            prim->v0 = prim->v1 = leftV;
            prim->v2 = prim->v3 = rightV;
            prim = prim->next;
        }
        self->ext.bgVortex.unk80 -= 4;
        SetGeomScreen(0xC0);
        SetGeomOffset(self->posX.i.hi, self->posY.i.hi);
        SetFarColor(0, 0, 0);
        SetFogNear(0x128, 0xC0);
        rotVector.vx = 0;
        rotVector.vy = 0;
        rotVector.vz = self->ext.bgVortex.unk80;
        rotVector.vx += self->ext.bgVortex.unk9C;
        rotVector.vy += self->ext.bgVortex.unk9E;
        rotVector.vz += self->ext.bgVortex.unkA0;
        RotMatrix(&rotVector, &m);
        SetRotMatrix(&m);
        transVector.vx = 0;
        transVector.vy = 0;
        transVector.vz = 0xC0;
        transVector.vz += self->ext.bgVortex.unkAE;
        TransMatrix(&m, &transVector);
        SetTransMatrix(&m);
        // the rings' colours, faded by the opacity
        scratchColor = (CVECTOR*)SP(0);
        for (i = 0; i < 16; i++, scratchColor++) {
            var_s2 = ((i & 1) * 2) + 1;
            scratchColor->b =
                ((D_us_801B3E34[i] / var_s2) * self->opacity) >> 7;
            scratchColor->r = scratchColor->g =
                (((D_us_801B3E34[i] >> 1) / var_s2) * self->opacity) >> 7;
            scratchColor->cd = 4;
        }
        prim = self->ext.bgVortex.prim;
        prim = prim->next;
        prim = prim->next;
        unused_Cvector.r = 0x80;
        unused_Cvector.g = 0x80;
        unused_Cvector.b = 0x80;
        unused_Cvector.cd = 4;
        scratchColor = (CVECTOR*)SP(0);
        for (i = 0; i < 15; i++) {
            for (j = 0; j < 16; j++) {
                var_s2 = i * 0x10;
                vectorIndex1 = var_s2 + j;
                vectorIndex2 = var_s2 + ((j + 1) & 0xF);
                gte_ldv0(&D_us_801B3634[vectorIndex1]);
                gte_rtps();
                gte_stsxy((long*)&prim->x0);
                gte_ldrgb(&scratchColor[i]);
                gte_dpcs();
                gte_strgb(&prim->r0);
                prim->type = PRIM_GT4;
                gte_ldv0(&D_us_801B3634[vectorIndex2]);
                gte_rtps();
                gte_stsxy((long*)&prim->x1);
                gte_ldrgb(&scratchColor[i]);
                gte_dpcs();
                if ((prim->y0 >= 0) || (prim->y1 >= 0)) {
                    gte_strgb(&prim->r1);
                    gte_ldv0(&D_us_801B3634[vectorIndex1] + 0x10);
                    gte_rtps();
                    if (prim->y0 <= 0x100 || prim->y1 <= 0x100) {
                        gte_stsxy((long*)&prim->x2);
                        gte_ldrgb(&scratchColor[i] + 1);
                        gte_dpcs();
                        gte_strgb(&prim->r2);
                        gte_ldv0(&D_us_801B3634[vectorIndex2] + 0x10);
                        gte_rtps();
                        gte_stsxy((long*)&prim->x3);
                        gte_ldrgb(&scratchColor[i] + 1);
                        gte_dpcs();
                        if (prim->x0 <= 0x100 || prim->x1 <= 0x100 ||
                            prim->x2 <= 0x100 || prim->x3 <= 0x100) {
                            gte_strgb(&prim->r3);
                            if (prim->x0 >= 0 || prim->x1 >= 0 ||
                                prim->x2 >= 0 || prim->x3 >= 0) {
                                gte_stszotz((long*)&zSize);
                                if (zSize < 0x55) {
                                    prim->priority = 0x58 - zSize;
                                } else {
                                    prim->priority = 3;
                                }
                                prim->drawMode = DRAW_COLORS;
                                prim = prim->next;
                            }
                        }
                    }
                }
            }
        }
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }
}
#endif

INCLUDE_RODATA("boss/rbo6/nonmatchings/unk_30D68", D_us_8019D0AC);

extern s32 D_us_801806E4;
extern s32 D_us_80181484;
extern s32 D_us_80181488;
extern SVECTOR D_us_8018148C[];
extern DraculaCoreFace D_us_8018150C[];
extern DraculaCoreSpark D_us_8018162C[];
extern SVECTOR D_us_801B3F34;
extern u16 D_us_801B3F3C;
extern u16 D_us_801B3F40;
extern u16 D_us_801B3F44;
extern s32 D_us_801B3F48;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_30D68", func_us_801B1738);
#else
// The room's centrepiece, drawn with the GTE: a glowing icosahedron and
// four ribbons of light that spiral around it, with sparks (0x30) rendered
// to an off-screen area. After Dracula dies it spins up, bursts and starts
// the ending (0x31)
void func_us_801B1738(Entity* self) {
    DRAWENV drawEnv;
    MATRIX m;
    MATRIX rot;
    MATRIX tmp;
    SVECTOR angle;
    SVECTOR ribbon[2];
    CVECTOR color;
    Entity* entity;
    Primitive* prim;
    DraculaCoreFace* face;
    DraculaCoreSpark* spark;
    SVECTOR* step;
    s16* ang;
    s32 i;
    s32 j;
    s32 n;
    s32 opz;
    s32 otz;
    s32 pri;
    s32 depth;
    s32 zs[7];
    u32 xy[7][2];
    u8 cols[7];
    u8 dim;
    s16 a;
    s16 r;
    s16 primIndex;
    u8 vTop;
    u8 vBottom;
    u8 u;

    FntPrint("d_step %x\n", self->step);
    switch (self->step) {
    case 0:
        InitializeEntity(&g_EInitInteractable);
        self->zPriority = 0x80;
        self->flags |= FLAG_UNK_10000;
        g_Tilemap.flags &= ~1;
        g_BgLayers[0].flags &= ~1;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x2E);
        if (primIndex == -1) {
            self->step = 0;
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->flags |= FLAG_HAS_PRIMS;
        if (g_api.func_800EDB08((POLY_GT4*)prim) == NULL) {
            g_api.FreePrimitives(primIndex);
            self->step = 0;
            self->flags &= ~FLAG_HAS_PRIMS;
            return;
        }
        prim->type = PRIM_ENV;
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
        g_api.func_800EDB08((POLY_GT4*)prim);
        if (prim == NULL) {
            g_api.FreePrimitives(primIndex);
            self->step = 0;
            self->flags &= ~FLAG_HAS_PRIMS;
            return;
        }
        prim->type = PRIM_ENV;
        prim->drawMode = DRAW_HIDE;
        // the icosahedron's twenty faces
        prim = prim->next;
        self->ext.rboDracBg.faces = prim;
        for (i = 0; i < 20; i++) {
            prim->type = PRIM_GT3;
            prim->tpage = 0x114;
            prim->clut = 0x194;
            prim->u0 = 0xC0;
            prim->v0 = 0xD;
            prim->u1 = 0x90;
            prim->v1 = 0x60;
            prim->u2 = 0xF0;
            prim->v2 = 0x60;
            prim->priority = 0x80;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        // the ribbons
        self->ext.rboDracBg.ribbons = prim;
        while (prim != NULL) {
            prim->priority = 0x80;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        for (i = 0; i < 4; i++) {
            self->ext.rboDracBg.ribbonAngle[i] = D_us_8018148C[i].pad;
        }
        self->scaleX = 0x100;
        D_us_801B3F3C = 0x100;
        D_us_801B3F40 = 0x200;
        D_us_801B3F44 = 0x700;
        self->ext.rboDracBg.spinX = 4;
        self->ext.rboDracBg.spinY = 8;
        D_us_801B3F48 = 0;
        self->rotate = 2;
        self->ext.rboDracBg.burst = 0;
        break;
    case 1:
        if (g_CutsceneFlags & 0x20) {
            D_us_80181484 = 1;
            D_us_801B3F48 = 0x10;
            self->step++;
        }
        break;
    case 2:
        if (++D_us_801B3F48 > 0x80) {
            D_us_801B3F48 = 0x80;
            self->step++;
        }
        break;
    case 3:
        if (D_us_801806E4 & 2) {
            self->step++;
        }
        break;
    case 4:
        // Dracula is dead: spin up and burst
        if (self->ext.rboDracBg.spinX < 0x18) {
            self->ext.rboDracBg.spinX += 2;
        }
        if (self->ext.rboDracBg.spinY < 0x18) {
            self->ext.rboDracBg.spinY += 2;
        }
        if (self->rotate < 0x200) {
            self->rotate += 2;
        }
        self->ext.rboDracBg.burst += 0x50;
        if (self->ext.rboDracBg.burst > 0x1200) {
            D_us_80181488 = 1;
            self->ext.rboDracBg.timer = 0x40;
            CreateEntityFromCurrentEntity(E_ID(UNK_31), &g_Entities[159]);
            self->step++;
        }
        break;
    case 5:
        if (D_us_801B3F48 < 0xA0) {
            D_us_801B3F48 += 2;
        }
        if (self->ext.rboDracBg.spinX > 0x10) {
            self->ext.rboDracBg.spinX -= 6;
        }
        if (self->ext.rboDracBg.spinY > 0x10) {
            self->ext.rboDracBg.spinY -= 6;
        }
        if (self->rotate > 0x10) {
            self->rotate -= 0xC;
        }
        if (!--self->ext.rboDracBg.timer) {
            self->step++;
        }
        break;
    case 6:
        D_us_801B3F48 -= 0xA;
        if (D_us_801B3F48 < 0x10) {
            D_us_801B3F48 = 0;
            self->ext.rboDracBg.timer = 0x60;
            self->step++;
        }
        break;
    case 7:
        prim = self->ext.rboDracBg.faces;
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        if (!--self->ext.rboDracBg.timer) {
            DestroyEntity(self);
        }
        return;
    default:
        break;
    }

    // the sparks are drawn to an off-screen area between these two
    // environments
    prim = &g_PrimBuf[self->primIndex];
    drawEnv = g_CurrentBuffer->draw;
    drawEnv.isbg = 1;
    drawEnv.r0 = 0;
    drawEnv.g0 = 0;
    drawEnv.b0 = 0;
    drawEnv.clip.x = 0x180;
    drawEnv.clip.y = 0x100;
    drawEnv.clip.w = 0x7F;
    drawEnv.clip.h = 0x80;
    drawEnv.ofs[0] = 0x180;
    drawEnv.ofs[1] = 0x100;
    SetDrawEnv(*(DR_ENV**)&prim->r1, &drawEnv);
    prim->priority = 0x27;
    prim->drawMode = DRAW_DEFAULT;
    prim = prim->next;
    n = g_Timer & 3;
    prim->priority = 0x29;
    prim->drawMode = DRAW_UNK_800;
    if (n < 3 && (rand() & 3)) {
        spark = &D_us_8018162C[n];
        entity = AllocEntity(&g_Entities[192], &g_Entities[219]);
        if (entity != NULL) {
            CreateEntityFromCurrentEntity(E_ID(UNK_30), entity);
            entity->zPriority = 0x28;
            entity->rotate = spark->rotate;
            if (D_us_80181488) {
                entity->rotate = entity->rotate - 0x100 + Random() * 2;
            } else {
                entity->rotate = entity->rotate - 0x80 + Random();
            }
            entity->posX.i.hi = spark->x;
            entity->posY.i.hi = spark->y;
        }
    }

    // the icosahedron
    D_us_801B3F34.vx += self->ext.rboDracBg.spinX;
    D_us_801B3F34.vy += self->ext.rboDracBg.spinY;
    D_us_801B3F34.vz += self->rotate;
    if (self->ext.rboDracBg.burst) {
        D_us_801B3F3C += 0xA;
        D_us_801B3F40 += 0x14;
        D_us_801B3F44 += 0xF;
    } else {
        D_us_801B3F3C += 2;
        D_us_801B3F40 += 4;
        D_us_801B3F44 += 3;
    }
    color.r = (u8)D_us_801B3F48 +
              (((D_us_801B3F48 / 2) * rcos((s16)D_us_801B3F3C)) >> 12);
    color.g = (u8)D_us_801B3F48 +
              (((D_us_801B3F48 / 2) * rsin((s16)D_us_801B3F40)) >> 12);
    color.cd = PRIM_GT3;
    color.b = (u8)D_us_801B3F48 +
              (((D_us_801B3F48 / 2) * rcos((s16)D_us_801B3F44)) >> 12);
    SetGeomScreen(0x600);
    SetGeomOffset(0x80, 0x80);
    SetFarColor(0, 0, 0);
    SetFogNear(0x200, 0x600);
    angle = D_us_801B3F34;
    RotMatrix(&angle, &m);
    SetRotMatrix(&m);
    gte_ldtr(0, 0, self->ext.rboDracBg.burst - 0x18);
    depth = (self->ext.rboDracBg.burst - 0x18) >> 2;
    face = D_us_8018150C;
    prim = self->ext.rboDracBg.faces;
    for (i = 0; i < 20; i++, face++) {
        gte_ldv3(face->v[0], face->v[1], face->v[2]);
        gte_rtpt();
        gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
        *(CVECTOR*)&prim->r0 = color;
        prim->drawMode = DRAW_COLORS | DRAW_UNK02;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        gte_nclip();
        gte_stopz(&opz);
        // the back faces only show once it bursts
        if (!self->ext.rboDracBg.burst && opz > 0) {
            prim->drawMode = DRAW_HIDE;
        }
        // as on PSX, three 0x100 bits never add up to more than 0x300
        if ((prim->x0 & 0x100) + (prim->x1 & 0x100) + (prim->x2 & 0x100) >
                0x300 ||
            (prim->y0 & 0x100) + (prim->y1 & 0x100) + (prim->y2 & 0x100) >
                0x300) {
            prim->drawMode = DRAW_HIDE;
        }
        gte_avsz3();
        gte_stszotz(&otz);
        prim->type = PRIM_GT3;
        pri = 0x80 - ((otz - depth) >> 2);
        if (!self->ext.rboDracBg.burst) {
            pri -= 2;
        }
        if (pri > 0x1F8) {
            pri = 0x1F8;
        }
        if (pri < 4) {
            pri = 4;
        }
        prim->priority = pri;
        prim = prim->next;
    }

    if (!D_us_80181484 || (D_us_801806E4 & 1)) {
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        return;
    }

    // the ribbons: each one rises from below, winding around the
    // icosahedron, and starts over
    ang = self->ext.rboDracBg.ribbonAngle;
    for (i = 0; i < 4; i++) {
        ang[i] += 0x38;
        if ((u16)(ang[i] - 1) < 0x38) {
            g_api.PlaySfx(0x614);
        }
        if (ang[i] > 0x2000) {
            ang[i] = -(ang[i] & 0xFFF);
        }
    }
    step = D_us_8018148C;
    prim = self->ext.rboDracBg.ribbons;
    SetGeomScreen(0x600);
    SetGeomOffset(0x80, 0x80);
    gte_ldtr(0, 0, 0x600);
    vTop = 0xC0;
    vBottom = 0xD0;
    ribbon[0].vx = 0;
    ribbon[0].vy = -12;
    ribbon[1].vx = 0;
    ribbon[1].vy = 12;
    for (i = 0; i < 4; i++, step++) {
        a = ang[i];
        if (a < 0) {
            continue;
        }
        angle.vx += step->vx;
        angle.vy += step->vy;
        angle.vz += step->vz;
        RotMatrix(&angle, &rot);
        dim = 0;
        for (j = 0; j < 7; j++) {
            r = 0x90 - (a * 120) / 6144;
            if (r < 0x18) {
                r = 0x18;
            }
            ribbon[0].vz = ribbon[1].vz = -r;
            m.m[0][0] = m.m[1][1] = m.m[2][2] = 0x1000;
            m.m[0][1] = m.m[0][2] = m.m[1][0] = 0;
            m.m[1][2] = m.m[2][0] = m.m[2][1] = 0;
            RotMatrixY(a, &m);
            // m = rot * m
            tmp = rot;
            MulMatrix(&tmp, &m);
            m = tmp;
            SetRotMatrix(&m);
            // as on PSX, only V0 and V1 are loaded: the depth comes from
            // whatever V2 the GTE still holds
            gte_ldv01c(ribbon);
            gte_rtpt();
            gte_stsxy01c(xy[j]);
            gte_stszotz(&zs[j]);
            if (a < 0) {
                if (++dim > 4) {
                    dim = 4;
                }
                cols[j] = -0x80 - (dim << 5);
            } else {
                dim = 0;
                cols[j] = 0x80;
            }
            a -= ratan2(0x10, r);
            if (a > 0x1800) {
                cols[j] = 0;
            }
        }
        for (j = 0, u = 0x90; j < 6; j++, u += 0x10) {
            prim->tpage = 0x1A;
            prim->u0 = prim->u2 = u;
            prim->u1 = prim->u3 = u + 0xF;
            prim->clut = 0x194;
            if (g_Timer & 2) {
                prim->v0 = prim->v1 = vTop;
                prim->v2 = prim->v3 = vBottom;
            } else {
                prim->v0 = prim->v1 = vBottom;
                prim->v2 = prim->v3 = vTop;
            }
            LOW(prim->x0) = xy[j][0];
            LOW(prim->x1) = xy[j + 1][0];
            LOW(prim->x2) = xy[j][1];
            LOW(prim->x3) = xy[j + 1][1];
            prim->drawMode = DRAW_UNK02;
            prim->priority = zs[j] - 0x100;
            if (cols[j + 1] != 0x80) {
                prim->r0 = prim->g0 = prim->b0 = cols[j];
                LOW(prim->r2) = LOW(prim->r0);
                prim->r1 = prim->g1 = prim->b1 = cols[j + 1];
                prim->drawMode |= DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                                  DRAW_TRANSP;
                LOW(prim->r3) = LOW(prim->r1);
            }
            prim = prim->next;
        }
    }
    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}
#endif

void func_us_801B2864(Entity* self) {
    extern u16 g_EInitInteractable;
    extern s32 D_us_80181488;
    extern u8 D_us_80181648;
    u16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(&g_EInitInteractable);
        self->palette = 0x2E5;
        self->animSet = 0xE;
        self->unk5A = 0x79;
        self->drawFlags = ENTITY_ROTATE;
        self->flags |= FLAG_UNK_10000;
        self->blendMode = BLEND_QUARTER | BLEND_TRANSP;
        self->flags &= ~FLAG_POS_CAMERA_LOCKED;
        self->facingLeft = Random() & 1;
        angle = self->rotate;
        if (self->facingLeft != 0) {
            self->rotate = -angle;
        }
        self->velocityX = rsin((s32)(s16)angle) * 0x28;
        self->velocityY = -(rcos((s32)(s16)angle) * 0x28);
        self->scaleX = 0xE0;
        self->drawFlags |= ENTITY_SCALEY | ENTITY_SCALEX;
        self->scaleY = 0x140;
        if (D_us_80181488 != 0) {
            self->scaleX = 0x120;
            self->scaleY = 0x120;
        }
        /* fall through */
    case 1:
        MoveEntity();
        if (AnimateEntity(&D_us_80181648, self) == 0) {
            DestroyEntity(self);
        }
        break;
    }
}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_801814AC[];
extern u8 D_us_801814B4[];
extern u8 D_us_801814BC[];
extern u8 D_us_801814C4[];
extern u8 D_us_801814CC[];
extern u8 D_us_801814D4[];
extern u8 D_us_801814DC[];
extern u8 D_us_801814E4[];
extern u8 D_us_801814EC[];
extern u8 D_us_801814F4[];
extern u8 D_us_801814FC[];
extern u8 D_us_80181504[];
u8 D_us_801814AC[] ALIGNED4 = {0x66, 0x00, 0xCA, 0xFF, 0xDF, 0xFF, 0x00, 0x00};
u8 D_us_801814B4[] ALIGNED4 = {0x3F, 0x00, 0x36, 0x00, 0xA9, 0xFF, 0x00, 0x00};
u8 D_us_801814BC[] ALIGNED4 = {0x00, 0x00, 0xCA, 0xFF, 0x94, 0xFF, 0x00, 0x00};
u8 D_us_801814C4[] ALIGNED4 = {0x00, 0x00, 0x88, 0xFF, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801814CC[] ALIGNED4 = {0x3F, 0x00, 0xCA, 0xFF, 0x57, 0x00, 0x00, 0x00};
u8 D_us_801814D4[] ALIGNED4 = {0x66, 0x00, 0x36, 0x00, 0x21, 0x00, 0x00, 0x00};
u8 D_us_801814DC[] ALIGNED4 = {0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801814E4[] ALIGNED4 = {0x00, 0x00, 0x36, 0x00, 0x6C, 0x00, 0x00, 0x00};
u8 D_us_801814EC[] ALIGNED4 = {0x9A, 0xFF, 0xCA, 0xFF, 0xDF, 0xFF, 0x00, 0x00};
u8 D_us_801814F4[] ALIGNED4 = {0xC1, 0xFF, 0x36, 0x00, 0xA9, 0xFF, 0x00, 0x00};
u8 D_us_801814FC[] ALIGNED4 = {0xC1, 0xFF, 0xCA, 0xFF, 0x57, 0x00, 0x00, 0x00};
u8 D_us_80181504[] ALIGNED4 = {0x9A, 0xFF, 0x36, 0x00, 0x21, 0x00, 0x00, 0x00};
s32 D_us_80181484 = 0;
s32 D_us_80181488 = 0;
SVECTOR D_us_8018148C[4] = {
    { 0x100, 0x200, 0x300, -0x40 },
    { 0, 0, 0, -0x1200 },
    { 0, 0, 0x400, 0xBA00 },
    { 0x200, 0x100, -0x200, 0x9700 },
};
DraculaCoreFace D_us_8018150C[0x18] = {
    { { &D_us_801814AC, &D_us_801814BC, &D_us_801814C4 } },
    { { &D_us_801814EC, &D_us_801814C4, &D_us_801814BC } },
    { { &D_us_801814EC, &D_us_801814BC, &D_us_801814F4 } },
    { { &D_us_801814EC, &D_us_801814FC, &D_us_801814C4 } },
    { { &D_us_801814AC, &D_us_801814C4, &D_us_801814CC } },
    { { &D_us_801814AC, &D_us_801814B4, &D_us_801814BC } },
    { { &D_us_801814CC, &D_us_801814D4, &D_us_801814AC } },
    { { &D_us_801814FC, &D_us_801814EC, &D_us_80181504 } },
    { { &D_us_801814B4, &D_us_801814AC, &D_us_801814D4 } },
    { { &D_us_801814C4, &D_us_801814FC, &D_us_801814CC } },
    { { &D_us_801814BC, &D_us_801814B4, &D_us_801814F4 } },
    { { &D_us_801814F4, &D_us_80181504, &D_us_801814EC } },
    { { &D_us_80181504, &D_us_801814F4, &D_us_801814DC } },
    { { &D_us_801814D4, &D_us_801814CC, &D_us_801814E4 } },
    { { &D_us_80181504, &D_us_801814E4, &D_us_801814FC } },
    { { &D_us_801814D4, &D_us_801814DC, &D_us_801814B4 } },
    { { &D_us_801814E4, &D_us_801814CC, &D_us_801814FC } },
    { { &D_us_801814DC, &D_us_801814F4, &D_us_801814B4 } },
    { { &D_us_801814D4, &D_us_801814E4, &D_us_801814DC } },
    { { &D_us_80181504, &D_us_801814DC, &D_us_801814E4 } },
    { { &D_us_801814FC, &D_us_801814E4, &D_us_801814E4 } },
    { { &D_us_801814B4, &D_us_801814DC, &D_us_801814DC } },
    { { &D_us_801814DC, &D_us_801814E4, &D_us_801814E4 } },
    { { &D_us_801814E4, &D_us_801814DC, &D_us_801814DC } },
};
DraculaCoreSpark D_us_8018162C[4] = {
    { 0x40, 9, 0x800 },
    { 0xC, 0x64, 0x2AB },
    { 0x74, 0x64, -0x2AB },
    { 0, 0, 0 },
};
// end pc_import_data.py
// clang-format on
#endif

#ifdef VERSION_PC
// rodata and bss
SVECTOR D_us_8019D0AC = {0};
SVECTOR D_us_801B3F34;
u16 D_us_801B3F3C;
u16 D_us_801B3F40;
u16 D_us_801B3F44;
s32 D_us_801B3F48;
#endif
