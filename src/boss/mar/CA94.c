// SPDX-License-Identifier: AGPL-3.0-or-later
#include "mar.h"

extern EInit g_EInitSpawner;
extern u8 D_us_80180690[];
extern u8 D_us_801806A4[];
extern u8 D_us_801806B8[];
extern u8 D_us_801806CC[];
extern u8 D_us_801806E4[];
extern u8 D_us_801806EC[];
extern u8 D_us_80180700[];
extern u8 D_us_80180718[];
extern u8 D_us_80180724[];
extern u8 D_us_80180738[];
extern u32 g_CutsceneFlags;
extern s32 g_SkipCutscene;

void func_us_8018CA94(Entity* self) {
    Entity* player;
    Tilemap* tilemap;
    s32 pan;

    // n.b.! unused, required for PSP
    tilemap = &g_Tilemap;
    player = &PLAYER;

    if (g_SkipCutscene && self->step < 12) {
        SetStep(12);
    }

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitSpawner);
        self->animSet = ANIMSET_OVL(2);
        self->animCurFrame = 1;
        self->unk5A = 0x48;
        self->palette = 0x210;
        break;

    case 1:
        if (g_CutsceneFlags & 0x4) {
            self->animCurFrame = 2;
            self->step++;
        }
        break;

    case 2:
        if (g_CutsceneFlags & 0x8) {
            SetStep(3);
        }
        break;

    case 3:
        AnimateEntity(D_us_80180690, self);
        if (g_CutsceneFlags & 0x10) {
            SetStep(4);
        }
        break;

    case 4:
        AnimateEntity(D_us_801806A4, self);
        if (g_CutsceneFlags & 0x20) {
            SetStep(5);
        }
        break;

    case 5:
        AnimateEntity(D_us_801806B8, self);
        if (g_CutsceneFlags & 0x40) {
            SetStep(6);
        }
        break;

    case 6:
        self->animCurFrame = 19;
        if (g_CutsceneFlags & 0x80) {
            self->step++;
        }
        break;

    case 7:
        self->animCurFrame = 0x12;
        if (g_CutsceneFlags & 0x100) {
            self->step++;
        }
        break;

    case 8:
        AnimateEntity(D_us_801806CC, self);
        if (g_CutsceneFlags & 0x200) {
            SetStep(9);
        }
        break;

    case 9:
        AnimateEntity(D_us_801806E4, self);
        if (g_CutsceneFlags & 0x400) {
            SetStep(10);
        }
        break;

    case 10:
        AnimateEntity(D_us_801806EC, self);
        if (g_CutsceneFlags & 0x800) {
            SetStep(11);
        }
        break;

    case 11:
        AnimateEntity(D_us_80180700, self);
        if (g_CutsceneFlags & 0x1000) {
            SetStep(12);
        }
        break;

    case 12:
        if (!AnimateEntity(D_us_80180718, self)) {
            SetStep(13);
            self->velocityX = FIX(1.5);
        }
        break;

    case 13:
        pan = AnimateEntity(D_us_80180724, self);
        if (pan & 0x80 && (self->pose == 3 || self->pose == 7)) {

            pan = (self->posX.i.hi - 120) / 16;

            if (pan < -8) {
                pan = -8;
            }
            if (pan > 8) {
                pan = 8;
            }

            g_api.PlaySfxVolPan(SFX_STOMP_SOFT_B, 80, pan);
        }

        MoveEntity();

        if (self->posX.i.hi > 184) {
            SetStep(14);
            self->velocityY = FIX(-4.0);
        }

        break;

    case 14:
        AnimateEntity(D_us_80180738, self);
        self->velocityY += FIX(0.1875);
        MoveEntity();

        if (self->velocityY > 0 && self->posY.i.hi > 0x83) {
            self->velocityY = 0;
            SetStep(15);
        }

        break;

    case 15:
        pan = AnimateEntity(D_us_80180724, self);
        if (pan & 0x80 && (self->pose == 3 || self->pose == 7)) {

            pan = (self->posX.i.hi - 120) / 16;

            if (pan < -8) {
                pan = -8;
            }
            if (pan > 8) {
                pan = 8;
            }

            g_api.PlaySfxVolPan(SFX_STOMP_SOFT_B, 80, pan);
        }

        MoveEntity();

        if (self->posX.i.hi > 256) {
            g_CutsceneFlags |= 0x2000;
        }

        if (self->posX.i.hi > 272) {
            DestroyEntity(self);
        }

        break;
    }
}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u8 D_us_80180690[0x14] = { 1, 2, 7, 3, 7, 4, 7, 5, 7, 6, 7, 7, 7, 8, 7, 9, 0x10, 0xA, 0xFF, 0 };
u8 D_us_801806A4[0x14] = {
    1,
    0xA,
    6,
    0xB,
    6,
    0xC,
    6,
    0xD,
    0x10,
    0xE,
    0xA,
    0xF,
    0xA,
    0xE,
    0xA,
    0xF,
    0x20,
    0xE,
    0xFF,
    0,
};
u8 D_us_801806B8[0x14] = {
    1,
    0xE,
    4,
    0x10,
    4,
    0x11,
    0xB,
    0x12,
    0xFF,
    0,
    0,
    0,
    1,
    0x13,
    0xFF,
    0,
    1,
    0x12,
    0xFF,
    0,
};
u8 D_us_801806CC[0x18] = {
    0x20,
    0x12,
    3,
    0x11,
    3,
    0x10,
    3,
    0x14,
    3,
    0x15,
    0x40,
    0x16,
    6,
    0x15,
    5,
    0xD,
    5,
    0xC,
    5,
    0xB,
    0x20,
    0xA,
    0xFF,
    0,
};
u8 D_us_801806E4[8] = { 0x10, 0xA, 0x10, 0x17, 8, 0xA, 0xFF, 0 };
u8 D_us_801806EC[0x14] = {
    1,
    0xA,
    2,
    0xB,
    2,
    0xC,
    6,
    0x18,
    6,
    0x19,
    0x50,
    0x18,
    3,
    0xC,
    3,
    0xB,
    8,
    0xA,
    0xFF,
    0,
};
u8 D_us_80180700[0x18] = {
    1,
    0xA,
    3,
    0xB,
    3,
    0xC,
    3,
    0xD,
    3,
    0xE,
    6,
    0x1B,
    0x10,
    0x1A,
    0x20,
    0x1B,
    3,
    0xE,
    3,
    0xD,
    3,
    0xC,
    0xFF,
    0,
};
u8 D_us_80180718[0xC] = { 4, 0x1C, 4, 0x1D, 4, 0x1E, 4, 0x1F, 0xFF, 0, 0, 0 };
u8 D_us_80180724[0x14] = {
    2,
    0x20,
    4,
    0x21,
    4,
    0x22,
    4,
    0x23,
    4,
    0x24,
    4,
    0x25,
    4,
    0x26,
    4,
    0x27,
    2,
    0x20,
    0,
    0,
};
u8 D_us_80180738[0xC] = { 8, 0x28, 8, 0x29, 0x60, 0x2A, 0xFF, 0, 1, 0xC, 0xFF, 0 };
// end pc_import_data.py
// clang-format on
#endif
