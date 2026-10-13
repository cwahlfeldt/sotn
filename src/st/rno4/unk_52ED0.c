// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rno4.h"

#if defined(VERSION_PC) || defined(M2CTX)
// RNO4's names for the water tables water_effects_rev.h uses
extern u16 D_us_80180E58[]; // g_WaterSounds
extern s16 D_us_80180E5C[]; // g_WaterXTbl
#endif

extern AnimateEntityFrame D_us_8018230C[];

#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", func_pspeu_0924B480);

INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", EntityAlucardWaterEffect);

INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", EntitySplashWater);

INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", EntitySurfacingWater);

INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", EntitySideWaterSplash);

INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", EntitySmallWaterDrop);

INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", EntityWaterDrop);
#else
#define g_WaterSounds D_us_80180E58
#define g_WaterXTbl D_us_80180E5C
#include "../water_effects_rev.h"
#endif

#ifdef VERSION_PC
extern EInit g_EInitDarkOctopus; // upstream declares it as u16
#endif
extern s16 D_us_801822CC[];
#ifdef VERSION_PC
extern s16 D_us_801822DC[]; // sensors: four values, not one
#else
extern s16 D_us_801822DC;
#endif
extern u8 D_us_801822E4[];
extern u8 D_us_801822F8[];
extern u8 D_us_80182328[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", func_us_801D511C);
#else
// The Dark Octopus (params 0). It walks, turning at walls and every 0x100
// frames, then stops to spray ink (0x3F) at an angle it sweeps one way,
// the other or at random. rno4.h calls its id 0x3D E_SUBWPN_IN_CONT. Dying, it flashes through a fading copy of
// itself (params 1, which with params 2 follows `copy`), waits for its
// upper part (0x3E, in the next slot) and shrinks into explosions.
void func_us_801D511C(Entity* self) {
    Collider col;
    Entity* other;
    s32 x;

    if ((self->flags & FLAG_DEAD) && self->step < 4) {
        self->hitboxState = 0;
        self->opacity = 0x80;
        self->drawFlags |= ENTITY_OPACITY;
        SetStep(4);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitDarkOctopus);
        if (self->params) {
            if (self->params == 1) {
                self->palette = PAL_FLAG(0x225);
            } else {
                self->palette = PAL_FLAG(0x224);
            }
            self->blendMode = BLEND_ADD | BLEND_TRANSP;
            self->step = 5;
            self->opacity = 0;
            self->drawFlags |= ENTITY_OPACITY;
            self->hitboxState = 0;
            self->flags |= FLAG_UNK_00200000 | FLAG_UNK_2000;
            self->zPriority = self->ext.darkOctopus.copy->zPriority + 1;
            break;
        }
        other = self + 1;
        CreateEntityFromEntity(E_UNK_3E, self, other);
        self->hitboxOffX = 1;
        self->hitboxOffY = 0xD;
        self->nextPart = other;
        self->ext.darkOctopus.timer = Random() & 3;
        break;

    case 1:
        if (UnkCollisionFunc3(D_us_801822CC) & 1) {
            self->ext.darkOctopus.walkTimer = 0x100;
            self->ext.darkOctopus.walkTimer = Random();
            self->velocityX = FIX(-0.5);
            if (Random() & 1) {
                self->velocityX = -self->velocityX;
            }
            self->step++;
        }
        break;

    case 2:
        UnkCollisionFunc2(D_us_801822DC);
        AnimateEntity(D_us_801822E4, self);
        if (self->velocityX > 0) {
            x = self->posX.i.hi + 0x18;
        } else {
            x = self->posX.i.hi - 0x18;
        }
        g_api.CheckCollision(x, self->posY.i.hi - 8, &col, 0);
        if (col.effects & EFFECT_SOLID) {
            self->ext.darkOctopus.walkTimer = 0;
        }
        if (!self->ext.darkOctopus.walkTimer) {
            self->ext.darkOctopus.walkTimer = 0x100;
            self->velocityX = -self->velocityX;
            self->facingLeft ^= 1;
        } else {
            self->ext.darkOctopus.walkTimer--;
        }
        if (!self->ext.darkOctopus.timer) {
            self->ext.darkOctopus.angle = (Random() & 0x7F) * 0x10 - 0x400;
            self->ext.darkOctopus.spin = Random() & 3;
            g_api.PlaySfx(SFX_UNK_RNO4_784);
            self->ext.darkOctopus.timer = 0x40;
            SetStep(3);
            break;
        }
        self->ext.darkOctopus.timer--;
        break;

    case 3:
        AnimateEntity(D_us_801822F8, self);
        switch (self->ext.darkOctopus.spin) {
        case 1:
            self->ext.darkOctopus.angle += 0x10;
            break;
        case 2:
            self->ext.darkOctopus.angle -= 0x10;
            break;
        case 3:
            AnimateEntity(D_us_801822F8, self);
            self->ext.darkOctopus.angle = (Random() & 0x7F) * 0x10 - 0x400;
            break;
        }
        switch (self->step_s) {
        case 0:
            if (!--self->ext.darkOctopus.timer) {
                self->ext.darkOctopus.timer = 0x40;
                self->step_s++;
            }
            // fallthrough
        case 1:
            AnimateEntity(D_us_801822F8, self);
            if (!(self->ext.darkOctopus.timer % 6)) {
                other = AllocEntity(&g_Entities[160], &g_Entities[188]);
                if (other != NULL) {
                    CreateEntityFromEntity(E_UNK_3F, self, other);
                    other->ext.et_801D5BA4.unk84 = self->ext.darkOctopus.angle;
                }
            }
            if (self->step_s && !(self->ext.darkOctopus.timer & 0xF)) {
                g_api.PlaySfx(SFX_NOISE_SWEEP_DOWN_A);
            }
            if (!--self->ext.darkOctopus.timer) {
                self->ext.darkOctopus.timer = 0x80;
                self->step_s++;
            }
            break;
        case 2:
            if (!--self->ext.darkOctopus.timer) {
                SetStep(2);
                self->ext.darkOctopus.timer = 0x80;
            }
            break;
        }
        break;

    case 4:
        switch (self->step_s) {
        case 0:
            other = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (other != NULL) {
                CreateEntityFromEntity(E_SUBWPN_IN_CONT, self, other); // this octopus
                other->params = 1;
                self->ext.darkOctopus.copy = other;
                other->ext.darkOctopus.copy = self;
            } else {
                self->ext.darkOctopus.copy = NULL;
            }
            self->step_s++;
            break;
        case 1:
            AnimateEntity(D_us_801822F8, self);
            self->opacity -= 4;
            if (!self->opacity) {
                other = self->ext.darkOctopus.copy;
                self->palette = PAL_FLAG(0x224);
                self->blendMode = 0;
                self->opacity = 0x80;
                if (other != NULL) {
                    DestroyEntity(other);
                }
                self->step_s++;
            }
            break;
        case 2:
            AnimateEntity(D_us_801822F8, self);
            if ((self + 1)->entityId != E_UNK_3E) {
                self->ext.darkOctopus.timer = 0x80;
                self->scaleX = 0x100;
                self->scaleY = 0x100;
                self->poseTimer = 0;
                self->pose = 0;
                self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY;
                self->step_s++;
            }
            break;
        case 3:
            if (!(g_Timer & 7)) {
                other = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (other != NULL) {
                    CreateEntityFromEntity(E_EXPLOSION, self, other);
                    other->params = 0x11;
                }
            }
            AnimateEntity(D_us_80182328, self);
            if (!(--self->ext.darkOctopus.timer & 0xF)) {
                g_api.PlaySfx(SFX_NOISE_SWEEP_DOWN_A);
            }
            if (self->ext.darkOctopus.timer) {
                self->scaleX--;
                break;
            }
            other = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (other != NULL) {
                CreateEntityFromEntity(E_EXPLOSION, self, other);
                other->params = 1;
                other->posY.i.hi += 0xC;
            }
            PlaySfxPositional(SFX_EXPLODE_B);
            DestroyEntity(self);
            break;
        }
        break;

    case 5:
        // a fading copy following the original
        other = self->ext.darkOctopus.copy;
        self->posX.i.hi = other->posX.i.hi;
        self->posY.i.hi = other->posY.i.hi;
        self->animCurFrame = other->animCurFrame;
        self->opacity = 0x80 - other->opacity;
        break;

    case 16:
#define PAD2_ANIM_DEBUG_PRINT()
#include "../pad2_anim_debug.h"
    }
}
#endif

extern s16 D_us_80182330[];
#ifndef VERSION_PC
extern u16 g_EInitDarkOctopus; // an EInit (rno4.h declares it on PC)
#endif

void func_us_801D58FC(Entity* self) {
    Entity* prevEnt;

    if ((self->flags & FLAG_DEAD) && (self->step < 2)) {
        self->hitboxState = 0;
        self->drawFlags |= ENTITY_OPACITY;
        self->opacity = 0x80;
        self->ext.et_801D58FC.unk82 = 0x80;
        SetStep(2);
    }

    prevEnt = self - 1;
    self->posX.i.hi = prevEnt->posX.i.hi;
    self->posY.i.hi = prevEnt->posY.i.hi;
    self->posY.i.hi += D_us_80182330[prevEnt->animCurFrame];

    switch (self->step) {
    case 0:
        InitializeEntity(&g_EInitDarkOctopus);
        self->hitboxWidth = 6;
        self->hitboxHeight = 0xB;
        self->hitboxOffX = 1;
        self->hitboxOffY = -5;
        self->nextPart = self - 1;

#if defined(VERSION_PSP)
        break;
#endif

    case 1:
        AnimateEntity(D_us_8018230C, self);

        break;

    case 2:
        if (!(g_Timer % 8)) {
            prevEnt =
                AllocEntity(&g_Entities[0xE0], &g_Entities[TOTAL_ENTITY_COUNT]);
            if (prevEnt) {
                CreateEntityFromEntity(E_EXPLOSION, self, prevEnt);
                prevEnt->params = 0x11;
            }
        }
        switch (self->step_s) {
        case 0:
            prevEnt =
                AllocEntity(&g_Entities[0xE0], &g_Entities[TOTAL_ENTITY_COUNT]);
            if (prevEnt) {
                CreateEntityFromEntity(E_SUBWPN_IN_CONT, self, prevEnt);
                prevEnt->params = 2;
                self->ext.et_801D58FC.entity = prevEnt;
                prevEnt->ext.et_801D58FC.entity = self;
            } else {
                self->ext.et_801D58FC.entity = 0;
            }
            self->step_s += 1;

            break;

        case 1:
            self->opacity -= 4;

            if (!self->opacity) {
                self->blendMode = BLEND_NO;
                self->opacity = 0x80;
                self->poseTimer = 0;
                self->pose = 0;
                self->palette = PAL_FLAG(0x224);
                self->drawFlags |= ENTITY_SCALEY;
                self->scaleY = 0x100;

                if (self->ext.et_801D58FC.entity != 0) {
                    prevEnt = self->ext.et_801D58FC.entity;
                    DestroyEntity(prevEnt);
                }

                self->step_s += 1;
            }
            break;

        case 2:
            self->scaleY -= 4;
            self->posY.val += FIX(0.1875);

            if (self->scaleY < 0x20) {
                self->step_s += 1;
            }

            break;

        case 3:
            DestroyEntity(self);
        }
    }
}

extern u16 D_us_80180B54;
#ifdef VERSION_PC
extern s16 D_us_801822DC[]; // sensors: four values, not one
#else
extern s16 D_us_801822DC;
#endif

void func_us_801D5BA4(Entity* self) {
    Entity* player;

    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180B54);
        self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY | ENTITY_ROTATE;
        self->drawFlags |= ENTITY_OPACITY;
        self->blendMode = BLEND_SUB | BLEND_TRANSP;
        self->opacity = 0x40;
        self->scaleX = 0x10;
        self->scaleY = 0x40;
        player = &PLAYER;
        self->zPriority = player->zPriority + 1;
        self->rotate = self->ext.et_801D5BA4.unk84;
        self->ext.et_801D5BA4.unk88 = 0x30;
        self->step_s = 0;
        return;

    case 1:
        UnkCollisionFunc2(&D_us_801822DC);
        self->velocityX =
            rsin(self->ext.et_801D5BA4.unk84) * self->ext.et_801D5BA4.unk88;
        self->velocityY =
            -rcos(self->ext.et_801D5BA4.unk84) * self->ext.et_801D5BA4.unk88;
        if (self->ext.et_801D5BA4.unk88 > 12) {
            self->ext.et_801D5BA4.unk88 -= 2;
        }
        switch (self->step_s) {
        case 0:
            self->scaleX += 8;
            self->scaleY += 0x10;
            if (self->scaleX == 0x100) {
                self->scaleY = 0x100;
                self->ext.et_801D5BA4.unk88 = 4;
                self->hitboxState = 0;
                self->step_s += 1;
                return;
            }
            break;

        case 1:
            self->scaleX += 8;
            self->scaleY += 8;
            self->opacity -= 2;
            if (!self->opacity) {
                DestroyEntity(self);
            }
        }
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", StepTowards);
#else
#include "../step_towards.h"
#endif

void func_us_801D5DC8(Primitive* prim) {
    s32 yPos;
    switch (prim->p2) {
    case 0:
        LOW(prim->x2) = Random() * 8;
        prim->x3 = (Random() & 0x1F) + 0x10;
        prim->drawMode = DRAW_UNK02;
        prim->p2 += 1;

    case 1:
        yPos = (prim->y0 << 0x10) + prim->y1;
        yPos = yPos + LOW(prim->x2);
        LOW(prim->x2) -= 0x1000;
        prim->y0 = yPos >> 0x10;
        prim->y1 = yPos & 0xffff;
        if (!(prim->x3 -= 1)) {
            prim->p2 = 0;
            prim->drawMode = DRAW_HIDE;
            prim->p3 = 0;
        }
        return;
    }
}

extern EInit g_EInitCaveTroll;
extern s16 D_us_80182344[];
extern u8 D_us_80182364[];
extern u8 D_us_8018236C[];
extern u8 D_us_80182374[];
extern u8 D_us_80182384[];
extern u8 D_us_8018238C[];
extern s8 D_us_801823C0[][4];
extern u8 D_us_801823D4[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", func_us_801D5E90);
#else
Primitive* FindFirstUnkPrim(Primitive* prim);
u16 GetAngleBetweenEntities(Entity* a, Entity* b);
// The Cave Troll. It hops toward Alucard, turning when he is behind it,
// and from close by either leaps and lashes its tongue (0x41) at him, or
// breathes (0x42) on him. It sparkles while it aims the tongue.
void func_us_801D5E90(Entity* self) {
    Collider col;
    Entity* other;
    Primitive* prim;
    s32 x;
    s32 y;
    s32 r;
    s32 dist;
    s32 side;
    s32 floor;
    s16 angle;
    u16 aim;
    s8* hitbox;

    if (self->flags & FLAG_DEAD) {
        PlaySfxPositional(SFX_QUICK_STUTTER_EXPLODE_B);
        other = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (other != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, other);
            other->params = 2;
        }
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCaveTroll);
        self->animCurFrame = 1;
        r = g_api.AllocPrimitives(PRIM_TILE_ALT, 0x20);
        if (r == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[r];
        self->primIndex = r;
        self->ext.caveTroll.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->u0 = prim->v0 = 1;
            prim->r0 = 0x80;
            prim->g0 = 0x80;
            prim->b0 = 0xC0;
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority + 1;
            prim = prim->next;
        }
        break;

    case 1:
        if (UnkCollisionFunc3(D_us_80182344) & 1) {
            SetStep(2);
        }
        break;

    case 2:
        if (GetDistanceToPlayerX() < 0x60) {
            SetStep(3);
        }
        break;

    case 3:
        // hops
        switch (self->step_s) {
        case 0:
            self->velocityY = FIX(-2);
            self->velocityX = self->facingLeft ? FIX(1.5) : FIX(-1.5);
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY += FIX(1.0 / 8);
            if (self->velocityY < 0) {
                self->animCurFrame = 2;
            }
            if (self->velocityY >= -0x7FFF) {
                self->animCurFrame = 3;
            }
            if (self->velocityY > 0x8000) {
                self->animCurFrame = 4;
            }
            g_api.CheckCollision(
                self->posX.i.hi, self->posY.i.hi + 0x19, &col, 0);
            if (!(col.effects & EFFECT_SOLID)) {
                break;
            }
            PlaySfxPositional(SFX_STOMP_HARD_D);
            floor = col.unk18;
            g_api.CheckCollision(
                self->posX.i.hi, self->posY.i.hi + 0x11, &col, 0);
            if (col.effects & EFFECT_SOLID) {
                SetSubStep(3);
                break;
            }
            self->posY.i.hi += floor;
            side = (GetSideToPlayer() & 1) ^ 1;
            dist = GetDistanceToPlayerX();
            if (side != self->facingLeft && dist > 0x40) {
                SetSubStep(3);
            } else {
                SetSubStep(2);
            }
            if (dist > 0x20 && dist < 0x80) {
                r = Random();
                if (!(r & 1)) {
                    SetStep(4);
                }
                if (r < 3 || self->hitPoints < 0x10) {
                    SetStep(5);
                }
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80182364, self)) {
                SetSubStep(0);
            }
            break;
        case 3:
            // turns around
            if (!AnimateEntity(D_us_8018236C, self)) {
                self->animCurFrame = 2;
                self->facingLeft ^= 1;
                SetSubStep(0);
            }
            break;
        }
        break;

    case 4:
        // leaps and lashes its tongue
        switch (self->step_s) {
        case 0:
            side = (GetSideToPlayer() & 1) ^ 1;
            if (self->facingLeft == side) {
                SetSubStep(2);
                break;
            }
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_8018236C, self)) {
                self->animCurFrame = 1;
                self->facingLeft ^= 1;
                SetSubStep(2);
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_8018238C, self)) {
                self->velocityY = FIX(-6);
                self->velocityX = 0;
                self->animCurFrame = 0x13;
                PlaySfxPositional(SFX_STOMP_HARD_E);
                self->step_s++;
            }
            break;
        case 3:
            MoveEntity();
            self->velocityY += FIX(3.0 / 16);
            if (self->velocityY > 0) {
                self->step_s++;
            }
            break;
        case 4:
            aim = (GetAngleBetweenEntities(self, &PLAYER) - 0x400) & 0xFFF;
            if (self->facingLeft) {
                aim = 0x1000 - aim;
            }
            if (aim & 0x800) {
                SetSubStep(8);
                break;
            }
            self->animCurFrame = 0x14;
            self->ext.caveTroll.sparkle = 1;
            self->ext.caveTroll.angle = aim;
            self->velocityY = 0;
            self->rotate = 0;
            self->drawFlags |= ENTITY_ROTATE;
            self->step_s++;
            // fallthrough
        case 5:
            FntPrint("angle %x\n", self->ext.caveTroll.angle);
            if (StepTowards(
                    &self->rotate, (s16)(self->ext.caveTroll.angle - 0x380),
                    0x28)) {
                self->step_s++;
            }
            break;
        case 6:
            other = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (other != NULL) {
                CreateEntityFromEntity(E_UNK_41, self, other);
                other->facingLeft = self->facingLeft;
                angle = self->ext.caveTroll.angle;
                if (self->facingLeft) {
                    other->posX.i.hi += (rsin(angle) * 9) >> 12;
                } else {
                    other->posX.i.hi -= (rsin(angle) * 9) >> 12;
                }
                other->ext.caveTroll.angle = angle;
                other->ext.caveTroll.troll = self;
                other->posY.i.hi += (rcos(angle) * 9) >> 12;
            }
            self->ext.caveTroll.timer = 0x40;
            PlaySfxPositional(SFX_GUARD_TINK);
            self->step_s++;
            break;
        case 7:
            if (!--self->ext.caveTroll.timer) {
                self->animCurFrame = 0x13;
                self->ext.caveTroll.sparkle = 0;
                self->step_s++;
            }
            break;
        case 8:
            StepTowards(&self->rotate, 0, 0x40);
            if (UnkCollisionFunc3(D_us_80182344) & 1) {
                PlaySfxPositional(SFX_STOMP_HARD_C);
                self->drawFlags = 0;
                SetSubStep(9);
                self->pose = 1;
            }
            break;
        case 9:
            if (!AnimateEntity(D_us_80182364, self)) {
                SetStep(3);
            }
            break;
        }
        break;

    case 5:
        // breathes on Alucard
        switch (self->step_s) {
        case 0:
            self->ext.caveTroll.breath = 0;
            side = (GetSideToPlayer() & 1) ^ 1;
            if (self->facingLeft == side) {
                SetSubStep(2);
                break;
            }
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_8018236C, self)) {
                self->animCurFrame = 1;
                self->facingLeft ^= 1;
                SetSubStep(2);
            }
            break;
        case 2:
            other = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (other == NULL) {
                SetStep(3);
                break;
            }
            PlaySfxPositional(SFX_RNO4_MAGIC_GLASS_BREAK);
            CreateEntityFromEntity(E_UNK_42, self, other);
            other->facingLeft = self->facingLeft;
            other->ext.caveTroll.troll = self;
            other->zPriority = self->zPriority + 1;
            SetSubStep(3);
            // fallthrough
        case 3:
            if (!AnimateEntity(D_us_80182374, self)) {
                self->ext.caveTroll.breath = 1;
                SetSubStep(4);
            }
            break;
        case 4:
            AnimateEntity(D_us_80182384, self);
            if (!self->ext.caveTroll.breath) {
                SetStep(3);
            }
            break;
        }
        break;

    case 0xFF:
#include "../pad2_anim_debug.h"
    }

    hitbox = D_us_801823C0[D_us_801823D4[self->animCurFrame]];
    self->hitboxOffX = hitbox[0];
    self->hitboxOffY = hitbox[1];
    self->hitboxWidth = hitbox[2];
    self->hitboxHeight = hitbox[3];
    if (self->ext.caveTroll.sparkle) {
        prim = FindFirstUnkPrim(self->ext.caveTroll.prim);
        if (prim != NULL) {
            x = self->posX.i.hi;
            y = self->posY.i.hi;
            angle = Random() << 4;
            r = Random() & 0x1F;
            x += (r * rcos(angle)) >> 12;
            prim->x0 = x;
            prim->p2 = 0;
            prim->p3 = 1;
            y += (r * rsin(angle)) >> 12;
            prim->y0 = y;
        }
    }
    for (prim = self->ext.caveTroll.prim; prim != NULL; prim = prim->next) {
        if (prim->p3) {
            func_us_801D5DC8(prim);
        }
        if (prim->next == NULL) {
            prim->x0 = 0;
            prim->y0 = 0;
            prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
        }
    }
}
#endif

extern u16 D_us_80180B6C;

void func_us_801D68E0(Entity* self) {
    Primitive* prim;
    Entity* ent;
    s32 xPosition;
    s32 primIndex;
    s32 yPosition;
    s16 angle;

    if (self->hitFlags && !(self->hitFlags & 0x80)) {
        self->hitboxState = 0;
        SetStep(2);
    }

    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180B6C);
        primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, 1);

        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }

        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801D68E0.unk7C = prim;

        prim->x0 = (prim->x1 = self->posX.i.hi);
        prim->y0 = (prim->y1 = self->posY.i.hi);
        prim->r0 = 0xFF;
        prim->g0 = 0x60;
        prim->b0 = 0x60;
        prim->r1 = 0x80;
        prim->g1 = 0x40;
        prim->b1 = 0x40;
        prim->priority = self->zPriority;
        prim->drawMode =
            DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;

        angle = self->ext.et_801D68E0.unk8C;

        if (self->facingLeft) {
            angle = -angle;
        }

        self->velocityX = rsin((s32)angle) * (-0x80);
        self->velocityY = rcos((s32)angle) << 7;
        self->ext.et_801D68E0.unk80 = 0x20;

    case 1:
        MoveEntity();

        prim = self->ext.et_801D68E0.unk7C;
        xPosition = (prim->x0 = self->posX.i.hi);
        yPosition = (prim->y0 = self->posY.i.hi);

        if (!(self->ext.et_801D68E0.unk80 -= 1)) {
            self->step = 2;
        }

        break;

    case 2:
        self->velocityX = -self->velocityX;
        self->velocityY = -self->velocityY;
        self->step++;

    case 3:
        MoveEntity();

        prim = self->ext.et_801D68E0.unk7C;
        xPosition = (prim->x0 = self->posX.i.hi);
        yPosition = (prim->y0 = self->posY.i.hi);

        prim = self->ext.et_801D68E0.unk7C;
        xPosition -= prim->x1;
        yPosition -= prim->y1;

        if ((abs(xPosition) < 8) && (abs(yPosition) < 8)) {
            DestroyEntity(self);
        }
    }

    ent = self->ext.et_801D68E0.unk9C;
#if defined(VERSION_PSP) || defined(FIX_UB)
    // Non-PSX versions fix a null pointer here
    if (ent != NULL) {
#else
    if (1) {
#endif
        if (ent->entityId != 0x40) {
            DestroyEntity(self);
        }
    }
}

extern EInit D_us_80180B78;
extern u8 D_us_8018239C[];
extern u8 D_us_801823B0[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_52ED0", func_us_801D6B8C);
#else
// The Cave Troll's breath: waits for the troll's breathing pose, then
// swells and shrinks, its hitbox reaching toward Alucard
void func_us_801D6B8C(Entity* self) {
    Entity* troll;
    s32 reach;
    s32 dx;
    s32 dy;
    s32 dist;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180B78);
        self->animCurFrame = 0;
        break;
    case 1:
        if (self->ext.caveTroll.troll->pose == 4) {
            self->step++;
        }
        break;
    case 2:
        AnimateEntity(D_us_8018239C, self);
        troll = self->ext.caveTroll.troll;
        if (troll->ext.caveTroll.breath) {
            self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
            self->scaleX = self->scaleY = 0x180;
            self->facingLeft = 0;
            self->ext.caveTroll.timer = 0x60;
            self->zPriority = troll->zPriority - 1;
            SetStep(3);
        }
        break;
    case 3:
        AnimateEntity(D_us_801823B0, self);
        if (self->scaleX < 0x200) {
            self->scaleX = self->scaleY += 0x20;
        } else if (!--self->ext.caveTroll.timer) {
            SetStep(4);
        }
        // fallthrough
    case 4:
        AnimateEntity(D_us_801823B0, self);
        if (!self->scaleX) {
            self->ext.caveTroll.troll->ext.caveTroll.breath = 0;
            DestroyEntity(self);
            return;
        }
        self->scaleX = self->scaleY -= 4;
        angle = GetAngleBetweenEntities(self, &PLAYER);
        reach = (self->scaleX * 0xF) >> 7;
        dx = GetDistanceToPlayerX();
        dy = GetDistanceToPlayerX();
        dist = SquareRoot0(dx * dx + dy * dy) - 4;
        if (dist < reach) {
            reach = dist;
        }
        self->hitboxOffX = (reach * rcos(angle)) >> 12;
        self->hitboxOffY = (reach * rsin(angle)) >> 12;
        break;
    }
    self->palette = (g_Timer & 1) ? PAL_FLAG(0x163) : PAL_FLAG(0x19C);
    if (self->ext.caveTroll.troll->entityId != E_UNK_40) {
        DestroyEntity(self);
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u16 D_us_80180E58[2] = { 0x7C2, 0x7C3 };
s16 D_us_80180E5C[0xF8] = {
    0xE0,
    0x20,
    3,
    0xB8,
    0xF0,
    1,
    7,
    -0x2000,
    0xC0,
    0x20,
    1,
    0xD8,
    0xF0,
    7,
    3,
    -0x2000,
    0xC0,
    0x30,
    0,
    0xF0,
    0x5B0,
    7,
    7,
    0,
    0x60,
    0xC0,
    0,
    0x5B0,
    0x5F0,
    7,
    7,
    0,
    0xB80,
    0x60,
    0,
    0xB0,
    0xF0,
    7,
    7,
    0,
    0xA40,
    0xA0,
    0,
    0xB0,
    0xE0,
    7,
    7,
    0,
    0x540,
    0x2B0,
    0,
    0xB0,
    0x130,
    0,
    7,
    0,
    0x490,
    0xB0,
    0,
    0xB0,
    0xF0,
    1,
    0,
    -0x1000,
    0x470,
    0x20,
    1,
    0xD0,
    0x120,
    7,
    0,
    -0x1000,
    0x260,
    0x230,
    0,
    0x120,
    0x190,
    7,
    7,
    0,
    0x260,
    0x20,
    4,
    0xB0,
    0x120,
    0,
    7,
    0x1000,
    0x1D0,
    0x90,
    0,
    0xB0,
    0xF0,
    0,
    4,
    0x1000,
    0x50,
    0x180,
    0,
    0xB0,
    0x1A0,
    7,
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
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x510,
    0x50,
    0,
    0xB0,
    0xF0,
    7,
    7,
    0,
    0x530,
    0x50,
    0,
    0xC0,
    0x130,
    7,
    7,
    0,
    0x530,
    0x310,
    0,
    0x100,
    0x1E0,
    7,
    7,
    0,
    0x840,
    0x90,
    0,
    0x100,
    0x1D0,
    7,
    7,
    0,
    0x2A0,
    0x160,
    0,
    0xB0,
    0x170,
    7,
    7,
    0,
    0x400,
    0x70,
    0,
    0xC0,
    0x160,
    7,
    7,
    0,
    0xA0,
    0x150,
    0,
    0xB0,
    0x130,
    7,
    7,
    0,
    0x10,
    0x1B0,
    0,
    0xF0,
    0x1F0,
    7,
    7,
    0,
    0x70,
    0xC10,
    0,
    0xB0,
    0x160,
    7,
    7,
    0,
    0x10,
    0x60,
    0,
    0xB0,
    0x140,
    7,
    7,
    0,
    0xC80,
    0x30,
    0,
    0xB0,
    0x130,
    7,
    7,
    0,
    0x60,
    0xD0,
    0,
    0xB0,
    0xE0,
    7,
    7,
    0,
    0x50,
    0x78,
    0,
    0xB8,
    0xE0,
    7,
    7,
    0,
    0x48,
    0x78,
    0,
    0xB0,
    0xF0,
    7,
    7,
    -0x2000,
    0x50,
    0x70,
    0,
    0xB0,
    0xE0,
    7,
    7,
    0,
    0x30,
    0x7C,
    0,
    0xB8,
    0xE0,
    7,
    7,
    0,
};
s16 D_us_801822CC[8] = { 0, 0x16, 0, 4, 8, -4, -0x10, 0 };
u8 D_us_801822E4[0x14] = { 7, 1, 7, 2, 7, 3, 7, 4, 7, 5, 7, 6, 7, 7, 7, 8, 0, 0, 0, 0 };
u8 D_us_801822F8[0x14] = { 4, 1, 4, 2, 4, 3, 4, 4, 4, 5, 4, 6, 4, 7, 4, 8, 0, 0, 0, 0 };
u8 D_us_80182328[8] = { 2, 0xF, 2, 0x10, 0, 0, 0, 0 };
AnimateEntityFrame D_us_8018230C[0xE] = {
    3,
    9,
    3,
    0xA,
    3,
    9,
    3,
    0xA,
    3,
    0xB,
    3,
    0xC,
    3,
    0xB,
    3,
    0xC,
    3,
    0xD,
    3,
    0xE,
    3,
    0xD,
    3,
    0xE,
    0,
    0,
    0,
    0,
};
s16 D_us_801822DC[4] = { 0, 0x16, 2, 0 };
s16 D_us_80182330[0xA] = { 0, 0, 0, 1, 1, 2, 2, 1, 1, 0 };
s16 D_us_80182344[0x10] = { 0, 0x19, 0, 4, 8, -4, -0x10, 0, 0, 0x19, 0xC, 0, 0x206, 0x306, 0x440, 0xFF };
u8 D_us_80182364[8] = { 4, 5, 6, 1, 4, 6, 0xFF, 0 };
u8 D_us_8018236C[8] = { 6, 7, 8, 8, 6, 9, 0xFF, 0 };
u8 D_us_80182374[0x10] = { 0x10, 1, 2, 0xA, 2, 0xB, 0x14, 0xA, 1, 0x12, 1, 0x11, 1, 0x10, 0xFF, 0 };
u8 D_us_80182384[8] = { 1, 0xD, 1, 0xE, 0, 0, 0, 0 };
u8 D_us_8018238C[0x10] = { 8, 1, 0x16, 0xC, 1, 0xE, 1, 0xD, 1, 0xF, 1, 0x11, 1, 0x12, 0xFF, 0 };
u8 D_us_8018239C[0x14] = {
    1,
    0x15,
    1,
    0x16,
    1,
    0x17,
    1,
    0x18,
    1,
    0x19,
    1,
    0x1A,
    1,
    0x1B,
    1,
    0x1C,
    1,
    0,
    0xFF,
    0,
};
u8 D_us_801823B0[0x10] = { 2, 0x1D, 2, 0x1E, 2, 0x1F, 2, 0x20, 2, 0x21, 2, 0x22, 0, 0, 0, 0 };
s8 D_us_801823C0[5][4] = {
    { 0, 0, 0, 0 },
    { 1, 7, 4, 0x12 },
    { 1, 3, 7, 0xE },
    { 4, 0, 4, 0x18 },
    { 0, 5, 5, 0x14 },
};
u8 D_us_801823D4[0x18] = { 0, 1, 2, 2, 1, 1, 1, 1, 1, 1, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 2, 0, 0, 0 };
// end pc_import_data.py
// clang-format on
#endif
