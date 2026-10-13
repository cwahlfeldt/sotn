// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo4.h"

extern u8* D_us_801805EC[];
extern u8 D_us_8018060C[];
extern u8 D_us_80180614[];
extern u16 D_us_8018061C[];
extern u8 D_us_8018062C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo4/nonmatchings/unk_17804", EntityBreakable);
#else
// RBO4's breakable tables have no names yet
#define g_eBreakableAnimations D_us_801805EC
#define g_eBreakableHitboxes D_us_8018060C
#define g_eBreakableExplosionTypes D_us_80180614
#define g_eBreakableanimSets D_us_8018061C
#define blend_modes D_us_8018062C
#include "../../st/e_breakable.h"
#undef g_eBreakableAnimations
#undef g_eBreakableHitboxes
#undef g_eBreakableExplosionTypes
#undef g_eBreakableanimSets
#undef blend_modes
#endif

extern EInit g_EInitTheCreature;
extern s16 D_us_8018063C[];
extern s16 D_us_80180654[];
extern s16 D_us_8018065C[];
extern u8 D_us_8018067C[];
extern u8 D_us_8018068C[];
extern u8 D_us_8018069C[];
extern u8 D_us_801806C0[];
extern u8 D_us_801806D0[];
extern u8 D_us_801806D8[];
extern u8 D_us_801806E0[];
extern u8 D_us_801806EC[];
extern s8 D_us_801806F4[];
extern u8 D_us_80180710[];
extern s32 D_us_801807EC;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo4/nonmatchings/unk_17804", func_us_80197938);
#else
// The Creature
void func_us_80197938(Entity* self) {
    Entity* entity;
    s8* hitbox;
    s32 collision;
    s32 velY;
    s32 x;

    if (self->flags & FLAG_DEAD) {
        if (self->step != 6) {
            SetStep(6);
        }
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitTheCreature);
        // his hammer
        CreateEntityFromCurrentEntity(E_ID(UNK_17), self + 1);
        SetStep(1);
        // fallthrough
    case 1:
        AnimateEntity(D_us_8018067C, self);
        self->hitboxState = 0;
        if (D_us_801807EC & 1) {
            self->hitboxState = 3;
            SetStep(2);
        }
        break;
    case 2:
        // walk towards Alucard, back off when he is close
        if (!self->step_s) {
            self->ext.theCreature.timer = 0x40;
            self->step_s++;
        }
        AnimateEntity(D_us_8018068C, self);
        collision = UnkCollisionFunc2(D_us_8018063C);
        if (collision & 0x60) {
            self->posX.val -= self->velocityX;
            self->velocityX = 0;
        }
        if (GetDistanceToPlayerX() < 0x68) {
            self->ext.theCreature.backOff = 1;
        }
        if (GetDistanceToPlayerX() > 0x80) {
            self->ext.theCreature.backOff = 0;
        }
        if (self->facingLeft == self->ext.theCreature.backOff) {
            self->velocityX = FIX(0.5);
        } else {
            self->velocityX = FIX(-0.5);
        }
        if (!--self->ext.theCreature.timer) {
            if (GetDistanceToPlayerX() < 0x50) {
                SetStep(3);
                if (collision & 0xE0) {
                    SetStep(4);
                }
            } else {
                SetStep(4);
            }
        }
        if (self->facingLeft != (GetSideToPlayer() & 1)) {
            SetStep(5);
        }
        break;
    case 5:
        // turn around
        if (!AnimateEntity(D_us_801806E0, self)) {
            SetStep(2);
        }
        if (!self->poseTimer && self->pose == 2) {
            self->facingLeft ^= 1;
        }
        break;
    case 3:
        // hammer slam
        if (!self->step_s) {
            if (!AnimateEntity(D_us_8018069C, self)) {
                g_api.ShakeCamera(1);
                PlaySfxPositional(0x7EA);
                self->ext.theCreature.timer = 0x20;
                self->step_s++;
            }
            if (!self->poseTimer && self->pose == 4) {
                PlaySfxPositional(0x7EB);
            }
        } else if (!--self->ext.theCreature.timer) {
            SetStep(2);
        }
        break;
    case 4:
        // curl up and roll, bouncing off the walls
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_801806D0, self)) {
                self->step_s++;
            }
            break;
        case 1:
            self->animCurFrame = 0x1E;
            self->drawFlags = ENTITY_ROTATE;
            self->rotate = 0;
            self->ext.theCreature.timer = 0x200;
            self->step_s++;
            // fallthrough
        case 2:
            if (self->facingLeft) {
                self->velocityX = FIX(-3);
            } else {
                self->velocityX = FIX(3);
            }
            self->rotate += 0xC0;
            collision = UnkCollisionFunc2(D_us_80180654);
            if (!(g_Timer & 0xF)) {
                PlaySfxPositional(0x608);
            }
            if (collision & 0x80) {
                self->velocityX = 0;
                self->velocityY = FIX(-4);
                PlaySfxPositional(0x84C);
                self->step_s++;
            }
            if (self->ext.theCreature.timer) {
                self->ext.theCreature.timer--;
            } else {
                x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
                if (x > 0xC0 && x < 0x140) {
                    self->ext.theCreature.timer = 0x30;
                    SetSubStep(4);
                }
            }
            break;
        case 3:
            self->rotate += 0x100;
            if (UnkCollisionFunc3(D_us_8018065C) & 1) {
                PlaySfxPositional(0x654);
                velY = -self->velocityY;
                velY -= velY >> 1;
                self->velocityY = velY;
                if (abs(velY) < FIX(0.5)) {
                    self->facingLeft ^= 1;
                    self->step_s--;
                }
            }
            break;
        case 4:
            self->rotate -= 0x100;
            self->velocityX -= self->velocityX >> 4;
            UnkCollisionFunc2(D_us_80180654);
            if (!--self->ext.theCreature.timer) {
                self->drawFlags = 0;
                self->step_s++;
            }
            break;
        case 5:
            if (!AnimateEntity(D_us_801806D8, self)) {
                SetStep(2);
            }
            break;
        }
        break;
    case 6:
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            D_us_801807EC |= 2;
            if (self->rotate) {
                self->rotate = 0;
            }
            self->animCurFrame = 1;
            PlaySfxPositional(0x7EC);
            self->step_s++;
            break;
        case 1:
            if (UnkCollisionFunc3(D_us_8018065C) & 1) {
                self->step_s++;
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_801806EC, self)) {
                PlaySfxPositional(0x8B9);
                // throw the hammer
                (self + 1)->ext.theCreature.throwHammer = 1;
                self->step_s++;
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_801806C0, self)) {
                self->ext.theCreature.timer = 0x50;
                PlaySfxPositional(0x7C5);
                self->step_s++;
            }
            break;
        case 4:
            entity = AllocEntity(&g_Entities[192], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_18), self, entity);
                entity->params = 0;
                entity->zPriority = self->zPriority + 2;
                entity->posX.i.hi = entity->posX.i.hi - 0x10 + (Random() & 0x1F);
                entity->posY.i.hi += 0x1C;
            }
            entity = AllocEntity(&g_Entities[192], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_18), self, entity);
                entity->params = 2;
                entity->zPriority = self->zPriority + 2;
                entity->posX.i.hi = entity->posX.i.hi - 0x10 + (Random() & 0x1F);
                entity->posY.i.hi += 0x1C;
            }
            if (!--self->ext.theCreature.timer) {
                self->animCurFrame = 0;
                D_us_801807EC |= 4;
                self->step_s++;
            }
            break;
        }
        break;
    case 0xFF:
#include "../../st/pad2_anim_debug.h"
    }
    hitbox = &D_us_801806F4[D_us_80180710[self->animCurFrame] * 4];
    self->hitboxOffX = *hitbox++;
    self->hitboxOffY = *hitbox++;
    self->hitboxWidth = *hitbox++;
    self->hitboxHeight = *hitbox++;
}
#endif

extern EInit D_us_801804D8;
extern s16 D_us_8018066C[];
extern s8 D_us_80180730[];
extern u8 D_us_80180750[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo4/nonmatchings/unk_17804", func_us_8019818C);
#else
// The Creature's hammer: follows his hands, flies off when he dies
void func_us_8019818C(Entity* self) {
    Entity* creature;
    Entity* entity;
    s8* hitbox;

    creature = self - 1;
    FntPrint("step %x\n", self->step);
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804D8);
        // fallthrough
    case 1:
        self->posX.i.hi = creature->posX.i.hi;
        self->posY.i.hi = creature->posY.i.hi;
        self->facingLeft = creature->facingLeft;
        hitbox = &D_us_80180730[D_us_80180750[creature->animCurFrame] * 4];
        self->hitboxOffX = *hitbox++;
        self->hitboxOffY = *hitbox++;
        self->hitboxWidth = *hitbox++;
        self->hitboxHeight = *hitbox++;
        if (self->ext.theCreature.throwHammer) {
            SetStep(2);
        }
        break;
    case 2:
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 0x1A;
            self->hitboxState = 0;
            self->posX.i.hi = creature->posX.i.hi;
            self->facingLeft = creature->facingLeft;
            self->posY.i.hi = creature->posY.i.hi;
            self->zPriority = creature->zPriority + 1;
            if (self->facingLeft) {
                self->posX.i.hi += 4;
            } else {
                self->posX.i.hi -= 4;
            }
            self->drawFlags = ENTITY_ROTATE;
            self->posY.i.hi -= 0x27;
            self->step_s++;
            // fallthrough
        case 1:
            self->animCurFrame = 0x1A;
            self->rotate += 0x80;
            if (UnkCollisionFunc3(D_us_8018066C) & 1) {
                self->ext.theCreature.timer = 0x30;
                self->step_s++;
            }
            break;
        case 2:
            if (!(self->ext.theCreature.timer & 0xF)) {
                PlaySfxPositional(0x661);
            }
            if (self->ext.theCreature.timer & 1) {
                entity = AllocEntity(&g_Entities[192], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_ID(UNK_18), self, entity);
                    entity->params = 1;
                    entity->zPriority = self->zPriority + 1;
                    entity->posX.i.hi = entity->posX.i.hi - 4 + (Random() & 7);
                    entity->posY.i.hi = entity->posY.i.hi - 4 + (Random() & 7);
                }
            }
            if (!--self->ext.theCreature.timer) {
                self->animCurFrame = 0;
                self->step_s++;
            }
            break;
        }
        break;
    }
    if (creature->entityId != E_ID(UNK_16)) {
        DestroyEntity(self);
    }
}
#endif

extern EInit g_EInitParticle;
extern EntityConfig D_us_801807C8[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo4/nonmatchings/unk_17804", func_us_8019846C);
#else
// dust and debris: params 0 rises, 1 floats up, 2 sprays out and curls
void func_us_8019846C(Entity* self) {
    EntityConfig* config;
    s32 params;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitParticle);
        params = self->params & 0xF;
        config = &D_us_801807C8[params];
        self->palette = config->palette + 0x2E0;
        self->blendMode = config->blendMode;
        self->animSet = config->animSet;
        self->unk5A = config->unk5A;
        self->step = params + 1;
        self->ext.creatureDebris.anim = config->animData;
        if (self->params & 0xFF00) {
            self->zPriority = (self->params & 0xFF00) >> 8;
        }
        if (self->params & 0xF0) {
            self->palette = PAL_FLAG(0x19F);
            self->blendMode = BLEND_TRANSP;
            self->facingLeft = 1;
        }
        return;
    case 1:
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0xC0;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 8) - 0x8000;
            self->velocityY = FIX(-0.75);
            self->ext.creatureDebris.accelY = -(Random() << 4) - 0x4000;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += self->ext.creatureDebris.accelY;
        self->opacity--;
        break;
    case 2:
        if (!self->step_s) {
            self->velocityY = FIX(-0.25);
            self->step_s++;
        }
        MoveEntity();
        break;
    case 3:
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY | ENTITY_ROTATE;
            self->opacity = 0x80;
            self->facingLeft = Random() & 1;
            angle = (rand() & 0x7FF) - 0x400;
            self->rotate = angle;
            if (self->facingLeft) {
                angle = -angle;
            }
            self->velocityX = rsin(angle) * 0x28;
            self->velocityY = -(rcos(angle) * 0x28);
            self->ext.creatureDebris.accel = (Random() << 4) + 0x1000;
            self->step_s++;
        }
        MoveEntity();
        angle = self->rotate;
        self->opacity--;
        if (self->facingLeft) {
            angle = -angle;
        }
        self->velocityX += (rsin(angle) * self->ext.creatureDebris.accel) >> 12;
        self->velocityY += (-self->ext.creatureDebris.accel * rcos(angle)) >> 12;
        break;
    }
    if (!AnimateEntity(self->ext.creatureDebris.anim, self)) {
        DestroyEntity(self);
    }
}
#endif

extern EInit g_EInitInteractable;
extern EInit D_us_801804E4;
extern s32 D_us_80180810;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo4/nonmatchings/unk_17804", func_us_80192B38_from_rbo3);
#else
// starts the fight once Alucard is inside, spawns the round door halves
void func_us_80192B38_from_rbo3(Entity* self) {
    Entity* entity;
    s32 i;
    s32 posX;
    s16 x;
    s16 y;

    switch (self->step) {
    case 0:
        entity = self + 1;
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        self->animCurFrame = 0;
        for (i = 1; i < 2; i++) {
            CreateEntityFromEntity(E_ID(UNK_19), self, entity);
            entity->params = i + 0x100;
            entity++;
            CreateEntityFromEntity(E_ID(UNK_19), self, entity);
            entity->params = i;
            entity++;
        }
        // fallthrough
    case 1:
        posX = PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (posX > 0x60 && posX < 0x1A0) {
            D_us_80180810 = 1;
            D_us_801807EC = 1;
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_THE_CREATURE_DEFEAT, TIMEATTACK_SET_VISITED);
            stopMusicFlag = true;
            currentMusicId = MU_FESTIVAL_OF_SERVANTS;
            self->step++;
        }
        break;
    case 3:
        if (D_us_801807EC & 2) {
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_THE_CREATURE_DEFEAT, TIMEATTACK_SET_RECORD);
            g_api.PlaySfx(SET_UNK_90);
            currentMusicId = MU_FINAL_TOCATTA;
            self->step++;
        }
        break;
    case 4:
        if (D_us_801807EC & 4) {
            self->step++;
        }
        break;
    case 5:
        x = 0x100 - g_Tilemap.scrollX.i.hi;
        y = 0x80 - g_Tilemap.scrollY.i.hi;
        entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_ID(LIFE_UP_SPAWN), self, entity);
            entity->params = 0x12;
            entity->posX.i.hi = x;
            entity->posY.i.hi = y;
            stopMusicFlag = true;
            D_us_80180810 = 0;
            currentMusicId = MU_FINAL_TOCATTA;
            self->step++;
        }
        break;
    case 2:
    case 6:
        if (g_api.func_80131F68() == false) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo4/nonmatchings/unk_17804", func_us_80198A18);
#else
// RNO1's EntityBossRoundDoorHalf
void func_us_80198A18(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804E4);
        self->drawFlags |= ENTITY_ROTATE;
        if ((self->params & 0xF) % 2) {
            self->rotate = ROT(-90);
        } else {
            self->rotate = 0;
        }
        self->zPriority = (0x40 - self->params) & 0xF;
        if (self->params & 0x100) {
            // Lower half
            self->animCurFrame = 0x64;
        } else {
            // Upper half
            self->animCurFrame = 0x62;
        }
        self->ext.et_801BDA0C.unk84 = 0;
        self->ext.et_801BDA0C.unk80 = 0;
        return;

    case 1:
        self->palette = PAL_FLAG(0x4F);
        return;
    }
}
#endif

extern u8 D_us_80180814[];
extern s16 D_us_8018081C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo4/nonmatchings/unk_17804", func_us_801C0B9C_from_no1);
#else
// NO1's func_us_801C0B9C: blocks that fall into place to close the room
// while the fight is on and slide away afterwards
void func_us_801C0B9C_from_no1(Entity* self) {
    Entity* nextEntity;
    s32 i;
    s32 params;
    s32 fgIndex;
    s32 posX;
    s32 posY;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804E4);
        self->animCurFrame = 0;
        self->ext.et_801C0B9C.unk84 = 0;
        if (!self->params) {
            nextEntity = self + 1;
            for (i = 1; i < 4; i++, nextEntity++) {
                CreateEntityFromCurrentEntity(E_ID(UNK_1B), nextEntity);
                nextEntity->params = i;
            }

            for (i = 0; i < 4; i++, nextEntity++) {
                CreateEntityFromCurrentEntity(E_ID(UNK_1B), nextEntity);
                nextEntity->params = i + 0x100;
            }
        }
        self->posY.i.hi = 0x68 - g_Tilemap.scrollY.i.hi;
        if (self->params & 0x100) {
            self->posX.i.hi = 0x1F0 - g_Tilemap.scrollX.i.hi;
        } else {
            self->posX.i.hi = 0x10 - g_Tilemap.scrollX.i.hi;
        }
        break;
    case 1:
        if (D_us_80180810 && !(self->params & 0xFF)) {
            self->step = 2;
        }
        if (self->ext.et_801C0B9C.unk84) {
            self->step = 2;
        }
        break;
    case 2:
        if (!AnimateEntity(D_us_80180814, self)) {
            self->velocityY = FIX(4);
            self->step++;
        }
        break;
    case 3:
        MoveEntity();
        self->velocityY += FIX(0.125);

        params = self->params & 0xFF;
        posY = (((3 - params) * 0x10) + 0x60) + g_Tilemap.scrollY.i.hi;
        if (posY < self->posY.i.hi) {
            if (params != 4) {
                nextEntity = self + 1;
                nextEntity->ext.et_801C0B9C.unk84 = 1;
            }
            self->posY.i.hi = posY;
            fgIndex = 0xC0;
            if (self->params & 0x100) {
                g_api.PlaySfx(0x650);
                fgIndex = 0xDE;
            }
            fgIndex += ((3 - params) << 5);
            (&g_Tilemap.fg[fgIndex])[0] = D_us_8018081C[(7 - params) * 2];
            (&g_Tilemap.fg[fgIndex])[1] = D_us_8018081C[(7 - params) * 2 + 1];
            self->velocityY = 0;
            self->step++;
        }
        break;
    case 4:
        if (!D_us_80180810) {
            self->step++;
        }
        break;
    case 5:
        fgIndex = 0xC0;
        params = self->params & 0xFF;
        if (self->params & 0x100) {
            fgIndex = 0xDE;
        }

        fgIndex += ((3 - params) << 5);
        (&g_Tilemap.fg[fgIndex])[0] = D_us_8018081C[(3 - params) * 2];
        (&g_Tilemap.fg[fgIndex])[1] = D_us_8018081C[(3 - params) * 2 + 1];
        self->step++;
    case 7:
        break;
    case 6:
        if (self->params & 0x100) {
            self->velocityX = FIX(0.5);
        } else {
            self->velocityX = FIX(-0.5);
        }
        MoveEntity();
        posX = g_Tilemap.scrollX.i.hi + self->posX.i.hi;
        if (posX < -0x20 || posX > 0x220) {
            self->step++;
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_801805A4[];
extern u8 D_us_801805AC[];
extern u8 D_us_801805B4[];
extern u8 D_us_801805BC[];
extern u8 D_us_801805C4[];
extern u8 D_us_801805CC[];
extern u8 D_us_801805D4[];
extern u8 D_us_801805DC[];
extern u8 D_us_80180770[];
extern u8 D_us_801807A8[];
u8 D_us_801805A4[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805AC[] ALIGNED4 = {0x04, 0x07, 0x04, 0x08, 0x04, 0x09, 0x00, 0x00};
u8 D_us_801805B4[] ALIGNED4 = {0x04, 0x1D, 0x04, 0x1E, 0x04, 0x1F, 0x00, 0x00};
u8 D_us_801805BC[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805C4[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805CC[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805D4[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805DC[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180770[] ALIGNED4 = {0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0xFF, 0x00, 0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0x00, 0x00};
u8 D_us_801807A8[] ALIGNED4 = {0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x04, 0x02, 0x05, 0x02, 0x06, 0x02, 0x07, 0x02, 0x08, 0x02, 0x09, 0x02, 0x0A, 0x02, 0x0B, 0x02, 0x0C, 0x02, 0x0D, 0x02, 0x0E, 0x00, 0x00, 0x00, 0x00};
u8* D_us_801805EC[8] = {
    D_us_801805A4,
    D_us_801805AC,
    D_us_801805B4,
    D_us_801805BC,
    D_us_801805C4,
    D_us_801805CC,
    D_us_801805D4,
    D_us_801805DC,
};
u8 D_us_8018060C[8] = { 8, 8, 8, 8, 8, 8, 8, 8 };
u8 D_us_80180614[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u16 D_us_8018061C[8] = { 3, 0x8001, 0x8001, 3, 3, 3, 3, 3 };
u8 D_us_8018062C[0x10] = { 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0, 0, 0, 0, 0, 0, 0, 0 };
s16 D_us_8018063C[0xC] = { 0, 0x17, 0x20, 0, 0, 0x17, 0, 4, 8, -4, -0x10, 0 };
s16 D_us_80180654[4] = { 0, 0x17, 4, 0 };
s16 D_us_8018065C[8] = { 0, 0x17, 0, 4, 4, -4, -8, 0 };
s16 D_us_8018066C[8] = { 0, 0xC, 0, 4, 4, -4, -8, 0 };
u8 D_us_8018067C[0x10] = { 6, 1, 6, 2, 6, 3, 6, 4, 6, 3, 6, 2, 0, 0, 0, 0 };
u8 D_us_8018068C[0x10] = { 6, 1, 6, 5, 6, 6, 6, 7, 6, 6, 6, 5, 0, 0, 0, 0 };
u8 D_us_8018069C[0x24] = {
    6,
    8,
    6,
    9,
    6,
    0xA,
    6,
    0xB,
    6,
    0xC,
    6,
    0xD,
    6,
    0xE,
    6,
    0xD,
    6,
    0xC,
    6,
    0xD,
    6,
    0xE,
    6,
    0xD,
    6,
    0xC,
    2,
    0xF,
    2,
    0x10,
    2,
    0x11,
    1,
    0x12,
    0xFF,
    0,
};
u8 D_us_801806C0[0x10] = { 8, 0x15, 0xFF, 0, 8, 0x16, 8, 0x17, 8, 0x18, 0x40, 0x19, 0xFF, 0, 0, 0 };
u8 D_us_801806D0[8] = { 8, 0x1B, 8, 0x1C, 8, 0x1D, 0xFF, 0 };
u8 D_us_801806D8[8] = { 8, 0x1D, 8, 0x1C, 8, 0x1B, 0xFF, 0 };
u8 D_us_801806E0[0xC] = { 8, 0x13, 8, 0x14, 8, 0x14, 8, 0x13, 0xFF, 0, 0, 0 };
u8 D_us_801806EC[8] = { 8, 7, 8, 8, 8, 9, 0xFF, 0 };
s8 D_us_801806F4[0x1C] = {
    0,
    0,
    0,
    0,
    -0xD,
    -0x11,
    0x17,
    0x26,
    0xAE,
    0xA0,
    0,
    0,
    -0xE,
    -0x15,
    0,
    0,
    -0xB,
    -0xB,
    0x15,
    0x21,
    -0xA,
    -8,
    0x14,
    0x1E,
    3,
    -1,
    0x15,
    0x15,
};
u8 D_us_80180710[0x20] = {
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
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
    1,
    4,
    5,
    6,
    0,
};
s8 D_us_80180730[0x20] = {
    0,
    0,
    0,
    0,
    0xAE,
    0xA0,
    0,
    0,
    -0x18,
    0xB7,
    0x24,
    0x16,
    -6,
    0xB3,
    0x25,
    0x11,
    0x1C,
    -0x22,
    0x11,
    0x30,
    0x1F,
    2,
    0xD,
    0x15,
    -0xE,
    -0x15,
    0,
    0,
    3,
    -1,
    0x19,
    0x19,
};
u8 D_us_80180750[0x20] = {
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    3,
    4,
    5,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    6,
    1,
    1,
    1,
    7,
    0,
};
EntityConfig D_us_801807C8[3] = {
    { 0x8004, 0x51, 0xE, 0x30, D_us_80180770 },
    { 0x8005, 0x54, 0xE, 0x30, D_us_801807A8 },
    { 0x8004, 0x51, 0xE, 0x30, D_us_80180770 },
};
s32 D_us_801807EC = 0;
s32 D_us_80180810 = 0;
u8 D_us_80180814[8] = { 1, 0x72, 1, 0x73, 1, 0x74, 0xFF, 0 };
s16 D_us_8018081C[0x10] = {
    0x410,
    0x438,
    0x3FC,
    0x40E,
    0x3BC,
    0x3CA,
    0x3A2,
    0x3A1,
    0x5B5,
    0x5B4,
    0x5B3,
    0x5B2,
    0x5B1,
    0x5B0,
    0x5AF,
    0x5AE,
};
// end pc_import_data.py
// clang-format on
#endif
