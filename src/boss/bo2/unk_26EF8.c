// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo2.h"

extern u8 D_us_80180C28[];
extern u8 D_us_80180C30[];
extern u8 D_us_80180C50[];
extern u8 D_us_80180C8C[];
extern u8 D_us_80180C98[];
extern s32 D_us_80180AAC;
extern s32 D_us_80180B74;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_26EF8", func_us_801A6EF8);
#else
#define ITEM_AXE_LORD_ARMOR 0x19

// Takes control of the player for the cutscenes before (params 0) and after
// (params 1) the fight
void func_us_801A6EF8(Entity* self) {
    s16 playerX = PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitSpawner);
        self->animSet = 0;
        return;
    case 1:
        if (playerX <= 0x60 && !self->params) {
            return;
        }
        if (!self->params) {
            D_us_80180B74 = 1;
            PLAYER.posX.i.hi = 0x60;
        }
        g_PauseAllowed = false;
        g_unkGraphicsStruct.pauseEnemies = true;
        // leave any transformation
        if (g_Player.status & PLAYER_STATUS_BAT_FORM) {
            g_Player.padSim = PAD_R1;
        } else if (g_Player.status & PLAYER_STATUS_MIST_FORM) {
            g_Player.padSim = PAD_L1;
        } else if (g_Player.status & PLAYER_STATUS_WOLF_FORM) {
            g_Player.padSim = PAD_R2;
        } else {
            g_Player.padSim = 0;
        }
        g_Player.demo_timer = 1;
        if (self->params) {
            self->step = 6;
            return;
        }
        g_CutsceneFlags |= 1;
        self->step++;
        return;
    case 2:
        g_Player.padSim = 0;
        if (g_Player.status & PLAYER_STATUS_WOLF_FORM) {
            if (g_Timer & 1) {
                g_Player.padSim = PAD_R2;
            }
        } else if (g_CutsceneFlags & 0x80) {
            g_Player.padSim = PAD_RIGHT;
            self->step++;
        }
        g_Player.demo_timer = 1;
        break;
    case 3:
        if (playerX < 0xE8) {
            g_Player.padSim = PAD_RIGHT;
        } else {
            g_Player.padSim = 0;
            PLAYER.posX.i.hi = 0xE8 - g_Tilemap.scrollX.i.hi;
            if (g_Status.wornEquipment[1] != ITEM_AXE_LORD_ARMOR) {
                AnimateEntity(D_us_80180C8C, self);
                PLAYER.animCurFrame = self->animCurFrame;
            }
            self->step = 4;
        }
        g_Player.demo_timer = 1;
        break;
    case 4:
        if (g_Status.wornEquipment[1] != ITEM_AXE_LORD_ARMOR) {
            AnimateEntity(D_us_80180C8C, self);
            PLAYER.animCurFrame = self->animCurFrame;
        }
        g_Player.padSim = 0;
        g_Player.demo_timer = 1;
        if (g_CutsceneFlags & 0x40) {
            SetStep(5);
        }
        break;
    case 5:
        if (!AnimateEntity(D_us_80180C98, self)) {
            g_PauseAllowed = true;
            g_unkGraphicsStruct.pauseEnemies = false;
            D_us_80180AAC = 1;
            DestroyEntity(self);
            return;
        }
        if (g_Status.wornEquipment[1] != ITEM_AXE_LORD_ARMOR) {
            PLAYER.animCurFrame = self->animCurFrame;
        }
        g_Player.padSim = 0;
        g_Player.demo_timer = 1;
        break;
    case 6:
        // the button press for leaving wolf form is overwritten right after
        if ((g_Player.status & PLAYER_STATUS_WOLF_FORM) && (g_Timer & 1)) {
            g_Player.padSim = PAD_R2;
        }
        if (g_Player.vram_flag & 1) {
            g_CutsceneFlags |= 1;
            self->step++;
        }
        g_Player.padSim = 0;
        g_Player.demo_timer = 1;
        break;
    case 7:
        if ((g_Player.status & PLAYER_STATUS_WOLF_FORM) && (g_Timer & 1)) {
            g_Player.padSim = PAD_R2;
        }
        if (g_CutsceneFlags & 0x1000) {
            g_PauseAllowed = true;
            g_unkGraphicsStruct.pauseEnemies = false;
            DestroyEntity(self);
            return;
        }
        g_Player.padSim = 0;
        g_Player.demo_timer = 1;
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_26EF8", func_us_801A7340);
#else
// The figure that appears during the cutscene before the fight
void func_us_801A7340(Entity* self) {
    if (g_SkipCutscene && self->step >= 3 && self->step <= 5) {
        SetStep(6);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCommon);
        self->animSet = ANIMSET_OVL(5);
        self->animCurFrame = 1;
        self->unk5A = 0x5A;
        self->palette = 0x22A;
        self->drawFlags = ENTITY_OPACITY;
        self->opacity = 0;
        break;
    case 1:
        if (g_CutsceneFlags & 0x80) {
            SetStep(2);
        }
        break;
    case 2:
        if (self->opacity < 0x80) {
            self->opacity++;
        }
        AnimateEntity(D_us_80180C28, self);
        if (g_CutsceneFlags & 4) {
            self->animCurFrame = 1;
            SetStep(3);
        }
        break;
    case 3:
        if (g_CutsceneFlags & 8) {
            SetStep(4);
        }
        break;
    case 4:
        AnimateEntity(D_us_80180C30, self);
        if (g_CutsceneFlags & 0x20) {
            SetStep(5);
        }
        break;
    case 5:
        AnimateEntity(D_us_80180C50, self);
        if (g_CutsceneFlags & 0x100) {
            self->animCurFrame = 1;
            SetStep(6);
        }
        break;
    case 6:
        AnimateEntity(D_us_80180C28, self);
        if (self->opacity > 8) {
            self->opacity--;
            break;
        }
        self->step = 7;
        self->palette = 0x15F;
        self->blendMode = 0x50;
        self->opacity = 0x60;
        break;
    case 7:
        AnimateEntity(D_us_80180C28, self);
        if (self->opacity) {
            self->opacity -= 2;
            break;
        }
        DestroyEntity(self);
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_26EF8", func_us_801A75B4);
#else
// Debug: starts the cutscene after the fight with the second pad
void func_us_801A75B4(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitSpawner);
        break;
    case 1:
        if (g_pads[1].tapped & PAD_TRIANGLE) {
            CreateEntityFromCurrentEntity(E_CUTSCENE_DIALOGUE, &g_Entities[200]);
            g_Entities[200].params = 1;
            CreateEntityFromCurrentEntity(E_UNK_26, &g_Entities[96]);
            g_Entities[96].params = 1;
            self->step++;
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u8 D_us_80180C28[8] = { 7, 1, 7, 2, 0, 0, 0, 0 };
u8 D_us_80180C30[0x20] = {
    0x10,
    1,
    4,
    3,
    4,
    4,
    4,
    5,
    4,
    6,
    4,
    0xC,
    0x40,
    0xD,
    0x90,
    0xE,
    4,
    0xC,
    4,
    6,
    4,
    5,
    4,
    4,
    4,
    3,
    0x20,
    1,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180C50[0x3C] = {
    0x10,
    1,
    4,
    3,
    4,
    4,
    4,
    5,
    4,
    7,
    4,
    8,
    0x30,
    9,
    0x50,
    0xA,
    7,
    0xB,
    7,
    0xA,
    7,
    0xB,
    7,
    0xA,
    7,
    0xB,
    7,
    0xA,
    7,
    0xB,
    7,
    0xA,
    7,
    0xB,
    7,
    0xA,
    7,
    0xB,
    7,
    0xA,
    7,
    0xB,
    0x10,
    0xA,
    0xC,
    9,
    4,
    8,
    4,
    7,
    4,
    5,
    4,
    4,
    4,
    3,
    0x20,
    1,
    0xFF,
    0,
};
u8 D_us_80180C8C[0xC] = { 4, 0xBB, 4, 0xBC, 4, 0xBD, 4, 0xBF, 0xFF, 0, 0, 0 };
u8 D_us_80180C98[0xC] = { 4, 0xC0, 4, 0xC1, 4, 0xC2, 4, 0xC3, 0xFF, 0, 0, 0 };
// end pc_import_data.py
// clang-format on
#endif
