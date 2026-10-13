// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo2.h"

extern s32 D_us_80180B5C;
extern EInit D_us_801804F4;
extern Primitive* FindFirstUnkPrim(Primitive* prim);
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/e_debug_cerberus_gate", EntityCerberusGateDebug);
#else
// The arena gates, as in BO7: they close when the player walks in and open
// once the reward for defeating Death appears.
void EntityCerberusGateDebug(Entity* self) {
    enum Step {
        INIT = 0,
        WAIT_FOR_PLAYER = 1,
        DROP = 2,
        ADD_COLLISION = 3,
        WAIT_FOR_REWARD = 4,
        RAISE = 5,
        SETTLE = 6,
    };

    s32 tileIdx;
    s32 primIdx;
    Primitive* prim;
    Collider collider;
    Entity* entity;
    s32 i;
    s16 x;
    s16 y;

    switch (self->step) {
    case INIT:
        InitializeEntity(D_us_801804F4);
        self->zPriority = 0x6A;
        if (self->params) {
            tileIdx = 0xDF;
            self->animCurFrame = 6;
            x = 0x1FA;
        } else {
            tileIdx = 0xC0;
            self->animCurFrame = 5;
            x = 6;
        }
        self->posX.i.hi = x - g_Tilemap.scrollX.i.hi;
        for (i = 0; i < 4; tileIdx += 0x20, i++) {
            g_Tilemap.fg[tileIdx] = 0x32E;
        }
        primIdx = g_api.AllocPrimitives(PRIM_GT4, 16);
        if (primIdx == -1) {
            self->step = 0;
            return;
        }
        prim = &g_PrimBuf[primIdx];
        self->primIndex = primIdx;
        self->ext.debugCerberusGate.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;

        prim->tpage = 0xF;
        prim->clut = 0x1B;
        if (self->params) {
            prim->u0 = prim->u2 = 0xE3;
            prim->u1 = prim->u3 = 0xD3;
        } else {
            prim->u0 = prim->u2 = 0xD4;
            prim->u1 = prim->u3 = 0xE4;
        }
        prim->v0 = prim->v1 = 0x9C;
        prim->v2 = prim->v3 = 0xDC;
        prim->y0 = prim->y1 = 0x1C;
        prim->y2 = prim->y3 = 0x5C;
        prim->priority = 0x6B;
        prim->drawMode = DRAW_UNK02;
        x = (self->params ? 0x1F0 : 0) - g_Tilemap.scrollX.i.hi;
        prim->x0 = prim->x2 = x;
        prim->x1 = prim->x3 = x + 0x10;
        prim = prim->next;

        while (prim != NULL) {
            prim->type = PRIM_TILE;
            prim->u0 = prim->v0 = 1;
            prim->r0 = 0x20;
            prim->g0 = 0x40;
            prim->b0 = 0x20;
            prim->priority = 0x6C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        // fallthrough
    case WAIT_FOR_PLAYER:
        if (D_us_80180B5C & 1) {
            self->step++;
        }
        return;

    case DROP:
        self->posY.val += self->velocityY;
        self->velocityY += FIX(0.5);
        if (self->posY.i.hi <= 0x60) {
            break;
        }
        x = self->posX.i.hi;
        y = self->posY.i.hi + 0x22;
        g_api.CheckCollision(x, y, &collider, 0);
        if (collider.effects == EFFECT_NONE) {
            break;
        }
        self->posY.i.hi += collider.unk18;
        self->velocityY = -self->velocityY;
        self->velocityY /= 4;
        PlaySfxPositional(SFX_EXPLODE_B);
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_INTENSE_EXPLOSION, self, entity);
            entity->params = 0x10;
            entity->posX.i.hi += -8 + (Random() & 7) * 2;
            entity->posY.i.hi += 0x20;
        }
        if (self->velocityY < -0x1FFF) {
            return;
        }
        self->step++;
        break;

    case ADD_COLLISION:
        tileIdx = self->params ? 0xDF : 0xC0;
        for (i = 0; i < 4; tileIdx += 0x20, i++) {
            g_Tilemap.fg[tileIdx] = 0x3A1;
        }
        self->step++;
        break;

    case WAIT_FOR_REWARD:
        if (D_us_80180B5C & 0x80) {
            self->step++;
        }
        break;

    case RAISE:
        self->posY.val -= FIX(0.25);
        if (g_Timer & 1) {
            prim = FindFirstUnkPrim(self->ext.debugCerberusGate.prim->next);
            if (prim != NULL) {
                prim->p3 = 1;
                prim->y0 = 0x5C;
                prim->drawMode = DRAW_UNK02;
                prim->x0 = self->posX.i.hi + (Random() & 0xF) - 8;
            }
        }
        y = (0x7A - self->posY.i.hi) >> 4;
        tileIdx = self->params ? 0x13F : 0x120;
        for (i = 0; i < y; tileIdx -= 0x20, i++) {
            g_Tilemap.fg[tileIdx] = 0x32E;
        }
        self->ext.debugCerberusGate.timer = 0x20;
        if (self->posY.i.hi < 0x3A) {
            self->step++;
        }
        // fallthrough
    case SETTLE:
        prim = self->ext.debugCerberusGate.prim;
        while (prim != NULL) {
            if (prim->p3) {
                prim->y0 += 2;
                if (prim->y0 > 0xA0) {
                    prim->drawMode = DRAW_HIDE;
                    prim->p3 = 0;
                }
            }
            prim = prim->next;
        }
        if (!--self->ext.debugCerberusGate.timer) {
            self->step++;
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s32 D_us_80180B5C = 0;
// end pc_import_data.py
// clang-format on
#endif
