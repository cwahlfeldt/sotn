// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo5.h"

extern s32 D_us_801806DC;
extern s32 D_us_801B5538;
extern u32 g_CutsceneFlags;
extern EInit g_EInitSpawner;
extern EInit g_EInitObtainable;
extern u8 D_us_80180C24[];
extern u8 D_us_80180C48[];
extern u8 D_us_80180C5C[];
extern u8 D_us_80180C6C[];
extern u8 D_us_80180C74[];
extern u8 D_us_80180C84[];
extern u8 D_us_80180C90[];
extern u8 D_us_80180C98[];
extern u8 D_us_80180CA8[];
extern u8 D_us_80180CC0[];
extern u8 D_us_80180CCC[];
extern u8 D_us_80180CD4[];
extern s16 D_us_80180CF0[];
extern s16 D_us_80180D00[];
extern u8 D_us_80180D10[];
extern u8 D_us_80180D20[];
extern u16 D_us_80180D30[];
void CutsceneCameraPan(s16 target);

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_25F88", func_us_801A5F88);
#else
// Once the Hippogryph's reward is picked up, walks Alucard to the middle
// of the room for Maria's cutscene
void func_us_801A5F88(Entity* self) {
    Entity* entity;
    s16 x;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitSpawner);
        return;
    case 1:
        switch (self->step_s) {
        case 0:
            for (entity = &g_Entities[160]; entity < &g_Entities[192];
                 entity++) {
                if (entity->entityId == E_PRIZE_DROP) {
                    self->ext.bo5RewardWatch.prizeDrop = entity;
                    self->step_s++;
                }
            }
            return;
        case 1:
            if (self->ext.bo5RewardWatch.prizeDrop->entityId != E_PRIZE_DROP) {
                D_us_801806DC = 0;
                self->step_s++;
                g_CutsceneFlags |= 0x8000;
            }
            return;
        case 2:
            SetStep(2);
            return;
        }
        return;
    case 2:
        g_PauseAllowed = false;
        g_unkGraphicsStruct.pauseEnemies = true;
        g_Player.padSim = PAD_RIGHT;
        if (g_Player.status & PLAYER_STATUS_WOLF_FORM) {
            g_Player.padSim = PAD_R2;
        }
        g_Player.demo_timer = 1;
        self->step++;
        return;
    case 3:
        g_Player.padSim = 0;
        if (g_Player.status & PLAYER_STATUS_WOLF_FORM) {
            if (g_Timer & 1) {
                g_Player.padSim = PAD_R2;
            }
        } else {
            g_Player.padSim = PAD_RIGHT;
            x = PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi;
            if (x > 0x120 && !(g_CutsceneFlags & 1)) {
                g_CutsceneFlags |= 1;
            }
            if (x > 0x140) {
                g_Player.padSim = 0;
                PLAYER.posX.i.hi = 0x140 - g_Tilemap.scrollX.i.hi;
                self->step++;
            }
            if (x < 0x100) {
                x = 0x100;
            }
            CutsceneCameraPan(x);
        }
        g_Player.demo_timer = 1;
        break;
    case 4:
        CutsceneCameraPan(0x140);
        if (g_CutsceneFlags & 2) {
            self->step++;
        }
        g_Player.padSim = 0;
        g_Player.demo_timer = 1;
        break;
    case 5:
        CutsceneCameraPan(0x100);
        if (g_unkGraphicsStruct.unk14 == 0x80) {
            g_PauseAllowed = true;
            if (g_unkGraphicsStruct.pauseEnemies) {
                g_unkGraphicsStruct.pauseEnemies = false;
            }
            DestroyEntity(self);
        }
        g_Player.padSim = 0;
        g_Player.demo_timer = 1;
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_25F88", func_us_801A62B4);
#else
// Maria, acting out the cutscene one flag at a time
void func_us_801A62B4(Entity* self) {
    if (D_us_801B5538 && self->step < 0xD) {
        SetStep(0xD);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitObtainable);
        self->animSet = ANIMSET_OVL(4);
        self->animCurFrame = 1;
        self->unk5A = 0x50;
        self->palette = 0x220;
        self->hitboxState = 0;
        self->posX.i.hi = 0x1C0 - g_Tilemap.scrollX.i.hi;
        return;
    case 1:
        if (g_CutsceneFlags & 4) {
            self->step++;
        }
        return;
    case 2:
        self->animCurFrame = 2;
        if (g_CutsceneFlags & 8) {
            self->step++;
        }
        return;
    case 3:
        AnimateEntity(D_us_80180C24, self);
        if (g_CutsceneFlags & 0x10) {
            SetStep(4);
        }
        break;
    case 4:
        AnimateEntity(D_us_80180C48, self);
        if (g_CutsceneFlags & 0x20) {
            SetStep(5);
        }
        break;
    case 5:
        AnimateEntity(D_us_80180C5C, self);
        if (g_CutsceneFlags & 0x40) {
            SetStep(6);
        }
        break;
    case 6:
        AnimateEntity(D_us_80180C6C, self);
        if (g_CutsceneFlags & 0x80) {
            SetStep(7);
        }
        break;
    case 7:
        AnimateEntity(D_us_80180C74, self);
        if (g_CutsceneFlags & 0x100) {
            SetStep(8);
        }
        break;
    case 8:
        AnimateEntity(D_us_80180C84, self);
        if (g_CutsceneFlags & 0x200) {
            SetStep(9);
        }
        break;
    case 9:
        AnimateEntity(D_us_80180C90, self);
        if (g_CutsceneFlags & 0x400) {
            SetStep(0xA);
        }
        break;
    case 10:
        AnimateEntity(D_us_80180C98, self);
        if (g_CutsceneFlags & 0x800) {
            SetStep(0xB);
        }
        break;
    case 11:
        AnimateEntity(D_us_80180CA8, self);
        if (g_CutsceneFlags & 0x1000) {
            SetStep(0xC);
        }
        break;
    case 12:
        AnimateEntity(D_us_80180CC0, self);
        if (g_CutsceneFlags & 0x2000) {
            SetStep(0xD);
        }
        break;
    case 13:
        if (!AnimateEntity(D_us_80180CCC, self)) {
            self->velocityX = FIX(0.75);
            SetStep(0xE);
        }
        break;
    case 14:
        AnimateEntity(D_us_80180CD4, self);
        MoveEntity();
        if (self->posX.i.hi > 0x110) {
            g_CutsceneFlags |= 2;
            DestroyEntity(self);
        }
        break;
    }
}
#endif

void func_us_801A666C(Primitive* prim, s16 arg1) {
    prim->x0 = prim->x2 -= arg1;
    prim->x1 = prim->x3 += arg1;
    prim->y0 = prim->y1 -= arg1;
    prim->y2 = prim->y3 += arg1;
}

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_25F88", func_us_801A66B0);
#else
// A four part image that unfolds, stays up for a while and folds away.
// kind 0 and 1 pick its positions, UVs and cluts
void func_us_801A66B0(Entity* self, s32 kind) {
    Primitive* prim;
    s16* pos;
    u8* uv;
    s32 primIndex;
    s32 i;

    if (D_us_801B5538) {
        DestroyEntity(self);
    }
    switch (self->step) {
    case 0:
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 4);
        if (primIndex == -1) {
            return;
        }
        InitializeEntity(g_EInitSpawner);
        prim = &g_PrimBuf[primIndex];
        self->animSet = ANIMSET_OVL(0);
        self->primIndex = primIndex;
        self->flags |= FLAG_HAS_PRIMS;
        if (!(kind & 0xFFFF)) {
            pos = D_us_80180CF0;
            uv = D_us_80180D10;
        } else {
            pos = D_us_80180D00;
            uv = D_us_80180D20;
        }
        for (i = 0; i < 4; i++) {
            prim->tpage = 0x14;
            prim->clut = D_us_80180D30[(kind & 0xFFFF) * 4 + i];
            prim->x0 = prim->x1 = prim->x2 = prim->x3 = pos[0];
            prim->y0 = prim->y1 = prim->y2 = prim->y3 = pos[1];
            prim->u0 = prim->u2 = uv[0];
            prim->v0 = prim->v1 = uv[1];
            prim->u1 = prim->u3 = uv[2];
            prim->v2 = prim->v3 = uv[3];
            pos += 2;
            uv += 4;
            prim->drawMode = DRAW_DEFAULT;
            prim->priority = self->zPriority;
            prim = prim->next;
        }
        self->ext.ILLEGAL.u16[0] = 0;
        return;
    // grow the parts one after the other, then fold them back
    case 1:
        if (self->ext.ILLEGAL.u16[0] & 1) {
            func_us_801A666C(&g_PrimBuf[self->primIndex], 1);
        }
        break;
    case 2:
        if (self->ext.ILLEGAL.u16[0] & 1) {
            func_us_801A666C(g_PrimBuf[self->primIndex].next, 1);
        }
        break;
    case 3:
        func_us_801A666C(g_PrimBuf[self->primIndex].next->next, 1);
        break;
    case 4:
        prim = g_PrimBuf[self->primIndex].next->next->next;
        func_us_801A666C(prim, 2);
        if (self->ext.ILLEGAL.u16[0] & 1) {
            prim->y0 = prim->y1 = prim->y1 - 1;
            prim->y2 = prim->y3 = prim->y3 + 1;
        }
        break;
    case 5:
        if (++self->ext.ILLEGAL.u16[0] >= 0xC0) {
            self->ext.ILLEGAL.u16[0] = 0;
            self->step++;
        }
        return;
    case 6:
        prim = g_PrimBuf[self->primIndex].next->next->next;
        func_us_801A666C(prim, -2);
        if (self->ext.ILLEGAL.u16[0] & 1) {
            prim->y0 = prim->y1 = prim->y1 + 1;
            prim->y2 = prim->y3 = prim->y3 - 1;
        }
        break;
    case 7:
        func_us_801A666C(g_PrimBuf[self->primIndex].next->next, -1);
        break;
    case 8:
        if (self->ext.ILLEGAL.u16[0] & 1) {
            func_us_801A666C(g_PrimBuf[self->primIndex].next, -1);
        }
        break;
    case 9:
        if (self->ext.ILLEGAL.u16[0] & 1) {
            func_us_801A666C(&g_PrimBuf[self->primIndex], -1);
        }
        if (++self->ext.ILLEGAL.u16[0] >= 8) {
            DestroyEntity(self);
        }
        return;
    default:
        return;
    }
    if (++self->ext.ILLEGAL.u16[0] >= 8) {
        self->ext.ILLEGAL.u16[0] = 0;
        self->step++;
    }
}
#endif

void func_801B1D68(Entity* self) { func_us_801A66B0(self, 0); }

void BO5_RicSetDeadPrologue(Entity* self) { func_us_801A66B0(self, 1); }

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u8 D_us_80180C24[0x24] = {
    0x30,
    2,
    8,
    3,
    8,
    4,
    8,
    5,
    8,
    6,
    0x20,
    7,
    4,
    8,
    4,
    9,
    4,
    0xA,
    0x20,
    0xB,
    0x70,
    0xA,
    5,
    9,
    5,
    8,
    5,
    7,
    0x10,
    6,
    8,
    5,
    0x50,
    4,
    0xFF,
    0,
};
u8 D_us_80180C48[0x14] = {
    8,
    4,
    0xB,
    0xC,
    8,
    0xD,
    8,
    0xE,
    8,
    0xF,
    8,
    0x10,
    8,
    0x11,
    0x10,
    0x12,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180C5C[0x10] = { 1, 0x12, 2, 0x13, 2, 0x14, 6, 0x20, 6, 0x21, 0x50, 0x20, 0xFF, 0, 0, 0 };
u8 D_us_80180C6C[8] = { 8, 0x14, 0x40, 0x1B, 0xFF, 0, 0, 0 };
u8 D_us_80180C74[0x10] = {
    0x80,
    0x1C,
    0x30,
    0x1D,
    7,
    0x1E,
    7,
    0x1F,
    7,
    0x14,
    7,
    0x13,
    0x20,
    0x12,
    0xFF,
    0,
};
u8 D_us_80180C84[0xC] = { 7, 0x12, 7, 0x13, 0x14, 0x14, 7, 0x15, 0x40, 0x16, 0xFF, 0 };
u8 D_us_80180C90[8] = { 7, 0x17, 7, 0x18, 0x3E, 0x19, 0xFF, 0 };
u8 D_us_80180C98[0x10] = {
    5,
    0x19,
    5,
    0x18,
    0x50,
    0x17,
    5,
    0x18,
    0x30,
    0x19,
    0x30,
    0x1A,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180CA8[0x18] = {
    7,
    0x12,
    7,
    0x24,
    7,
    0x25,
    7,
    0x26,
    7,
    0x27,
    7,
    0x28,
    7,
    0xC,
    7,
    6,
    7,
    5,
    7,
    4,
    0x40,
    3,
    0xFF,
    0,
};
u8 D_us_80180CC0[0xC] = { 0x80, 2, 0x50, 3, 8, 0x29, 0x60, 0x2A, 0xFF, 0, 0, 0 };
u8 D_us_80180CCC[8] = { 1, 0x2A, 8, 0x2B, 0xFF, 0, 0, 0 };
u8 D_us_80180CD4[0x1C] = {
    2,
    0x2C,
    4,
    0x2D,
    5,
    0x2E,
    4,
    0x2F,
    4,
    0x30,
    4,
    0x31,
    4,
    0x32,
    4,
    0x33,
    5,
    0x34,
    4,
    0x35,
    4,
    0x36,
    4,
    0x37,
    2,
    0x2C,
    0,
    0,
};
s16 D_us_80180CF0[8] = { 0xAC, 0x78, 0xA8, 0x78, 0xA2, 0x7A, 0x92, 0x80 };
s16 D_us_80180D00[8] = { 0x50, 0xA0, 0x54, 0x9C, 0x5A, 0x9A, 0x6A, 0x9A };
u8 D_us_80180D10[0x10] = {
    0x58,
    0x40,
    0x60,
    0x48,
    0x50,
    0x40,
    0x58,
    0x48,
    0x50,
    0x30,
    0x60,
    0x40,
    0x60,
    0x58,
    0x80,
    0x80,
};
u8 D_us_80180D20[0x10] = {
    0x58,
    0x40,
    0x60,
    0x48,
    0x50,
    0x40,
    0x58,
    0x48,
    0x50,
    0x30,
    0x60,
    0x40,
    0x60,
    0x30,
    0x80,
    0x58,
};
u16 D_us_80180D30[8] = { 0x222, 0x222, 0x222, 0x221, 0x222, 0x222, 0x222, 0x222 };
// end pc_import_data.py
// clang-format on
#endif
