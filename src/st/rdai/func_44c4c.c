// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/func_44c4c", func_us_801C4C4C);
#else
void func_us_801C4B2C(void);
void UnkPolyFunc2(Primitive* prim);
void UnkPrimHelper(Primitive* prim);

// The Black Panther sinks into its shadow (step 7): its sprite, cut into
// eight slices, squashes into the floor while two glows rise and a spark
// flies up. Done at step_s 4, when only the shadow is left.
void func_us_801C4C4C(void) {
    Primitive* prim;
    Primitive* last;
    s32 primIndex;
    s32 i;
    s32 y;
    u8 v0, v2;

    switch (g_CurrentEntity->step_s) {
    case 0:
        g_CurrentEntity->hitboxState = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 20);
        if (primIndex == -1) {
            DestroyEntity(g_CurrentEntity);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        g_CurrentEntity->primIndex = primIndex;
        g_CurrentEntity->ext.blackPanther.prim = prim;
        g_CurrentEntity->flags |= FLAG_HAS_PRIMS;
        v0 = 0xB5;
        v2 = 0xB8;
        for (y = -7; y < 9; y += 2) {
            prim->tpage = 0x12;
            prim->clut = 0x164;
            if (g_CurrentEntity->facingLeft) {
                prim->u0 = 0x40;
                prim->u1 = 0;
                prim->u2 = 0x40;
                prim->u3 = 0;
            } else {
                prim->u0 = 0;
                prim->u1 = 0x40;
                prim->u2 = 0;
                prim->u3 = 0x40;
            }
            prim->v0 = prim->v1 = v0;
            prim->v2 = prim->v3 = v2;
            prim->r0 = prim->g0 = prim->b0 = 0x80;
            prim->r1 = prim->g1 = prim->b1 = 0x80;
            prim->r2 = prim->g2 = prim->b2 = 0x80;
            prim->r3 = prim->g3 = prim->b3 = 0x80;
            if (g_CurrentEntity->facingLeft) {
                prim->x0 = g_CurrentEntity->posX.i.hi - 0x30;
            } else {
                prim->x0 = g_CurrentEntity->posX.i.hi - 0x10;
            }
            prim->x1 = prim->x0 + 0x40;
            v2 += 2;
            prim->x2 = prim->x0;
            prim->x3 = prim->x1;
            prim->y0 = prim->y1 = g_CurrentEntity->posY.i.hi + y - 3;
            prim->y2 = prim->y3 = prim->y0 + 4;
            prim->drawMode = DRAW_UNK02;
            prim->priority = g_CurrentEntity->zPriority + 2;
            last = prim;
            prim = prim->next;
            v0 += 2;
        }
        g_CurrentEntity->ext.blackPanther.lastSlice = last;

        // a glow rising from the floor
        prim->tpage = 0x12;
        prim->u1 = prim->u3 = 0x40;
        prim->v0 = prim->v1 = 0xEE;
        prim->v2 = prim->v3 = 0xEF;
        prim->u0 = prim->u2 = 0;
        prim->r0 = prim->g0 = prim->b0 = 0x80;
        prim->r1 = prim->g1 = prim->b1 = 0x80;
        prim->r2 = prim->g2 = prim->b2 = 0x80;
        prim->r3 = prim->g3 = prim->b3 = 0x80;
        prim->clut = 0x164;
        if (g_CurrentEntity->facingLeft) {
            prim->x0 = g_CurrentEntity->posX.i.hi - 0x30;
        } else {
            prim->x0 = g_CurrentEntity->posX.i.hi - 0x10;
        }
        prim->x1 = prim->x0 + 0x40;
        prim->x2 = prim->x0;
        prim->x3 = prim->x1;
        prim->y0 = prim->y1 = g_CurrentEntity->posY.i.hi + 8;
        prim->y2 = prim->y3 = prim->y0;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
        prim->priority = g_CurrentEntity->zPriority + 3;
        g_CurrentEntity->ext.blackPanther.glowA = prim;

        // a second, taller glow
        prim = prim->next;
        prim->tpage = 0x12;
        prim->clut = 0x164;
        prim->u1 = prim->u3 = 0x7F;
        prim->v0 = prim->v1 = 0xC8;
        prim->v2 = prim->v3 = 0xC9;
        prim->u0 = prim->u2 = 0x40;
        prim->r0 = prim->g0 = prim->b0 = 0x80;
        prim->r1 = prim->g1 = prim->b1 = 0x80;
        prim->r2 = prim->g2 = prim->b2 = 0x80;
        prim->r3 = prim->g3 = prim->b3 = 0x80;
        if (g_CurrentEntity->facingLeft) {
            prim->x0 = g_CurrentEntity->posX.i.hi - 0x30;
        } else {
            prim->x0 = g_CurrentEntity->posX.i.hi - 0x10;
        }
        prim->x1 = prim->x0 + 0x40;
        prim->x2 = prim->x0;
        prim->x3 = prim->x1;
        prim->y0 = prim->y1 = g_CurrentEntity->posY.i.hi + 8;
        prim->y2 = prim->y3 = prim->y0;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
        prim->priority = g_CurrentEntity->zPriority + 2;
        g_CurrentEntity->ext.blackPanther.glowB = prim;

        // the spark, moved by UnkPrimHelper with its parameters in the next
        prim = prim->next;
        UnkPolyFunc2(prim);
        prim->tpage = 0x12;
        prim->u1 = prim->u3 = 0x64;
        prim->v0 = prim->v1 = 0xC0;
        prim->u0 = prim->u2 = 0x5C;
        prim->v2 = prim->v3 = 0xC8;
        prim->clut = 0x164;
        if (g_CurrentEntity->facingLeft) {
            prim->next->x1 = g_CurrentEntity->posX.i.hi - 0x12;
        } else {
            prim->next->x1 = g_CurrentEntity->posX.i.hi + 0x12;
        }
        prim->next->y0 = g_CurrentEntity->posY.i.hi - 0x28;
        LOW(prim->next->u0) = 0;
        LOW(prim->next->r1) = 0;
        LOH(prim->next->r2) = 8;
        LOH(prim->next->b2) = 8;
        prim->next->b3 = 0x80;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
        prim->priority = g_CurrentEntity->zPriority + 2;
        g_CurrentEntity->ext.blackPanther.spark = prim;

        // the shadow it becomes
        prim = prim->next->next;
        UnkPolyFunc2(prim);
        prim->tpage = 0x12;
        prim->clut = 0x164;
        if (g_CurrentEntity->facingLeft) {
            prim->u0 = 0x3C;
            prim->u1 = 0;
            prim->u2 = 0x3C;
            prim->u3 = 0;
        } else {
            prim->u0 = 0;
            prim->u1 = 0x3C;
            prim->u2 = 0;
            prim->u3 = 0x3C;
        }
        prim->v0 = prim->v1 = 0xD0;
        prim->v2 = prim->v3 = 0xDD;
        if (g_CurrentEntity->facingLeft) {
            prim->next->x1 = g_CurrentEntity->posX.i.hi - 0x12;
        } else {
            prim->next->x1 = g_CurrentEntity->posX.i.hi + 0x12;
        }
        prim->next->y0 = g_CurrentEntity->posY.i.hi + 2;
        LOW(prim->next->u0) = 0;
        LOW(prim->next->r1) = 0;
        LOH(prim->next->r2) = 2;
        LOH(prim->next->b2) = 1;
        prim->next->b3 = 0x80;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
        prim->priority = g_CurrentEntity->zPriority + 2;
        g_CurrentEntity->ext.blackPanther.shadow = prim;

        PlaySfxPositional(SFX_NOISE_SWEEP_DOWN_A);
        g_CurrentEntity->ext.blackPanther.timer = 0x10;
        g_CurrentEntity->animCurFrame = 0;
        g_CurrentEntity->step_s++;
        break;

    case 1:
        g_CurrentEntity->ext.blackPanther.timer--;
        g_CurrentEntity->ext.blackPanther.sliceStep = 0;
        if (!g_CurrentEntity->ext.blackPanther.timer) {
            g_CurrentEntity->ext.blackPanther.glowAStep = 0;
            g_CurrentEntity->ext.blackPanther.glowBStep = 0;
            g_CurrentEntity->ext.blackPanther.unk97 = 0;
            g_CurrentEntity->ext.blackPanther.flashTimer = 0;
            g_CurrentEntity->ext.blackPanther.subTimer = 0x24;
            g_CurrentEntity->ext.blackPanther.glowBTimer = 0x30;
            g_CurrentEntity->ext.blackPanther.blink = 0;
            g_CurrentEntity->step_s++;
        }
        break;

    case 2:
        g_CurrentEntity->ext.blackPanther.timer++;
        switch (g_CurrentEntity->ext.blackPanther.sliceStep) {
        case 0:
            // the slices narrow and their top sinks
            func_us_801C4B2C();
            prim = g_CurrentEntity->ext.blackPanther.prim;
            prim->y0 -= 3;
            prim->y1 -= 3;
            if (g_CurrentEntity->ext.blackPanther.timer & 1) {
                prim->x0++;
                prim->x1--;
            }
            if (prim->x1 - prim->x0 < 0x20) {
                g_CurrentEntity->ext.blackPanther.timer = 0xC;
                g_CurrentEntity->ext.blackPanther.sliceStep++;
            }
            prim = prim->next;
            for (i = 0; i < 6; i++) {
                prim->x2 += 2;
                prim->x3 -= 2;
                prim = prim->next;
            }
            break;
        case 1:
            func_us_801C4B2C();
            prim = g_CurrentEntity->ext.blackPanther.prim;
            prim->y0 += 6;
            prim->y1 += 6;
            prim = prim->next;
            for (i = 0; i < 6; i++) {
                prim->x2++;
                prim->x3--;
                prim = prim->next;
            }
            if (g_CurrentEntity->ext.blackPanther.timer & 1) {
                prim = g_CurrentEntity->ext.blackPanther.lastSlice;
                prim->x2++;
                prim->x3 -= 2;
            }
            prim = g_CurrentEntity->ext.blackPanther.prim;
            if (g_CurrentEntity->posY.i.hi + 8 < prim->y0) {
                for (i = 0; i < 8; i++) {
                    prim->drawMode |= DRAW_HIDE;
                    prim = prim->next;
                }
            }
            break;
        }

        switch (g_CurrentEntity->ext.blackPanther.glowAStep) {
        case 0:
            if (!--g_CurrentEntity->ext.blackPanther.subTimer) {
                prim = g_CurrentEntity->ext.blackPanther.glowA;
                prim->drawMode &= ~DRAW_HIDE;
                g_CurrentEntity->ext.blackPanther.subTimer = 0xF;
                g_CurrentEntity->ext.blackPanther.glowAStep++;
            }
            break;
        case 1:
            prim = g_CurrentEntity->ext.blackPanther.glowA;
            prim->v2 = prim->v3 = prim->v2 + 1;
            prim->y0 = prim->y1 = prim->y0 - 2;
            if (!(g_CurrentEntity->ext.blackPanther.timer & 3)) {
                prim->x0--;
                prim->x1++;
            }
            if (!--g_CurrentEntity->ext.blackPanther.subTimer) {
                g_CurrentEntity->ext.blackPanther.glowAStep++;
            }
            break;
        case 2:
            prim = g_CurrentEntity->ext.blackPanther.glowA;
            if (!(g_CurrentEntity->ext.blackPanther.timer % 3)) {
                prim->v2 = prim->v3 = prim->v2 - 1;
            }
            prim->y0 = prim->y1 = prim->y0 + 1;
            if (prim->y2 < prim->y0) {
                prim->drawMode |= DRAW_HIDE;
                g_CurrentEntity->ext.blackPanther.glowAStep = 8;
            }
            break;
        }

        switch (g_CurrentEntity->ext.blackPanther.glowBStep) {
        case 0:
            if (!--g_CurrentEntity->ext.blackPanther.glowBTimer) {
                prim = g_CurrentEntity->ext.blackPanther.glowB;
                prim->drawMode &= ~DRAW_HIDE;
                g_CurrentEntity->ext.blackPanther.glowBStep++;
            }
            break;
        case 1:
            prim = g_CurrentEntity->ext.blackPanther.glowB;
            prim->v2 = prim->v3 = prim->v2 + 4;
            prim->y0 = prim->y1 = prim->y0 - 4;
            if (prim->v2 >= 0xF9) {
                prim = g_CurrentEntity->ext.blackPanther.spark;
                prim->drawMode &= ~DRAW_HIDE;
                LOW(prim->next->r1) = FIX(-1);
                prim = g_CurrentEntity->ext.blackPanther.shadow;
                prim->drawMode &= ~DRAW_HIDE;
                g_CurrentEntity->ext.blackPanther.glowBStep++;
            }
            break;
        case 2:
            prim = g_CurrentEntity->ext.blackPanther.glowB;
            prim->y0 = prim->y1 = prim->y0 + 1;
            if (prim->y0 == prim->y3) {
                g_CurrentEntity->ext.blackPanther.timer = 1;
                g_CurrentEntity->ext.blackPanther.blink = 0;
                prim->drawMode |= DRAW_HIDE;
                g_CurrentEntity->step_s++;
            }
            prim = g_CurrentEntity->ext.blackPanther.spark;
            UnkPrimHelper(prim);
            LOW(prim->next->r1) += FIX(1.0 / 16);
            prim = g_CurrentEntity->ext.blackPanther.shadow;
            UnkPrimHelper(prim);
            if (LOH(prim->next->b2) < 0xD) {
                LOH(prim->next->b2)++;
            }
            if (LOH(prim->next->r2) < 0x3C) {
                LOH(prim->next->r2) += 2;
            }
            break;
        }
        break;

    case 3:
        // the shadow spreads while the spark falls back
        prim = g_CurrentEntity->ext.blackPanther.shadow;
        g_CurrentEntity->ext.blackPanther.timer++;
        UnkPrimHelper(prim);
        if (!(g_CurrentEntity->ext.blackPanther.timer & 3)) {
            g_CurrentEntity->ext.blackPanther.blink ^= 1;
        }
        if (LOH(prim->next->b2) < 0xD) {
            LOH(prim->next->b2)++;
        }
        if (LOH(prim->next->r2) < 0x3C) {
            LOH(prim->next->r2)++;
        }
        prim = g_CurrentEntity->ext.blackPanther.spark;
        UnkPrimHelper(prim);
        LOW(prim->next->r1) += FIX(1.0 / 16);
        if (g_CurrentEntity->posY.i.hi + 6 < prim->next->y0) {
            prim->drawMode |= DRAW_HIDE;
            g_CurrentEntity->step_s++;
        }
        break;
    }
}
#endif
