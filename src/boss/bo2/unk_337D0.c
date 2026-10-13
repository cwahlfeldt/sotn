// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo2.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_337D0", func_us_801B37D0);
#else
// Keeps the Werewolf inside the arena
void func_us_801B37D0(void) {
    s32 x = g_CurrentEntity->posX.i.hi + g_Tilemap.scrollX.i.hi;

    if (g_CurrentEntity->velocityX > 0 && x + 8 > 0x1B0) {
        g_CurrentEntity->posX.i.hi = 0x1A8 - g_Tilemap.scrollX.i.hi;
    }
    if (g_CurrentEntity->velocityX < 0 && x - 8 < 0x50) {
        g_CurrentEntity->posX.i.hi = 0x58 - g_Tilemap.scrollX.i.hi;
    }
}
#endif

extern s16 D_us_80181850[];
extern s16 D_us_80181858[];
extern u8 D_us_80181868[];
extern u8 D_us_80181874[];
extern u8 D_us_80181884[];
extern u8 D_us_801818A4[];
extern u8 D_us_801818AC[];
extern u8 D_us_801818BC[];
extern u8 D_us_801818D0[];
extern u8 D_us_801818D8[];
extern u8 D_us_801818E0[];
extern u8 D_us_801818EC[];
extern u8 D_us_801818FC[];
extern u8 D_us_80181908[];
extern u8 D_us_80181930[];
extern s8 D_us_80181958[][4];
extern u8 D_us_801819F8[];
extern s32 D_us_80180AAC;
extern s32 D_us_80180AB0;
extern EInit g_EInitWerewolfARE;

#ifdef NON_MATCHING
static void WerewolfAfterImage(Entity* self) {
    Entity* entity = AllocEntity(&g_Entities[224], &g_Entities[256]);

    if (entity != NULL) {
        CreateEntityFromEntity(E_WEREWOLF_SPIN_ATTACK_AFTER_IMAGE, self, entity);
        entity->facingLeft = self->facingLeft;
        entity->rotate = self->rotate;
        entity->params = self->animCurFrame;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_337D0", func_us_801B385C);
#else
// The Werewolf. The fight controller (func_us_801A3E04) picks its moves; the
// Minotaur sits three entities after it.
void func_us_801B385C(Entity* self) {
    Entity* minotaur = self + 3;
    Entity* entity;
    s8* hitbox;
    s32 velX;
    s32 x;
    s32 arrived;
    s32 angle;

    FntPrint("wolf_step %x\n", self->step);
    FntPrint("wolf_set_step %x\n", self->ext.minotaurWerewolf.nextStep);
    if ((self->flags & FLAG_DEAD) && self->step < 0x20) {
        SetStep(0x20);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitWerewolfARE);
        self->hitboxState = 0;
        self->zPriority += 8;
        CreateEntityFromCurrentEntity(E_WEREWOLF_ATTACK_HITBOX, self + 1);
        // fallthrough
    case 1:
        AnimateEntity(D_us_80181868, self);
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        UnkCollisionFunc3(D_us_80181858);
        switch (self->step_s) {
        case 0:
            self->palette = 0x815F;
            self->blendMode = 0x50;
            self->opacity = 0;
            self->drawFlags |= ENTITY_OPACITY;
            self->step_s++;
            // fallthrough
        case 1:
            self->opacity += 4;
            if (self->opacity == 0x80) {
                self->blendMode = 0;
                self->opacity = 0;
                self->palette = g_EInitWerewolfARE[3];
                self->step_s++;
            }
            break;
        case 2:
            self->opacity += 4;
            if (self->opacity == 0x80) {
                self->drawFlags = 0;
                D_us_80180AB0 = 1;
                self->step_s++;
            }
            break;
        case 3:
            if (D_us_80180AAC) {
                self->hitboxState = 3;
                SetStep(2);
            }
            break;
        }
        break;

    case 2:
        if (!self->step_s) {
            if (!self->ext.minotaurWerewolf.idleTimer) {
                self->ext.minotaurWerewolf.idleTimer = 0x20;
            }
            self->ext.minotaurWerewolf.isReady = 0;
            self->step_s++;
        }
        AnimateEntity(D_us_80181868, self);
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        if (self->ext.minotaurWerewolf.idleTimer) {
            self->ext.minotaurWerewolf.idleTimer--;
        } else {
            self->ext.minotaurWerewolf.isReady = 1;
        }
        if (self->ext.minotaurWerewolf.targetX) {
            self->ext.minotaurWerewolf.isReady = 0;
            SetStep(4);
        } else if (self->ext.minotaurWerewolf.hasCommand) {
            self->ext.minotaurWerewolf.isReady = 0;
            self->ext.minotaurWerewolf.hasCommand = 0;
            SetStep(self->ext.minotaurWerewolf.nextStep);
        } else {
            velX = PLAYER.velocityX;
            if (self->facingLeft) {
                velX = -velX;
            }
            if (self->facingLeft == PLAYER.facingLeft &&
                GetDistanceToPlayerX() < 0x38 && velX > 0) {
                self->ext.minotaurWerewolf.isReady = 0;
                SetStep(6);
            }
        }
        break;

    case 4:
        // targetX starts as a distance from the player
        if (!self->step_s) {
            if (PLAYER.posX.i.hi < self->posX.i.hi) {
                x = self->ext.minotaurWerewolf.targetX + PLAYER.posX.i.hi +
                    g_Tilemap.scrollX.i.hi;
                if (x > 0x190) {
                    x = 0x190;
                }
            } else {
                x = PLAYER.posX.i.hi - self->ext.minotaurWerewolf.targetX +
                    g_Tilemap.scrollX.i.hi;
                if (x < 0x70) {
                    x = 0x70;
                }
            }
            self->ext.minotaurWerewolf.targetX = x;
            if (x - (self->posX.i.hi + g_Tilemap.scrollX.i.hi) > 0) {
                self->velocityX = FIX(1.125);
            } else {
                self->velocityX = FIX(-1.125);
            }
            self->step_s++;
        }
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        AnimateEntity(D_us_80181874, self);
        UnkCollisionFunc2(D_us_80181850);
        x = self->posX.i.hi + g_Tilemap.scrollX.i.hi -
            self->ext.minotaurWerewolf.targetX;
        arrived = false;
        if (self->velocityX > 0) {
            arrived = x > 0;
        } else if (x < 0) {
            arrived = true;
        }
        if (arrived) {
            self->ext.minotaurWerewolf.targetX = 0;
            SetStep(2);
        }
        break;

    case 6:
        if (!AnimateEntity(D_us_80181884, self)) {
            SetStep(2);
        }
        if (!self->poseTimer && self->pose == 6) {
            PlaySfxPositional(SFX_WHIP_TWIRL_SWISH);
        }
        break;

    case 8:
        switch (self->step_s) {
        case 0:
            x = minotaur->posX.i.hi - self->posX.i.hi;
            if (self->facingLeft == minotaur->facingLeft && abs(x) < 0x20) {
                self->ext.minotaurWerewolf.partnerSignal = 1;
            } else {
                self->ext.minotaurWerewolf.partnerSignal = 0;
            }
            minotaur->ext.minotaurWerewolf.partnerSignal =
                self->ext.minotaurWerewolf.partnerSignal ^ 1;
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_801818A4, self)) {
                self->animCurFrame = 0x2F;
                self->velocityX = FIX(-2);
                if (self->facingLeft) {
                    self->velocityX = FIX(2);
                }
                self->velocityY = FIX(-2);
                SetSubStep(3);
            }
            break;
        case 3:
            if (UnkCollisionFunc3(D_us_80181858) & 1) {
                self->step_s++;
            }
            break;
        case 4:
            if (!AnimateEntity(D_us_801818AC, self)) {
                SetStep(4);
            }
            if (!self->poseTimer && self->pose == 4) {
                PlaySfxPositional(SFX_EXPLODE_F);
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_UNK_1D, self, entity);
                    entity->ext.werewolfProjectile.minotaur = minotaur;
                    entity->facingLeft = self->facingLeft;
                    entity->posY.i.hi -= 0x10;
                    entity->ext.werewolfProjectile.partnerSignal =
                        self->ext.minotaurWerewolf.partnerSignal;
                }
            }
            break;
        }
        break;

    case 0xA:
        if (!self->step_s) {
            self->velocityX = 0;
            self->velocityY = 0;
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            self->step_s++;
        }
        UnkCollisionFunc2(D_us_80181850);
        if (!AnimateEntity(D_us_801818BC, self)) {
            SetStep(2);
        }
        self->velocityX -= self->velocityX / 16;
        if (!self->poseTimer && self->pose == 3) {
            PlaySfxPositional(SFX_SCRAPE_B);
            if (self->facingLeft) {
                self->velocityX = FIX(4);
            } else {
                self->velocityX = FIX(-4);
            }
        }
        break;

    case 0xC:
        if (!self->step_s) {
            if (!AnimateEntity(D_us_801818D0, self)) {
                self->ext.minotaurWerewolf.timer = 0x80;
                self->step_s++;
            }
        } else {
            AnimateEntity(D_us_801818D8, self);
            if (!--self->ext.minotaurWerewolf.timer) {
                SetStep(0xE);
            }
        }
        break;

    case 0xE:
        if (self->flags & 0xF) {
            self->blendMode = 0;
        } else if (g_Timer & 1) {
            self->blendMode = 0x30;
        } else {
            self->blendMode = 0x70;
        }
        switch (self->step_s) {
        case 0:
            self->velocityX = 0;
            self->velocityY = 0;
            self->ext.minotaurWerewolf.partnerSignal = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (self->ext.minotaurWerewolf.partnerSignal) {
                self->step_s++;
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_801818E0, self)) {
                SetSubStep(3);
            }
            if (!self->poseTimer && self->pose == 1) {
                if (self->facingLeft) {
                    self->velocityX = FIX(6);
                } else {
                    self->velocityX = FIX(-6);
                }
                self->ext.minotaurWerewolf.timer = 0x30;
            }
            break;
        case 3:
            UnkCollisionFunc2(D_us_80181850);
            self->velocityX -= self->velocityX / 64;
            self->animCurFrame = 0x2B;
            if (!--self->ext.minotaurWerewolf.timer) {
                SetSubStep(4);
            }
            break;
        case 4:
            UnkCollisionFunc2(D_us_80181850);
            self->velocityX -= self->velocityX / 16;
            if (!AnimateEntity(D_us_801818EC, self)) {
                self->blendMode = 0;
                SetStep(4);
            }
            break;
        }
        break;

    case 0x12:
        switch (self->step_s) {
        case 0:
            if (self->facingLeft) {
                self->velocityX = FIX(3.5);
            } else {
                self->velocityX = FIX(-3.5);
            }
            self->ext.minotaurWerewolf.timer = 0x20;
            self->animCurFrame = 0x17;
            self->velocityY = 0;
            self->drawFlags |= ENTITY_ROTATE;
            self->step_s++;
            // fallthrough
        case 1:
            UnkCollisionFunc2(D_us_80181850);
            self->rotate -= 0x100;
            if (!(g_Timer & 8)) {
                PlaySfxPositional(SFX_WEREWOLF_SPIN_ATTACK);
            }
            if (!(self->ext.minotaurWerewolf.timer & 3)) {
                WerewolfAfterImage(self);
            }
            if (!--self->ext.minotaurWerewolf.timer) {
                self->step_s = 2;
            }
            break;
        case 2:
            self->drawFlags = 0;
            SetStep(4);
            break;
        }
        break;

    case 0x16:
        // spin at the Minotaur and bounce off it onto the player
        switch (self->step_s) {
        case 0:
            self->ext.minotaurWerewolf.partnerSignal = 0;
            if (minotaur->posX.i.hi < self->posX.i.hi) {
                self->facingLeft = 0;
            } else {
                self->facingLeft = 1;
            }
            if (self->facingLeft) {
                self->velocityX = FIX(3.5);
            } else {
                self->velocityX = FIX(-3.5);
            }
            self->velocityY = 0;
            if (minotaur->flags & FLAG_DEAD) {
                SetStep(0xA);
                return;
            }
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_80181930, self)) {
                self->animCurFrame = 0x17;
                self->drawFlags |= ENTITY_ROTATE;
                self->step_s++;
            }
            break;
        case 2:
            UnkCollisionFunc2(D_us_80181850);
            self->rotate -= 0x100;
            if (!(g_Timer & 7)) {
                PlaySfxPositional(SFX_WEREWOLF_SPIN_ATTACK);
            }
            if (!(self->ext.minotaurWerewolf.timer & 3)) {
                WerewolfAfterImage(self);
            }
            x = self->posX.i.hi - minotaur->posX.i.hi;
            if (self->facingLeft) {
                x = -x;
            }
            if (x < 0x20) {
                minotaur->ext.minotaurWerewolf.partnerSignal = 1;
                self->rotate = 0;
                self->step_s++;
            }
            if (minotaur->flags & FLAG_DEAD) {
                SetSubStep(7);
            }
            break;
        case 3:
            self->velocityY = FIX(-8.5);
            self->velocityX = (PLAYER.posX.val - self->posX.val) / 64;
            self->animCurFrame = 8;
            if (self->velocityX > 0) {
                self->facingLeft = 1;
            } else {
                self->facingLeft = 0;
            }
            self->step_s++;
            break;
        case 4:
            MoveEntity();
            self->velocityY += FIX(0.25);
            if (self->posY.i.hi + g_Tilemap.scrollY.i.hi < 0x2C) {
                self->posY.i.hi = 0x2C - g_Tilemap.scrollY.i.hi;
                self->ext.minotaurWerewolf.timer = 8;
                self->step_s++;
            }
            break;
        case 5:
            self->animCurFrame = 0x17;
            self->rotate -= 0x180;
            if (!--self->ext.minotaurWerewolf.timer) {
                self->ext.minotaurWerewolf.timer = 0xC;
                self->step_s++;
            }
            break;
        case 6:
            self->animCurFrame = 0x1B;
            self->rotate = 0;
            if (!--self->ext.minotaurWerewolf.timer) {
                angle = GetAngleBetweenEntities(self, &PLAYER);
                self->velocityX = rcos(angle) * 0x50;
                self->velocityY = rsin(angle) * 0x50;
                self->ext.minotaurWerewolf.savedAttack = self->attack;
                self->attack *= 2;
                self->step_s++;
            }
            break;
        case 7:
            self->animCurFrame = 0x17;
            self->rotate -= 0x180;
            if (!(g_Timer & 7)) {
                PlaySfxPositional(SFX_WEREWOLF_SPIN_ATTACK);
            }
            if (UnkCollisionFunc3(D_us_80181858) & 1) {
                PlaySfxPositional(SFX_STOMP_HARD_B);
                self->ext.minotaurWerewolf.timer = 0x10;
                self->attack = self->ext.minotaurWerewolf.savedAttack;
                self->step_s++;
            }
            if (!(self->ext.minotaurWerewolf.timer & 3)) {
                WerewolfAfterImage(self);
            }
            break;
        case 8:
            self->drawFlags = 0;
            self->animCurFrame = 0xC;
            if (!--self->ext.minotaurWerewolf.timer) {
                SetStep(4);
            }
            break;
        }
        break;

    case 0x18:
        // spin through the Minotaur's breath and carry its flames
        switch (self->step_s) {
        case 0:
            self->ext.minotaurWerewolf.partnerSignal = 0;
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            self->step_s++;
            // fallthrough
        case 1:
            if (self->ext.minotaurWerewolf.partnerSignal) {
                self->step_s++;
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80181930, self)) {
                self->animCurFrame = 0x17;
                self->drawFlags |= ENTITY_ROTATE;
                if (self->facingLeft) {
                    self->velocityX = FIX(3.5);
                } else {
                    self->velocityX = FIX(-3.5);
                }
                self->ext.minotaurWerewolf.timer = 0x40;
                self->step_s++;
            }
            break;
        case 3:
            UnkCollisionFunc2(D_us_80181850);
            self->rotate -= 0x100;
            if (!(g_Timer & 7)) {
                PlaySfxPositional(SFX_WEREWOLF_SPIN_ATTACK);
            }
            if (!(self->ext.minotaurWerewolf.timer & 3)) {
                WerewolfAfterImage(self);
            }
            if (!(minotaur->flags & FLAG_DEAD)) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(
                        E_MINOTAUR_SPIT_LIQUID, self, entity);
                    entity->rotate = self->rotate;
                    angle = self->rotate;
                    if (self->facingLeft) {
                        angle = -angle;
                    }
                    angle = (s16)angle;
                    entity->posX.val += rsin(angle) * 0xC0;
                    entity->posY.val -= rcos(angle) * 0xC0;
                    entity->facingLeft = self->facingLeft;
                    entity->zPriority = self->zPriority + 1;
                }
            }
            if (!--self->ext.minotaurWerewolf.timer) {
                self->step_s++;
            }
            break;
        case 4:
            self->drawFlags = 0;
            SetStep(4);
            break;
        }
        break;

    case 0x14:
        // spin up, drop onto the player and raise a fire pillar
        if (self->flags & 0xF) {
            self->blendMode = 0;
        } else if (g_Timer & 1) {
            self->blendMode = 0x30;
        } else {
            self->blendMode = 0x70;
        }
        switch (self->step_s) {
        case 0:
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            self->ext.minotaurWerewolf.partnerSignal = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_80181930, self)) {
                self->step_s++;
            }
            break;
        case 2:
            self->velocityY = FIX(-4);
            self->velocityX = 0;
            self->drawFlags = ENTITY_ROTATE;
            self->step_s++;
            // fallthrough
        case 3:
            MoveEntity();
            self->rotate -= 0x100;
            if (!(g_Timer & 7)) {
                PlaySfxPositional(SFX_WEREWOLF_SPIN_ATTACK);
            }
            self->animCurFrame = (g_Timer & 1) ? 0x18 : 0x19;
            if (!(self->ext.minotaurWerewolf.timer & 3)) {
                WerewolfAfterImage(self);
            }
            if (self->posY.i.hi + g_Tilemap.scrollY.i.hi < 0x28) {
                self->step_s++;
            }
            break;
        case 4:
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            if (self->facingLeft) {
                self->velocityX = FIX(2.5);
            } else {
                self->velocityX = FIX(-2.5);
            }
            self->velocityY = 0;
            self->step_s++;
            // fallthrough
        case 5:
            MoveEntity();
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            if (self->facingLeft) {
                self->velocityX = FIX(2.5);
            } else {
                self->velocityX = FIX(-2.5);
            }
            self->rotate -= 0x100;
            if (!(g_Timer & 7)) {
                PlaySfxPositional(SFX_WEREWOLF_SPIN_ATTACK);
            }
            self->animCurFrame = (g_Timer & 1) ? 0x18 : 0x19;
            if (self->ext.minotaurWerewolf.partnerSignal) {
                self->animCurFrame = 0xA;
                self->drawFlags = 0;
                self->velocityX = 0;
                self->velocityY = FIX(2);
                self->step_s++;
            }
            break;
        case 6:
            if (UnkCollisionFunc3(D_us_80181858) & 1) {
                self->animCurFrame = 0xB;
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_UNK_1F, self, entity);
                    entity->posY.i.hi += 0x1B;
                    entity->zPriority = self->zPriority - 1;
                }
                PlaySfxPositional(SFX_FIREBALL_SHOT_A);
                self->ext.minotaurWerewolf.timer = 0x40;
                self->step_s++;
            }
            break;
        case 7:
            if (!--self->ext.minotaurWerewolf.timer) {
                self->blendMode = 0;
                SetStep(4);
            }
            break;
        }
        break;

    case 0x20:
        self->ext.minotaurWerewolf.isReady = 1;
        minotaur->ext.minotaurWerewolf.partnerSignal = 1;
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            self->rotate = 0;
            self->velocityX = 0;
            self->velocityY = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (UnkCollisionFunc3(D_us_80181858) & 1) {
                self->step_s++;
            }
            // fallthrough
        case 2:
            if (!AnimateEntity(D_us_801818FC, self)) {
                PlaySfxPositional(SFX_EXPLODE_A);
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(
                        E_WEREWOLF_DEATH_FLAMES, self, entity);
                    entity->posY.i.hi += 0x1B;
                    entity->zPriority = self->zPriority;
                }
                SetSubStep(3);
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80181908, self)) {
                SetSubStep(4);
            }
            break;
        case 4:
            self->ext.minotaurWerewolf.targetX = 0;
            self->animCurFrame = 0;
            break;
        }
        break;

    case 0xFF:
#include "../../st/pad2_anim_debug.h"
    }

    hitbox = D_us_80181958[D_us_801819F8[self->animCurFrame]];
    self->hitboxOffX = *hitbox++;
    self->hitboxOffY = *hitbox++;
    self->hitboxWidth = *hitbox++;
    self->hitboxHeight = *hitbox++;
    func_us_801B37D0();
}
#endif

#include "e_werewolf_attack_hitbox.h"

extern EInit D_us_80180578;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_337D0", func_us_801B503C);
#else
// Thrown by the Werewolf; the Minotaur can bat it back
void func_us_801B503C(Entity* self) {
    Entity* minotaur;
    s32 velX;
    s32 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180578);
        self->animCurFrame = 0x39;
        if (self->facingLeft) {
            self->velocityX = FIX(4.5);
        } else {
            self->velocityX = FIX(-4.5);
        }
        minotaur = self->ext.werewolfProjectile.minotaur;
        self->velocityY = 0;
        velX = minotaur->velocityX;
        if (self->facingLeft) {
            velX = -velX;
        }
        if (velX > 0 || (minotaur->flags & FLAG_DEAD)) {
            self->ext.werewolfProjectile.partnerSignal = 0;
            minotaur->ext.minotaurWerewolf.partnerSignal = 1;
        }
        // fallthrough
    case 1:
        MoveEntity();
        self->velocityY -= FIX(0.078125);
        angle = ratan2(-self->velocityY, abs(self->velocityX));
        if (angle > 0x200) {
            self->animCurFrame = 0x3A;
        }
        if (angle > 0x280) {
            self->animCurFrame = 0x3B;
        }
        if (angle > 0x300) {
            self->animCurFrame = 0x3C;
        }
        if (angle > 0x380) {
            self->animCurFrame = 0x3D;
        }
        if (!self->ext.werewolfProjectile.partnerSignal) {
            return;
        }
        PlaySfxPositional(SFX_ANIME_SWORD_B);
        minotaur = self->ext.werewolfProjectile.minotaur;
        if (abs(self->posX.i.hi - minotaur->posX.i.hi) < 0x10) {
            minotaur->ext.minotaurWerewolf.partnerSignal = 1;
            self->velocityY = FIX(2);
            self->animCurFrame = 0x39;
            self->facingLeft ^= 1;
            self->velocityX *= -2;
            self->step++;
        }
        break;
    case 2:
        MoveEntity();
        break;
    }
}
#endif

#include "e_werewolf_after_image.h"

extern EInit D_us_80180584;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_337D0", func_us_801B52FC);
#else
// A pillar of fire from where the Werewolf lands; the first one raises two
// smaller ones beside it
void func_us_801B52FC(Entity* self) {
    Primitive* prim;
    Entity* entity;
    s32 primIndex;
    s32 speed;
    s32 i;
    s16 offsetX;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180584);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 3);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->tpage = 0x16;
            prim->clut = 0x211;
            prim->u0 = prim->u2 = 0x80;
            prim->u1 = prim->u3 = 0xA0;
            prim->v0 = prim->v1 = prim->v2 = prim->v3 = 0;
            if (!self->params) {
                prim->x0 = prim->x2 = self->posX.i.hi - 0x18;
                prim->x1 = prim->x3 = self->posX.i.hi + 0x18;
            } else {
                prim->x0 = prim->x2 = self->posX.i.hi - 0xC;
                prim->x1 = prim->x3 = self->posX.i.hi + 0xC;
            }
            prim->y0 = prim->y1 = prim->y2 = prim->y3 = self->posY.i.hi;
            prim->drawMode = DRAW_UNK02;
            prim->priority = self->zPriority;
            prim = prim->next;
        }
        if (!self->params) {
            for (i = 0, offsetX = -0x24; i < 2; i++, offsetX += 0x48) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_UNK_1F, self, entity);
                    entity->params = 1;
                    entity->posX.i.hi += offsetX;
                }
            }
        }
        // fallthrough
    case 1:
        prim = self->ext.prim;
        speed = self->params ? 2 : 4;
        prim->v2 = prim->v3 = prim->v3 + speed;
        prim->y0 = prim->y1 = prim->y1 - speed * 2;
        if (prim->v2 - prim->v0 >= 0x40) {
            self->step++;
        }
        break;
    case 2:
        prim = self->ext.prim;
        speed = self->params ? 2 : 4;
        prim->y0 = prim->y1 = prim->y1 + speed;
        prim->v2 = prim->v3 = prim->v3 - speed / 2;
        if (prim->y2 - prim->y0 <= 0) {
            self->step++;
        }
        break;
    case 3:
        // the original goes on to read the destroyed entity's prims
        DestroyEntity(self);
        return;
    }

    prim = self->ext.prim;
    self->hitboxWidth = (abs(prim->x0 - prim->x1) - 8) / 2;
    self->hitboxHeight = abs(prim->y0 - prim->y2) * 3 / 10;
    self->hitboxOffY = 0xE - self->hitboxHeight;
    if (g_Timer & 1) {
        prim->u0 = prim->u2 = 0x80;
        prim->u1 = prim->u3 = 0xA0;
    } else {
        prim->u0 = prim->u2 = 0xA0;
        prim->u1 = prim->u3 = 0x80;
    }
}
#endif

extern SVECTOR D_us_80181ACC[];
// sprite bank 4
extern s16* sprites_bo2_3[];
Primitive* FindFirstUnkPrim2(Primitive* prim, u8 index);
void UnkPolyFunc2(Primitive* prim);
void UnkPolyFunc0(Primitive* prim);

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_337D0", EntityWerewolfDeathFlames);
#else
// RARE's death flames with a longer burn and a faster fade
void EntityWerewolfDeathFlames(Entity* self) {
    long otz;
    SVECTOR svecTwo;
    VECTOR vec;
    MATRIX matrix;
    Primitive* prim;
    s16* ptr;
    s32 posY;
    s32 i;
    SVECTOR* sVec;
    s32 color;
    s32 primIndex;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x80);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.werewolf.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->tpage = 0x17;
            prim->clut = 0x223;
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority;
            prim = prim->next;
        }
        self->ext.werewolf.timer = 0x140;
        // fallthrough
    case 1:
        if (!--self->ext.werewolf.timer) {
            DestroyEntity(self);
            return;
        }
        if (!(self->ext.werewolf.timer & 0x1F)) {
            PlaySfxPositional(SFX_FIREBALL_SHOT_B);
        }
        if (!(self->ext.werewolf.timer & 3)) {
            self->ext.werewolf.unk9C -= 0x20;
            color = self->ext.werewolf.timer;
            if (color > 0x80) {
                color = 0x80;
            }
            for (i = 0; i < 3; i++) {
                prim = FindFirstUnkPrim2(self->ext.werewolf.prim, 2);
                if (prim != NULL) {
                    UnkPolyFunc2(prim);
                    prim->next->x2 = 0;
                    prim->next->y2 = 0;
                    prim->next->x1 =
                        self->ext.werewolf.unk9C + (i * 0x555) + Random();
                    prim->next->y1 = 0;
                    PGREY(prim, 0) = PGREY(prim, 1) = PGREY(prim, 2) =
                        PGREY(prim, 3) = color;
                    prim->next->drawMode = DRAW_HIDE;
                }
            }
        }

        SetGeomScreen(0x200);
        SetGeomOffset(self->posX.i.hi, self->posY.i.hi);
        prim = self->ext.werewolf.prim;
        sVec = D_us_80181ACC;
        posY = self->posY.i.hi + 8;
        while (prim != NULL) {
            if (prim->p3 == 8) {
                vec.vx = 0;
                vec.vy = prim->next->y1;
                vec.vz = 0x200;
                TransMatrix(&matrix, &vec);
                SetTransMatrix(&matrix);
                svecTwo.vx = 0;
                svecTwo.vy = prim->next->x1;
                svecTwo.vz = 0x180;
                RotMatrix(&svecTwo, &matrix);
                SetRotMatrix(&matrix);
                gte_ldv3c(sVec);
                gte_rtpt();
                gte_stsxy3_gt3(prim);
                gte_ldv0(&sVec[3]);
                gte_rtps();
                gte_stsxy((long*)&prim->x3);
                gte_avsz4();
                gte_stszotz(&otz);
                if (otz > 0x80) {
                    prim->priority = self->zPriority - 1;
                } else {
                    prim->priority = self->zPriority + 1;
                }
                if (posY < prim->y2) {
                    prim->y2 = posY;
                }
                if (posY < prim->y3) {
                    prim->y3 = posY;
                }
                prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                                 DRAW_UNK02 | DRAW_TRANSP;
            }
            prim = prim->next;
        }

        prim = self->ext.werewolf.prim;
        while (prim != NULL) {
            if (prim->p3 == 8) {
                prim->next->y1 -= 3;
                prim->next->x1 += 0x30;
                prim->r0 -= 2;
                if (!prim->r0) {
                    UnkPolyFunc0(prim);
                    continue;
                }
                prim->g0 = prim->b0 = prim->r0;
                PGREY(prim, 1) = PGREY(prim, 2) = PGREY(prim, 3) = prim->r0;
                if (!prim->next->y2) {
                    prim->next->x2++;
                    if (prim->next->x2 > 0xD) {
                        UnkPolyFunc0(prim);
                        continue;
                    }
                    ptr = sprites_bo2_3[prim->next->x2];
                    ptr += 8;
                    prim->u0 = prim->u2 = *ptr++;
                    prim->v0 = prim->v1 = *ptr++;
                    prim->u1 = prim->u3 = *ptr++;
                    prim->v2 = prim->v3 = *ptr++;
                    prim->next->y2 = 3;
                } else {
                    prim->next->y2--;
                }
            }
            prim = prim->next;
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s16 D_us_80181850[4] = { 0, 0x1B, 4, 0 };
s16 D_us_80181858[8] = { 0, 0x1B, 0, 4, 4, -4, -8, 0 };
u8 D_us_80181868[0xC] = { 6, 1, 6, 2, 6, 3, 6, 2, 0, 0, 0, 0 };
u8 D_us_80181874[0x10] = { 4, 4, 4, 5, 6, 6, 4, 5, 6, 4, 0x10, 2, 2, 4, 0, 0 };
u8 D_us_80181884[0x20] = {
    6,
    0xD,
    3,
    0xF,
    5,
    0x11,
    3,
    0xF,
    5,
    0x11,
    6,
    0xF,
    7,
    0xD,
    2,
    0xE,
    2,
    0x10,
    2,
    0x12,
    1,
    0x13,
    1,
    0x14,
    1,
    0x15,
    2,
    0x16,
    2,
    0xD,
    0xFF,
    0,
};
u8 D_us_801818A4[8] = { 4, 1, 3, 0x26, 3, 0x27, 0xFF, 0 };
u8 D_us_801818AC[0x10] = { 8, 0x1C, 6, 0x1D, 7, 0x1E, 6, 0x1F, 9, 0x20, 0xC, 0x21, 0xFF, 0, 0, 0 };
u8 D_us_801818BC[0x14] = {
    3,
    0xC,
    3,
    0x24,
    3,
    0x23,
    0x11,
    0x22,
    9,
    0x23,
    9,
    0x24,
    9,
    0x25,
    5,
    0xC,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801818D0[8] = { 0xC, 2, 6, 0x26, 0xFF, 0, 0, 0 };
u8 D_us_801818D8[8] = { 1, 0x42, 1, 0x27, 0, 0, 0, 0 };
u8 D_us_801818E0[0xC] = { 3, 0x28, 3, 0x29, 2, 0x2A, 0xFF, 0, 4, 0x2B, 0, 0 };
u8 D_us_801818EC[0x10] = { 3, 0x2A, 4, 0x29, 3, 0x2C, 4, 0x2D, 5, 0x2E, 4, 0x2F, 0x1B, 0xC, 0xFF, 0 };
u8 D_us_801818FC[0xC] = { 0xF, 0xC, 5, 0x27, 5, 0x26, 4, 0xD, 0x2A, 0x30, 0xFF, 0 };
u8 D_us_80181908[0x28] = {
    7,
    0x30,
    3,
    0x31,
    5,
    0x30,
    6,
    0x31,
    0xB,
    0x32,
    0xA,
    0x37,
    4,
    0x33,
    0x18,
    0x37,
    9,
    0x33,
    8,
    0x34,
    6,
    0x35,
    0x4D,
    0x36,
    0xFF,
    0,
    0,
    0,
    3,
    0x3E,
    2,
    0x3F,
    2,
    0x40,
    2,
    0x41,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80181930[0x28] = {
    4,
    0xC,
    2,
    0xB,
    2,
    0xA,
    1,
    9,
    0xFF,
    0,
    0,
    0,
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
s8 D_us_80181958[0x28][4] = {
    { 0, 0, 0, 0 },
    { 0, 7, 0xA, 0x12 },
    { 0, 6, 7, 0x12 },
    { 0, 6, 6, 0x13 },
    { 0, 5, 6, 0x14 },
    { -1, 4, 5, 0x14 },
    { 0, 3, 4, 0x15 },
    { 0, -1, 7, 0x10 },
    { 1, -3, 8, 0xE },
    { -5, 6, 8, 0xF },
    { 2, 0xC, 9, 0xC },
    { 1, 7, 8, 0x11 },
    { 0, 8, 8, 0x11 },
    { -1, 8, 8, 0x11 },
    { 0, 1, 7, 8 },
    { 0, 3, 0xD, 0xC },
    { 0, 2, 0xC, 0xF },
    { 0, 5, 8, 0x16 },
    { 6, 8, 0xA, 0x10 },
    { 7, 0xE, 7, 0x11 },
    { -7, 0xD, 7, 0x10 },
    { -4, 2, 0xA, 0xB },
    { -4, 4, 9, 0xB },
    { 0xA, 0xF, 0xA, 0xD },
    { 7, 0xE, 7, 0xC },
    { 5, 0xD, 9, 0xC },
    { 1, 0xC, 9, 0xC },
    { 3, 6, 7, 0x12 },
    { 2, 0xB, 7, 0xF },
    { 2, 1, 0xB, 0xE },
    { 1, -4, 0xE, 8 },
    { 6, 0, 0x12, 0xA },
    { 5, -5, 0x13, 0xA },
    { 5, 0, 0xD, 7 },
    { 0, 0, 8, 9 },
    { -1, 2, 6, 0xD },
    { -0x10, -0x15, 0, 0 },
    { -0x10, -0x2E, 0, 0 },
    { -0x11, -0xA, 0, 0 },
    { -0x10, -0xF, 0, 0 },
};
u8 D_us_801819F8[0x44] = {
    0,
    1,
    1,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    0xA,
    0xB,
    0xB,
    0xB,
    0xB,
    0xC,
    0xD,
    0xD,
    0xD,
    0xD,
    0xD,
    0xE,
    0xE,
    0xE,
    0xF,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1A,
    0x1B,
    0x1C,
    0x1D,
    0x1E,
    0x1F,
    0x20,
    0x21,
    0x22,
    0x22,
    0x23,
    0x24,
    0x24,
    0x24,
    0x24,
    0x24,
    0x24,
    0x24,
    0x24,
    0x25,
    0x26,
    0x26,
    0x26,
    0x26,
    0x26,
    0x27,
    0x27,
    0x27,
    0x27,
    0x1C,
    0,
};
SVECTOR D_us_80181ACC[4] = {
    { -0x10, -0x18, -0x12, 0 },
    { 0x10, -0x18, -0x12, 0 },
    { -0x10, 0x18, -0x12, 0 },
    { 0x10, 0x18, -0x12, 0 },
};
u8 werewolf_attack_hitbox_indices[0x44] = {
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
    2,
    1,
    1,
    1,
    1,
    1,
    1,
    3,
    4,
    5,
    6,
    7,
    8,
    1,
    1,
    1,
    1,
    1,
    1,
    9,
    0xA,
    0xB,
    0xC,
    0xD,
    0xE,
    0xF,
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
    0x10,
    0x11,
    0x11,
    0x11,
    0x11,
    0x11,
    0x12,
    0x12,
    0x12,
    0x12,
    1,
    0,
};
s8 werewolf_attack_hitboxes[0x13][4] = {
    { 0, 0, 0, 0 },
    { -0x10, -0x15, 0, 0 },
    { 6, 0x18, 4, 8 },
    { -0x13, -3, 7, 4 },
    { -0x13, -3, 6, 9 },
    { -0x14, -3, 0xA, 7 },
    { -0x1C, -3, 0xC, 4 },
    { -0x20, -3, 0x12, 4 },
    { -0x30, -3, 7, 4 },
    { 0x19, 8, 6, 9 },
    { 0x14, -6, 8, 8 },
    { -0x19, 0xE, 4, 7 },
    { 0xA, 0x16, 4, 5 },
    { 0xA, 0x17, 4, 6 },
    { 1, 0x17, 0x18, 4 },
    { -8, 0x17, 0x15, 4 },
    { -0x10, -0x2E, 0, 0 },
    { -0x11, -0xA, 0, 0 },
    { -0x10, -0xF, 0, 0 },
};
// end pc_import_data.py
// clang-format on
#endif
