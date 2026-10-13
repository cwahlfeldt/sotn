// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo7.h"

// Cerberus: the boss itself, its fireballs, the flames and the arena
// controller. PSX keeps these as INCLUDE_ASM; the PC build uses the C below
// (functional, not matching).

typedef struct {
    u16 animSet;
    u16 unk5A;
    u16 palette;
    u16 blendMode;
    u8* anim;
} CerberusFlameParams;

extern s32 D_us_80180848; // arena state, see CERBERUS_* below
extern u8 D_us_801806FC[];
extern u8 D_us_80180708[];
extern u8 D_us_8018071C[];
extern u8 D_us_8018072C[];
extern u8 D_us_8018073C[];
extern u8 D_us_80180748[];
extern u8 D_us_80180754[];
extern u8 D_us_80180760[];
extern u8 D_us_8018076C[];
extern u8 D_us_80180778[];
extern u8 D_us_80180788[];
extern s16 D_us_801806D8[];
extern s16 D_us_801806F0[][2];
extern s8 D_us_801807E4[][4];
extern u8 D_us_8018081C[];
extern u8* D_us_801807BC[];
extern u16 D_us_801806E8[];
extern u8 D_us_801807C8[];
extern CerberusFlameParams D_us_801808A4[];
extern EInit g_EInitCerberos;
extern EInit D_us_8018046C;
extern EInit D_us_80180478;
extern EInit g_EInitInteractable;
extern EInit g_EInitParticle;

#define CERBERUS_PLAYER_ENTERED 1
#define CERBERUS_FIGHT_STARTED 2
#define CERBERUS_DEFEATED 4
#define CERBERUS_DEATH_DONE 8
#define CERBERUS_REWARD_SPAWNED 0x10

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/unk_14CE0", func_us_80194338_from_rbo0);
#else
// same as RBO0's func_us_80194338 with a different floor height
bool func_us_80194338_from_rbo0(s16* offsets) {
    s32 posY;

    offsets++;
    posY = g_CurrentEntity->posY.i.hi + *offsets + g_Tilemap.scrollY.i.hi;
    posY = 0xD0 - posY;
    if (posY <= 0) {
        g_CurrentEntity->posY.i.hi += posY;
        g_CurrentEntity->velocityX = 0;
        g_CurrentEntity->velocityY = 0;
        return true;
    }
    return false;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/unk_14CE0", func_us_80194D3C);
#else
static void CerberusSpitFireball(Entity* self) {
    Entity* fireball;

    PlaySfxPositional(SFX_B07_STOMP);
    PlaySfxPositional(SFX_WARG_ATTACK);
    fireball = AllocEntity(&g_Entities[160], &g_Entities[192]);
    if (fireball != NULL) {
        CreateEntityFromEntity(E_UNK_1D, self, fireball);
        fireball->facingLeft = self->facingLeft;
        if (self->facingLeft) {
            fireball->posX.i.hi += 0x18;
        } else {
            fireball->posX.i.hi -= 0x18;
        }
        fireball->params = self->ext.cerberus.fireballType;
        fireball->zPriority = self->zPriority + 1;
    }
}

// leaps (step 9 forward, step 4 backward) share everything but direction
static void CerberusLeap(Entity* self, bool backwards) {
    switch (self->step_s) {
    case 0:
        if (!AnimateEntity(D_us_80180778, self)) {
            SetSubStep(1);
        }
        break;
    case 1:
        self->velocityY = FIX(-6);
        if (self->facingLeft ^ backwards) {
            self->velocityX = FIX(-2.5);
        } else {
            self->velocityX = FIX(2.5);
        }
        self->step_s++;
        // fallthrough
    case 2:
        MoveEntity();
        self->velocityY += FIX(0.21875);
        if (func_us_80194338_from_rbo0(D_us_801806D8) & 1) {
            PlaySfxPositional(SFX_START_SLAM_B);
            g_api.ShakeCamera(1);
            self->step_s++;
        }
        break;
    case 3:
        if (!AnimateEntity(D_us_80180788, self)) {
            SetStep(3);
        }
        break;
    }
}

void func_us_80194D3C(Entity* self) {
    Entity* entity;
    s32 posX;
    s32 side;
    s32 dist;
    s32 i;
    s8* hitbox;

    if (self->flags & FLAG_DEAD) {
        if (self->step != 8) {
            SetStep(8);
        }
    }

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCerberos);
        self->hitboxState = 0;
        SetStep(2);
        // fallthrough
    case 2:
        // idle until the arena controller starts the fight
        AnimateEntity(D_us_801806FC, self);
        if (D_us_80180848 & CERBERUS_FIGHT_STARTED) {
            self->hitboxState = 3;
            SetStep(3);
        }
        break;

    case 3:
        // pace back and forth, then pick an attack
        if (!self->step_s) {
            self->ext.cerberus.timer = 0x40;
            self->step_s++;
        }
        if (!AnimateEntity(D_us_80180708, self)) {
            PlaySfxPositional(SFX_WARG_GROWL);
        }
        if (GetDistanceToPlayerX() < 0x68) {
            self->ext.cerberus.backOff = 1;
        }
        if (GetDistanceToPlayerX() > 0x80) {
            self->ext.cerberus.backOff = 0;
        }
        if (GetDistanceToPlayerX() > 0x30) {
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        }
        MoveEntity();
        if (self->facingLeft == self->ext.cerberus.backOff) {
            self->velocityX = FIX(-0.5);
        } else {
            self->velocityX = FIX(0.5);
        }
        if (!--self->ext.cerberus.timer) {
            posX = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
            SetStep(10);
            if (self->facingLeft) {
                posX = 0x200 - posX;
            }
            side = (GetSideToPlayer() & 1) ^ 1;
            if (posX < 0xD0 && self->facingLeft == side) {
                SetStep(9);
            }
            dist = posX < 0 ? -posX : posX;
            if (dist > 0x50 && self->facingLeft != side) {
                SetStep(9);
            }
            if (posX > 0x140 && self->facingLeft == side) {
                SetStep(6);
            }
            if (posX > 0x180 && self->facingLeft == side) {
                SetStep(4);
            }
            if ((g_Player.status & PLAYER_STATUS_BAT_FORM) ||
                (PLAYER.posY.i.hi < 0xA0 && (g_Player.vram_flag & 1))) {
                SetStep(5);
            }
        }
        break;

    case 11:
        // unused: charge and slow down
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_8018071C, self)) {
                if (self->facingLeft) {
                    self->velocityX = FIX(-6);
                } else {
                    self->velocityX = FIX(6);
                }
                self->ext.cerberus.timer = 0x40;
                self->step_s++;
            }
            break;
        case 1:
            MoveEntity();
            self->velocityX -= self->velocityX >> 6;
            if (!--self->ext.cerberus.timer) {
                SetStep(3);
            }
            break;
        }
        break;

    case 5:
        // flame breath against a player above or flying
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180754, self)) {
                SetSubStep(1);
            }
            break;
        case 1:
            if (!AnimateEntity(D_us_80180748, self)) {
                PlaySfxPositional(SFX_WARG_ATTACK);
                PlaySfxPositional(SFX_SHAFT_FIRE_ATTACK);
                self->ext.cerberus.timer = 0x80;
                SetSubStep(2);
            }
            break;
        case 2:
            if (!(self->ext.cerberus.timer & 3)) {
                for (i = 0; i < 3; i++) {
                    entity = AllocEntity(&g_Entities[128], &g_Entities[224]);
                    if (entity != NULL) {
                        CreateEntityFromEntity(E_UNK_1E, self, entity);
                        if (self->facingLeft) {
                            entity->posX.i.hi -= D_us_801806F0[i][0];
                        } else {
                            entity->posX.i.hi += D_us_801806F0[i][0];
                        }
                        entity->params = i;
                        entity->posY.i.hi += D_us_801806F0[i][1];
                        entity->zPriority = self->zPriority + 1;
                        entity->facingLeft = self->facingLeft;
                    }
                }
            }
            if (!--self->ext.cerberus.timer) {
                self->step_s++;
            }
            break;
        case 3:
            if (AnimateEntity(D_us_80180760, self)) {
                SetStep(3);
            }
            break;
        }
        break;

    case 10:
        // single fireball
        if (!AnimateEntity(D_us_8018073C, self)) {
            SetStep(3);
        }
        if (!self->poseTimer && self->pose == 3) {
            CerberusSpitFireball(self);
        }
        break;

    case 9:
        CerberusLeap(self, false);
        break;

    case 4:
        CerberusLeap(self, true);
        break;

    case 6:
        // volley of fireballs of each type
        switch (self->step_s) {
        case 0:
            self->ext.cerberus.flameCount = 8;
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_8018072C, self)) {
                if (!--self->ext.cerberus.flameCount) {
                    SetStep(3);
                }
            }
            if (!self->poseTimer && self->pose == 5) {
                CerberusSpitFireball(self);
                if (++self->ext.cerberus.fireballType >= 3) {
                    self->ext.cerberus.fireballType = 0;
                }
            }
            break;
        }
        break;

    case 8:
        // death
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            D_us_80180848 |= CERBERUS_DEFEATED;
            PlaySfxPositional(SFX_WARG_DEATH_HOWL);
            self->step_s++;
            // fallthrough
        case 1:
            if (!(g_Timer & 7)) {
                PlaySfxPositional(SFX_EXPLODE_B);
                entity = AllocEntity(&g_Entities[192], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_UNK_20, self, entity);
                    entity->params = 1;
                    entity->zPriority = self->zPriority + 1;
                    entity->posX.i.hi += (Random() & 0x3F) - 0x20;
                    entity->posY.i.hi += (Random() & 0x1F) - 0xC;
                }
            }
            if (!AnimateEntity(D_us_8018076C, self)) {
                PlaySfxPositional(SFX_BOSS_LARGE_FLAMES);
                self->ext.cerberus.timer = 0x50;
                self->step_s++;
            }
            break;
        case 2:
            entity = AllocEntity(&g_Entities[192], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_20, self, entity);
                entity->params = 2;
                entity->zPriority = self->zPriority + 1;
                entity->posX.i.hi += (Random() & 0x3F) - 0x20;
                if (self->facingLeft) {
                    entity->posX.i.hi -= 0xC;
                } else {
                    entity->posX.i.hi += 0xC;
                }
                entity->posY.i.hi += 0x18;
            }
            if (!--self->ext.cerberus.timer) {
                self->animCurFrame = 0;
                self->step_s++;
            }
            break;
        case 3:
            D_us_80180848 |= CERBERUS_DEATH_DONE;
            self->step_s++;
            break;
        case 4:
            break;
        }
        break;

    case 0xFF:
        // debug: browse the animation frames with square and circle
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
            if (!self->step_s) {
                self->animCurFrame--;
                self->step_s |= 1;
            }
        } else {
            self->step_s = 0;
        }
        break;
    }

    // keep inside the arena
    posX = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
    if (self->velocityX < 0) {
        if (posX < 0x68) {
            self->posX.i.hi = 0x68 - g_Tilemap.scrollX.i.hi;
        }
    } else if (posX > 0x1A0) {
        self->posX.i.hi = 0x1A0 - g_Tilemap.scrollX.i.hi;
    }

    hitbox = D_us_801807E4[D_us_8018081C[self->animCurFrame]];
    self->hitboxOffX = hitbox[0];
    self->hitboxOffY = hitbox[1];
    self->hitboxWidth = (u8)hitbox[2];
    self->hitboxHeight = (u8)hitbox[3];
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/unk_14CE0", func_us_801959E0);
#else
// fireball
void func_us_801959E0(Entity* self) {
    Entity* explosion;

    if (D_us_80180848 & CERBERUS_DEFEATED) {
        DestroyEntity(self);
        return;
    }
    if (self->flags & FLAG_DEAD) {
        explosion = AllocEntity(&g_Entities[160], &g_Entities[192]);
        if (explosion != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, explosion);
            explosion->params = 1;
        }
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_8018046C);
        if (self->facingLeft) {
            self->velocityX = FIX(6);
        } else {
            self->velocityX = FIX(-6);
        }
        // fallthrough
    case 1:
        MoveEntity();
        AnimateEntity(D_us_801807BC[self->params], self);
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/unk_14CE0", func_us_80195AF0);
#else
// flame breath
void func_us_80195AF0(Entity* self) {
    Entity* entity;
    s16 angle;

    if (D_us_80180848 & CERBERUS_DEFEATED) {
        DestroyEntity(self);
        return;
    }
    if (!self->step) {
        angle = -0x680;
        InitializeEntity(D_us_80180478);
        self->drawFlags = ENTITY_ROTATE | ENTITY_SCALEX;
        self->scaleX = 0x80;
        self->rotate = -0x280;
        self->palette = D_us_801806E8[self->params];
        if (self->facingLeft) {
            angle = -0x180;
        }
        self->velocityX = (rcos(angle) * FIX(6)) >> 12;
        self->velocityY = (rsin(angle) * FIX(6)) >> 12;
    }
    MoveEntity();
    if (!AnimateEntity(D_us_801807C8, self)) {
        DestroyEntity(self);
    }
    if (!self->poseTimer && self->pose == 8) {
        entity = AllocEntity(&g_Entities[128], &g_Entities[224]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_UNK_1E, self, entity);
            entity->params = self->params;
            entity->facingLeft = self->facingLeft;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/unk_14CE0", func_us_80195C50);
#else
// arena controller: fire and heat haze behind the boss, starts the fight,
// switches the music and spawns the reward once Cerberus is defeated
void func_us_80195C50(Entity* self) {
    DRAWENV drawEnv;
    RECT clip;
    Primitive* prim;
    Entity* entity;
    s32 primIndex;
    s32 i;
    s16 x;
    s32 brightness;
    s16 posX, posY;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->flags |= FLAG_UNK_10000;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 7);
        if (primIndex == -1) {
            self->step = 0;
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->flags |= FLAG_HAS_PRIMS;
        if (g_api.func_800EDB08(prim) == NULL) {
            goto fail;
        }
        prim->type = PRIM_ENV;
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
        g_api.func_800EDB08(prim);
        if (prim == NULL) { // the original checks the prim, not the result
        fail:
            g_api.FreePrimitives(primIndex);
            self->step = 0;
            self->flags &= ~FLAG_HAS_PRIMS;
            return;
        }
        prim->type = PRIM_ENV;
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;

        // red glow at the bottom of the arena
        self->ext.prim = prim;
        prim->type = PRIM_G4;
        prim->r0 = prim->g0 = prim->b0 = 0;
        prim->x1 = prim->x3 = 0x42;
        prim->y2 = prim->y3 = 0xB0;
        prim->r2 = 0xFF;
        prim->g2 = prim->b2 = 0;
        prim->x0 = prim->x2 = 0;
        prim->y0 = prim->y1 = 0;
        prim->priority = 0xF;
        prim->drawMode = DRAW_DEFAULT;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r2);
        prim = prim->next;

        // the background, redrawn from the offscreen copy
        x = 0x40;
        for (i = 0; i < 4; i++) {
            prim->tpage = 0x110;
            if (i & 1) {
                prim->u0 = prim->u2 = 0x3F;
                prim->u1 = prim->u3 = 0;
            } else {
                prim->u0 = prim->u2 = 0;
                prim->u1 = prim->u3 = 0x3F;
            }
            prim->x0 = prim->x2 = i << 6;
            prim->priority = 0x13;
            prim->v0 = prim->v1 = 0x10;
            prim->v2 = prim->v3 = 0xF0;
            prim->x1 = prim->x3 = x;
            prim->y0 = prim->y1 = 0x10;
            prim->y2 = prim->y3 = 0xF0;
            prim->drawMode = DRAW_DEFAULT;
            prim = prim->next;
            x += 0x40;
        }
        g_api.PlaySfx(SFX_BOSS_LARGE_FLAMES);
        // fallthrough
    case 1:
        if ((u32)(PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi - 0x41) < 0x17F) {
            D_us_80180848 |= CERBERUS_PLAYER_ENTERED;
            self->step++;
        }
        if (g_Timer % 95 == 0) {
            g_api.PlaySfx(SFX_BOSS_LARGE_FLAMES);
        }
        break;
    case 2:
        if (g_Timer % 95 == 0) {
            g_api.PlaySfx(SFX_BOSS_LARGE_FLAMES);
        }
        if ((u32)(PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi - 0x81) < 0xFF) {
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_CERBERUS_DEFEAT, TIMEATTACK_SET_VISITED);
            stopMusicFlag = 1;
            currentMusicId = MU_FESTIVAL_OF_SERVANTS;
            D_us_80180848 |= CERBERUS_FIGHT_STARTED;
            self->step++;
        }
        break;
    case 3:
        if (!g_api.func_80131F68()) {
            stopMusicFlag = 0;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        // fallthrough
    case 4:
        if (D_us_80180848 & CERBERUS_DEFEATED) {
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_CERBERUS_DEFEAT, TIMEATTACK_SET_RECORD);
            g_api.PlaySfx(0x90);
            currentMusicId = MU_ABANDONED_PIT;
            self->step++;
        }
        break;
    case 5:
        if (D_us_80180848 & CERBERUS_DEATH_DONE) {
            self->step++;
        }
        break;
    case 6:
        posX = 0x100 - g_Tilemap.scrollX.i.hi;
        posY = 0x80 - g_Tilemap.scrollY.i.hi;
        entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_LIFE_UP_SPAWN, self, entity);
            entity->posX.i.hi = posX;
            entity->posY.i.hi = posY;
            entity->params = 5;
            stopMusicFlag = 1;
            currentMusicId = MU_ABANDONED_PIT;
            D_us_80180848 |= CERBERUS_REWARD_SPAWNED;
            self->step++;
        }
        // fallthrough
    case 7:
        if (!g_api.func_80131F68()) {
            stopMusicFlag = 0;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;
    }

    // copy the background into the offscreen area the haze prims sample
    prim = &g_PrimBuf[self->primIndex];
    drawEnv = g_CurrentBuffer->draw;
    drawEnv.isbg = true;
    drawEnv.r0 = drawEnv.g0 = drawEnv.b0 = 0;
    clip.x = 0;
    clip.y = 0x100;
    clip.w = 0x7F;
    clip.h = 0xFF;
    drawEnv.clip = clip;
    drawEnv.ofs[0] = 0;
    drawEnv.ofs[1] = 0x100;
    SetDrawEnv(*(DR_ENV**)&prim->r1, &drawEnv);
    prim->priority = 0xE;
    prim->drawMode = DRAW_DEFAULT;
    prim = prim->next;
    prim->priority = 0x11;
    prim->drawMode = DRAW_UNK_800;

    prim = self->ext.prim;
    self->rotate += 0x100;
    if (D_us_80180848 & CERBERUS_DEFEATED) {
        PrimDecreaseBrightness(prim, 4);
    } else {
        brightness = (rcos(self->rotate) << 7) >> 12;
        if (brightness < 0) {
            brightness = -brightness;
        }
        prim->g2 = brightness;
        LOW(prim->r3) = LOW(prim->r2);
    }
    if (!(D_us_80180848 & CERBERUS_DEFEATED) && !(g_Timer & 3)) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromCurrentEntity(E_UNK_20, entity);
            entity->posX.i.hi = (Random() & 0x3F) + 0xA;
            entity->posY.i.hi = 0xB0;
            entity->zPriority = 0x10;
            entity->params = 0;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/unk_14CE0", func_us_801963D8);
#else
// flames: 1 = rising embers, 2 = drifting smoke, 3 = death burst
void func_us_801963D8(Entity* self) {
    CerberusFlameParams* params;
    s32 variant;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitParticle);
        variant = self->params & 0xF;
        params = &D_us_801808A4[variant];
        self->palette = params->palette + 0x2E0;
        self->blendMode = params->blendMode;
        self->animSet = params->animSet;
        self->unk5A = params->unk5A;
        self->step = variant + 1;
        self->ext.shaftDeathFlames.anim = params->anim;
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
            self->opacity = 0x50;
            self->flags &= ~FLAG_POS_CAMERA_LOCKED;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 8) - FIX(0.5);
            self->velocityY = FIX(-1.5);
            self->ext.shaftDeathFlames.accel = -(Random() * 16) - 0x1000;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += self->ext.shaftDeathFlames.accel;
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
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0xC0;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 8) - FIX(0.5);
            self->velocityY = FIX(-0.75);
            self->ext.shaftDeathFlames.accel = -(Random() * 16) - 0x4000;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += self->ext.shaftDeathFlames.accel;
        self->opacity--;
        break;

    default:
        return;
    }
    if (!AnimateEntity(self->ext.shaftDeathFlames.anim, self)) {
        DestroyEntity(self);
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180798[];
extern u8 D_us_801807A4[];
extern u8 D_us_801807B0[];
extern u8 D_us_8018084C[];
extern u8 D_us_80180884[];
u8 D_us_80180798[] ALIGNED4 = {0x03, 0x19, 0x05, 0x1A, 0x07, 0x1B, 0x08, 0x1C, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801807A4[] ALIGNED4 = {0x03, 0x1D, 0x05, 0x1E, 0x07, 0x1F, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801807B0[] ALIGNED4 = {0x03, 0x21, 0x05, 0x22, 0x07, 0x23, 0x08, 0x24, 0x00, 0x00, 0x00, 0x00};
u8 D_us_8018084C[] ALIGNED4 = {0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0xFF, 0x00, 0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0x00, 0x00};
u8 D_us_80180884[] ALIGNED4 = {0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x04, 0x02, 0x05, 0x02, 0x06, 0x02, 0x07, 0x02, 0x08, 0x02, 0x09, 0x02, 0x0A, 0x02, 0x0B, 0x02, 0x0C, 0x02, 0x0D, 0x02, 0x0E, 0x00, 0x00, 0x00, 0x00};
s16 D_us_801806D8[8] = { 0, 0x20, 0, 4, 8, -4, -0x10, 0 };
u16 D_us_801806E8[4] = { 0x2E4, 0x2EA, 0x2E9, 0 };
u8 D_us_801806FC[0xC] = { 0x12, 3, 0x10, 2, 0x16, 1, 0x10, 2, 0, 0, 0, 0 };
u8 D_us_80180708[0x14] = { 0x12, 3, 6, 4, 4, 5, 6, 6, 0x12, 7, 6, 6, 4, 5, 6, 4, 0, 0, 0, 0 };
u8 D_us_8018071C[0x10] = { 2, 7, 2, 6, 2, 5, 2, 4, 2, 3, 2, 2, 0xFF, 0, 0, 0 };
u8 D_us_8018072C[0x10] = { 3, 3, 4, 8, 4, 9, 3, 0xA, 5, 0xB, 1, 0xC, 1, 0xD, 0, 0 };
u8 D_us_8018073C[0xC] = { 0x10, 3, 2, 0xA, 0x18, 0xB, 2, 0xC, 3, 0xD, 0xFF, 0 };
u8 D_us_80180748[0xC] = { 8, 0x11, 5, 0x12, 2, 0x13, 2, 0x14, 0x20, 0x15, 0xFF, 0 };
u8 D_us_80180754[0xC] = { 8, 3, 6, 0x16, 6, 0x17, 0x1A, 0x11, 0xFF, 0, 0, 0 };
u8 D_us_80180760[0xC] = { 8, 0x11, 6, 0x17, 6, 0x16, 8, 3, 0xFF, 0, 0, 0 };
u8 D_us_8018076C[0xC] = { 8, 3, 6, 0x16, 6, 0x17, 0x40, 0x18, 0xFF, 0, 0, 0 };
u8 D_us_80180778[0x10] = { 4, 3, 4, 0x16, 4, 0x17, 0x1A, 0x18, 3, 0x17, 1, 0x16, 0xFF, 0, 0, 0 };
u8 D_us_80180788[0x10] = { 1, 0x16, 2, 0x17, 0x10, 0x18, 6, 0x17, 6, 0x16, 8, 3, 0xFF, 0, 0, 0 };
u8* D_us_801807BC[3] = { D_us_80180798, D_us_801807A4, D_us_801807B0 };
u8 D_us_801807C8[0x1C] = {
    2,
    1,
    2,
    2,
    2,
    3,
    2,
    4,
    2,
    5,
    2,
    6,
    2,
    7,
    2,
    8,
    2,
    9,
    1,
    0xA,
    1,
    0xB,
    1,
    0xC,
    1,
    0xD,
    0xFF,
    0,
};
s8 D_us_801807E4[0xE][4] = {
    { 0, 0, 0, 0 },
    { 5, 8, 0x1C, 0x16 },
    { 4, 8, 0x12, 0x18 },
    { 5, 0xB, 0x1B, 0x14 },
    { 5, 0x10, 0x1C, 0x10 },
    { -4, 1, 0xC, 3 },
    { -4, 0, 0xC, 3 },
    { -4, 0, 0xC, 4 },
    { -5, 0, 0xC, 4 },
    { -5, 1, 0xC, 4 },
    { -4, 1, 0xC, 4 },
    { -5, 0, 0xB, 2 },
    { -4, -1, 0xC, 3 },
    { -5, 0, 0xC, 3 },
};
u8 D_us_8018081C[0x2C] = {
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
    2,
    2,
    2,
    2,
    2,
    1,
    3,
    4,
    5,
    6,
    7,
    8,
    6,
    6,
    9,
    0xA,
    0xB,
    0xC,
    7,
    0xD,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
CerberusFlameParams D_us_801808A4[3] = {
    { 0x8004, 0x50, 1, 0x30, D_us_8018084C },
    { 0x8003, 0x53, 3, 0x30, D_us_80180884 },
    { 0x8004, 0x50, 3, 0x30, D_us_8018084C },
};
s16 D_us_801806F0[3][2] = { { -0x18, 1 }, { -0x17, -0x14 }, { -0xD, -8 } };
// end pc_import_data.py
// clang-format on
#endif
