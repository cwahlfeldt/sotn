// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo2.h"

extern u8* g_eBreakableAnimations[];
extern u8 g_eBreakableHitboxes[];
extern u8 g_eBreakableExplosionTypes[];
extern u16 g_eBreakableanimSets[];
extern u8 blend_modes[];

#include "../../st/e_breakable.h"

extern EInit D_us_80180524;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_224DC", func_us_801A2610);
#else
// Keeps the Minotaur inside the arena
void func_us_801A2610(void) {
    s32 x = g_CurrentEntity->posX.i.hi + g_Tilemap.scrollX.i.hi;

    if (g_CurrentEntity->velocityX > 0 && x + 0x10 > 0x1B0) {
        g_CurrentEntity->posX.i.hi = 0x1A0 - g_Tilemap.scrollX.i.hi;
    }
    if (g_CurrentEntity->velocityX < 0 && x - 0x10 < 0x50) {
        g_CurrentEntity->posX.i.hi = 0x60 - g_Tilemap.scrollX.i.hi;
    }
}
#endif

extern s16 D_us_801806C8[];
extern s16 D_us_801806D0[];
extern u8 D_us_801806E0[];
extern u8 D_us_801806F4[];
extern u8 D_us_80180704[];
extern u8 D_us_80180714[];
extern u8 D_us_80180724[];
extern u8 D_us_80180734[];
extern u8 D_us_8018073C[];
extern u8 D_us_80180750[];
extern u8 D_us_80180770[];
extern u8 D_us_8018077C[];
extern u8 D_us_80180790[];
extern u8 D_us_80180798[];
extern u8 D_us_801807AC[];
extern u8 D_us_801807B4[];
extern u8 D_us_801807D0[];
extern u8 D_us_801807F4[];
extern u8 D_us_801807FC[];
extern u8 D_us_80180808[];
extern u8 D_us_80180810[];
extern u8 D_us_80180818[];
extern u8 D_us_80180824[];
extern u8 D_us_80180850[];
extern u8 D_us_80180858[];
extern s8 D_us_80180880[][4];
extern u8 D_us_80180908[];
extern s32 D_us_80180AAC;
extern s32 D_us_80180AB0;
extern EInit g_EInitMinotaurus;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_224DC", func_us_801A269C);
#else
// The Minotaur. The fight controller (func_us_801A3E04) picks its moves; the
// Werewolf sits three entities before it.
void func_us_801A269C(Entity* self) {
    Entity* werewolf = self - 3;
    Entity* entity;
    s8* hitbox;
    u8* anim;
    s32 velX;
    s32 x;
    s32 arrived;
    s32 i;

    if ((self->flags & FLAG_DEAD) && self->step < 0x40) {
        SetStep(0x40);
    }
    FntPrint("mino_step %x\n", self->step);
    FntPrint("mino_set_step %x\n", self->ext.minotaurWerewolf.nextStep);
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitMinotaurus);
        self->hitboxState = 0;
        CreateEntityFromCurrentEntity(E_MINOTAUR_ATTACK_HITBOX, self + 1);
        // fallthrough
    case 1:
        AnimateEntity(D_us_801806F4, self);
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        UnkCollisionFunc3(D_us_801806D0);
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
                self->palette = g_EInitMinotaurus[3];
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
        AnimateEntity(D_us_801806F4, self);
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        if (self->ext.minotaurWerewolf.idleTimer) {
            self->ext.minotaurWerewolf.idleTimer--;
        } else {
            self->ext.minotaurWerewolf.isReady = 1;
        }
        if (self->ext.minotaurWerewolf.targetX) {
            self->ext.minotaurWerewolf.isReady = 0;
            SetStep(7);
        } else if (self->ext.minotaurWerewolf.hasCommand) {
            self->ext.minotaurWerewolf.hasCommand = 0;
            self->ext.minotaurWerewolf.isReady = 0;
            SetStep(self->ext.minotaurWerewolf.nextStep);
        } else {
            velX = PLAYER.velocityX;
            if (self->facingLeft) {
                velX = -velX;
            }
            if (self->facingLeft == PLAYER.facingLeft) {
                if (GetDistanceToPlayerX() > 0x48 &&
                    GetDistanceToPlayerX() < 0x60 &&
                    (g_Player.status & PLAYER_STATUS_UNK400) && velX > 0) {
                    self->ext.minotaurWerewolf.isReady = 0;
                    SetStep(0x12);
                }
                if (self->facingLeft == PLAYER.facingLeft &&
                    (g_Player.status & PLAYER_STATUS_SUBWPN)) {
                    self->ext.minotaurWerewolf.isReady = 0;
                    SetStep(0x10);
                }
            }
        }
        break;

    case 7:
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
                self->velocityX = FIX(1);
            } else {
                self->velocityX = FIX(-1);
            }
            self->step_s++;
        }
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        AnimateEntity(D_us_801806E0, self);
        UnkCollisionFunc2(D_us_801806C8);
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

    case 0xB:
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180724, self)) {
                self->step_s++;
            }
            break;
        case 1:
            werewolf->ext.minotaurWerewolf.partnerSignal = 1;
            self->ext.minotaurWerewolf.timer = 0x40;
            PlaySfxPositional(SFX_MINOTAUR_BREATH_ATTACK);
            self->step_s++;
            // fallthrough
        case 2:
            if (!(g_Timer & 3)) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(
                        E_MINOTAUR_SPIT_LIQUID, self, entity);
                    if (self->facingLeft) {
                        entity->posX.i.hi += 0x26;
                    } else {
                        entity->posX.i.hi -= 0x26;
                    }
                    entity->posY.i.hi -= 4;
                    entity->rotate = -0x500;
                    entity->facingLeft = self->facingLeft;
                    entity->zPriority = self->zPriority + 1;
                }
            }
            if (!--self->ext.minotaurWerewolf.timer) {
                SetStep(7);
            }
            break;
        }
        break;

    case 0xC:
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180734, self)) {
                werewolf->ext.minotaurWerewolf.partnerSignal = 1;
                SetSubStep(1);
            }
            break;
        case 1:
            if (self->facingLeft) {
                self->velocityX = FIX(4.5);
            } else {
                self->velocityX = FIX(-4.5);
            }
            self->velocityY = 0;
            self->animCurFrame = 0x19;
            PlaySfxPositional(SFX_SCRAPE_A);
            self->step_s++;
            // fallthrough
        case 2:
            if (UnkCollisionFunc2(D_us_801806C8) & 0xF0) {
                self->ext.minotaurWerewolf.timer = 1;
                self->step_s++;
            }
            break;
        case 3:
            AnimateEntity(D_us_8018073C, self);
            if (!--self->ext.minotaurWerewolf.timer) {
                SetSubStep(4);
            }
            break;
        case 4:
            SetStep(7);
            break;
        }
        break;

    case 0xE:
        if (!AnimateEntity(D_us_80180750, self)) {
            SetStep(7);
        }
        if (!self->poseTimer && self->pose == 8) {
            for (i = 0; i < 8; i++) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_MINOTAUR_FIREBALL, self, entity);
                    entity->posY.i.hi += 0x28;
                    entity->params = i;
                    entity->facingLeft = self->facingLeft;
                }
            }
        }
        break;

    case 0x22:
        switch (self->step_s) {
        case 0:
            self->ext.minotaurWerewolf.partnerSignal = 0;
            if (werewolf->posX.i.hi < self->posX.i.hi) {
                self->facingLeft = 0;
            } else {
                self->facingLeft = 1;
            }
            if (werewolf->flags & FLAG_DEAD) {
                SetStep(0x12);
                return;
            }
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_80180770, self)) {
                SetSubStep(2);
            }
            break;
        case 2:
            if (self->ext.minotaurWerewolf.partnerSignal) {
                PlaySfxPositional(SFX_MINOTAUR_ATTACK);
                self->step_s++;
            }
            if (werewolf->flags & FLAG_DEAD) {
                SetSubStep(3);
                return;
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_8018077C, self)) {
                SetStep(7);
            }
            break;
        default:
            return;
        }
        break;

    case 0x10:
        if (!AnimateEntity(D_us_80180790, self)) {
            SetStep(2);
        }
        break;

    case 0x12:
        if (!self->step_s) {
            self->velocityX = 0;
            self->velocityY = 0;
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            self->step_s++;
        }
        UnkCollisionFunc2(D_us_801806C8);
        if (!AnimateEntity(D_us_80180798, self)) {
            SetStep(2);
        }
        if (werewolf->flags & FLAG_DEAD) {
            self->velocityX -= self->velocityX / 8;
            if (!self->poseTimer && self->pose == 7) {
                PlaySfxPositional(SFX_MINOTAUR_ATTACK);
                if (self->facingLeft) {
                    self->velocityX = FIX(4);
                } else {
                    self->velocityX = FIX(-4);
                }
            }
        }
        break;

    case 8:
        switch (self->step_s) {
        case 0:
            self->velocityY = FIX(-6.5);
            if (self->facingLeft) {
                self->velocityX = FIX(3);
            } else {
                self->velocityX = FIX(-3);
            }
            self->ext.minotaurWerewolf.partnerSignal = 0;
            self->step_s++;
            // fallthrough
        case 1:
            AnimateEntity(D_us_80180704, self);
            UnkCollisionFunc3(D_us_801806D0);
            if (self->velocityY > 0) {
                PlaySfxPositional(SFX_BOSS_WING_FLAP);
                SetSubStep(2);
            }
            break;
        case 2:
            AnimateEntity(D_us_80180714, self);
            if (UnkCollisionFunc3(D_us_801806D0) & 1) {
                if (!(werewolf->flags & FLAG_DEAD) &&
                    werewolf->ext.minotaurWerewolf.partnerSignal) {
                    if (werewolf->posX.i.hi < self->posX.i.hi) {
                        self->facingLeft = 0;
                    } else {
                        self->facingLeft = 1;
                    }
                    SetSubStep(3);
                } else {
                    SetStep(7);
                }
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180850, self)) {
                SetSubStep(4);
            }
            break;
        case 4:
            if (self->ext.minotaurWerewolf.partnerSignal) {
                if (!AnimateEntity(D_us_80180858, self)) {
                    SetStep(7);
                }
            }
            break;
        }
        break;

    case 0x14:
        if (!AnimateEntity(D_us_801807AC, self)) {
            SetStep(0x18);
        }
        break;

    case 0x18:
        if (!AnimateEntity(D_us_801807B4, self)) {
            SetStep(0x16);
        }
        break;

    case 0x16:
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_801807D0, self)) {
                SetSubStep(1);
            }
            if (!self->poseTimer && self->pose == 5) {
                PlaySfxPositional(SFX_UNK_BO2_789);
            }
            if (!self->poseTimer && self->pose == 3) {
                (self + 1)->ext.minotaur.axeThrown = 1;
                self->ext.minotaurWerewolf.axeThrown = 0;
                self->ext.minotaurWerewolf.savedAttack = self->attack;
                self->attack *= 2;
            }
            break;
        case 1:
            if (self->facingLeft) {
                self->velocityX = FIX(2.5);
            } else {
                self->velocityX = FIX(-2.5);
            }
            self->velocityY = 0;
            self->step_s++;
            // fallthrough
        case 2:
            AnimateEntity(D_us_801807F4, self);
            if (UnkCollisionFunc2(D_us_801806C8) & 0xF0) {
                werewolf->ext.minotaurWerewolf.partnerSignal = 1;
                SetSubStep(3);
            } else if (GetDistanceToPlayerX() < 8) {
                werewolf->ext.minotaurWerewolf.partnerSignal = 1;
                PlaySfxPositional(SFX_MINOTAURUS_JUMP_ATTACK);
                SetSubStep(3);
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_801807FC, self)) {
                self->velocityX = 0;
                self->velocityY = FIX(-7);
                SetSubStep(4);
            }
            break;
        case 4:
            anim = D_us_80180810;
            if (self->velocityY < 0) {
                anim = D_us_80180808;
            }
            AnimateEntity(anim, self);
            if (UnkCollisionFunc3(D_us_801806D0) & 1) {
                PlaySfxPositional(SFX_STOMP_HARD_A);
                (self + 1)->ext.minotaur.axeThrown = 0;
                self->attack = self->ext.minotaurWerewolf.savedAttack;
                SetSubStep(5);
            }
            break;
        case 5:
            AnimateEntity(D_us_80180818, self);
            if (self->ext.minotaurWerewolf.axeThrown) {
                SetStep(7);
            }
            break;
        }
        break;

    case 0x21:
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 0x2B;
            self->step_s++;
            break;
        case 1:
            self->facingLeft ^= 1;
            self->step_s++;
            break;
        case 2:
            self->animCurFrame = 1;
            SetStep(7);
            break;
        }
        break;

    case 0x40:
        self->ext.minotaurWerewolf.isReady = 1;
        werewolf->ext.minotaurWerewolf.partnerSignal = 1;
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            self->velocityX = 0;
            self->velocityY = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (UnkCollisionFunc3(D_us_801806D0) & 1) {
                self->ext.minotaurWerewolf.targetX = -0x10;
                self->ext.minotaurWerewolf.timer = 0;
                PlaySfxPositional(SFX_EXPLODE_A);
                self->step_s++;
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80180824, self)) {
                self->step_s = 3;
            }
            self->ext.minotaurWerewolf.timer &= 1;
            // fallthrough
        case 3:
            // targetX sweeps the flames from -16 to 16
            if (self->ext.minotaurWerewolf.timer & 1) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(
                        E_MINOTAUR_DEATH_PUFF, self, entity);
                    entity->posX.i.hi +=
                        (s16)self->ext.minotaurWerewolf.targetX;
                    entity->posY.i.hi += 0x20;
                    entity->zPriority = self->zPriority + 1;
                    x = self->ext.minotaurWerewolf.targetX;
                    if (x < 0) {
                        x = -x;
                    }
                    entity->params =
                        x + (self->ext.minotaurWerewolf.timer >> 1);
                }
                self->ext.minotaurWerewolf.targetX += 8;
                if (self->ext.minotaurWerewolf.targetX > 0x10) {
                    self->ext.minotaurWerewolf.targetX = -0x10;
                }
            }
            if (!(g_Timer & 0x1F)) {
                PlaySfxPositional(SFX_FIREBALL_SHOT_B);
            }
            if (self->ext.minotaurWerewolf.timer++ > 0x40) {
                self->step_s++;
            }
            break;
        case 4:
            self->animCurFrame = 0;
            self->ext.minotaurWerewolf.targetX = 0;
            break;
        }
        break;

    case 0xFF:
        FntPrint("charal %x\n", self->animCurFrame);
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
        break;
    }

    hitbox = D_us_80180880[D_us_80180908[self->animCurFrame]];
    self->hitboxOffX = *hitbox++;
    self->hitboxOffY = *hitbox++;
    self->hitboxWidth = *hitbox++;
    self->hitboxHeight = *hitbox++;
    func_us_801A2610();
}
#endif

#include "e_minotaur.h"

extern s32 D_us_80180AB4[][4];
extern s32 D_us_80180B74;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_224DC", func_us_801A3E04);
#else
// The fight controller. It spawns both bosses and plays the move list in
// D_us_80180AB4: each entry holds {distance, step} for the Minotaur, then for
// the Werewolf. A new command is sent once both bosses are ready.
void func_us_801A3E04(Entity* self) {
    Entity* werewolf = self + 1;
    Entity* minotaur = self + 4;
    Entity* entity;
    s32* cmd;
    Primitive* prim;
    s32 primIndex;
    s16 scrollX = g_Tilemap.scrollX.i.hi;
    s16 scrollY = g_Tilemap.scrollY.i.hi;
    s16 y;
    u8 index;

    FntPrint("set_step %x\n", self->step);
    FntPrint("boss_go %x\n", D_us_80180AAC);
    if (self->step >= 5 && (werewolf->flags & FLAG_DEAD) &&
        (minotaur->flags & FLAG_DEAD) && self->step != 8) {
        SetStep(8);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_G4, 1);
        if (primIndex == -1) {
            self->step = 0;
            return;
        }
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.minotaurWerewolfFight.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        prim->r0 = prim->g0 = prim->b0 = 0;
        prim->x0 = prim->x2 = 0;
        prim->x1 = prim->x3 = 0x100;
        prim->y0 = prim->y1 = 0;
        prim->y2 = prim->y3 = 0x100;
        prim->priority = 0x1F0;
        prim->drawMode = DRAW_HIDE;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r0);
        self->ext.minotaurWerewolfFight.command = 0;
        // fallthrough
    case 1:
        if (g_CastleFlags[RICHTER_CS_AFTER_M_AND_W] ||
            g_PlayableCharacter != PLAYER_ALUCARD || g_DemoMode != Demo_None) {
            if (GetDistanceToPlayerX() < 0x10) {
                D_us_80180B74 = 1;
                stopMusicFlag = 0;
                currentMusicId = MU_FESTIVAL_OF_SERVANTS;
                g_api.PlaySfx(MU_FESTIVAL_OF_SERVANTS);
                SetStep(2);
                return;
            }
        } else if (g_CutsceneFlags & 0x10) {
            SetStep(2);
            return;
        }
        break;

    case 2:
        // the flames the bosses rise from
        switch (self->step_s) {
        case 0:
            CreateEntityFromCurrentEntity(E_UNK_28, &g_Entities[112]);
            y = 0xD0 - scrollY;
            g_Entities[112].posX.i.hi = 0x150 - scrollX;
            g_Entities[112].posY.i.hi = y;
            g_Entities[112].params = 0;
            CreateEntityFromCurrentEntity(E_UNK_28, &g_Entities[113]);
            g_Entities[113].posX.i.hi = 0xA0 - scrollX;
            g_Entities[113].posY.i.hi = y;
            g_Entities[113].params = 1;
            self->ext.minotaurWerewolfFight.timer = 0x80;
            self->step_s++;
            // fallthrough
        case 1:
            if (!--self->ext.minotaurWerewolfFight.timer) {
                SetStep(3);
                return;
            }
            break;
        }
        break;

    case 3:
        CreateEntityFromCurrentEntity(E_MINOTAUR, minotaur);
        CreateEntityFromCurrentEntity(E_WEREWOLF, werewolf);
        minotaur->posX.i.hi = 0x160 - scrollX;
        minotaur->posY.i.hi = 0xA8 - scrollY;
        werewolf->posX.i.hi = 0xA0 - scrollX;
        werewolf->posY.i.hi = 0xB5 - scrollY;
        g_api.TimeAttackController(
            TIMEATTACK_EVENT_MINOTAUR_WEREWOLF_DEFEAT, TIMEATTACK_SET_VISITED);
        SetStep(4);
        // fallthrough
    case 4:
        switch (self->step_s) {
        case 0:
            self->ext.minotaurWerewolfFight.timer = 0x100;
            self->step_s++;
            // fallthrough
        case 1:
            if (--self->ext.minotaurWerewolfFight.timer) {
                return;
            }
            self->step_s++;
            break;
        case 2:
            // without the cutscene the fight starts right away
            if (g_CastleFlags[RICHTER_CS_AFTER_M_AND_W] ||
                g_PlayableCharacter != PLAYER_ALUCARD ||
                g_DemoMode != Demo_None) {
                D_us_80180AAC = 1;
            }
            SetStep(5);
            return;
        }
        break;

    case 5:
        cmd = D_us_80180AB4[self->ext.minotaurWerewolfFight.command];
        if (!(minotaur->flags & FLAG_DEAD)) {
            minotaur->ext.minotaurWerewolf.targetX = cmd[0];
            minotaur->ext.minotaurWerewolf.nextStep = cmd[1];
        }
        if (!(werewolf->flags & FLAG_DEAD)) {
            werewolf->ext.minotaurWerewolf.targetX = cmd[2];
            werewolf->ext.minotaurWerewolf.nextStep = cmd[3];
        }
        SetStep(6);
        return;

    case 6:
        if (minotaur->ext.minotaurWerewolf.isReady &&
            werewolf->ext.minotaurWerewolf.isReady) {
            cmd = D_us_80180AB4[self->ext.minotaurWerewolfFight.command];
            if (cmd[3] == 0xE && cmd[1] == 0xC &&
                abs(minotaur->posX.i.hi - werewolf->posX.i.hi) < 0x40) {
                minotaur->ext.minotaurWerewolf.nextStep = 0xB;
                werewolf->ext.minotaurWerewolf.nextStep = 0x18;
            }
            werewolf->ext.minotaurWerewolf.hasCommand = 1;
            minotaur->ext.minotaurWerewolf.hasCommand = 1;
            SetStep(7);
            return;
        }
        break;

    case 7:
        index = self->ext.minotaurWerewolfFight.command;
        self->ext.minotaurWerewolfFight.command = index + 1;
        cmd = D_us_80180AB4[self->ext.minotaurWerewolfFight.command];
        // skip this move when the player stands between the bosses
        if (cmd[3] == 0x18 && (minotaur->posX.i.hi - PLAYER.posX.i.hi) *
                                      (werewolf->posX.i.hi - PLAYER.posX.i.hi) <
                                  0) {
            self->ext.minotaurWerewolfFight.command = index + 2;
            cmd += 4;
        }
        if (cmd[0] == -1) {
            self->ext.minotaurWerewolfFight.command = 0;
        }
        SetStep(5);
        return;

    case 8:
        switch (self->step_s) {
        case 0:
            g_api.PlaySfx(SET_UNK_92);
            self->ext.minotaurWerewolfFight.timer = 0x100;
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_MINOTAUR_WEREWOLF_DEFEAT,
                TIMEATTACK_SET_RECORD);
            self->step_s++;
            // fallthrough
        case 1:
            if (!--self->ext.minotaurWerewolfFight.timer) {
                self->step_s++;
                if (!g_CastleFlags[RICHTER_CS_AFTER_M_AND_W] &&
                    g_PlayableCharacter == PLAYER_ALUCARD &&
                    g_DemoMode == Demo_None) {
                    CreateEntityFromCurrentEntity(
                        E_CUTSCENE_DIALOGUE, &g_Entities[200]);
                    g_Entities[200].params = 1;
                    CreateEntityFromCurrentEntity(E_UNK_26, &g_Entities[96]);
                    g_Entities[96].params = 1;
                    return;
                }
                self->step_s++;
            }
            break;
        case 2:
            if (g_CutsceneFlags & 0x1000) {
                self->step_s++;
            }
            break;
        case 3:
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_LIFE_UP_SPAWN, self, entity);
                stopMusicFlag = 1;
                D_us_80180B74 = 0;
                entity->posX.i.hi = 0x100 - g_Tilemap.scrollX.i.hi;
                entity->params = 2;
                entity->posY.i.hi = 0x80 - g_Tilemap.scrollY.i.hi;
                currentMusicId = MU_WANDERING_GHOSTS;
                self->step_s++;
            }
            break;
        case 4:
            if (!g_api.func_80131F68()) {
                stopMusicFlag = 0;
                g_api.PlaySfx(currentMusicId);
                self->step_s++;
            }
            break;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_224DC", func_us_801A460C);
#else
// A wall of flames where a boss appears. It grows from the floor and clips the
// boss sprite to its width until the Minotaur has faded in.
void func_us_801A460C(Entity* self) {
    DRAWENV drawEnv;
    RECT clipRect;
    DR_ENV* dr_env;
    Primitive* prim;
    Entity* entity;
    s32 primIndex;
    s32 left;
    s32 right;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 2);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.bossFlameWall.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        dr_env = g_api.func_800EDB08((POLY_GT4*)prim);
        if (dr_env == NULL) {
            DestroyEntity(self);
            return;
        }
        prim->type = PRIM_ENV;
        drawEnv = g_CurrentBuffer->draw;
        clipRect.x = 0;
        clipRect.y = 0x14;
        clipRect.w = 0x100;
        clipRect.h = 0xCF;
        drawEnv.isbg = 0;
        drawEnv.r0 = drawEnv.g0 = drawEnv.b0 = 0;
        drawEnv.dtd = 0;
        drawEnv.ofs[0] = 0;
        drawEnv.clip = clipRect;
        SetDrawEnv(dr_env, &drawEnv);
        prim->drawMode = DRAW_HIDE;
        prim->priority = self->params * 4 + 0x9F;
        prim = prim->next;
        if (g_api.func_800EDB08((POLY_GT4*)prim) == NULL) {
            DestroyEntity(self);
            return;
        }
        prim->type = PRIM_ENV;
        prim->drawMode = DRAW_UNK_800 | DRAW_HIDE;
        prim->priority = self->params * 4 + 0xA1;
        self->ext.bossFlameWall.halfWidth = 0;
        // fallthrough
    case 1:
        switch (self->step_s) {
        case 0:
            if (++self->ext.bossFlameWall.timer & 1) {
                self->ext.bossFlameWall.halfWidth++;
            }
            if (!(g_Timer & 0xF)) {
                g_api.PlaySfx(SFX_FIREBALL_SHOT_B);
            }
            if (self->ext.bossFlameWall.halfWidth >= 0x1E) {
                self->step_s++;
            }
            break;
        case 1:
            if (D_us_80180AB0) {
                self->ext.bossFlameWall.timer = 0x80;
                self->step_s++;
            }
            if (!(g_Timer & 0xF)) {
                g_api.PlaySfx(SFX_FIREBALL_SHOT_B);
            }
            break;
        case 2:
            if (!--self->ext.bossFlameWall.timer) {
                DestroyEntity(self);
                return;
            }
            break;
        }
        prim = self->ext.bossFlameWall.prim;
        left = self->posX.i.hi - self->ext.bossFlameWall.halfWidth;
        right = self->ext.bossFlameWall.halfWidth + self->posX.i.hi;
        if (left < 0) {
            left = 0;
        }
        if (left > 0x100) {
            left = 0x100;
        }
        if (right < 0) {
            right = 0;
        }
        if (right > 0x100) {
            right = 0x100;
        }
        drawEnv = g_CurrentBuffer->draw;
        clipRect.w = right - left;
        clipRect.y = 0x14;
        drawEnv.isbg = 0;
        clipRect.h = 0xD0 - g_Tilemap.scrollY.i.hi - 0x15;
        clipRect.x = drawEnv.ofs[0] + left;
        drawEnv.clip = clipRect;
        SetDrawEnv(PRIM_DR_ENV(prim), &drawEnv);
        prim->drawMode = DRAW_DEFAULT;
        prim->next->drawMode = DRAW_UNK_800;
        if (D_us_80180AB0 || (g_Timer & 1)) {
            return;
        }
        if (Random() & 1) {
            entity = AllocEntity(&g_Entities[144], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_UNK_29, entity);
                entity->zPriority = self->params * 4 + 0xA0;
                entity->posX.i.hi =
                    self->posX.i.hi - self->ext.bossFlameWall.halfWidth / 2;
                entity->posY.i.hi = 0xD0;
                entity->posX.i.hi -= Random() & 0xF;
                entity->rotate = Random() + 0x40;
                return;
            }
        } else {
            entity = AllocEntity(&g_Entities[144], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_UNK_29, entity);
                entity->zPriority = self->params * 4 + 0xA0;
                entity->posX.i.hi =
                    self->posX.i.hi + self->ext.bossFlameWall.halfWidth / 2;
                entity->posY.i.hi = 0xD0;
                entity->posX.i.hi += Random() & 0xF;
                entity->rotate = -0x40 - Random();
            }
        }
        break;
    }
}
#endif

#include "e_unk_29.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_224DC", EntityBossTorch);
#else
#define PAL_TORCH_A 0x5A
#define PAL_TORCH_B 0x5B
#define g_EInitEnvironment D_us_80180524
#include "../../st/are/e_boss_torch.h"
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo2/nonmatchings/unk_224DC", EntityBossDoors);
#else
// ARE's doors, which also close when the fight starts
void EntityBossDoors(Entity* self) {
    Entity* nextEntity;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitEnvironment);
        if (self->params & 0x100) {
            self->animCurFrame = 2;
            self->zPriority = 0x6A;
            self->step = 16;
            break;
        }

        if (self->params & 0x200) {
            self->animCurFrame = 3;
            self->zPriority = 0x6B;
            self->step = 32;
            break;
        }

        self->animCurFrame = 1;
        self->zPriority = 0x6A;
        if (!self->params) {
            self->facingLeft = 1;
            self->posX.i.hi = 0x4F - g_Tilemap.scrollX.i.hi;
            self->posY.i.hi = 0xA6 - g_Tilemap.scrollY.i.hi;

            nextEntity = self + 1;
            CreateEntityFromEntity(E_BOSS_DOORS, self, nextEntity);
            nextEntity->params = 0x100;
            nextEntity->posX.i.hi -= 7;
            nextEntity->facingLeft = self->facingLeft;
            nextEntity->posY.i.hi += 0xD;

            nextEntity = self + 2;
            CreateEntityFromEntity(E_BOSS_DOORS, self, nextEntity);
            nextEntity->params = 0x200;
            nextEntity->posX.i.hi -= 8;
            nextEntity->facingLeft = self->facingLeft;
            nextEntity->posY.i.hi += 0x31;

            self->posY.i.hi += 0x40;
        } else {
            self->facingLeft = 0;
            self->posX.i.hi = 0x1B1 - g_Tilemap.scrollX.i.hi;
            self->posY.i.hi = 0x96 - g_Tilemap.scrollY.i.hi;

            nextEntity = self + 1;
            CreateEntityFromEntity(E_BOSS_DOORS, self, nextEntity);
            nextEntity->params = 0x100;
            nextEntity->posX.i.hi += 7;
            nextEntity->facingLeft = self->facingLeft;
            nextEntity->posY.i.hi += 0x14;

            nextEntity = self + 2;
            CreateEntityFromEntity(E_BOSS_DOORS, self, nextEntity);
            nextEntity->params = 0x200;
            nextEntity->posX.i.hi += 6;
            nextEntity->facingLeft = self->facingLeft;
            nextEntity->posY.i.hi += 0x41;

            self->posY.i.hi += 0x50;
        }
        // fallthrough
    case 1:
        if (D_us_80180B74) {
            g_api.PlaySfx(SFX_STONE_MOVE_A);
            if (!self->params) {
                self->ext.areBossDoor.timer = 0x40;
            } else {
                self->ext.areBossDoor.timer = 0x50;
            }
            g_Tilemap.fg[0x104] = 0xF6;
            g_Tilemap.fg[0x124] = 0xFE;
            g_Tilemap.fg[0x144] = 0x106;
            g_Tilemap.fg[0x164] = 0xFE;
            g_Tilemap.fg[0xFB] = 0xEE;
            g_Tilemap.fg[0x11B] = 0x17F;
            g_Tilemap.fg[0x13B] = 0x187;
            g_Tilemap.fg[0x15B] = 0x17F;
            g_Tilemap.fg[0x17B] = 0x187;
            self->step++;
        }
        break;
    case 2:
        if (self->ext.areBossDoor.timer & 1) {
            self->posX.i.hi++;
        } else {
            self->posX.i.hi--;
        }
        self->posY.i.hi--;

        if (!--self->ext.areBossDoor.timer) {
            PlaySfxPositional(SFX_LEVER_METAL_BANG);
            self->step++;
        }
        break;
    case 3:
        if (!D_us_80180B74) {
            self->ext.areBossDoor.timer = 0x60;
            g_Tilemap.fg[0x104] = 0x1A5;
            g_Tilemap.fg[0x124] = 0x1A6;
            g_Tilemap.fg[0x144] = 0x1A7;
            g_Tilemap.fg[0x164] = 0x1A6;
            g_Tilemap.fg[0xFB] = 0x1A4;
            g_Tilemap.fg[0x11B] = 0x1A8;
            g_Tilemap.fg[0x13B] = 0x1A9;
            g_Tilemap.fg[0x15B] = 0x1A8;
            g_Tilemap.fg[0x17B] = 0x1A9;
            self->step++;
        }
        break;
    case 4:
        if (self->ext.areBossDoor.timer & 1) {
            self->posX.i.hi++;
        } else {
            self->posX.i.hi--;
        }
        self->posY.i.hi++;

        if (!--self->ext.areBossDoor.timer) {
            self->step++;
        }
        break;
    case 16:
        if (D_us_80180B74) {
            self->palette = (g_Timer & 1) + 1;
        } else {
            self->palette = 0;
        }
        break;
    case 32:
        break;
    case 255:
#include "../../st/pad2_anim_debug.h"
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180668[];
extern u8 D_us_80180670[];
u8 D_us_80180668[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180670[] ALIGNED4 = {0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 anim_death_puff[0x1C] = {
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
u8 anim_spit[0x1C] = {
    1,
    9,
    1,
    0xA,
    1,
    0xB,
    1,
    0xC,
    1,
    0xD,
    1,
    0xE,
    1,
    0xF,
    1,
    0x10,
    1,
    0x11,
    1,
    0x12,
    1,
    0x13,
    1,
    0x14,
    0xFF,
    0,
    0,
    0,
};
u8 anim_torch_puff[0x1C] = {
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
u8 blend_modes[0x10] = { 0x30, 0x30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s16 D_us_801806C8[4] = { 0, 0x28, 0x10, 0 };
s16 D_us_801806D0[8] = { 0, 0x28, 0, 4, 0x10, -4, -0x20, 0 };
u8 D_us_801806E0[0x14] = { 6, 1, 0xE, 0x4C, 9, 2, 0xA, 3, 9, 4, 6, 0x49, 6, 4, 8, 3, 7, 2, 0, 0 };
u8 D_us_801806F4[0x10] = { 0xA, 5, 9, 6, 7, 7, 0xA, 8, 9, 7, 7, 6, 0, 0, 0, 0 };
u8 D_us_80180704[0x10] = { 7, 0xA, 5, 0xB, 6, 0xC, 5, 0x4A, 0x1A, 0x4B, 3, 0x50, 0x31, 0xD, 0xFF, 0 };
u8 D_us_80180714[0x10] = { 6, 0xE, 2, 0xF, 2, 0x10, 3, 0x11, 3, 0x51, 0x43, 0x11, 0xFF, 0, 0, 0 };
u8 D_us_80180724[0x10] = { 6, 0x13, 5, 0x14, 1, 0x4D, 0x1C, 0x58, 3, 0x4E, 3, 0x15, 1, 0x4F, 0xFF, 0 };
u8 D_us_80180734[8] = { 6, 0x16, 6, 0x17, 0x20, 0x18, 0xFF, 0 };
u8 D_us_8018073C[0x14] = {
    6,
    0x2C,
    6,
    0x2D,
    6,
    0x2E,
    6,
    0x2D,
    0,
    0,
    0,
    0,
    0xD,
    0x1A,
    0xC,
    0x1B,
    0xC,
    0x1C,
    0,
    0,
};
u8 D_us_80180750[0x20] = {
    4,
    1,
    6,
    0x1D,
    0x15,
    0x1E,
    4,
    0x1F,
    3,
    0x20,
    0xC,
    0x21,
    6,
    0x22,
    4,
    0x23,
    3,
    0x24,
    1,
    0x52,
    0x16,
    0x24,
    6,
    0x23,
    5,
    0x25,
    4,
    9,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180770[0xC] = { 4, 1, 4, 0x1D, 6, 0x1E, 1, 0x1F, 0xFF, 0, 0, 0 };
u8 D_us_8018077C[0x14] = {
    2,
    0x20,
    4,
    0x22,
    3,
    0x23,
    3,
    0x24,
    1,
    0x52,
    0x16,
    0x24,
    6,
    0x23,
    5,
    0x25,
    4,
    9,
    0xFF,
    0,
};
u8 D_us_80180790[8] = { 4, 0x26, 0x10, 0x27, 4, 0x26, 0xFF, 0 };
u8 D_us_80180798[0x14] = {
    0x10,
    1,
    0x10,
    0x11,
    8,
    0x51,
    4,
    6,
    4,
    5,
    3,
    0x12,
    3,
    0x53,
    0x31,
    0x12,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801807AC[8] = { 2, 0x28, 6, 0x29, 6, 0x28, 0, 0 };
u8 D_us_801807B4[0x1C] = {
    6,
    0x2F,
    6,
    0x30,
    6,
    0x31,
    1,
    0x32,
    1,
    0x54,
    1,
    0x32,
    1,
    0x54,
    1,
    0x32,
    1,
    0x54,
    1,
    0x32,
    1,
    0x54,
    0x31,
    0x32,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801807D0[0x24] = {
    4,
    8,
    4,
    9,
    4,
    0xA,
    6,
    0x31,
    6,
    0x30,
    6,
    0x2F,
    6,
    0x33,
    6,
    0x34,
    7,
    0x35,
    4,
    0x36,
    4,
    0x37,
    4,
    0x38,
    4,
    0x39,
    4,
    0x33,
    4,
    0x56,
    4,
    0x57,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801807F4[8] = { 1, 0x3A, 1, 0x3B, 0, 0, 0, 0 };
u8 D_us_801807FC[0xC] = { 5, 0x3C, 4, 0x3D, 3, 0x3E, 1, 0x3F, 0xFF, 0, 0, 0 };
u8 D_us_80180808[8] = { 1, 0x3F, 1, 0x55, 0, 0, 0, 0 };
u8 D_us_80180810[8] = { 1, 0x3F, 1, 0x40, 0, 0, 0, 0 };
u8 D_us_80180818[0xC] = { 4, 0x31, 4, 0x30, 4, 0x31, 4, 0x32, 4, 0x31, 0xFF, 0 };
u8 D_us_80180824[0x10] = {
    0x18,
    0x41,
    8,
    0x43,
    8,
    0x44,
    8,
    0x45,
    8,
    0x46,
    8,
    0x47,
    0x18,
    0x48,
    0xFF,
    0,
};
u8 D_us_80180850[8] = { 3, 0xA, 3, 0xB, 4, 0xC, 0xFF, 0 };
u8 D_us_80180858[0xC] = { 2, 0xB, 2, 0xE, 2, 0xF, 2, 0x49, 2, 0x11, 0xFF, 0 };
s8 D_us_80180880[0x22][4] = {
    { 0, 0, 0, 0 },
    { 0, 4, 0x10, 0x21 },
    { 0, 8, 0x10, 0x1C },
    { -4, 3, 0xF, 0x21 },
    { 1, 4, 0xF, 0x21 },
    { 0xC, 8, 0xC, 0x1C },
    { 0, 0, 0x10, 0x1D },
    { -2, -4, 0xD, 0x21 },
    { -3, 3, 0xD, 0x20 },
    { 0, -3, 0x10, 0x1F },
    { -5, 6, 0xE, 0x1F },
    { 1, 4, 0x11, 0x21 },
    { 3, 1, 0x10, 0x23 },
    { -1, 6, 0x11, 0x1E },
    { 1, 8, 0x12, 0x1D },
    { -2, 0xD, 0x10, 0x19 },
    { -0xE, 0xE, 0x10, 0x19 },
    { -5, 7, 0x10, 0x10 },
    { -9, 3, 0x10, 0x19 },
    { 0, 1, 0x11, 0x1F },
    { 0, 0xA, 0x10, 0x19 },
    { -3, 0xE, 0x10, 0x19 },
    { -0xD, 0xF, 0xF, 0x15 },
    { -0xC, 0xE, 0x10, 0x17 },
    { 0, 3, 0x10, 0x1F },
    { -1, 9, 0x12, 0x1B },
    { 0, -1, 0x10, 0x21 },
    { 0xC0, 0xA8, 0, 0 },
    { 2, -3, 0x11, 0xC },
    { -0x10, 0x11, 0x10, 0x14 },
    { -0x18, -8, 0x10, 0x10 },
    { -0x13, -0x15, 0x10, 0x16 },
    { -0xD, -0x1F, 0xE, 0x23 },
    { 4, 1, 0x10, 0x23 },
};
u8 D_us_80180908[0x5C] = {
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
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    1,
    0xA,
    0xB,
    0xC,
    0xD,
    0xE,
    0xF,
    0x10,
    0x11,
    0x12,
    0x12,
    0x13,
    1,
    1,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    1,
    0x19,
    1,
    0x1A,
    1,
    1,
    1,
    1,
    0x1C,
    0x1C,
    0x1C,
    1,
    0xB,
    0xC,
    0xC,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0x1D,
    0x1D,
    0xF,
    0x1E,
    0x1F,
    0x20,
    0x20,
    0x1B,
    0x1B,
    0x1B,
    0x1B,
    0x1B,
    0x1B,
    0x1B,
    0x1B,
    1,
    5,
    5,
    1,
    0xC,
    0xB,
    0xD,
    6,
    1,
    1,
    0xA,
    0xC,
    0x20,
    0xF,
    0x10,
    0x21,
    0,
    0,
    0,
};
s32 D_us_80180AAC = 0;
s32 D_us_80180AB0 = 0;
s32 D_us_80180AB4[9][4] = {
    { 0x70, 0xC, 0x60, 0xE },
    { 0x70, 0x22, 0x68, 0x16 },
    { 0x60, 0x16, 0x60, 0x14 },
    { 0x60, 8, 0x70, 8 },
    { -1, 0, 0, 0 },
    { 0x40, 0x12, 0x40, 0xA },
    { 0x70, 8, 0x70, 0xE },
    { 0x80, 0x16, 0x60, 0x12 },
    { 0x60, 0xE, 0x50, 8 },
};
s32 D_us_80180B74 = 0;
u8* g_eBreakableAnimations[8] = { D_us_80180668, D_us_80180670, NULL, NULL, NULL, NULL, NULL, NULL };
u16 g_eBreakableanimSets[8] = { 3, 3, 0, 0, 0, 0, 0, 0 };
u8 g_eBreakableExplosionTypes[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u8 g_eBreakableHitboxes[8] = { 8, 8, 0, 0, 0, 0, 0, 0 };
u8 minotaur_attack_hitbox_indices[0x5C] = {
    0,
    1,
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
    0xC,
    0xD,
    0xE,
    0xF,
    0x10,
    0x11,
    0x11,
    0x26,
    0x26,
    0x25,
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
    0x12,
    0x12,
    3,
    0,
    0x13,
    0x13,
    0x13,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    0x12,
    1,
    9,
    9,
    1,
    0x22,
    0x23,
    0x11,
    0xA,
    0xE,
    0x1E,
    0xF,
    0x12,
    0x12,
    0x12,
    0x12,
    0x24,
    0,
    0,
    0,
};
s8 minotaur_attack_hitboxes[0x27][4] = {
    { 0, 0, 0, 0 },
    { -0x26, -0x12, 0x11, 0xC },
    { -0x22, -0x10, 0x11, 0xC },
    { -0x23, -0xF, 0x11, 0xB },
    { -0x25, -0xF, 0x11, 0xB },
    { -0x27, -0x10, 0x11, 0xC },
    { -0x24, -0x12, 0x11, 0xC },
    { -8, -0x34, 0xB, 0x15 },
    { 0x21, -0x1F, 0xC, 0xB },
    { 0x32, 7, 0xA, 0xB },
    { 0x21, -8, 0xE, 0xC },
    { 0x1B, -0x24, 0xE, 0xC },
    { -5, -0x38, 0xA, 0x1A },
    { -0x2C, -0x19, 0x18, 0x28 },
    { -0x22, 0x1E, 0x11, 0xB },
    { -0x2E, 0xB, 0x1C, 0xA },
    { -0x24, -5, 0x11, 0xB },
    { -0x21, -0xE, 0xD, 0xD },
    { 0xC0, 0xA8, 0, 0 },
    { -0x30, 0x98, 0, 0 },
    { -8, -0x20, 0xC, 9 },
    { -0x28, -0xF, 0xE, 0xD },
    { -0x2C, -0x10, 0xA, 0xB },
    { -0x28, -0x1A, 0xC, 0xB },
    { -0x1E, -0x1E, 0xB, 0xB },
    { -0x17, -0x1F, 0xC, 0xD },
    { -8, -0x1D, 0xC, 0x10 },
    { -9, -0x19, 0xB, 0x12 },
    { -7, -0x1B, 0xB, 0x11 },
    { 0xD, -0x15, 0xB, 0xB },
    { 0x14, -0x16, 0xB, 0xB },
    { -0x15, -0x1F, 0xB, 0xD },
    { -0x1E, -6, 0x11, 0xB },
    { -0x13, 0x14, 0xC, 0xC },
    { -0x1A, -0x1E, 0xD, 0xD },
    { -0x26, -0x11, 0xD, 0xD },
    { -0x19, -0x1E, 0xD, 0xD },
    { -0x2D, 8, 0xD, 0xD },
    { -0x1E, 2, 0xD, 0xD },
};
s32 fireball_config[0xA][2] = {
    { 0x60000, 0x100 },
    { 0x60000, 0x180 },
    { 0x60000, 0x200 },
    { 0x60000, 0x280 },
    { 0x60000, 0x300 },
    { 0x60000, 0x380 },
    { 0x60000, 0x400 },
    { 0x60000, 0x480 },
    { 0x60000, 0x500 },
    { 0x60000, 0x580 },
};
// end pc_import_data.py
// clang-format on
#endif
