// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo6.h"

#ifdef VERSION_PC
s32 PrimDecreaseBrightness(Primitive* prim, u8 amount);
#endif

extern void EntityCutscene(Entity* self);
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A362C);
#else
// runs the conversation twice a frame while D_8003C73C is set
void func_us_801A362C(Entity* self) {
    if (D_8003C73C) {
        EntityCutscene(self);
    }
    if (self->entityId) {
        EntityCutscene(self);
    }
}
#endif

extern u32 g_CutsceneFlags;
extern EInit g_EInitInteractable;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A367C);
#else
// holds Alucard in place, kneeling, until the conversation lets go
void func_us_801A367C(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        g_PauseAllowed = false;
        g_unkGraphicsStruct.pauseEnemies = true;
        PLAYER.facingLeft = 0;
        if (g_Status.wornEquipment[1] != 0x19) {
            PLAYER.animCurFrame = 0xBF;
        }
        PLAYER.posX.i.hi = 0x70;
        break;
    case 1:
        if (g_Status.wornEquipment[1] != 0x19) {
            PLAYER.animCurFrame = 0xBF;
        }
        PLAYER.posX.i.hi = 0x70;
        if (g_CutsceneFlags & 2) {
            self->step++;
        }
        break;
    case 2:
        g_PauseAllowed = true;
        g_unkGraphicsStruct.pauseEnemies = false;
        g_Entities[E_AFTERIMAGE_1].ext.afterImage.disableFlag = 1;
        DestroyEntity(self);
        return;
    }
    g_Player.padSim = PAD_NONE;
    g_Player.demo_timer = 1;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A37B4);
#else
// the screen fades in from white, then cues the conversation
void func_us_801A37B4(Entity* self) {
    Primitive* prim;
    s16 primIndex;

    switch (self->step) {
    case 0:
        primIndex = g_api.AllocPrimitives(PRIM_G4, 1);
        if (primIndex != -1) {
            InitializeEntity(g_EInitInteractable);
            prim = &g_PrimBuf[primIndex];
            self->primIndex = primIndex;
            self->flags |= FLAG_HAS_PRIMS;
            prim->x1 = prim->x3 = 0x100;
            prim->y2 = prim->y3 = 0xF0;
            PCOL(prim) = 0xFF;
            prim->priority = 0x1F8;
            prim->x0 = prim->x2 = 0;
            prim->y0 = prim->y1 = 0;
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        }
        break;
    case 1:
        self->step++;
        break;
    case 2:
        prim = &g_PrimBuf[self->primIndex];
        PCOL(prim) = prim->b3 - 4;
        if (prim->r0 < 4) {
            DestroyEntity(self);
            g_CutsceneFlags |= 1;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A399C);
#else
// a white flash on cutscene flag 0x100 that fades out
void func_us_801A399C(Entity* self) {
    Primitive* prim;
    s16 primIndex;

    switch (self->step) {
    case 0:
        primIndex = g_api.AllocPrimitives(PRIM_G4, 1);
        if (primIndex != -1) {
            InitializeEntity(g_EInitInteractable);
            prim = &g_PrimBuf[primIndex];
            self->primIndex = primIndex;
            self->flags |= FLAG_HAS_PRIMS;
            prim->x1 = prim->x3 = 0x100;
            prim->y2 = prim->y3 = 0xF0;
            prim->x0 = prim->x2 = 0;
            prim->y0 = prim->y1 = 0;
            PCOL(prim) = 0;
            prim->priority = 0x80;
            prim->drawMode = DRAW_DEFAULT;
        }
        break;
    case 1:
        if (g_CutsceneFlags & 0x100) {
            prim = &g_PrimBuf[self->primIndex];
            PCOL(prim) = 0xFF;
            prim->drawMode =
                DRAW_UNK_40 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            self->step++;
        }
        break;
    case 2:
        prim = &g_PrimBuf[self->primIndex];
        PCOL(prim) = prim->b3 - 4;
        if (prim->r0 < 4) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A3BE0);
#else
// a glowing square in the middle of the screen: dims on flag 4 (sets
// 0x200), brightens on flag 0x80 (sets 0x100 past 0xC0), then flashes
// the screen and sets 0x20
void func_us_801A3BE0(Entity* self) {
    Primitive* prim;
    s16 primIndex;

    switch (self->step) {
    case 0:
        primIndex = g_api.AllocPrimitives(PRIM_G4, 1);
        if (primIndex != -1) {
            InitializeEntity(g_EInitInteractable);
            prim = &g_PrimBuf[primIndex];
            self->primIndex = primIndex;
            self->flags |= FLAG_HAS_PRIMS;
            prim->x0 = prim->x2 = 0x60;
            prim->x1 = prim->x3 = 0xA0;
            prim->y0 = prim->y1 = 0x40;
            prim->y2 = prim->y3 = 0xC0;
            PCOL(prim) = 0xFF;
            prim->drawMode =
                DRAW_UNK_40 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            prim->priority = 0x82;
        }
        break;
    case 1:
        prim = &g_PrimBuf[self->primIndex];
        if (g_CutsceneFlags & 4) {
            PCOL(prim) = prim->b3 - 1;
            if (!prim->r0) {
                PCOL(prim) = 0;
                g_CutsceneFlags |= 0x200;
                self->step++;
            }
        }
        break;
    case 2:
        prim = &g_PrimBuf[self->primIndex];
        if (g_CutsceneFlags & 0x80) {
            // as on PSX, b3 runs one ahead of the other channels
            prim->r0 = prim->b2 = prim->b1 = prim->b0 = prim->g3 = prim->g2 =
                prim->g1 = prim->g0 = prim->r3 = prim->r2 = prim->r1 =
                    prim->b3;
            prim->b3++;
            if (prim->r0 > 0xC0) {
                g_CutsceneFlags |= 0x100;
            }
            if (prim->r0 > 0xFD) {
                PCOL(prim) = 0;
                prim->drawMode = DRAW_DEFAULT;
                SetStep(3);
                g_api.PlaySfx(0x62F);
            }
        }
        break;
    case 3:
        prim = &g_PrimBuf[self->primIndex];
        prim->type = PRIM_G4;
        prim->x1 = prim->x3 = 0x100;
        prim->y0 = prim->y1 = 0x10;
        prim->y2 = prim->y3 = 0xF0;
        prim->x0 = prim->x2 = 0;
        prim->priority = 0x7F;
        prim->drawMode = DRAW_DEFAULT;
        if (self->poseTimer & 1) {
            PCOL(prim) = 0x80;
        } else {
            PCOL(prim) = 0;
        }
        if (++self->poseTimer > 8) {
            DestroyEntity(self);
            g_CutsceneFlags |= 0x20;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A4028);
#else
// a sprite that rises and shrinks on flag 0x80 until flag 0x100
void func_us_801A4028(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = ANIMSET_OVL(1);
        self->palette = PAL_FLAG(0x200);
        self->unk5A = 0x48;
        self->animCurFrame = 0x35;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleX = 0x100;
        self->scaleY = 0x100;
        self->zPriority = 0x81;
        break;
    case 1:
        if (g_CutsceneFlags & 0x80) {
            self->velocityY = FIX(0.5);
            self->step++;
        }
        break;
    case 2:
        self->velocityY -= 0x180;
        MoveEntity();
        self->scaleX--;
        self->scaleY--;
        if (g_CutsceneFlags & 0x100) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A9208_from_bo6);
#else
// BO6's white flash; RBO6's copy destroys itself after the flash, so its
// ending steps are never reached
void func_us_801A9208_from_bo6(Entity* self) {
    Primitive* prim;
    s16 primIndex;

    switch (self->step) {
    case 0:
        primIndex = g_api.AllocPrimitives(PRIM_G4, 1);
        if (primIndex != -1) {
            InitializeEntity(g_EInitInteractable);
            prim = &g_PrimBuf[primIndex];
            g_unkGraphicsStruct.pauseEnemies = true;
            g_PauseAllowed = false;
            self->primIndex = primIndex;
            self->animSet = 0;
            self->flags |= FLAG_HAS_PRIMS;
            prim->x1 = prim->x3 = 0x100;
            prim->y0 = prim->y1 = 4;
            prim->y2 = prim->y3 = 0xE8;
            PCOL(prim) = 0x80;
            prim->priority = 0x1F8;
            prim->x0 = prim->x2 = 0;
            prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            self->ext.utimer.t = 0;
            g_api.PlaySfx(0x62F);
        }
        self->flags |= FLAG_UNK_10000;
        break;
    case 1:
        self->ext.utimer.t++;
        prim = &g_PrimBuf[self->primIndex];
        if (self->ext.utimer.t > 8) {
            self->step++;
            prim->drawMode = DRAW_HIDE;
            DestroyEntity(self);
        } else if (self->ext.utimer.t & 1) {
            prim->drawMode = DRAW_HIDE;
        } else {
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        }
        break;
    case 2:
        if (g_CutsceneFlags & 0x100) {
            g_unkGraphicsStruct.unk28 = 0;
            self->ext.utimer.t = 0;
            self->step++;
        }
        break;
    case 3:
        prim = &g_PrimBuf[self->primIndex];
        self->step++;
        PCOL(prim) = 0;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        break;
    case 4:
        prim = &g_PrimBuf[self->primIndex];
        PCOL(prim) = prim->b3 + 1;
        if (prim->r0 == 0xFF) {
            prim->drawMode = DRAW_COLORS;
            self->step++;
        }
        break;
    case 5:
        prim = &g_PrimBuf[self->primIndex];
        PCOL(prim) = prim->b3 - 4;
        if (prim->r0 < 5) {
            g_PauseAllowed = true;
            g_unkGraphicsStruct.pauseEnemies = false;
            D_800978B4 = g_RoomCount >= 0x737 ? 3 : 5;
            g_GameState = Game_Ending;
            g_GameStep = Play_Reset;
            self->step++;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", DecreaseBrightness);
#else
// despite the name, brightens the four vertex colours of a prim by amount,
// up to 0xFF; returns 0 once all are saturated
s32 DecreaseBrightness(Primitive* prim, s32 amount) {
    u8* color;
    u8* channel;
    s32 i;
    s32 c;
    s32 ret;

    ret = 0;
    color = &prim->r0;
    for (i = 0; i < 4; i++, color += OFF(Primitive, r1) - OFF(Primitive, r0)) {
        for (channel = color; channel < color + 3; channel++) {
            c = *channel + amount;
            if (c >= 0x100) {
                c = 0xFF;
            } else {
                ret |= 1;
            }
            *channel = c;
        }
    }
    return ret;
}
#endif

extern s32 D_us_80180914; // flame intensity, 0 for none
extern u32 D_us_8018091C; // the direction Alucard walks off in
extern s32 D_us_80180920; // flame sound volume, 0 for full
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A4594);
#else
// Alucard leaves as the castle crumbles: a light opens up in the middle,
// the curtains of light close in, flames rise, everything fades to white,
// then to black, and the ending starts (3 for the best ending with 0x737
// rooms or more, else 5)
void func_us_801A4594(Entity* self) {
    Entity* entity;
    Primitive* prim;
    s32 i;
    s32 x;
    s32 c;
    s32 done;
    s16 primIndex;

    FntPrint("end_step %x\n", self->step);
    g_Player.demo_timer = 0x80;
    g_unkGraphicsStruct.pauseEnemies = true;
    g_Player.padSim = PAD_NONE;
    g_Player.status |= PLAYER_STATUS_UNK100000;
    g_PauseAllowed = false;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x20);
        if (primIndex == -1) {
            self->step = 0;
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.rboDracEnding.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        // a transformed Alucard turns back
        if (g_Player.status & PLAYER_STATUS_MIST_FORM) {
            g_Player.padSim = PAD_L1;
        } else if (g_Player.status & PLAYER_STATUS_WOLF_FORM) {
            g_Player.padSim = PAD_R2;
        } else if (g_Player.status & PLAYER_STATUS_BAT_FORM) {
            g_Player.padSim = PAD_R1;
        }
        // fallthrough
    case 1:
        prim = self->ext.rboDracEnding.prim;
        prim->type = PRIM_G4;
        PGREY(prim, 0) = 0;
        prim->tpage = 0x1A;
        prim->clut = 0x2E2;
        prim->u1 = prim->u3 = 0x3F;
        prim->v0 = prim->v1 = 0xC0;
        prim->v2 = prim->v3 = 0xFF;
        prim->x0 = prim->x2 = 0x80;
        prim->x1 = prim->x3 = 0x80;
        prim->y0 = prim->y1 = 0x80;
        // as on PSX, y1 is set twice and y2 not at all
        prim->y1 = prim->y3 = 0x80;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                         DRAW_UNK02 | DRAW_TRANSP;
        prim->u0 = prim->u2 = 0;
        prim->priority = 0x90;
        LOW(prim->r3) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r1) = LOW(prim->r0);

        prim = prim->next;
        self->ext.rboDracEnding.flash = prim;
        prim->type = PRIM_G4;
        prim->x0 = prim->x2 = 0;
        prim->x1 = prim->x3 = 0x100;
        prim->y0 = prim->y1 = 0x10;
        prim->y2 = prim->y3 = 0xF0;
        prim->priority = 0x50;
        prim->drawMode = DRAW_HIDE;

        // the two curtains of light, left and right
        prim = prim->next;
        self->ext.rboDracEnding.curtain = prim;
        prim->type = PRIM_G4;
        prim->x0 = prim->x2 = 0x40;
        prim->x1 = prim->x3 = 0;
        prim->y0 = prim->y1 = 0x10;
        prim->y2 = prim->y3 = 0xF0;
        prim->priority = 0x90;
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
        prim->type = PRIM_G4;
        prim->x0 = prim->x2 = 0xC0;
        prim->x1 = prim->x3 = 0x100;
        prim->y0 = prim->y1 = 0x10;
        prim->y2 = prim->y3 = 0xF0;
        prim->priority = 0x90;
        prim->drawMode = DRAW_HIDE;

        prim = prim->next;
        self->ext.rboDracEnding.fade = prim;
        prim->type = PRIM_G4;
        prim->x0 = prim->x2 = 0;
        prim->x1 = prim->x3 = 0x100;
        prim->y0 = prim->y1 = 0x10;
        prim->y2 = prim->y3 = 0xF0;
        prim->priority = 0x1FF;
        prim->drawMode = DRAW_HIDE;
        self->step++;
        // fallthrough
    case 2:
        // the light grows
        prim = self->ext.rboDracEnding.prim;
        prim->x0 = prim->x2 = prim->x2 - 3;
        prim->x1 = prim->x3 = prim->x3 + 3;
        prim->y0 = prim->y1 = prim->y1 - 3;
        prim->y2 = prim->y3 = prim->y3 + 3;
        c = prim->r0 + 8;
        if (c > 0x60) {
            self->ext.rboDracEnding.timer = 0x10;
            g_api.PlaySfx(0x62F);
            c = 0x60;
            self->step++;
        }
        PGREY(prim, 0) = c;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        break;
    case 3:
        prim = self->ext.rboDracEnding.prim;
        if (self->ext.rboDracEnding.timer & 1) {
            prim->x0 = prim->x2 = prim->x2 - 1;
            prim->x1 = prim->x3 = prim->x3 + 1;
            prim->y0 = prim->y1 = prim->y1 - 1;
            prim->y2 = prim->y3 = prim->y3 + 1;
        }
        if (!--self->ext.rboDracEnding.timer) {
            self->step++;
        }
        break;
    case 4:
        prim = self->ext.rboDracEnding.prim;
        if (!PrimDecreaseBrightness(prim, 4)) {
            prim->drawMode = DRAW_HIDE;
            self->step++;
        }
        prim = self->ext.rboDracEnding.flash;
        prim->r0 += 4;
        prim->b0 += 2;
        prim->g0 += 3;
        prim->drawMode = DRAW_COLORS;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        D_us_80180914 = 1;
        // fallthrough
    case 5:
        // the curtains brighten
        prim = self->ext.rboDracEnding.curtain;
        c = prim->r1;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
        if (c < 0xC0) {
            c += 4;
        } else {
            self->step++;
        }
        prim->r1 = prim->b1 = prim->g1 = c;
        LOW(prim->r3) = LOW(prim->r1);
        prim = prim->next;
        prim->r1 = prim->b1 = prim->g1 = c;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
        LOW(prim->r3) = LOW(prim->r1);
        if (self->step == 5 && D_us_80180914 < 4 && !(g_Timer & 0xF)) {
            D_us_80180914++;
        }
        break;
    case 6:
        // the flash turns orange; the curtains close in
        prim = self->ext.rboDracEnding.flash;
        prim->drawMode = DRAW_COLORS;
        if (prim->r0 < 0xA0) {
            prim->r0 += 4;
        }
        if (prim->g0 < 0x80) {
            prim->g0 += 3;
        }
        done = 1;
        if (prim->g0 < 0x30) {
            prim->b0 += 2;
        }
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        prim = self->ext.rboDracEnding.curtain;
        c = prim->r1 + 1;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
        if (c > 0xFF) {
            c = 0xFF;
        }
        prim->r1 = prim->b1 = prim->g1 = c;
        LOW(prim->r3) = LOW(prim->r1);
        if (prim->x0 < 0x80) {
            done = 0;
            prim->x0 = ++prim->x2;
        }
        prim = prim->next;
        prim->r1 = prim->b1 = prim->g1 = c;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
        LOW(prim->r3) = LOW(prim->r1);
        if (prim->x0 > 0x80) {
            done = 0;
            prim->x0 = --prim->x2;
        }
        if (done) {
            self->step++;
        }
        break;
    case 7:
        // the curtains turn white, the flames grow quieter
        prim = self->ext.rboDracEnding.curtain;
        done = DecreaseBrightness(prim, 2);
        prim = prim->next;
        done += DecreaseBrightness(prim, 2);
        D_us_80180920 = 0x80 - (prim->r0 >> 1);
        entity = AllocEntity(&g_Entities[64], &g_Entities[TOTAL_ENTITY_COUNT]);
        if (entity != NULL) {
            CreateEntityFromCurrentEntity(E_ID(UNK_32), entity);
            x = Random();
            if (x < 0x20) {
                x += 0x60;
            }
            if (x > 0xE0) {
                x -= 0x80;
            }
            entity->posX.i.hi = x;
            entity->posY.i.hi = 0xF0;
            entity->params = 0;
        }
        if (!done) {
            self->step++;
        }
        break;
    case 8:
        D_us_8018091C = PLAYER.posX.i.hi > 0x80 ? PAD_LEFT : PAD_RIGHT;
        self->step++;
        // fallthrough
    case 9:
        // Alucard walks off as the screen turns white
        g_Player.padSim = D_us_8018091C;
        prim = self->ext.rboDracEnding.fade;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
        if (!DecreaseBrightness(prim, 4)) {
            self->step++;
        }
        return;
    case 10:
        // then black
        prim = self->ext.rboDracEnding.fade;
        prim->drawMode = DRAW_DEFAULT;
        if (!PrimDecreaseBrightness(prim, 8)) {
            self->step++;
        }
        return;
    case 11:
        g_PauseAllowed = true;
        g_unkGraphicsStruct.pauseEnemies = false;
        D_800978B4 = g_RoomCount >= 0x737 ? 3 : 5;
        g_GameState = Game_Ending;
        g_GameStep = Play_Reset;
        self->step++;
        // fallthrough
    case 12:
        g_PauseAllowed = true;
        g_unkGraphicsStruct.pauseEnemies = false;
        return;
    }
    // the flames, with their crackle
    if (D_us_80180914) {
        if (!(g_Timer & 7)) {
            if (D_us_80180920) {
                g_api.PlaySfxVolPan(0x69D, D_us_80180920, 0);
            } else {
                g_api.PlaySfx(0x69D);
            }
        }
        if (!(g_Timer & 1)) {
            entity =
                AllocEntity(&g_Entities[64], &g_Entities[TOTAL_ENTITY_COUNT]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_ID(UNK_32), entity);
                x = Random();
                if (x < 0x20) {
                    x += 0x60;
                }
                if (x > 0xE0) {
                    x -= 0x80;
                }
                entity->posX.i.hi = x;
                entity->posY.i.hi = 0xF0;
                entity->params = 0;
            }
        }
    }
}
#endif

extern EInit g_EInitParticle;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_2362C", func_us_801A4F14);
#else
// a rising flame of the ending: a column of quads following the flame's
// last heights, with a glow around its head
void func_us_801A4F14(Entity* self) {
    Primitive* prim;
    Primitive* glow;
    s32 i;
    s32 r;
    s32 size2;
    s16 primIndex;
    s16 x;
    s16 y;
    s16 xl;
    s16 xr;
    u8 size;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitParticle);
        self->animCurFrame = 0;
        r = Random() % D_us_80180914;
        size = r + 2;
        self->hitboxWidth = size;
        self->velocityY = FIX(-1.5) - ((Random() % (r + 3)) << 17);
        self->ext.rboDracFlame.accel = -0x1000 - (Random() % size) * 0x3000;
        self->opacity = (Random() % (r + 1)) * 8 + 0x20;
        if (r + 1 >= D_us_80180914 / 2) {
            self->zPriority = 0x90;
        } else {
            self->zPriority = 0x54;
        }
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 7);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.rboDracFlame.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        // the head
        prim->tpage = 0x1A;
        prim->clut = 0x2EC;
        prim->u1 = prim->u3 = 0x40;
        prim->v0 = prim->v1 = 0xC0;
        prim->u0 = prim->u2 = 0;
        prim->v2 = prim->v3 = 0xE0;
        PGREY(prim, 0) = self->opacity;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                         DRAW_UNK02 | DRAW_TRANSP;
        prim->priority = self->zPriority;
        // the body
        prim = prim->next;
        for (i = 0; i < 4; i++) {
            prim->tpage = 0x1A;
            prim->clut = 0x2EC;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x40;
            prim->v0 = prim->v1 = 0xE0;
            prim->v2 = prim->v3 = 0xE2;
            PGREY(prim, 0) = self->opacity;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                             DRAW_UNK02 | DRAW_TRANSP;
            prim->priority = self->zPriority + 1;
            prim = prim->next;
        }
        // the tail
        prim->u1 = prim->u3 = 0x40;
        prim->tpage = 0x1A;
        prim->clut = 0x2EC;
        prim->u0 = prim->u2 = 0;
        prim->v0 = prim->v1 = 0xE0;
        prim->v2 = prim->v3 = 0xFF;
        PGREY(prim, 0) = self->opacity;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                         DRAW_UNK02 | DRAW_TRANSP;
        prim->priority = self->zPriority;
        // the glow
        prim = prim->next;
        self->ext.rboDracFlame.glow = prim;
        prim->u1 = prim->u3 = 0x3F;
        prim->tpage = 0x1A;
        prim->clut = 0x2EC;
        prim->u0 = prim->u2 = 0;
        prim->v0 = prim->v1 = 0xC0;
        prim->v2 = prim->v3 = 0xFF;
        PGREY(prim, 0) = self->opacity + 0x20;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                         DRAW_UNK02 | DRAW_TRANSP | DRAW_UNK_40;
        prim->priority = self->zPriority;
        if (self->hitboxWidth < 4) {
            prim->drawMode = DRAW_HIDE;
        }
        for (i = 0; i < 6; i++) {
            self->ext.rboDracFlame.history[i] = self->posY.i.hi;
        }
        break;
    case 1:
        MoveEntity();
        self->velocityY += self->ext.rboDracFlame.accel;
        break;
    }

    for (i = 5; i > 0; i--) {
        self->ext.rboDracFlame.history[i] = self->ext.rboDracFlame.history[i - 1];
    }
    size = self->hitboxWidth;
    prim = self->ext.rboDracFlame.prim;
    x = self->posX.i.hi;
    y = self->posY.i.hi - size;
    xl = x - size;
    xr = x + size;
    self->ext.rboDracFlame.history[0] = self->posY.i.hi;
    prim->y0 = prim->y1 = y;
    size2 = size * 2;
    prim->x0 = xl;
    prim->x1 = xr;
    glow = self->ext.rboDracFlame.glow;
    glow->x0 = x - size2;
    glow->x1 = x + size2;
    glow->y0 = glow->y1 = y - size;
    for (i = 0; i < 5; i++) {
        y = self->ext.rboDracFlame.history[i];
        prim->x2 = xl;
        prim->x3 = xr;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        prim->y2 = prim->y3 = y;
        prim = prim->next;
        prim->y0 = prim->y1 = y;
        prim->x0 = xl;
        prim->x1 = xr;
    }
    prim->x2 = x - size;
    prim->x3 = x + size;
    prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
    y += self->hitboxWidth;
    prim->y2 = prim->y3 = y;
    glow->x2 = x - size2;
    glow->x3 = x + size2;
    glow->y2 = glow->y3 = y + self->hitboxWidth * 2;
    if (y < 0x10) {
        DestroyEntity(self);
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s32 D_us_80180914 = 0;
u32 D_us_8018091C = 0;
s32 D_us_80180920 = 0;
// end pc_import_data.py
// clang-format on
#endif
