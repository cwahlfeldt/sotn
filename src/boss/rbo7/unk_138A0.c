// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo7.h"

#include "../../st/e_breakable.h"

extern EInit D_us_8018042C;
extern s32 D_us_80180564;
extern s16 D_us_80180568[];
extern s16 D_us_80180580[];
extern s16 D_us_80180598[];
extern s16 D_us_801805B0[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo7/nonmatchings/unk_138A0", func_us_801BAB18_from_bo0);
#else
// BO0's EntityBossDoors, mirrored: the doors close from the left
void func_us_801BAB18_from_bo0(Entity* self) {
    s32 offsetX;
    s32 i;
    s32 fgIndex;
    s16* fgTiles;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_8018042C);
        self->zPriority = 0x69;
        self->animCurFrame = 0xE;
        if (self->params) {
            self->posX.i.hi = -0x18 - g_Tilemap.scrollX.i.hi;
        }
        if (self->params) {
            fgIndex = 0x322;
            fgTiles = D_us_80180568;
        } else {
            fgIndex = 0x33F;
            fgTiles = D_us_80180598;
        }
        for (i = 0; i < 4; i++, fgTiles += 3) {
            (&g_Tilemap.fg[fgIndex])[0] = fgTiles[0];
            (&g_Tilemap.fg[fgIndex])[-1] = fgTiles[1];
            (&g_Tilemap.fg[fgIndex])[-2] = fgTiles[2];
            fgIndex -= 32;
        }
        // fallthrough
    case 1:
        offsetX = PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (offsetX > 0x18) {
            D_us_80180564 = 1;
            if (self->params) {
                CreateEntityFromEntity(E_ID(UNK_16), self, self + 1);
                (self + 1)->params = 0;
                (self + 1)->posX.i.hi = 0x218 - g_Tilemap.scrollX.i.hi;
            }
            self->step++;
        }
        break;
    case 2:
        if (!self->step_s) {
            if (self->params) {
                self->velocityX = FIX(0.5);
            } else {
                self->velocityX = FIX(-0.5);
            }
            self->step_s++;
        }
        GetPlayerCollisionWith(self, 0x18, 0x20, 5);
        MoveEntity();
        offsetX = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (self->params) {
            if (offsetX > 0x18) {
                self->posX.i.hi = 0x18 - g_Tilemap.scrollX.i.hi;
                self->step++;
            }
        } else if (offsetX < 0x1E8) {
            self->posX.i.hi = 0x1E8 - g_Tilemap.scrollX.i.hi;
            self->step++;
        }
        break;
    case 3:
        if (self->params) {
            fgIndex = 0x322;
            fgTiles = D_us_80180580;
        } else {
            fgIndex = 0x33F;
            fgTiles = D_us_801805B0;
        }
        for (i = 0; i < 4; i++, fgTiles += 3) {
            (&g_Tilemap.fg[fgIndex])[0] = fgTiles[0];
            (&g_Tilemap.fg[fgIndex])[-1] = fgTiles[1];
            (&g_Tilemap.fg[fgIndex])[-2] = fgTiles[2];
            fgIndex -= 32;
        }
        self->step++;
        break;
    case 4:
        if (!D_us_80180564) {
            self->step++;
        }
        break;
    case 5:
        if (self->params) {
            fgIndex = 0x322;
            fgTiles = D_us_80180568;
        } else {
            fgIndex = 0x33F;
            fgTiles = D_us_80180598;
        }
        for (i = 0; i < 4; i++, fgTiles += 3) {
            (&g_Tilemap.fg[fgIndex])[0] = fgTiles[0];
            (&g_Tilemap.fg[fgIndex])[-1] = fgTiles[1];
            (&g_Tilemap.fg[fgIndex])[-2] = fgTiles[2];
            fgIndex -= 32;
        }
        if (self->params) {
            self->velocityX = FIX(-0.5);
        } else {
            self->velocityX = FIX(0.5);
        }
        self->step++;
        break;
    case 6:
        MoveEntity();
        offsetX = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (self->params) {
            if (offsetX < -0x30) {
                self->step++;
            }
        } else if (offsetX > 0x230) {
            self->step++;
        }
        break;
    case 0xFF:
#include "../../st/pad2_anim_debug.h"
    }
}
#endif

extern EInit g_EInitInteractable;
extern s32 D_us_80180564;
extern s32 D_us_801805C8;

void func_us_80192B38_from_rbo3(Entity* self) {
    Entity* entity;
    s32 x;
    s32 y;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        /* fall through */
    case 1:
        entity = &PLAYER;
        x = entity->posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (x > 0x80 && x < 0x180) {
            D_us_80180564 = 1;
            D_us_801805C8 = 1;
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_AKMODAN_II_DEFEAT, TIMEATTACK_SET_VISITED);
            stopMusicFlag = true;
            currentMusicId = MU_FESTIVAL_OF_SERVANTS;
            self->step++;
        }
        break;

    case 2:
        if (g_api.func_80131F68() == false) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;

    case 3:
        if (D_us_801805C8 & 2) {
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_AKMODAN_II_DEFEAT, TIMEATTACK_SET_RECORD);
            g_api.PlaySfx(SET_UNK_90);
            currentMusicId = MU_FINAL_TOCATTA;
            self->step++;
        }
        break;

    case 4:
        if (D_us_801805C8 & 4) {
            self->step++;
        }
        break;

    case 5:
        x = 0x100 - g_Tilemap.scrollX.i.hi;
        y = 0x180 - g_Tilemap.scrollY.i.hi;
        entity = AllocEntity(&g_Entities[0xA0], &g_Entities[0xC0]);
        if (entity == NULL) {
            break;
        }
        CreateEntityFromEntity(E_ID(LIFE_UP_SPAWN), self, entity);
        entity->posX.i.hi = x;
        entity->posY.i.hi = y;
        entity->params = 0x13;
        D_us_80180564 = 0;
        stopMusicFlag = true;
        currentMusicId = MU_FINAL_TOCATTA;
        self->step++;
        break;

    case 6:
        if (g_api.func_80131F68() == false) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;
    }
}

extern EInit g_EInitAkmodanII;
extern s16 D_us_801805DC[];
extern u8 D_us_801805E4[];
extern u8 D_us_801805F0[];
extern u8 D_us_801805FC[];
extern u8 D_us_80180610[];
extern u8 D_us_80180618[];
extern u8 D_us_80180624[];
extern u8 D_us_80180638[];
extern u8 D_us_80180640[];
extern s16 D_us_801806CC[];
extern s8 D_us_80180704[];
extern u8 D_us_80180720[];
extern Primitive* FindFirstUnkPrim(Primitive* prim);
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo7/nonmatchings/unk_138A0", func_us_801940B4);
#else
// Akmodan II
void func_us_801940B4(Entity* self) {
    DRAWENV drawEnv;
    RECT clipRect;
    DR_ENV* dr_env;
    Entity* entity;
    Primitive* prim;
    Primitive* bodyPrim;
    s16* hand;
    s8* hitbox;
    s32 primIndex;
    s32 i;
    s32 side;
    s32 velX;
    s32 dx;
    s32 dy;
    s32 x;
    s32 y;
    s32 rnd;
    s32 flip;
    s32 active;
    s16 angle;

    FntPrint("step %x\n", self->step);
    FntPrint("step_s %x\n", self->step_s);
    if (self->flags & FLAG_DEAD) {
        if (self->step != 8) {
            SetStep(8);
        }
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitAkmodanII);
        self->hitboxState = 0;
        self->ext.akmodanII.zPriority = self->zPriority;
        CreateEntityFromEntity(E_ID(HARPY_KICK), self, self + 1);
        // fallthrough
    case 1:
        // the coffin's lid and base
        CreateEntityFromEntity(E_ID(UNK_1A), self, self + 2);
        (self + 2)->params = 2;
        (self + 2)->zPriority = self->zPriority + 1;
        CreateEntityFromEntity(E_ID(UNK_1A), self, self + 3);
        (self + 3)->params = 3;
        (self + 3)->zPriority = self->zPriority - 1;
        SetStep(2);
        // fallthrough
    case 2:
        switch (self->step_s) {
        case 0:
            if (D_us_801805C8 & 1) {
                self->step_s++;
            }
            break;
        case 1:
            if (!AnimateEntity(D_us_801805E4, self)) {
                PlaySfxPositional(0x608);
                self->ext.akmodanII.risen = 1;
                SetSubStep(2);
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_801805F0, self)) {
                self->hitboxState = 3;
                SetStep(3);
            }
            break;
        }
        break;
    case 3:
        if (!self->step_s) {
            self->ext.akmodanII.timer = 0x80;
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            self->step_s++;
        }
        if (!AnimateEntity(D_us_801805FC, self)) {
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        }
        if (!self->poseTimer && (self->pose == 2 || self->pose == 8)) {
            PlaySfxPositional(0x81E);
        }
        if (self->facingLeft) {
            self->velocityX = FIX(0.5);
        } else {
            self->velocityX = FIX(-0.5);
        }
        UnkCollisionFunc2(D_us_801805DC);
        if (!(Random() & 0xF)) {
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1D), self, entity);
                entity->posY.i.hi += 0x20;
                entity->zPriority = self->zPriority + 1;
                if (self->facingLeft) {
                    entity->posX.i.hi -= 6;
                } else {
                    entity->posX.i.hi += 6;
                }
            }
        }
        if (!--self->ext.akmodanII.timer) {
            SetStep(4);
        }
        side = (GetSideToPlayer() & 1) ^ 1;
        velX = PLAYER.velocityX;
        if (self->facingLeft) {
            velX = -velX;
        }
        if (GetDistanceToPlayerX() < 0x40 && GetDistanceToPlayerX() > 0x38 &&
            self->facingLeft == side && velX > 0) {
            SetStep(4);
        }
        if (self->hitFlags & 3) {
            SetStep(6);
        }
        if (GetDistanceToPlayerX() < 0x26 && self->facingLeft == side) {
            x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
            if (x > 0x50 && x < 0x1B0) {
                SetStep(5);
            }
        }
        break;
    case 4:
        if (!AnimateEntity(D_us_80180640, self)) {
            self->hitboxOffX = 8;
            self->hitboxOffY = -0x24;
            self->hitboxWidth = 0x10;
            self->hitboxHeight = 0x24;
            SetStep(3);
        }
        if (!self->poseTimer && self->pose == 3) {
            PlaySfxPositional(0x81D);
        }
        break;
    case 5:
        // grab Alucard, squeeze him four times and throw him
        switch (self->step_s) {
        case 0:
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_80180618, self)) {
                SetStep(3);
            }
            hand = &D_us_801806CC[(self->animCurFrame - 0x11) * 2];
            dx = hand[0];
            dy = hand[1];
            if (self->facingLeft) {
                dx = -dx;
            }
            dx = dx + self->posX.i.hi - PLAYER.posX.i.hi;
            dy = dy + self->posY.i.hi - PLAYER.posY.i.hi;
            if (abs(dx) < 8 && abs(dy) < 0xC &&
                !(g_Player.status & 0x010401C6)) {
                g_Player.unk60 = 1;
                g_Player.unk64 = 8;
                g_Player.unk62 = 0;
                self->ext.akmodanII.savedX = g_Tilemap.x;
                self->ext.akmodanII.savedWidth = g_Tilemap.width;
                self->ext.akmodanII.savedY = g_Tilemap.y;
                self->ext.akmodanII.savedHeight = g_Tilemap.height;
                g_Tilemap.x = g_Tilemap.scrollX.i.hi;
                g_Tilemap.width = g_Tilemap.scrollX.i.hi + 0x100;
                self->ext.akmodanII.grabbing = 1;
                self->ext.akmodanII.count = 0;
                self->step_s = 2;
            }
            break;
        case 2:
            hand = &D_us_801806CC[(self->animCurFrame - 0x11) * 2];
            dx = hand[0];
            dy = hand[1];
            if (self->facingLeft) {
                dx = -dx;
            }
            dx = dx + self->posX.i.hi - PLAYER.posX.i.hi;
            dy = dy + self->posY.i.hi - PLAYER.posY.i.hi;
            if (dx > 0) {
                PLAYER.posX.i.hi++;
            }
            if (dx < 0) {
                PLAYER.posX.i.hi--;
            }
            if (dy > 0) {
                PLAYER.posY.i.hi++;
            }
            if (dy < 0) {
                PLAYER.posY.i.hi--;
            }
            if (!dx && !dy) {
                self->step_s++;
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180618, self)) {
                SetSubStep(4);
            }
            break;
        case 4:
            i = AnimateEntity(D_us_80180624, self);
            if (i & 0x80) {
                self->poseTimer -= self->ext.akmodanII.count;
            }
            if (!i) {
                self->ext.akmodanII.count++;
                g_Player.unk60 = 3;
                g_Player.unk64 = g_api.enemyDefs[0x10E].attack / 2;
            }
            if ((u16)(self->animCurFrame - 0x15) < 5) {
                self->zPriority = g_unkGraphicsStruct.g_zEntityCenter + 4;
            } else {
                self->zPriority = g_unkGraphicsStruct.g_zEntityCenter - 4;
            }
            if (self->ext.akmodanII.count >= 4) {
                SetSubStep(5);
            }
            break;
        case 5:
            g_Player.unk60 = 4;
            g_Player.unk64 = g_api.enemyDefs[0x10E].attack;
            self->ext.akmodanII.grabbing = 0;
            if (self->facingLeft) {
                PLAYER.velocityX = FIX(6);
            } else {
                PLAYER.velocityX = FIX(-6);
            }
            PLAYER.velocityY = FIX(-2);
            if (g_Player.status & 0x40000) {
                PLAYER.velocityX = 0;
            }
            self->zPriority = self->ext.akmodanII.zPriority;
            g_Tilemap.x = self->ext.akmodanII.savedX;
            g_Tilemap.width = self->ext.akmodanII.savedWidth;
            self->step_s++;
            // fallthrough
        case 6:
            if (!AnimateEntity(D_us_80180638, self)) {
                SetStep(3);
            }
            break;
        }
        if (self->step_s >= 3 && self->step_s < 6) {
            hand = &D_us_801806CC[(self->animCurFrame - 0x11) * 2];
            dx = hand[0];
            if (self->facingLeft) {
                dx = -dx;
            }
            PLAYER.posX.i.hi = self->posX.i.hi + dx;
            PLAYER.posY.i.hi = self->posY.i.hi + hand[1];
        }
        break;
    case 6:
        // hit: burst eight poison clouds towards Alucard
        if (!self->step_s) {
            angle = ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                           PLAYER.posX.i.hi - self->posX.i.hi) +
                    0x400;
            for (i = 0; i < 8; i++) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_ID(UNK_1C), self, entity);
                    entity->rotate = angle;
                    if (self->facingLeft) {
                        entity->posX.i.hi -= 4;
                    } else {
                        entity->posX.i.hi += 4;
                    }
                    entity->zPriority = self->zPriority + 1;
                }
            }
            PlaySfxPositional(0x81B);
            self->step_s++;
        }
        if (!AnimateEntity(D_us_80180610, self)) {
            SetStep(3);
        }
        break;
    case 8:
        switch (self->step_s) {
        case 0:
            D_us_801805C8 |= 2;
            self->hitboxState = 0;
            if (self->ext.akmodanII.grabbing) {
                g_Tilemap.x = self->ext.akmodanII.savedX;
                g_Player.unk60 = 0;
                g_Tilemap.width = self->ext.akmodanII.savedWidth;
            }
            PlaySfxPositional(0x81C);
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_80180610, self)) {
                self->step_s++;
            }
            break;
        case 2:
            // as Ctulhu's death: draw him into a 128x128 VRAM area and show
            // that area where he stands
            primIndex = g_api.AllocPrimitives(PRIM_GT4, 8);
            if (primIndex == -1) {
                self->step = 0;
                return;
            }
            prim = &g_PrimBuf[primIndex];
            self->primIndex = primIndex;
            self->ext.akmodanII.prim = prim;
            self->flags |= FLAG_HAS_PRIMS;
            dr_env = g_api.func_800EDB08((POLY_GT4*)prim);
            if (dr_env == NULL) {
                FntPrint("NO DR_ENV \n");
                DestroyEntity(self);
                return;
            }
            prim->type = PRIM_ENV;
            prim->priority = 0x13;
            if (self->params) {
                prim->priority = 0x17;
            }
            prim->drawMode = DRAW_DEFAULT;
            drawEnv = g_CurrentBuffer->draw;
            drawEnv.isbg = 1;
            if (self->params) {
                y = 0x180;
            } else {
                y = 0x100;
            }
            clipRect.x = 0;
            clipRect.y = y;
            clipRect.w = 0x80;
            clipRect.h = 0x80;
            drawEnv.clip = clipRect;
            drawEnv.ofs[0] = 0;
            drawEnv.ofs[1] = 0x100;
            SetDrawEnv(dr_env, &drawEnv);
            prim = prim->next;
            dr_env = g_api.func_800EDB08((POLY_GT4*)prim);
            if (dr_env == NULL) {
                FntPrint("NO DR_ENV \n");
                DestroyEntity(self);
                return;
            }
            prim->type = PRIM_ENV;
            prim->priority = 0x16;
            if (self->params) {
                prim->priority = 0x1A;
            }
            prim->drawMode = DRAW_UNK_800;
            prim = prim->next;
            self->ext.akmodanII.bodyPrim = prim;
            if (self->params) {
                y = 0xFF;
            } else {
                y = 0x7F;
            }
            prim->type = PRIM_GT4;
            prim->tpage = 0x110;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x3F;
            prim->v0 = prim->v1 = y - 0x70;
            prim->v2 = prim->v3 = y;
            prim->x0 = prim->x2 = self->posX.i.hi - 0x20;
            prim->x1 = prim->x3 = self->posX.i.hi + 0x1F;
            prim->y2 = prim->y3 = self->posY.i.hi + 0x1F;
            prim->y0 = prim->y1 = self->posY.i.hi - 0x51;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            prim->priority = self->ext.akmodanII.zPriority + 1;
            prim = prim->next;
            prim->type = PRIM_TILE;
            prim->u0 = 0x80;
            prim->v0 = 0x80;
            prim->x0 = 0;
            prim->r0 = prim->g0 = prim->b0 = 0;
            prim->y0 = (self->params != 0) << 7;
            prim->priority = 0x15;
            if (self->params) {
                prim->priority = 0x19;
            }
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE | DRAW_TRANSP;
            prim = prim->next;
            while (prim != NULL) {
                prim->drawMode = DRAW_HIDE;
                prim = prim->next;
            }
            self->zPriority = 0x14;
            if (self->params) {
                self->zPriority = 0x18;
            }
            if (self->params) {
                y = 0x100;
            } else {
                y = 0x80;
            }
            self->posX.i.hi = 0x20;
            self->posY.i.hi = y - 0x20;
            self->palette = g_EInitAkmodanII[3];
            self->flags &= ~(FLAG_POS_CAMERA_LOCKED | 0xF);
            self->step_s++;
            break;
        case 3:
            bodyPrim = self->ext.akmodanII.bodyPrim;
            self->palette = PAL_FLAG(0x1C7);
            self->flags |= FLAG_POS_CAMERA_LOCKED;
            self->zPriority = self->ext.akmodanII.zPriority;
            self->posX.i.hi = bodyPrim->x0 + 0x20;
            self->posY.i.hi = bodyPrim->y2 - 0x1F;
            prim = self->ext.akmodanII.prim;
            prim->type = PRIM_ENV;
            dr_env = PRIM_DR_ENV(prim);
            drawEnv = g_CurrentBuffer->draw;
            drawEnv.isbg = 0;
            drawEnv.dtd = 0;
            if (self->params) {
                y = 0x180;
            } else {
                y = 0x100;
            }
            clipRect.x = 0;
            clipRect.y = y;
            clipRect.w = 0x80;
            clipRect.h = 0x80;
            drawEnv.clip = clipRect;
            drawEnv.ofs[0] = 0;
            drawEnv.ofs[1] = 0x100;
            SetDrawEnv(dr_env, &drawEnv);
            prim->priority = 0x13;
            if (self->params) {
                prim->priority = 0x17;
            }
            prim->drawMode = DRAW_DEFAULT;
            self->ext.akmodanII.timer = 0x100;
            self->step_s++;
            // fallthrough
        case 4:
            // smoke over his body; the params 1 puffs erase the VRAM copy
            rnd = Random() & 0x1F;
            x = rnd - 0x10;
            y = -(Random() & 0x3F);
            flip = Random() & 1;
            FntPrint("timer %x\n", self->ext.akmodanII.timer);
            if (!--self->ext.akmodanII.timer) {
                self->ext.akmodanII.timer = 0x50;
                self->step_s++;
            }
            entity = AllocEntity(&g_Entities[STAGE_ENTITY_START],
                                 &g_Entities[TOTAL_ENTITY_COUNT]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_ID(CTULHU_DEATH), entity);
                entity->facingLeft = flip;
                entity->params = 0;
                entity->posX.i.hi += x;
                entity->posY.i.hi = entity->posY.i.hi - 8 + (y + 0x20);
            }
            if (!(g_Timer & 7)) {
                entity = AllocEntity(&g_Entities[STAGE_ENTITY_START],
                                     &g_Entities[TOTAL_ENTITY_COUNT]);
                if (entity != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(CTULHU_DEATH), entity);
                    entity->posX.i.hi = rnd + 0x10;
                    if (self->params) {
                        entity->posY.i.hi = y + 0x104;
                    } else {
                        entity->posY.i.hi = y + 0x84;
                    }
                    entity->params = 1;
                    entity->facingLeft = flip;
                    entity->zPriority = 0x14;
                    if (self->params) {
                        entity->zPriority = 0x18;
                    }
                }
            }
            break;
        case 5:
            if (!--self->ext.akmodanII.timer) {
                self->step_s++;
            }
            break;
        case 6:
            // crumble: one sprite per pixel of his body's last frame
            if (self->flags & FLAG_HAS_PRIMS) {
                g_api.FreePrimitives(self->primIndex);
                self->flags &= ~FLAG_HAS_PRIMS;
            }
            primIndex = g_api.func_800EDB58(PRIM_SPRT | 0x10, 0x100);
            if (primIndex == -1) {
                self->step_s = 6;
                return;
            }
            prim = &g_PrimBuf[primIndex];
            self->primIndex = primIndex;
            self->ext.akmodanII.bodyPrim = prim;
            self->flags |= FLAG_HAS_PRIMS;
            prim->type = PRIM_GT4;
            prim->tpage = 0x12;
            prim->clut = 0x1C7;
            prim->u0 = prim->u2 = 0x28;
            prim->v0 = prim->v1 = 0x80;
            prim->u1 = prim->u3 = 0;
            prim->v2 = prim->v3 = 0xC0;
            x = self->posX.i.hi;
            y = self->posY.i.hi;
            if (self->facingLeft) {
                prim->x0 = prim->x2 = x - 0x18;
                prim->x1 = prim->x3 = x + 0x10;
            } else {
                prim->x1 = prim->x3 = x - 0x10;
                prim->x0 = prim->x2 = x + 0x18;
            }
            prim->y2 = prim->y3 = y + 0x20;
            prim->y0 = prim->y1 = y - 0x20;
            prim->drawMode = DRAW_UNK02;
            prim->priority = self->zPriority;
            prim = prim->next;
            self->ext.akmodanII.prim = prim;
            while (prim != NULL) {
                prim->tpage = 0x12;
                prim->clut = 0x1C7;
                prim->u1 = prim->v1 = 1;
                prim->drawMode = DRAW_HIDE;
                prim->p3 = 0;
                prim->priority = self->zPriority;
                prim = prim->next;
            }
            self->animCurFrame = 0;
            self->ext.akmodanII.timer = 1;
            self->ext.akmodanII.row = 0;
            PlaySfxPositional(0x81F);
            self->step_s++;
            break;
        case 7:
            // every 6 frames, turn the top row of the body into pixels
            if (!--self->ext.akmodanII.timer) {
                self->ext.akmodanII.timer = 6;
                x = self->posX.i.hi - 0x10;
                y = self->posY.i.hi + (self->ext.akmodanII.row - 0x20);
                for (i = 0, velX = 0; i < 0x28; i++, velX -= 0x1000) {
                    prim = FindFirstUnkPrim(self->ext.akmodanII.prim);
                    if (prim != NULL) {
                        prim->p3 = 2;
                        prim->u0 = i;
                        prim->v0 = self->ext.akmodanII.row - 0x80;
                        if (self->facingLeft) {
                            prim->x0 = x - (i - 0x20);
                        } else {
                            prim->x0 = x + i;
                        }
                        prim->y0 = y;
                        // x1/y1 and x2/y2 hold the velocity, u2/v2 the
                        // delay, x3/y3 the fractional position
                        if (self->facingLeft) {
                            LOW(prim->x1) = velX;
                        } else {
                            LOW(prim->x1) = i << 12;
                        }
                        LOW(prim->x2) = 0;
                        prim->x3 = 0;
                        prim->y3 = 0;
                        *(s16*)&prim->u2 = Random() & 0x1F;
                        prim->drawMode = DRAW_UNK02;
                    }
                }
                bodyPrim = self->ext.akmodanII.bodyPrim;
                bodyPrim->v0 = ++bodyPrim->v1;
                bodyPrim->y0 = ++bodyPrim->y1;
                if (++self->ext.akmodanII.row >= 0x40) {
                    self->step_s++;
                }
            }
            // fallthrough
        case 8:
            prim = self->ext.akmodanII.prim;
            active = 0;
            while (prim->next != NULL) {
                if (prim->p3 == 2) {
                    active |= 1;
                    if (!*(s16*)&prim->u2) {
                        *(s16*)&prim->u2 = 0x20;
                        prim->p3 = 8;
                    } else {
                        --*(s16*)&prim->u2;
                    }
                }
                if (prim->p3 == 8) {
                    x = (prim->x0 << 16) + (u16)prim->x3 + LOW(prim->x1);
                    y = (prim->y0 << 16) + (u16)prim->y3 + LOW(prim->x2);
                    prim->x0 = x >> 16;
                    prim->y0 = y >> 16;
                    active |= 1;
                    prim->x3 = x;
                    prim->y3 = y;
                    LOW(prim->x2) -= FIX(0.25);
                    if (!--*(s16*)&prim->u2) {
                        prim->drawMode = DRAW_HIDE;
                        prim->p3 = 0;
                    }
                }
                prim = prim->next;
            }
            prim->x0 = prim->y0 = -0x10;
            prim->drawMode = DRAW_UNK02;
            prim->priority = self->zPriority;
            if (!active) {
                D_us_801805C8 |= 4;
                g_api.FreePrimitives(self->primIndex);
                self->flags &= ~FLAG_HAS_PRIMS;
                self->step_s++;
            }
            break;
        }
        break;
    case 0xFF:
        FntPrint("charal %x\n", self->animCurFrame);
        self->hitboxState = 1;
        self->hitboxWidth = self->hitboxHeight = 2;
        self->hitboxOffX = self->hitboxOffY = 0;
        if (g_pads[1].pressed & PAD_SQUARE) {
            if (self->params) {
                break;
            }
            self->animCurFrame++;
            self->params |= 1;
        } else {
            self->params = 0;
        }
        if (g_pads[1].pressed & PAD_CIRCLE) {
            if (self->step_s) {
                break;
            }
            self->animCurFrame--;
            self->step_s |= 1;
        } else {
            self->step_s = 0;
        }
        return;
    }
    hitbox = &D_us_80180704[D_us_80180720[self->animCurFrame] * 4];
    self->hitboxOffX = *hitbox++;
    self->hitboxOffY = *hitbox++;
    self->hitboxWidth = *hitbox++;
    self->hitboxHeight = *hitbox++;
    FntPrint("tsukami %x\n", g_Player.unk60);
}
#endif

extern EInit D_us_80180444;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo7/nonmatchings/unk_138A0", func_us_801957C0);
#else
// the coffin: params 2 is the lid, which slides off as Akmodan II rises,
// params 3 the base
void func_us_801957C0(Entity* self) {
    Entity* akmodan;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180444);
        self->hitboxState = 0;
        if (self->params == 2) {
            self->step = 2;
        }
        if (self->params == 3) {
            self->step = 3;
        }
        break;
    case 2:
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 0;
            akmodan = self - self->params;
            if (akmodan->ext.akmodanII.risen) {
                self->animCurFrame = 4;
                self->ext.akmodanII.timer = 0x40;
                self->step_s++;
            }
            break;
        case 1:
            if (self->facingLeft) {
                self->velocityX = FIX(0.75);
            } else {
                self->velocityX = FIX(-0.75);
            }
            self->step_s++;
            // fallthrough
        case 2:
            MoveEntity();
            if (!--self->ext.akmodanII.timer) {
                self->zPriority -= 2;
                self->step_s++;
            }
            break;
        }
        break;
    case 3:
        if (!self->step_s) {
            self->animCurFrame = 0;
            akmodan = self - self->params;
            if (akmodan->ext.akmodanII.risen) {
                self->animCurFrame = 2;
                self->step_s++;
            }
        }
        break;
    }
}
#endif

extern EInit D_us_80180444;
extern s8 D_us_8018074C[];
extern u8 D_us_80180764[];

void EntityHarpyKick(Entity* self) {
    s32 animFrame;
    s8* hitbox;
    Entity* harpy;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180444);
        /* fall through */
    case 1:
        harpy = self - 1;
        self->facingLeft = harpy->facingLeft;
        self->posX.val = harpy->posX.val;
        self->posY.val = harpy->posY.val;

        animFrame = harpy->animCurFrame;
        animFrame -= 0x23;
        if (animFrame < 0) {
            animFrame = 0;
        }

        hitbox = D_us_8018074C;
        animFrame = D_us_80180764[animFrame];
        hitbox += animFrame * 4;
        self->hitboxOffX = *hitbox++;
        self->hitboxOffY = *hitbox++;
        self->hitboxWidth = *hitbox++;
        self->hitboxHeight = *hitbox++;

        if (harpy->entityId != E_ID(UNK_19)) {
            DestroyEntity(self);
        }
        break;
    }
}

extern EInit D_us_80180450;
extern u8 D_us_80180664[];
extern u8 D_us_80180670[];
extern u8 D_us_80180684[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo7/nonmatchings/unk_138A0", func_us_80195A8C);
#else
// poison cloud, eight at a time when Akmodan II is hit
void func_us_80195A8C(Entity* self) {
    s16 angle;

    if (D_us_801805C8 & 2) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180450);
        self->blendMode = BLEND_TRANSP;
        self->drawFlags = ENTITY_ROTATE | ENTITY_SCALEX | ENTITY_SCALEY;
        self->hitboxWidth = self->hitboxHeight = 6;
        angle = self->rotate + 0x180 - Random() * 3;
        self->rotate = angle;
        self->posX.i.hi += (rsin(angle) * 8) >> 12;
        self->posY.i.hi += -(rcos(angle) * 0x1C) >> 12;
        self->velocityX = rsin(angle) * 0x18;
        self->velocityY = -(rcos(angle) * 0x18);
        self->scaleX = self->scaleY = 0x60;
        self->ext.akmodanII.count = 0;
        // fallthrough
    case 1:
        if (!AnimateEntity(D_us_80180664, self)) {
            SetStep(2);
        }
        MoveEntity();
        self->scaleX += 0xC;
        self->scaleY += 0xC;
        break;
    case 2:
        if (!self->step_s) {
            self->velocityX >>= 2;
            self->velocityY >>= 2;
            self->step_s++;
        }
        MoveEntity();
        if (!AnimateEntity(D_us_80180670, self)) {
            self->ext.akmodanII.count++;
        }
        if (self->ext.akmodanII.count > 0x10) {
            self->opacity = 0x80;
            self->hitboxState = 0;
            self->drawFlags |= ENTITY_OPACITY;
            SetStep(3);
        }
        break;
    case 3:
        self->opacity -= 2;
        self->scaleY += 0x18;
        self->scaleX += 0x18;
        if (!AnimateEntity(D_us_80180684, self)) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern u8 D_us_8018068C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo7/nonmatchings/unk_138A0", func_us_80195D04);
#else
// poison wisps rising from Akmodan II as he walks
void func_us_80195D04(Entity* self) {
    if (D_us_801805C8 & 2) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180450);
        self->animSet = ANIMSET_DRA(5);
        self->blendMode = BLEND_TRANSP;
        self->velocityY = FIX(-0.375);
        self->hitboxWidth = self->hitboxHeight = 3;
        self->unk5A = 0;
        self->palette += PAL_UNK_FLAG;
        // fallthrough
    case 1:
        MoveEntity();
        if (self->pose >= 0x1A) {
            self->hitboxState = 0;
        }
        if (!AnimateEntity(D_us_8018068C, self)) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern u8 D_us_8018076C[];

void EntityCtulhuDeath(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = 14;
        self->unk5A = 121;
        self->drawFlags = ENTITY_OPACITY;
        if (self->params) {
            self->palette = 0x2EE;
            self->opacity = 8;
            self->blendMode = BLEND_TRANSP | BLEND_SUB;
            self->flags &= ~FLAG_POS_CAMERA_LOCKED;
        } else {
            self->palette = 0x2E4;
            self->zPriority += 2;
            self->opacity = 0x14;
            self->blendMode = BLEND_TRANSP | BLEND_ADD;
        }
        /* fall through */
    case 1:
        self->posY.val -= FIX(1);
        if (!AnimateEntity(D_us_8018076C, self)) {
            DestroyEntity(self);
        }
        break;
    }
}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s32 D_us_80180564 = 0;
s32 D_us_801805C8 = 0;
s8 D_us_8018074C[0x18] = {
    0,
    0,
    0,
    0,
    -0x14,
    -4,
    4,
    4,
    -0x10,
    -4,
    8,
    4,
    -0x10,
    -4,
    0x10,
    4,
    -0x18,
    -4,
    0x10,
    4,
    -0x20,
    -4,
    0x18,
    4,
};
u8 D_us_80180764[8] = { 0, 1, 2, 3, 4, 5, 0, 0 };
u8 D_us_8018076C[0x1C] = {
    3,
    1,
    3,
    2,
    3,
    3,
    3,
    4,
    3,
    5,
    3,
    6,
    3,
    7,
    3,
    8,
    3,
    9,
    3,
    0xA,
    3,
    0xB,
    3,
    0xC,
    3,
    0xD,
    0xFF,
    0,
};
s16 D_us_80180568[0xC] = { 0x21, 0x22, 0x48, 0x28, 0x29, 0x27, 0x28, 0x29, 0x27, 0x28, 0x29, 0x27 };
s16 D_us_80180580[0xC] = {
    0x6BF,
    0x6C1,
    0x6C5,
    0x6C0,
    0x6C2,
    0x6C4,
    0x6C0,
    0x6C2,
    0x6C4,
    0x6C0,
    0x6C2,
    0x6C4,
};
s16 D_us_80180598[0xC] = { 0x20, 0x21, 0x22, 0x27, 0x28, 0x29, 0x27, 0x28, 0x29, 0x27, 0x28, 0x29 };
s16 D_us_801805B0[0xC] = {
    0x6C3,
    0x6BF,
    0x6C1,
    0x6C4,
    0x6C0,
    0x6C2,
    0x6C4,
    0x6C0,
    0x6C2,
    0x6C4,
    0x6C0,
    0x6C2,
};
s16 D_us_801805DC[4] = { 0, 0x20, 8, 0 };
u8 D_us_801805E4[0xC] = { 0x10, 0x10, 0x10, 1, 0x10, 0x10, 0x10, 1, 0xFF, 0, 0, 0 };
u8 D_us_801805F0[0xC] = { 0x40, 3, 2, 5, 0x2B, 6, 3, 7, 0x15, 8, 0xFF, 0 };
u8 D_us_801805FC[0x14] = { 7, 9, 7, 0xA, 7, 0xB, 7, 0xC, 7, 0xD, 7, 0xC, 7, 0xB, 7, 0xA, 0, 0, 0, 0 };
u8 D_us_80180610[8] = { 3, 0xE, 0xA, 0xF, 3, 0xE, 0xFF, 0 };
u8 D_us_80180618[0xC] = { 6, 0x11, 0x24, 0x12, 4, 0x13, 0x20, 0x14, 0xFF, 0, 0, 0 };
u8 D_us_80180624[0x14] = {
    5,
    0x15,
    5,
    0x16,
    5,
    0x17,
    5,
    0x18,
    5,
    0x19,
    5,
    0x1A,
    5,
    0x1B,
    5,
    0x1C,
    0,
    0,
    0,
    0,
};
u8 D_us_80180638[8] = { 4, 0x1D, 0x15, 0x1E, 0xFF, 0, 0, 0 };
u8 D_us_80180640[0x24] = {
    4,
    0x20,
    0x11,
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
    0xC,
    0x28,
    4,
    0x27,
    4,
    0x26,
    4,
    0x25,
    4,
    0x24,
    4,
    0x23,
    4,
    0x22,
    4,
    0x21,
    4,
    0x20,
    0xFF,
    0,
};
u8 D_us_80180664[0xC] = { 2, 1, 2, 8, 2, 9, 2, 0xA, 2, 0xB, 0xFF, 0 };
u8 D_us_80180670[0x14] = { 2, 8, 2, 9, 2, 0xA, 2, 0xB, 2, 0xA, 2, 9, 2, 0xA, 2, 0xB, 0, 0, 0, 0 };
u8 D_us_80180684[8] = { 2, 0xC, 2, 0xD, 2, 0xE, 0xFF, 0 };
u8 D_us_8018068C[0x40] = {
    3,
    1,
    3,
    2,
    4,
    3,
    4,
    4,
    4,
    5,
    4,
    6,
    4,
    5,
    4,
    4,
    4,
    5,
    4,
    6,
    4,
    7,
    4,
    8,
    4,
    7,
    4,
    8,
    4,
    9,
    3,
    0xA,
    3,
    0xB,
    3,
    0xC,
    3,
    0xD,
    3,
    0xE,
    3,
    0xF,
    3,
    0x10,
    3,
    0x11,
    3,
    0x12,
    2,
    0x13,
    2,
    0x14,
    2,
    0x15,
    2,
    0x16,
    2,
    0x17,
    2,
    0x18,
    0xFF,
    0,
    0,
    0,
};
s8 D_us_80180704[0x1C] = {
    0,
    0,
    0,
    0,
    4,
    -4,
    4,
    0x24,
    4,
    0,
    4,
    0x20,
    4,
    8,
    4,
    0x18,
    -4,
    0,
    4,
    0x20,
    -4,
    4,
    4,
    0x1C,
    0xC,
    0,
    4,
    0x20,
};
u8 D_us_80180720[0x2C] = {
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    2,
    2,
    2,
    2,
    3,
    3,
    1,
    4,
    4,
    5,
    5,
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
    4,
    2,
    6,
    6,
    2,
    2,
    2,
    2,
    2,
    2,
    0,
    0,
    0,
};
s16 D_us_801806CC[0x1C] = {
    -0x1A,
    -0xE,
    -0x18,
    0,
    -0x20,
    0,
    -0x1E,
    -8,
    -0x10,
    -8,
    -0x12,
    -8,
    -0x10,
    -8,
    -8,
    -8,
    8,
    -8,
    0x10,
    -8,
    0x12,
    -8,
    0x10,
    -8,
    -0x12,
    -8,
    -0x12,
    -8,
};
// end pc_import_data.py
// clang-format on
#endif
