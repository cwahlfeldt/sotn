// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/func_45aa0", func_us_801C5AA0);
#else
void func_us_801C4B2C(void);

// The Black Panther rises out of its shadow (step 9), turned toward Alucard,
// and flashes before it can be hit again. Done at step_s 7.
void func_us_801C5AA0(void) {
    Primitive* prim;
    s32 i;
    u8 facing;
    u8 v0, v2;

    switch (g_CurrentEntity->step_s) {
    case 0:
        facing = g_CurrentEntity->facingLeft;
        g_CurrentEntity->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        if (facing != g_CurrentEntity->facingLeft) {
            if (g_CurrentEntity->facingLeft) {
                g_CurrentEntity->posX.i.hi += 0x18;
            } else {
                g_CurrentEntity->posX.i.hi -= 0x18;
            }
        }
        v2 = 0xB6;
        v0 = 0xB5;
        prim = g_CurrentEntity->ext.blackPanther.prim;
        for (i = 0; i < 8; i++) {
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
            prim->y0 = prim->y1 = g_CurrentEntity->posY.i.hi + 8;
            prim->y2 = prim->y3 = prim->y0;
            prim->drawMode = DRAW_UNK02;
            prim->priority = g_CurrentEntity->zPriority + 2;
            prim = prim->next;
            v0 += 2;
        }
        func_us_801C4B2C();
        PlaySfxPositional(SFX_NOISE_SWEEP_DOWN_A);
        g_CurrentEntity->ext.blackPanther.subTimer = 8;
        g_CurrentEntity->step_s++;
        break;
    case 1:
        prim = g_CurrentEntity->ext.blackPanther.shadow;
        prim->y0 -= 2;
        prim->y1 -= 2;
        if (!--g_CurrentEntity->ext.blackPanther.subTimer) {
            g_CurrentEntity->ext.blackPanther.subTimer = 0x10;
            g_CurrentEntity->step_s++;
        }
        break;
    case 2:
        prim = g_CurrentEntity->ext.blackPanther.shadow;
        prim->y0--;
        prim->y1--;
        prim = g_CurrentEntity->ext.blackPanther.prim;
        prim->y0 -= 4;
        prim->y1 -= 4;
        func_us_801C4B2C();
        if (!--g_CurrentEntity->ext.blackPanther.subTimer) {
            g_CurrentEntity->ext.blackPanther.subTimer = 0xF;
            g_CurrentEntity->step_s++;
        }
        break;
    case 3:
        prim = g_CurrentEntity->ext.blackPanther.shadow;
        prim->y0 += 2;
        prim->y1 += 2;
        if (g_CurrentEntity->posY.i.hi < prim->y0) {
            prim->drawMode |= DRAW_HIDE;
        }
        prim = g_CurrentEntity->ext.blackPanther.prim;
        prim->y0 -= 2;
        prim->y1 -= 2;
        func_us_801C4B2C();
        if (!--g_CurrentEntity->ext.blackPanther.subTimer) {
            g_CurrentEntity->ext.blackPanther.subTimer = 0xF;
            g_CurrentEntity->step_s++;
        }
        break;
    case 4:
        prim = g_CurrentEntity->ext.blackPanther.shadow;
        prim->y0 += 2;
        prim->y1 += 2;
        if (g_CurrentEntity->posY.i.hi < prim->y0) {
            prim->drawMode |= DRAW_HIDE;
        }
        prim = g_CurrentEntity->ext.blackPanther.prim;
        prim->y0 += 2;
        prim->y1 += 2;
        func_us_801C4B2C();
        if (g_CurrentEntity->posY.i.hi - 6 < prim->y0) {
            g_CurrentEntity->step_s++;
        }
        break;
    case 5:
        // the sprite is back: flash white, the palette kept in hitEffect
        g_CurrentEntity->animCurFrame = 1;
        g_CurrentEntity->ext.blackPanther.flashTimer = 0x18;
        g_CurrentEntity->hitEffect = g_CurrentEntity->palette;
        g_CurrentEntity->palette = PAL_FLAG(0x164);
        g_api.FreePrimitives(g_CurrentEntity->primIndex);
        g_CurrentEntity->flags &= ~FLAG_HAS_PRIMS;
        g_CurrentEntity->step_s++;
        break;
    case 6:
        if (!--g_CurrentEntity->ext.blackPanther.flashTimer) {
            g_CurrentEntity->hitboxState = 3;
            g_CurrentEntity->step_s++;
            g_CurrentEntity->palette = g_CurrentEntity->hitEffect;
        }
        break;
    }
}
#endif
