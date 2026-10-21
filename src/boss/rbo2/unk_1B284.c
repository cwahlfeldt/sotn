// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo2.h"
#include "sfx.h"

extern u8* D_us_8018061C[];
extern u8 D_us_8018063C[];
extern u8 D_us_80180644[];
extern u16 D_us_8018064C[];
extern u8 D_us_8018065C[];

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", EntityBreakable);
#else
// RBO2's breakable tables have no names yet
#define g_eBreakableAnimations D_us_8018061C
#define g_eBreakableHitboxes D_us_8018063C
#define g_eBreakableExplosionTypes D_us_80180644
#define g_eBreakableanimSets D_us_8018064C
#define blend_modes D_us_8018065C
#include "../../st/e_breakable.h"
#undef g_eBreakableAnimations
#undef g_eBreakableHitboxes
#undef g_eBreakableExplosionTypes
#undef g_eBreakableanimSets
#undef blend_modes
#endif

#ifdef VERSION_PC
s16 func_us_8019A98C_from_rcen(s16 delta, s16 base, s16 target);
void func_us_8019B430(void);
int func_801CDC80(s16* value, s16 target, s16 step);
void func_us_8019ED80(Entity* self);
void polarPlacePartsWithAngvel(s16* entOffsets);
void func_801CDF1C(s16 entIndices[], unkStr_801CDD80* arg1, s32 arg2);
void func_801CE1E8(s32 step);
void polarPlacePartsList(s16* offsets);
#endif

// a part of Death's second form
typedef struct {
    s16 eArrayOffset;
    s16 eArrayParentOffset;
    s16 length;
    s16 animCurFrame;
    s16 zOffset;
    s16 rotOffset;
} DeathPartInit;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019A98C_from_rcen);
#else
// RCEN's _LimitAngleChange
s16 func_us_8019A98C_from_rcen(s16 delta, s16 base, s16 target) {
    s16 diff;
    s16 ret;

    base &= 0xFFF;
    diff = target - base;
    ret = diff;

    if (diff > ROT(180)) {
        ret = diff - ROT(360);
    }

    if (diff < ROT(-180)) {
        ret = diff + ROT(360);
    }

    if (abs(ret) > delta) {
        if (diff < 0) {
            ret = base - delta;
        } else {
            ret = base + delta;
        }
        return ret;
    }

    return target;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019B430);
#else
// small spirits drifting down while Death flies about
void func_us_8019B430(void) {
    Entity* entity;
    s16 x;
    s16 y;

    if (!(g_Timer & 0xF)) {
        entity = AllocEntity(&g_Entities[160], &g_Entities[168]);
        if (entity != NULL) {
            CreateEntityFromCurrentEntity(E_ID(UNK_17), entity);
            x = rand() & 0xFF;
            y = (rand() & 0x3F) + 0x20;
            entity->posX.i.hi = x;
            entity->posY.i.hi = y;
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_801CDC80);
#else
int func_801CDC80(s16* value, s16 target, s16 step) {
    if (abs(*value - target) < step) {
        *value = target;
        return true;
    }

    if (*value > target) {
        *value -= step;
    }

    if (*value < target) {
        *value += step;
    }

    return false;
}
#endif

extern EInit g_EInitDeath1;
extern s32 D_us_80180B5C;
extern u8 D_us_8018066C[];
extern u8 D_us_8018067C[];
extern u8 D_us_80180694[];
extern u8 D_us_801806A8[];
extern u8 D_us_801806B0[];
#ifdef VERSION_PC
#define D_us_801806B8 (D_us_801806B0 + 8) // see D_us_801806B0
#else
extern u8 D_us_801806B8[];
#endif
extern u8 D_us_801806D0[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019B52C);
#else
// steers towards (tx, ty) at the given speed
#define DEATH_FLY_TO(tx, ty, speed)                                            \
    dx = (tx) - self->posX.i.hi;                                               \
    dy = (ty) - self->posY.i.hi;                                               \
    angle = ratan2(dy, dx);                                                    \
    self->velocityX = (rcos(angle) * (speed)) >> 12;                           \
    self->velocityY = (rsin(angle) * (speed)) >> 12;

// Death, first form. His two orbs (0x18) sit in the next two slots; at
// death he turns into his second form (0x1B) in slot 96
void func_us_8019B52C(Entity* self) {
    Primitive* prim;
    s32 i;
    s32 x;
    s32 y;
    s32 dx;
    s32 dy;
    s32 done;
    s16 angle;
    s16 hp;

    if ((self->flags & FLAG_DEAD) && self->step != 9) {
        SetStep(9);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitDeath1);
        self->hitboxOffY = -5;
        self->hitboxState = 2;
        i = g_api.AllocPrimitives(PRIM_TILE, 3);
        if (i == -1) {
            self->step = 0;
            break;
        }
        prim = &g_PrimBuf[i];
        self->primIndex = i;
        self->ext.rboDeath.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->u0 = prim->v0 = 0;
            prim->drawMode = DRAW_HIDE | DRAW_UNK02;
            prim->priority = self->zPriority;
            prim = prim->next;
        }
        SetStep(1);
        // fallthrough
    case 1:
        AnimateEntity(D_us_8018066C, self);
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        self->hitboxState = 0;
        if (D_us_80180B5C & 2) {
            self->hitboxState = 2;
            SetStep(2);
        }
        break;
    case 2:
        // fly about next to Alucard, losing speed
        if (self->hitFlags & 3) {
            SetStep(8);
            break;
        }
        switch (self->step_s) {
        case 0:
            self->ext.rboDeath.timer = ((Random() & 3) << 5) + 0x20;
            y = PLAYER.posY.i.hi;
            if (PLAYER.posX.i.hi < self->posX.i.hi) {
                x = PLAYER.posX.i.hi - 0x60;
            } else {
                x = PLAYER.posX.i.hi + 0x60;
            }
            y -= Random() & 0x3F;
            // as on PSX: the target is stored relative to him and read back
            // as a position
            prim = self->ext.rboDeath.prim;
            prim->x0 = x - self->posX.i.hi;
            prim->y0 = y - self->posY.i.hi;
            self->ext.rboDeath.angle = rand() & 0xFFF;
            self->ext.rboDeath.speed = 0x1C000;
            self->step_s++;
            // fallthrough
        case 1:
            AnimateEntity(D_us_8018066C, self);
            MoveEntity();
            prim = self->ext.rboDeath.prim;
            angle = func_us_8019A98C_from_rcen(
                0x30, self->ext.rboDeath.angle,
                ratan2(prim->y0 - self->posY.i.hi, prim->x0 - self->posX.i.hi));
            self->velocityX = (rcos(angle) * self->ext.rboDeath.speed) >> 12;
            self->velocityY = (rsin(angle) * self->ext.rboDeath.speed) >> 12;
            self->ext.rboDeath.angle = angle;
            if (self->ext.rboDeath.speed > 0xE000) {
                self->ext.rboDeath.speed -= self->ext.rboDeath.speed >> 4;
            }
            func_us_8019B430();
            if (!--self->ext.rboDeath.timer) {
                self->step_s--;
            }
            hp = g_api.enemyDefs[0x164].hitPoints;
            if (!self->ext.rboDeath.orbsChase) {
                if (self->hitPoints < hp - hp / 4) {
                    SetStep(3);
                }
            } else if (self->hitPoints < hp / 2) {
                SetStep(4);
            }
            break;
        }
        break;
    case 3:
        // summon the orbs
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_8018067C, self)) {
                SetSubStep(1);
            }
            break;
        case 1:
            CreateEntityFromEntity(E_ID(UNK_18), self, self + 1);
            (self + 1)->params = 0;
            (self + 1)->ext.rboDeath.source = self;
            (self + 1)->posX.i.hi -= 0x28;
            (self + 1)->zPriority = self->zPriority + 1;
            (self + 1)->posY.i.hi -= 0x1C;
            CreateEntityFromEntity(E_ID(UNK_18), self, self + 2);
            (self + 2)->params = 1;
            self->ext.rboDeath.timer = 0x80;
            (self + 2)->ext.rboDeath.source = self;
            (self + 2)->posX.i.hi += 0x28;
            (self + 2)->zPriority = self->zPriority + 1;
            self->step_s++;
            (self + 2)->posY.i.hi -= 0x1C;
            // fallthrough
        case 2:
            if (!(self->ext.rboDeath.timer & 0xF)) {
                PlaySfxPositional(0x6E2);
            }
            AnimateEntity(D_us_801806A8, self);
            if (!--self->ext.rboDeath.timer) {
                SetSubStep(3);
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180694, self)) {
                PlaySfxPositional(0x7F2);
                self->ext.rboDeath.orbsChase = 1;
                self->step_s++;
            }
            break;
        case 4:
            if (!AnimateEntity(D_us_801806B0, self)) {
                SetStep(2);
            }
            break;
        }
        break;
    case 4:
        // call the orbs back
        switch (self->step_s) {
        case 0:
            self->ext.rboDeath.orbsReturn = 1;
            self->velocityX = 0;
            self->velocityY = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_8018067C, self)) {
                PlaySfxPositional(0x7EF);
                SetSubStep(2);
            }
            break;
        case 2:
            AnimateEntity(D_us_801806A8, self);
            if ((self + 1)->ext.rboDeath.ready &&
                (self + 2)->ext.rboDeath.ready) {
                self->ext.rboDeath.orbsSkulls = 1;
                SetStep(5);
            }
            break;
        }
        break;
    case 5:
        // bob up and down, drifting to the middle; the orbs cast skulls
        AnimateEntity(D_us_801806A8, self);
        MoveEntity();
        self->ext.rboDeath.orbitAngle += 0x40;
        self->velocityY = (rsin(self->ext.rboDeath.orbitAngle) * 0xC000) >> 12;
        if (self->posY.i.hi < 0x68) {
            self->velocityY += 0x1000;
        } else {
            self->velocityY -= 0x1000;
        }
        if (self->posX.i.hi < 0x80) {
            if (self->velocityX < 0xE000) {
                self->velocityX += 0x800;
            }
        } else if (self->velocityX > -0xE000) {
            self->velocityX -= 0x800;
        }
        if (self->hitPoints < (s16)(g_api.enemyDefs[0x164].hitPoints / 3)) {
            SetStep(6);
        }
        break;
    case 6:
        // the orbs turn into sickles
        switch (self->step_s) {
        case 0:
            D_us_80180B5C |= 4;
            self->step_s++;
            // fallthrough
        case 1:
            AnimateEntity(D_us_801806A8, self);
            if ((self + 1)->ext.rboDeath.ready &&
                (self + 2)->ext.rboDeath.ready) {
                self->ext.rboDeath.unk8C = 1;
                self->ext.rboDeath.timer = 0x40;
                PlaySfxPositional(0x7F1);
                self->step_s++;
            }
            break;
        case 2:
            AnimateEntity(D_us_801806A8, self);
            if (!--self->ext.rboDeath.timer) {
                self->ext.rboDeath.orbitAngle = 0x200;
                SetStep(7);
            }
            break;
        }
        break;
    case 7:
        // circle Alucard, throwing the sickles
        switch (self->step_s) {
        case 0:
            AnimateEntity(D_us_8018066C, self);
            if ((self + 1)->ext.rboDeath.ready &&
                (self + 2)->ext.rboDeath.ready) {
                SetSubStep(1);
            }
            break;
        case 1:
            if (!AnimateEntity(D_us_801806B8, self)) {
                self->ext.rboDeath.throwSickles = 1;
                self->ext.rboDeath.timer = 0x10;
                SetSubStep(2);
            }
            if (!self->poseTimer && self->pose == 4) {
                PlaySfxPositional(0x7F0);
            }
            break;
        case 2:
            self->ext.rboDeath.throwSickles = 0;
            if (!--self->ext.rboDeath.timer) {
                SetSubStep(0);
            }
            break;
        }
        MoveEntity();
        angle = self->ext.rboDeath.orbitAngle;
        x = PLAYER.posX.i.hi + ((rcos(angle) * 0x60) >> 12);
        angle = ratan2(PLAYER.posY.i.hi - ((rsin(angle) * 0x60) >> 12) -
                           self->posY.i.hi,
                       x - self->posX.i.hi);
        self->ext.rboDeath.speed = 0x1C000;
        angle = func_us_8019A98C_from_rcen(0x30, self->ext.rboDeath.angle, angle);
        self->velocityX = (rcos(angle) * self->ext.rboDeath.speed) >> 12;
        self->velocityY = (rsin(angle) * self->ext.rboDeath.speed) >> 12;
        self->ext.rboDeath.angle = angle;
        x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (x > 0x180) {
            self->ext.rboDeath.orbitAngle = 0x600;
        }
        if (x < 0x80) {
            self->ext.rboDeath.orbitAngle = 0x200;
        }
        break;
    case 8:
        // hit: thrown back
        if (!self->step_s) {
            self->ext.rboDeath.timer = 0x40;
            if (GetSideToPlayer() & 1) {
                self->velocityX = FIX(4);
            } else {
                self->velocityX = FIX(-4);
            }
            self->velocityY = FIX(-4);
            if (Random() & 1) {
                PlaySfxPositional(0x7F7);
            } else {
                PlaySfxPositional(0x7F8);
            }
            x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
            if (x > 0x180) {
                self->velocityX = FIX(-7);
            }
            if (x < 0x80) {
                self->velocityX = FIX(7);
            }
            self->step_s++;
        }
        AnimateEntity(D_us_801806D0, self);
        MoveEntity();
        self->velocityX -= self->velocityX >> 5;
        self->velocityY -= self->velocityY >> 5;
        self->velocityY += FIX(1.0 / 16);
        if (!--self->ext.rboDeath.timer) {
            SetStep(2);
        }
        break;
    case 9:
        // death: fly to the middle, open a beam of light, change form
        switch (self->step_s) {
        case 0:
            prim = self->ext.rboDeath.prim;
            self->hitboxState = 0;
            D_us_80180B5C |= 8;
            prim->x0 = 0x90;
            prim->y0 = 0x68;
            PlaySfxPositional(0x7F3);
            self->step_s++;
            // fallthrough
        case 1:
            AnimateEntity(D_us_8018066C, self);
            MoveEntity();
            prim = self->ext.rboDeath.prim;
            dx = prim->x0 - self->posX.i.hi;
            dy = prim->y0 - self->posY.i.hi;
            angle = ratan2(dy, dx);
            self->velocityX = rcos(angle) * 16;
            self->velocityY = rsin(angle) * 16;
            if (SquareRoot0(dx * dx + dy * dy) < 8) {
                self->step_s++;
            }
            break;
        case 2:
            prim = self->ext.rboDeath.prim;
            self->ext.rboDeath.fade = 0x40;
            self->ext.rboDeath.spread = 0;
            self->opacity = 0x80;
            self->drawFlags |= ENTITY_OPACITY;
            // the beam, and its left and right edges fading out
            prim->type = PRIM_G4;
            prim->x0 = prim->x2 = self->posX.i.hi;
            prim->y0 = prim->y1 = 0;
            prim->y2 = prim->y3 = 0x100;
            prim->x1 = prim->x3 = self->posX.i.hi;
            PGREY(prim, 0) = self->ext.rboDeath.fade;
            prim->priority = 0xC0;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim = prim->next;
            prim->type = PRIM_G4;
            prim->x0 = prim->x2 = self->posX.i.hi;
            PGREY(prim, 0) = 0;
            prim->y0 = prim->y1 = 0;
            prim->y2 = prim->y3 = 0x100;
            prim->x1 = prim->x3 = self->posX.i.hi;
            LOW(prim->r2) = LOW(prim->r0);
            PGREY(prim, 1) = self->ext.rboDeath.fade;
            prim->priority = 0xC0;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            LOW(prim->r3) = LOW(prim->r1);
            prim = prim->next;
            prim->type = PRIM_G4;
            prim->x0 = prim->x2 = self->posX.i.hi;
            prim->y0 = prim->y1 = 0;
            prim->y2 = prim->y3 = 0x100;
            prim->x1 = prim->x3 = self->posX.i.hi;
            PGREY(prim, 0) = self->ext.rboDeath.fade;
            PGREY(prim, 1) = 0;
            prim->priority = 0xC0;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r1);
            self->ext.rboDeath.timer = 0x20;
            PlaySfxPositional(0x636);
            self->step_s++;
            // fallthrough
        case 3:
            // the edges move apart
            prim = self->ext.rboDeath.prim->next;
            prim->x0 = prim->x2 = prim->x2 - 2;
            prim = prim->next;
            prim->x1 = prim->x3 = prim->x3 + 2;
            if (self->posX.i.hi == prim->x3 - 0x30) {
                PlaySfxPositional(0x7F4);
                self->step_s++;
            }
            break;
        case 4:
            // the beam widens to the whole screen
            done = 0;
            self->ext.rboDeath.spread += 4;
            x = self->posX.i.hi - self->ext.rboDeath.spread;
            if (x < 0) {
                done = 1;
                x = 0;
            }
            prim = self->ext.rboDeath.prim;
            prim->x0 = prim->x2 = x;
            prim = prim->next;
            prim->x0 = prim->x2 = x - 0x30;
            prim->x1 = prim->x3 = x;
            x = self->posX.i.hi + self->ext.rboDeath.spread;
            if (x > 0x100) {
                done++;
                x = 0x100;
            }
            prim = self->ext.rboDeath.prim;
            prim->x1 = prim->x3 = x;
            prim = prim->next->next;
            prim->x0 = prim->x2 = x;
            prim->x1 = prim->x3 = x + 0x30;
            if (self->opacity) {
                self->opacity -= 8;
            }
            if (self->ext.rboDeath.fade < 0xF0) {
                self->ext.rboDeath.fade += 8;
            }
            prim = self->ext.rboDeath.prim;
            PGREY(prim, 0) = self->ext.rboDeath.fade;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim = prim->next;
            PGREY(prim, 0) = 0;
            LOW(prim->r2) = LOW(prim->r0);
            PGREY(prim, 1) = self->ext.rboDeath.fade;
            LOW(prim->r3) = LOW(prim->r1);
            prim = prim->next;
            PGREY(prim, 0) = self->ext.rboDeath.fade;
            PGREY(prim, 1) = 0;
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r1);
            if (done == 2) {
                self->step_s++;
            }
            break;
        case 5:
            prim = self->ext.rboDeath.prim;
            while (prim != NULL) {
                prim->drawMode = DRAW_HIDE;
                prim = prim->next;
            }
            CreateEntityFromEntity(E_ID(UNK_1B), self, &g_Entities[96]);
            self->step_s++;
            break;
        case 6:
            self->animCurFrame = 0;
            break;
        }
        break;
    case 0xFF:
#include "../../st/pad2_anim_debug.h"
    }
    y = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
    x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
    if (self->velocityX < 0) {
        if (x < 0x20) {
            self->posX.i.hi = 0x20 - g_Tilemap.scrollX.i.hi;
        }
    } else if (x > 0x1E0) {
        self->posX.i.hi = 0x1E0 - g_Tilemap.scrollX.i.hi;
    }
    if (self->velocityY < 0) {
        if (y < 0x50) {
            self->posY.i.hi = 0x50 - g_Tilemap.scrollY.i.hi;
        }
    } else if (y > 0xE0) {
        self->posY.i.hi = 0xE0 - g_Tilemap.scrollY.i.hi;
    }
}
#endif

extern EInit D_us_801804A0;
extern u8 D_us_801806E0[];
extern u8 D_us_801806F8[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019C718);
#else
// a spirit: appears, then flies at Alucard spinning
void func_us_8019C718(Entity* self) {
    s16 x;
    s16 y;
    s32 tx;
    s16 angle;

    if (D_us_80180B5C & 8) {
        self->flags |= FLAG_DEAD;
    }
    if ((self->flags & FLAG_DEAD) && self->step != 3) {
        PlaySfxPositional(0x60F);
        self->hitboxState = 0;
        SetStep(3);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804A0);
        self->facingLeft = Random() & 1;
        self->hitboxState = 0;
        // fallthrough
    case 1:
        if (!AnimateEntity(D_us_801806E0, self)) {
            self->hitboxState = 3;
            self->animCurFrame = 0x4F;
            self->drawFlags = ENTITY_ROTATE;
            SetStep(2);
        }
        return;
    case 2:
        if (!self->step_s) {
            x = PLAYER.posX.i.hi;
            y = PLAYER.posY.i.hi;
            tx = x - 0x20 + (Random() & 0x3F);
            angle = ratan2(y - 0x20 + (Random() & 0x3F) - self->posY.i.hi,
                           tx - self->posX.i.hi);
            self->velocityX = (rcos(angle) << 16) >> 12;
            self->velocityY = (rsin(angle) << 16) >> 12;
            PlaySfxPositional(0x689);
            self->step_s++;
        }
        MoveEntity();
        self->rotate += 0xC0;
        return;
    case 3:
        if (!AnimateEntity(D_us_801806F8, self)) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern EInit D_us_801804AC;
extern u8 D_us_80180708[];
extern u8 D_us_80180718[];
extern u8 D_us_8018072C[];
extern u8 D_us_80180744[];
extern u8 D_us_8018075C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019C924);
#else
// one of Death's orbs (params 0 on his left, 1 on his right); in his
// last phase it turns into a sickle he throws
void func_us_8019C924(Entity* self) {
    Entity* entity;
    Entity* death;
    s32 x;
    s32 y;
    s32 dx;
    s32 dy;
    s32 dist;
    s32 anim;
    s16 angle;

    FntPrint("ball_step %x\n", self->step);
    if (D_us_80180B5C & 8) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, entity);
            entity->params = 3;
        }
        DestroyEntity(self);
        return;
    }
    death = self->ext.rboDeath.source;
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804AC);
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleX = self->scaleY = 0x80;
        self->hitboxState = 0;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        PlaySfxPositional(0x6DE);
        // fallthrough
    case 1:
        if (!AnimateEntity(D_us_80180708, self)) {
            self->step++;
        }
        // fallthrough
    case 2:
        self->scaleX += 4;
        if (self->scaleX > 0x100) {
            self->hitboxState = 1;
            self->drawFlags = 0;
            SetStep(3);
        }
        self->scaleY = self->scaleX;
        return;
    case 3:
        AnimateEntity(D_us_80180718, self);
        if (death->ext.rboDeath.orbsChase) {
            self->ext.rboDeath.angle = 0x400;
            SetStep(4);
        }
        return;
    case 4:
        // chase Alucard
        AnimateEntity(D_us_80180718, self);
        MoveEntity();
        x = PLAYER.posX.i.hi;
        y = PLAYER.posY.i.hi;
        x = x - 0x20 + (Random() & 0x3F);
        angle = LimitAngleChange(
            0x40, self->ext.rboDeath.angle,
            ratan2(y - 0x20 + (Random() & 0x3F) - self->posY.i.hi,
                   x - self->posX.i.hi));
        self->velocityX = (rcos(angle) * 0x7000) >> 12;
        self->velocityY = (rsin(angle) * 0x7000) >> 12;
        self->ext.rboDeath.angle = angle;
        if (death->ext.rboDeath.orbsReturn) {
            SetStep(5);
        }
        break;
    case 5:
        // back to Death's side
        AnimateEntity(D_us_80180718, self);
        MoveEntity();
        if (!self->params) {
            x = death->posX.i.hi - 0x20;
        } else {
            x = death->posX.i.hi + 0x20;
        }
        DEATH_FLY_TO(x, death->posY.i.hi - 0x1C, 0x1C000);
        if (SquareRoot0(dx * dx + dy * dy) < 4) {
            self->ext.rboDeath.ready = 1;
            self->step++;
        }
        break;
    case 6:
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            AnimateEntity(D_us_80180718, self);
            if (death->ext.rboDeath.orbsSkulls) {
                self->ext.rboDeath.ready = 0;
                SetSubStep(1);
            }
            break;
        case 1:
            if (!AnimateEntity(D_us_8018072C, self)) {
                SetStep(7);
            }
            break;
        }
        break;
    case 7:
        // cast skulls that fly at Alucard and on to the other orb
        switch (self->step_s) {
        case 0:
            entity = AllocEntity(&g_Entities[176], &g_Entities[184]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_19), self, entity);
                entity->ext.rboDeath.source = self;
                if (!self->params) {
                    entity->ext.rboDeath.target = self + 1;
                } else {
                    entity->ext.rboDeath.target = self - 1;
                }
                entity->facingLeft = self->params;
                entity->zPriority = self->zPriority + 1;
                self->ext.rboDeath.unk8C = 0;
                PlaySfxPositional(0x6E6);
                self->step_s++;
            }
            break;
        case 1:
            self->ext.rboDeath.timer = 0x60;
            self->step_s++;
            break;
        case 2:
            if (!--self->ext.rboDeath.timer) {
                self->step_s = 0;
            }
            break;
        }
        if (g_Timer & 1) {
            self->animCurFrame = 0x39;
        } else {
            self->animCurFrame = 0;
        }
        MoveEntity();
        if (!self->params) {
            x = death->posX.i.hi - 0x50;
        } else {
            x = death->posX.i.hi + 0x50;
        }
        DEATH_FLY_TO(x, death->posY.i.hi - 0x1C, 0xE000);
        if (SquareRoot0(dx * dx + dy * dy) < 3) {
            self->velocityX = 0;
            self->velocityY = 0;
        }
        if (D_us_80180B5C & 4) {
            SetStep(8);
        }
        break;
    case 8:
        // become a sickle
        switch (self->step_s) {
        case 0:
            anim = AnimateEntity(D_us_80180744, self);
            MoveEntity();
            if (!self->params) {
                x = death->posX.i.hi - 0x20;
            } else {
                x = death->posX.i.hi + 0x20;
            }
            DEATH_FLY_TO(x, death->posY.i.hi - 0x1C, 0x1C000);
            if (SquareRoot0(dx * dx + dy * dy) < 4 && !anim) {
                self->ext.rboDeath.ready = 1;
                SetSubStep(1);
            }
            break;
        case 1:
            AnimateEntity(D_us_80180718, self);
            if (death->ext.rboDeath.unk8C) {
                self->facingLeft = self->params;
                SetSubStep(2);
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_8018075C, self)) {
                self->attackElement = g_api.enemyDefs[0x168].attackElement;
                self->attack = g_api.enemyDefs[0x168].attack;
                self->hitboxWidth = g_api.enemyDefs[0x168].hitboxWidth;
                self->hitboxState = 1;
                self->blendMode = 0;
                self->drawFlags = ENTITY_ROTATE;
                self->hitboxHeight = g_api.enemyDefs[0x168].hitboxHeight;
                SetStep(9);
            }
            break;
        }
        break;
    case 9:
        // the sickle: wait at Death's side, fly at Alucard spinning, return
        switch (self->step_s) {
        case 0:
            self->ext.rboDeath.ready = 1;
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            func_801CDC80(&self->rotate, 0, 0x40);
            if (!self->params) {
                x = death->posX.i.hi - 0x20;
            } else {
                x = death->posX.i.hi + 0x20;
            }
            DEATH_FLY_TO(x, death->posY.i.hi - 0x10, 0xE000);
            if (SquareRoot0(dx * dx + dy * dy) < 3) {
                self->velocityX = 0;
                self->velocityY = 0;
            }
            if (death->ext.rboDeath.throwSickles) {
                self->ext.rboDeath.ready = 0;
                self->step_s++;
            }
            break;
        case 2:
            self->ext.rboDeath.angle =
                ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                       PLAYER.posX.i.hi - self->posX.i.hi);
            self->ext.rboDeath.speed = FIX(6);
            self->step_s++;
            // fallthrough
        case 3:
            MoveEntity();
            self->rotate -= 0x100;
            if (!(self->rotate & 0xF00)) {
                PlaySfxPositional(0x7FA);
            }
            angle = self->ext.rboDeath.angle;
            self->velocityX = (rcos(angle) * self->ext.rboDeath.speed) >> 12;
            self->velocityY = (rsin(angle) * self->ext.rboDeath.speed) >> 12;
            self->ext.rboDeath.speed -= self->ext.rboDeath.speed >> 4;
            if (self->ext.rboDeath.speed < 0x4000) {
                self->ext.rboDeath.timer = 0x20;
                self->step_s++;
            }
            if (!(g_Timer & 3)) {
                entity = AllocEntity(&g_Entities[208], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_ID(UNK_1A), self, entity);
                    entity->params = self->animCurFrame;
                    entity->facingLeft = self->facingLeft;
                    entity->rotate = self->rotate;
                }
            }
            break;
        case 4:
            x = self->rotate - 0x200;
            self->rotate = x & 0xFFF;
            if (!(x & 0xF00)) {
                PlaySfxPositional(0x7FA);
            }
            if (!--self->ext.rboDeath.timer) {
                self->step_s++;
            }
            break;
        case 5:
            MoveEntity();
            func_801CDC80(&self->rotate, 0, 0x40);
            if (!self->params) {
                x = death->posX.i.hi - 0x20;
            } else {
                x = death->posX.i.hi + 0x20;
            }
            dx = x - self->posX.i.hi;
            dy = death->posY.i.hi - 0x10 - self->posY.i.hi;
            angle = ratan2(dy, dx);
            dist = SquareRoot0(dx * dx + dy * dy);
            if (dist > 0x20) {
                angle = LimitAngleChange(0x60, self->ext.rboDeath.angle, angle);
            }
            self->velocityX = (rcos(angle) * FIX(3)) >> 12;
            self->velocityY = (rsin(angle) * FIX(3)) >> 12;
            self->ext.rboDeath.angle = angle;
            if (dist < 8) {
                self->step_s = 0;
            }
            break;
        }
        break;
    }
}
#endif

extern EInit D_us_801804B8;
extern u8 D_us_80180770[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019D4CC);
#else
// a skull from an orb: grows, flies at Alucard, then to the other orb
void func_us_8019D4CC(Entity* self) {
    Entity* entity;
    Entity* target;
    s32 dx;
    s32 dy;
    s32 dist;
    s16 angle;

    FntPrint("skull_step %x\n", self->step);
    if (D_us_80180B5C & 8) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, entity);
            entity->params = 1;
        }
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804B8);
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        self->scaleX = self->scaleY = 0;
        // fallthrough
    case 1:
        self->posX.i.hi = self->ext.rboDeath.source->posX.i.hi;
        self->posY.i.hi = self->ext.rboDeath.source->posY.i.hi;
        AnimateEntity(D_us_80180770, self);
        self->scaleX += 0xC;
        if (self->scaleX > 0x100) {
            self->hitboxState = 1;
            self->ext.rboDeath.timer = 0x80;
            self->drawFlags = 0;
            self->ext.rboDeath.angle = 0x400;
            self->step++;
        }
        self->scaleY = self->scaleX;
        if (D_us_80180B5C & 4) {
            DestroyEntity(self);
        }
        return;
    case 2:
        AnimateEntity(D_us_80180770, self);
        MoveEntity();
        angle = LimitAngleChange(0x20, self->ext.rboDeath.angle,
                                 ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                                        PLAYER.posX.i.hi - self->posX.i.hi));
        self->velocityX = (rcos(angle) * FIX(3)) >> 12;
        self->velocityY = (rsin(angle) * FIX(3)) >> 12;
        self->ext.rboDeath.angle = angle;
        self->ext.rboDeath.timer--;
        dx = PLAYER.posX.i.hi - self->posX.i.hi;
        if (self->facingLeft) {
            dx = -dx;
        }
        if (!self->ext.rboDeath.timer || dx < 0 || (self->hitFlags & 0x80)) {
            self->step++;
        }
        break;
    case 3:
        AnimateEntity(D_us_80180770, self);
        MoveEntity();
        target = self->ext.rboDeath.target;
        dx = target->posX.i.hi - self->posX.i.hi;
        dy = target->posY.i.hi - self->posY.i.hi;
        angle = ratan2(dy, dx);
        dist = SquareRoot0(dx * dx + dy * dy);
        if (dist > 0x20) {
            angle = LimitAngleChange(0x60, self->ext.rboDeath.angle, angle);
        }
        self->velocityX = (rcos(angle) * FIX(3)) >> 12;
        self->velocityY = (rsin(angle) * FIX(3)) >> 12;
        self->ext.rboDeath.angle = angle;
        if (dist < 4) {
            self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
            self->scaleX = self->scaleY = 0x100;
            self->hitboxState = 0;
            self->step++;
        }
        break;
    case 4:
        AnimateEntity(D_us_80180770, self);
        target = self->ext.rboDeath.target;
        self->posX.i.hi = target->posX.i.hi;
        self->scaleX -= 0x10;
        self->scaleY = self->scaleX;
        self->posY.i.hi = target->posY.i.hi;
        if (!self->scaleX) {
            target->ext.rboDeath.unk8C = 1;
            DestroyEntity(self);
            return;
        }
        if (D_us_80180B5C & 4) {
            DestroyEntity(self);
        }
        return;
    case 5:
        self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
        MoveEntity();
        return;
    default:
        return;
    }
    // trail
    if (!(g_Timer & 3)) {
        entity = AllocEntity(&g_Entities[208], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_ID(UNK_1A), self, entity);
            entity->params = self->animCurFrame;
            entity->facingLeft = self->facingLeft;
        }
    }
    if (D_us_80180B5C & 4) {
        SetStep(5);
    }
}
#endif

extern EInit D_us_801804C4;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019D950);
#else
// a fading copy (trail) of an orb, sickle or skull
void func_us_8019D950(Entity* self) {
    if (D_us_80180B5C & 8) {
        DestroyEntity(self);
        return;
    }
    if (!self->step) {
        InitializeEntity(D_us_801804C4);
        self->drawFlags = ENTITY_OPACITY;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        self->opacity = 0x60;
        self->animCurFrame = self->params;
        if (self->rotate) {
            self->drawFlags |= ENTITY_ROTATE;
        }
    }
    self->opacity -= 8;
    if (!self->opacity) {
        DestroyEntity(self);
    }
}
#endif

extern EInit g_EInitDeath2;
extern DeathPartInit D_us_8018078C[];
extern s16 D_us_8018081C[];
extern s16 D_us_80180834[];
extern s16 D_us_80180844[];
extern unkStr_801CDD80 D_us_801808A4[];
extern unkStr_801CDD80 D_us_801808E4[];
extern unkStr_801CDD80 D_us_8018095C[];
extern unkStr_801CDD80 D_us_801809E4[];
extern unkStr_801CDD80 D_us_80180A34[];
extern unkStr_801CDD80 D_us_80180A84[];
extern s16 D_us_80180A9C[];
extern unkStr_801CDD80 D_us_80180AB8[];
extern s32 D_us_80180784;
extern s32 D_us_80180788;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019DA04);
#else
// Death, second form: a skeleton of parts (0x1C) that circles Alucard,
// swings his sickle and throws it
void func_us_8019DA04(Entity* self) {
    Entity* entity;
    Entity* prev;
    DeathPartInit* parts;
    s16* p;
    s32 i;
    s32 x;
    s32 y;
    s32 side;
    s16 angle;

    if ((self->flags & FLAG_DEAD) && self->step != 6) {
        func_801CE1E8(6);
    }
    switch (self->step) {
    case 0:
        prev = self;
        InitializeEntity(g_EInitDeath2);
        self->animCurFrame = 2;
        self->hitboxHeight = 0xC;
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        for (parts = D_us_8018078C, i = 0; parts->eArrayOffset; parts++) {
            entity = self + parts->eArrayOffset;
            CreateEntityFromCurrentEntity(E_ID(UNK_1C), entity);
            entity->ext.rboDeath2.length = parts->length;
            entity->ext.rboDeath2.parent = self + parts->eArrayParentOffset;
            entity->params = i;
            entity->ext.rboDeath2.rotOffset = parts->rotOffset;
            i++;
            entity->nextPart = prev;
            prev = entity;
            entity->parent = self;
            entity->zPriority = self->zPriority + parts->zOffset;
        }
        self->parent = NULL;
        self->nextPart = prev;
        polarPlacePartsList(D_us_8018081C);
        self->ext.rboDeath2.orbitAngle = 0x200;
        func_801CE1E8(1);
        // fallthrough
    case 1:
        func_801CDF1C(D_us_80180844, D_us_801808A4, 0);
        polarPlacePartsWithAngvel(D_us_80180844);
        if (!self->ext.rboDeath2.unkB4[0] && !self->ext.rboDeath2.unkB0[0]) {
            func_801CE1E8(2);
        }
        break;
    case 2:
        // circle Alucard
        if (!self->step_s) {
            self->ext.rboDeath2.timer = ((Random() & 3) << 5) + 0x40;
            self->step_s++;
        }
        MoveEntity();
        side = (GetSideToPlayer() & 1) ^ 1;
        if (self->facingLeft != side) {
            if (self->facingLeft) {
                self->posX.i.hi += 0xC;
            } else {
                self->posX.i.hi -= 0xC;
            }
            self->facingLeft = side;
        }
        angle = self->ext.rboDeath2.orbitAngle;
        x = PLAYER.posX.i.hi + ((rcos(angle) * 0x60) >> 12);
        angle = func_us_8019A98C_from_rcen(
            0x30, self->ext.rboDeath2.angle,
            ratan2(PLAYER.posY.i.hi - ((rsin(angle) * 0x60) >> 12) -
                       self->posY.i.hi,
                   x - self->posX.i.hi));
        self->velocityX = (rcos(angle) << 16) >> 12;
        self->velocityY = (rsin(angle) << 16) >> 12;
        self->ext.rboDeath2.angle = angle;
        x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (x > 0x180) {
            self->ext.rboDeath2.orbitAngle = 0x600;
        }
        if (x < 0x80) {
            self->ext.rboDeath2.orbitAngle = 0x200;
        }
        func_801CDF1C(D_us_80180834, D_us_801808E4, 0);
        polarPlacePartsWithAngvel(D_us_80180834);
        func_801CDF1C(D_us_80180A9C, D_us_80180AB8, 1);
        polarPlacePartsWithAngvel(D_us_80180A9C);
        polarPlacePartsList(D_us_8018081C);
        if (!self->ext.rboDeath2.timer) {
            side = (GetSideToPlayer() & 1) ^ 1;
            if (side == self->facingLeft && GetDistanceToPlayerX() > 0x50) {
                func_801CE1E8(4);
            } else {
                func_801CE1E8(3);
            }
        } else {
            self->ext.rboDeath2.timer--;
            if (self->hitFlags & 3) {
                func_801CE1E8(5);
            }
        }
        break;
    case 3:
        // swing the sickle
        if (!self->step_s) {
            self->velocityX = 0;
            self->velocityY = 0;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY -= self->velocityY / 16;
        func_801CDF1C(D_us_80180834, D_us_8018095C, 0);
        polarPlacePartsWithAngvel(D_us_80180834);
        func_801CDF1C(D_us_80180A9C, D_us_80180AB8, 1);
        polarPlacePartsWithAngvel(D_us_80180A9C);
        polarPlacePartsList(D_us_8018081C);
        if ((u16)(self->ext.rboDeath2.unkB0[0] - 2) < 3) {
            D_us_80180788 = 1;
        } else {
            D_us_80180788 = 0;
        }
        if (!self->ext.rboDeath2.unkB4[0]) {
            if (self->ext.rboDeath2.unkB0[0] == 2) {
                self->velocityY = FIX(4);
                PlaySfxPositional(0x66C);
            }
            if (!self->ext.rboDeath2.unkB4[0] &&
                !self->ext.rboDeath2.unkB0[0]) {
                func_801CE1E8(2);
            }
        }
        break;
    case 4:
        // throw the sickle (the hand, part params 3, does it)
        switch (self->step_s) {
        case 0:
            func_801CDF1C(D_us_80180834, D_us_801809E4, 0);
            polarPlacePartsWithAngvel(D_us_80180834);
            if (!self->ext.rboDeath2.unkB4[0]) {
                if (!self->ext.rboDeath2.unkB0[0]) {
                    self->step_s++;
                }
                if (!self->ext.rboDeath2.unkB4[0] &&
                    self->ext.rboDeath2.unkB0[0] == 3) {
                    D_us_80180784 = 1;
                }
            }
            break;
        case 1:
            if (!D_us_80180784) {
                func_801CE1E8(2);
            }
            break;
        }
        func_801CDF1C(D_us_80180A9C, D_us_80180AB8, 1);
        polarPlacePartsWithAngvel(D_us_80180A9C);
        polarPlacePartsList(D_us_8018081C);
        break;
    case 5:
        // hit
        if (!self->step_s) {
            if (GetSideToPlayer() & 1) {
                self->velocityX = FIX(2);
            } else {
                self->velocityX = FIX(-2);
            }
            self->velocityY = 0;
            self->step_s++;
        }
        MoveEntity();
        self->velocityX -= self->velocityX >> 4;
        func_801CDF1C(D_us_80180834, D_us_80180A34, 0);
        polarPlacePartsWithAngvel(D_us_80180834);
        polarPlacePartsList(D_us_8018081C);
        if (!self->ext.rboDeath2.unkB4[0] && !self->ext.rboDeath2.unkB0[0]) {
            func_801CE1E8(2);
            side = (GetSideToPlayer() & 1) ^ 1;
            if (side == self->facingLeft && GetDistanceToPlayerX() > 0x50) {
                func_801CE1E8(4);
            } else {
                func_801CE1E8(3);
            }
        }
        break;
    case 6:
        // death: recoil, fall apart, burn away
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            D_us_80180B5C |= 0x10;
            if (GetSideToPlayer() & 1) {
                self->velocityX = FIX(2);
            } else {
                self->velocityX = FIX(-2);
            }
            self->velocityY = 0;
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityX -= self->velocityX >> 4;
            func_801CDF1C(D_us_80180834, D_us_80180A34, 0);
            polarPlacePartsWithAngvel(D_us_80180834);
            polarPlacePartsList(D_us_8018081C);
            if (!self->ext.rboDeath2.unkB4[0] &&
                !self->ext.rboDeath2.unkB0[0]) {
                self->step_s++;
            }
            break;
        case 2:
            func_801CDF1C(D_us_80180844, D_us_80180A84, 0);
            polarPlacePartsWithAngvel(D_us_80180844);
            if (!self->ext.rboDeath2.unkB4[0] &&
                !self->ext.rboDeath2.unkB0[0]) {
                self->ext.rboDeath2.timer = 0x40;
                self->velocityX = 0;
                D_us_80180B5C |= 0x20;
                self->step_s++;
            }
            break;
        case 3:
            if (!--self->ext.rboDeath2.timer) {
                self->ext.rboDeath2.timer = 0x80;
                self->step_s++;
                g_api.PlaySfx(0x7C5);
            }
            break;
        case 4:
            entity = AllocEntity(
                &g_Entities[STAGE_ENTITY_START], &g_Entities[TOTAL_ENTITY_COUNT]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1F), self, entity);
                entity->params = 1;
                entity->posY.i.hi = 0xF0;
                entity->posX.i.hi = entity->posX.i.hi - 0x10 + (rand() & 0x1F);
                entity->zPriority =
                    self->zPriority - (self->ext.rboDeath2.timer >> 4) + 8;
            }
            if (!--self->ext.rboDeath2.timer) {
                self->animCurFrame = 0;
                self->hitboxState = 0;
                D_us_80180B5C |= 0x40;
                self->step_s++;
            }
            break;
        }
        break;
    case 0xFF:
#include "../../st/pad2_anim_debug.h"
    }
    for (p = D_us_8018081C; *p; p++) {
        entity = self + *p;
        entity->ext.rboDeath2.unkA8 = 0;
        entity->facingLeft = self->facingLeft;
    }
    y = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
    x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
    if (self->velocityX < 0) {
        if (x < 0x20) {
            self->posX.i.hi = 0x20 - g_Tilemap.scrollX.i.hi;
        }
    } else if (x > 0x1E0) {
        self->posX.i.hi = 0x1E0 - g_Tilemap.scrollX.i.hi;
    }
    if (self->velocityY < 0) {
        if (y < 0x30) {
            self->posY.i.hi = 0x30 - g_Tilemap.scrollY.i.hi;
        }
    } else if (y > 0xB0) {
        self->posY.i.hi = 0xB0 - g_Tilemap.scrollY.i.hi;
    }
}
#endif

extern EInit D_us_801804E8;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019E558);
#else
// a part of the second form; params 3 is the hand holding the sickle,
// params 6 the sickle's blade
void func_us_8019E558(Entity* self) {
    Entity* entity;
    s16 angle;

    switch (self->step) {
    case 0:
        if (self->params == 3 || self->params == 6) {
            InitializeEntity(D_us_801804E8);
        } else {
            InitializeEntity(g_EInitDeath2);
        }
        self->drawFlags |= ENTITY_ROTATE;
        self->animCurFrame = D_us_8018078C[self->params].animCurFrame;
        // fallthrough
    case 1:
        self->rotate = self->ext.rboDeath2.rotate;
        if (D_us_80180B5C & 0x10) {
            self->step = 2;
        }
        break;
    case 2:
        self->rotate = self->ext.rboDeath2.rotate;
        if (!(Random() & 0x3F)) {
            PlaySfxPositional(0x654);
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1F), self, entity);
                entity->params = 0;
                entity->zPriority = self->zPriority + 1;
            }
        }
        if (D_us_80180B5C & 0x40) {
            self->animCurFrame = 0;
            self->step = 3;
        }
        break;
    case 3:
        self->animCurFrame = 0;
        self->hitboxState = 0;
        return;
    }
    switch (self->params) {
    default:
        self->hitboxState = 0;
        return;
    case 3:
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 0xA;
            if (D_us_80180784) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_ID(UNK_1D), self, entity);
                    entity->ext.rboDeath.source = self;
                    entity->facingLeft = self->facingLeft;
                    self->ext.rboDeath2.returned = 0;
                    self->step_s++;
                } else {
                    D_us_80180784 = 0;
                }
            }
            break;
        case 1:
            self->animCurFrame = 9;
            if (self->ext.rboDeath2.returned) {
                D_us_80180784 = 0;
                self->step_s = 0;
            }
            break;
        }
        if ((D_us_80180B5C & 0x20) && self->animCurFrame == 0xA) {
            // dying: drop the sickle
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1D), self, entity);
                entity->facingLeft = self->facingLeft;
                entity->rotate = self->rotate;
                entity->zPriority = self->zPriority;
                self->animCurFrame = 9;
                self->step_s = 8;
            }
        }
        // fallthrough
    case 6:
        angle = self->rotate;
        self->hitboxOffX = -(rsin(angle) * 0xC) >> 12;
        self->hitboxOffY = (u32)(rcos(angle) * 3) >> 10;
        if (D_us_80180788) {
            self->hitboxState = 1;
        } else {
            self->hitboxState = 0;
        }
        if ((D_us_80180B5C & 0x20) && self->animCurFrame == 0xE) {
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1D), self, entity);
                entity->facingLeft = self->facingLeft;
                entity->rotate = self->rotate;
                entity->zPriority = self->zPriority;
                self->animCurFrame = 0xD;
            }
        }
        return;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019E920);
#else
// the thrown sickle: flies at Alucard, slows down, returns to the hand
void func_us_8019E920(Entity* self) {
    Entity* entity;
    Entity* hand;
    s32 dx;
    s32 dy;
    s32 dist;
    s16 angle;

    if ((D_us_80180B5C & 0x10) && self->step != 3) {
        self->hitboxState = 0;
        if (self->step) {
            SetStep(3);
        }
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804E8);
        self->animCurFrame = 0xF;
        self->drawFlags = ENTITY_ROTATE;
        if (self->facingLeft) {
            self->ext.rboDeath.angle = 0;
        } else {
            self->ext.rboDeath.angle = 0x800;
        }
        self->ext.rboDeath.speed = FIX(8);
        // fallthrough
    case 1:
        MoveEntity();
        self->rotate -= 0x100;
        if (!(g_Timer & 7)) {
            PlaySfxPositional(0x66E);
        }
        // as on PSX, the turned angle is not kept
        angle = LimitAngleChange(0x10, self->ext.rboDeath.angle,
                                 ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                                        PLAYER.posX.i.hi - self->posX.i.hi));
        self->velocityX = (rcos(angle) * self->ext.rboDeath.speed) >> 12;
        self->velocityY = (rsin(angle) * self->ext.rboDeath.speed) >> 12;
        if (!(g_Timer & 3)) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1E), self, entity);
                entity->params = self->animCurFrame;
                entity->facingLeft = self->facingLeft;
                entity->rotate = self->rotate;
            }
        }
        self->ext.rboDeath.speed -= self->ext.rboDeath.speed >> 4;
        if (self->ext.rboDeath.speed < 0x4000) {
            self->ext.rboDeath.timer = 0x20;
            self->step++;
        }
        return;
    case 2:
        MoveEntity();
        self->rotate -= 0x100;
        if (!(g_Timer & 7)) {
            PlaySfxPositional(0x66E);
        }
        hand = self->ext.rboDeath.source;
        dx = hand->posX.i.hi - self->posX.i.hi;
        dy = hand->posY.i.hi - self->posY.i.hi;
        angle = ratan2(dy, dx);
        dist = SquareRoot0(dx * dx + dy * dy);
        angle = LimitAngleChange(
            dist > 0x20 ? 0x60 : 0x30, self->ext.rboDeath.angle, angle);
        self->velocityX = (rcos(angle) << 18) >> 12;
        self->velocityY = (rsin(angle) << 18) >> 12;
        self->ext.rboDeath.angle = angle;
        if (dist < 8) {
            PlaySfxPositional(0x676);
            hand->ext.rboDeath2.returned = 1;
            DestroyEntity(self);
        }
        break;
    case 3:
        // Death died: fall
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            self->velocityX = 0;
            self->velocityY = 0;
            self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY += FIX(0.25);
            self->rotate -= 0x80;
            break;
        }
        break;
    }
}
#endif

extern EInit D_us_801804DC;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019ECCC);
#else
// a fading copy (trail) of the thrown sickle
void func_us_8019ECCC(Entity* self) {
    if (D_us_80180B5C & 0x10) {
        DestroyEntity(self);
        return;
    }
    if (!self->step) {
        InitializeEntity(D_us_801804DC);
        self->drawFlags = ENTITY_OPACITY;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        self->opacity = 0x60;
        self->animCurFrame = self->params;
        if (self->rotate) {
            self->drawFlags |= ENTITY_ROTATE;
        }
    }
    self->opacity -= 8;
    if (!self->opacity) {
        DestroyEntity(self);
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019ED80);
#else
// RBO1's polarPlacePart: each part's rotOffset is added to its angle
void func_us_8019ED80(Entity* self) {
    s16 angle;
    Entity* src;

    angle = self->ext.rboDeath2.rotate + self->ext.rboDeath2.rotOffset;
    if (g_CurrentEntity->facingLeft) {
        angle = -angle;
    }
    src = self->ext.rboDeath2.parent;
    self->posX.val = src->posX.val;
    self->posY.val = src->posY.val;
    self->posX.val -= self->ext.rboDeath2.length * rsin(angle) * 16;
    self->ext.rboDeath2.unkA8 |= 1;
    self->posY.val += self->ext.rboDeath2.length * rcos(angle) * 16;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", polarPlacePartsWithAngvel);
#else
// the giant brothers' helpers, as in RBO1
void polarPlacePartsWithAngvel(s16* entOffsets) {
    Entity* ent;

    while (*entOffsets) {
        if (*entOffsets != 0xFF) {
            ent = g_CurrentEntity + *entOffsets;
            ent->ext.GH_Props.rotate += ent->ext.GH_Props.rotVel;
            func_us_8019ED80(ent);
        }
        entOffsets++;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_801CDD00);
#else
void func_801CDD00(Entity* entity, s16 arg1, s16 arg2) {
    s16 temp_t0 = arg1 - entity->ext.GH_Props.rotate;

    if (temp_t0 > 0x800) {
        temp_t0 = temp_t0 - 0x1000;
    }

    if (temp_t0 < -0x800) {
        temp_t0 = temp_t0 + 0x1000;
    }

    temp_t0 = temp_t0 / arg2;
    entity->ext.GH_Props.rotVel = temp_t0;
    entity->ext.GH_Props.unkA4 = arg1;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_801CDD80);
#else
void func_801CDD80(s16* entOffsets, unkStr_801CDD80* arg1) {
    Entity* var_s1;
    s16* ptr = arg1->unk4;

    while (*entOffsets) {
        if (*entOffsets != 0xFF) {
            var_s1 = g_CurrentEntity + *entOffsets;
            func_801CDD00(var_s1, *ptr, arg1->unk0);
        }
        ptr++;
        entOffsets++;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_801CDF1C);
#else
void func_801CDF1C(s16 entIndices[], unkStr_801CDD80* arg1, s32 arg2) {

    arg1 += (u16)g_CurrentEntity->ext.GH_Props.unkB0[arg2];

    if (!g_CurrentEntity->ext.GH_Props.unkB4[arg2]) {
        func_801CDD80(entIndices, arg1);
        g_CurrentEntity->ext.GH_Props.unkB4[arg2] = arg1->unk0;
    }
    if (!--g_CurrentEntity->ext.GH_Props.unkB4[arg2]) {
        arg1++;
        if (!arg1->unk0) {
            g_CurrentEntity->ext.GH_Props.unkB0[arg2] = 0;
        } else {
            ++g_CurrentEntity->ext.GH_Props.unkB0[arg2];
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_801CE1E8);
#else
void func_801CE1E8(s32 step) {
    s32 i;

    g_CurrentEntity->step = (u16)step;
    g_CurrentEntity->step_s = 0;
    g_CurrentEntity->pose = 0;
    g_CurrentEntity->poseTimer = 0;
    for (i = 0; i < 4; i++) {
        g_CurrentEntity->ext.GH_Props.unkB0[i] = 0;
        g_CurrentEntity->ext.GH_Props.unkB4[i] = 0;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_801CE228);
#else
void func_801CE228(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        g_CurrentEntity->ext.GH_Props.unkB0[i] = 0;
        g_CurrentEntity->ext.GH_Props.unkB4[i] = 0;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", polarPlacePartsList);
#else
void polarPlacePartsList(s16* offsets) {
    Entity* entity;

    while (*offsets) {
        entity = g_CurrentEntity + *offsets;
        if (!entity->ext.GH_Props.unkA8) {
            func_us_8019ED80(entity);
        }
        offsets++;
    }
}
#endif

// decompiled in src/boss/bo1/e_explosion_flame.c
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019D260_from_rcen);
#else
// BO1's func_us_8019D260_from_rcen with RBO2's flame id
void func_us_8019D260_from_rcen(void) {
    s16 angle;
    s32 i;
    s8 type;
    Entity* entity;

    type = Random() & 3;
    angle = ((Random() & 0xF) << 8) - 0x800;
    for (i = 0; i < 6; i++) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_ID(UNK_1F), g_CurrentEntity, entity);
            entity->ext.et_801A518C.unk89 = 6 - i;
            entity->ext.et_801A518C.unk88 = type;
            entity->params = 2;
            entity->ext.et_801A518C.unk84 = angle;
            entity->zPriority = g_CurrentEntity->zPriority + 1;
        }
    }
}
#endif

extern EInit g_EInitParticle;
extern EntityConfig D_us_80180B44[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/unk_1B284", func_us_8019F260);
#else
// flames: params 0 rises, 1 bursts upward and leaves a params 0 flame
void func_us_8019F260(Entity* self) {
    Entity* entity;
    EntityConfig* config;
    s32 params;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitParticle);
        params = self->params & 0xF;
        config = &D_us_80180B44[params];
        self->palette = config->palette + 0x2E0;
        self->blendMode = config->blendMode;
        self->animSet = config->animSet;
        self->unk5A = config->unk5A;
        self->step = params + 1;
        self->ext.et_801A518C.anim = config->animData;
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
            self->velocityY = FIX(-0.25);
            self->step_s++;
        }
        MoveEntity();
        break;
    case 2:
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0x60;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 8) - 0x8000;
            self->velocityY = FIX(-2.5);
            self->ext.et_801A518C.acceleration = -(Random() << 4) - 0x1000;
            self->step_s++;
        }
        MoveEntity();
        self->opacity--;
        self->velocityY += self->ext.et_801A518C.acceleration;
        if (!self->poseTimer && self->pose == 8) {
            entity = AllocEntity(
                &g_Entities[STAGE_ENTITY_START], &g_Entities[TOTAL_ENTITY_COUNT]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1F), self, entity);
                entity->params = 1;
                entity->zPriority = self->zPriority - 1;
            }
        }
        break;
    default:
        return;
    }
    if (!AnimateEntity(self->ext.et_801A518C.anim, self)) {
        DestroyEntity(self);
    }
}
#endif

extern EInit g_EInitInteractable;
extern s32 D_us_80180B5C;
extern u32 g_CutsceneFlags;

void func_us_8019F4AC(Entity* self) {
    Entity* entity;
    u32 posX;
    s32 posY;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->flags |= FLAG_UNK_10000;
        // fallthrough

    case 1:
        posX = PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi;
        if ((posX - 0x41) < 0x17F) {
            D_us_80180B5C |= 1;
            self->step++;
        }
        break;

    case 2:
        if (g_CastleFlags[DEATH_FIGHT_CS] ||
            (g_PlayableCharacter != PLAYER_ALUCARD) ||
            (g_DemoMode != Demo_None)) {
            posX = PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi;
            if ((posX - 0x81) >= 0xFF) {
                break;
            }
        } else if (!(g_CutsceneFlags & 2)) {
            break;
        }
        g_api.TimeAttackController(
            TIMEATTACK_EVENT_DEATH_DEFEAT, TIMEATTACK_SET_VISITED);
        stopMusicFlag = true;
        currentMusicId = MU_DEATH_BALLAD;
        D_us_80180B5C |= 2;
        self->step++;
        break;

    case 3:
        if (g_api.func_80131F68() == false) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        // fallthrough

    case 4:
        if (D_us_80180B5C & 0x10) {
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_DEATH_DEFEAT, TIMEATTACK_SET_RECORD);
            g_api.PlaySfx(SET_UNK_90);
            currentMusicId = MU_ABANDONED_PIT;
            self->step++;
        }
        break;

    case 5:
        if (D_us_80180B5C & 0x40) {
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
            entity->params = 0x15;
            stopMusicFlag = true;
            currentMusicId = MU_ABANDONED_PIT;
            D_us_80180B5C |= 0x80;
            self->step++;
        }
        break;

    case 7:
        if (g_api.func_80131F68() == false) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;
    }

    FntPrint("set_step %x\n", self->step);
    FntPrint("boss_flag %x\n", D_us_80180B5C);
}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_801805D4[];
extern u8 D_us_801805DC[];
extern u8 D_us_801805E4[];
extern u8 D_us_801805EC[];
extern u8 D_us_801805F4[];
extern u8 D_us_801805FC[];
extern u8 D_us_80180604[];
extern u8 D_us_8018060C[];
extern u8 D_us_8018085C[];
extern u8 D_us_80180874[];
extern u8 D_us_8018088C[];
extern u8 D_us_801808C4[];
extern u8 D_us_801808D4[];
extern u8 D_us_8018090C[];
extern u8 D_us_8018091C[];
extern u8 D_us_8018092C[];
extern u8 D_us_8018093C[];
extern u8 D_us_8018094C[];
extern u8 D_us_80180994[];
extern u8 D_us_801809A4[];
extern u8 D_us_801809B4[];
extern u8 D_us_801809C4[];
extern u8 D_us_801809D4[];
extern u8 D_us_80180A14[];
extern u8 D_us_80180A24[];
extern u8 D_us_80180A54[];
extern u8 D_us_80180A6C[];
extern u8 D_us_80180AEC[];
extern u8 D_us_80180B24[];
u8 D_us_801805D4[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805DC[] ALIGNED4 = {0x04, 0x07, 0x04, 0x08, 0x04, 0x09, 0x00, 0x00};
u8 D_us_801805E4[] ALIGNED4 = {0x04, 0x1D, 0x04, 0x1E, 0x04, 0x1F, 0x00, 0x00};
u8 D_us_801805EC[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805F4[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805FC[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180604[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_8018060C[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_8018085C[] ALIGNED4 = {0x00, 0xFF, 0x00, 0xFF, 0x80, 0x01, 0x80, 0x01, 0x00, 0x02, 0x80, 0x01, 0x00, 0x02, 0x00, 0x01, 0x80, 0x01, 0x00, 0x02, 0x80, 0x03, 0x00, 0x00};
u8 D_us_80180874[] ALIGNED4 = {0xC0, 0xFE, 0x00, 0x01, 0x00, 0x01, 0xC0, 0x01, 0x40, 0x02, 0x80, 0x01, 0x00, 0x03, 0x40, 0x01, 0xC0, 0x01, 0x40, 0x02, 0xC0, 0x03, 0x00, 0x00};
u8 D_us_8018088C[] ALIGNED4 = {0x00, 0x00, 0x00, 0xFF, 0x80, 0x01, 0x00, 0x00, 0x40, 0x02, 0x00, 0x02, 0x00, 0x03, 0xC0, 0xFF, 0x00, 0x01, 0x80, 0x01, 0x40, 0x02, 0x00, 0x00};
u8 D_us_801808C4[] ALIGNED4 = {0x80, 0xFF, 0xC0, 0xFF, 0x00, 0x02, 0x80, 0xFF, 0x80, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00};
u8 D_us_801808D4[] ALIGNED4 = {0x20, 0x00, 0xC0, 0xFE, 0x00, 0x01, 0x80, 0x00, 0xC0, 0x02, 0x40, 0x02, 0x80, 0x03, 0x00, 0x00};
u8 D_us_8018090C[] ALIGNED4 = {0x00, 0x01, 0x00, 0x04, 0x80, 0x06, 0x80, 0x04, 0x00, 0x07, 0x00, 0x07, 0x00, 0x09, 0x00, 0x00};
u8 D_us_8018091C[] ALIGNED4 = {0x80, 0x01, 0x00, 0x06, 0x00, 0x07, 0x80, 0x07, 0x80, 0x08, 0x40, 0x07, 0x80, 0x09, 0x00, 0x00};
u8 D_us_8018092C[] ALIGNED4 = {0x80, 0xFE, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00};
u8 D_us_8018093C[] ALIGNED4 = {0x00, 0xFD, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x00, 0x02, 0x80, 0x01, 0x00, 0x02, 0x00, 0x00};
u8 D_us_8018094C[] ALIGNED4 = {0x40, 0xFD, 0x20, 0x01, 0xC0, 0x01, 0xC0, 0x01, 0x80, 0x02, 0x80, 0x01, 0x00, 0x02, 0x00, 0x00};
u8 D_us_80180994[] ALIGNED4 = {0x00, 0x01, 0x00, 0x06, 0x00, 0x08, 0x80, 0xFF, 0x80, 0x03, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00};
u8 D_us_801809A4[] ALIGNED4 = {0x00, 0x02, 0x80, 0x06, 0x00, 0x09, 0xC0, 0x00, 0x80, 0x03, 0x80, 0x0A, 0x00, 0x04, 0x00, 0x00};
u8 D_us_801809B4[] ALIGNED4 = {0x80, 0xFE, 0x00, 0x03, 0x80, 0x04, 0x00, 0xFE, 0x00, 0x02, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00};
u8 D_us_801809C4[] ALIGNED4 = {0x40, 0xFE, 0x80, 0x03, 0xC0, 0x04, 0x80, 0xFE, 0x40, 0x02, 0x80, 0x04, 0x00, 0x04, 0x00, 0x00};
u8 D_us_801809D4[] ALIGNED4 = {0x80, 0xFF, 0x00, 0xFF, 0x00, 0x02, 0x40, 0x00, 0x80, 0x01, 0x80, 0x02, 0x00, 0x02, 0x00, 0x00};
u8 D_us_80180A14[] ALIGNED4 = {0x80, 0xFE, 0x00, 0xFF, 0x00, 0x01, 0x80, 0xFE, 0x00, 0x02, 0x80, 0x01, 0x80, 0x02, 0x00, 0x00};
u8 D_us_80180A24[] ALIGNED4 = {0x40, 0xFE, 0x00, 0xFF, 0x40, 0x01, 0x80, 0xFE, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x02, 0x00, 0x00};
u8 D_us_80180A54[] ALIGNED4 = {0x80, 0x01, 0x00, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x00, 0x00};
u8 D_us_80180A6C[] ALIGNED4 = {0x00, 0x02, 0x00, 0xFF, 0x00, 0xFF, 0x80, 0xFF, 0x80, 0xFF, 0x00, 0xFF, 0x80, 0xFF, 0x00, 0x00, 0xC0, 0xFF, 0x80, 0xFF, 0x00, 0xFF, 0x00, 0x00};
u8 D_us_80180AEC[] ALIGNED4 = {0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0xFF, 0x00, 0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x04, 0x02, 0x05, 0x02, 0x06, 0x02, 0x07, 0x02, 0x08, 0x02, 0x09, 0x02, 0x0A, 0x02, 0x0B, 0x02, 0x0C, 0x02, 0x0D, 0xFF, 0x00};
u8 D_us_80180B24[] ALIGNED4 = {0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x04, 0x02, 0x05, 0x02, 0x06, 0x02, 0x07, 0x02, 0x08, 0x02, 0x09, 0x02, 0x0A, 0x02, 0x0B, 0x02, 0x0C, 0x02, 0x0D, 0x02, 0x0E, 0xFF, 0x00, 0x00, 0x00};
u8* D_us_8018061C[8] = {
    D_us_801805D4,
    D_us_801805DC,
    D_us_801805E4,
    D_us_801805EC,
    D_us_801805F4,
    D_us_801805FC,
    D_us_80180604,
    D_us_8018060C,
};
u8 D_us_8018063C[8] = { 8, 8, 8, 8, 8, 8, 8, 8 };
u8 D_us_80180644[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u16 D_us_8018064C[8] = { 3, 0x8001, 0x8001, 3, 3, 3, 3, 3 };
u8 D_us_8018065C[0x10] = { 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0, 0, 0, 0, 0, 0, 0, 0 };
u8 D_us_8018066C[0x10] = { 6, 1, 6, 2, 6, 3, 6, 4, 6, 5, 6, 6, 0, 0, 0, 0 };
u8 D_us_8018067C[0x18] = {
    5,
    7,
    4,
    8,
    3,
    9,
    5,
    0xA,
    4,
    0xB,
    6,
    0xC,
    4,
    0xD,
    4,
    0xE,
    1,
    0xF,
    4,
    0x10,
    0x18,
    0xF,
    0xFF,
    0,
};
u8 D_us_80180694[0x14] = {
    3,
    0xA,
    3,
    0xB,
    4,
    0xC,
    4,
    0xD,
    3,
    0xE,
    1,
    0xF,
    1,
    0x10,
    0x20,
    0xF,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801806A8[8] = { 2, 0xF, 2, 0x11, 2, 0x12, 0, 0 };
#ifdef VERSION_PC
// func_us_8019B52C plays this right after D_us_80180694 without resetting the
// pose, so it starts at pose 8, past its own end: on PSX that plays the tail of
// D_us_801806B8, which follows it in the overlay's data. As two globals nothing
// keeps them adjacent (x86-64 aligns the second one to 16 bytes and pose 8
// lands in the padding, so Death attacks 47 frames early), so here they are
// one array.
u8 D_us_801806B0[8 + 0x18] = {
    0x10, 0xA,  6, 8,    5, 7,    0xFF, 0,
    // D_us_801806B8
    2,    1,    6, 0x13, 7, 0x14, 8,    0x15, 9, 0x16, 6, 0x17,
    6,    0x18, 7, 0x19, 7, 0x1A, 8,    0x1B, 4, 0x1C, 0xFF, 0,
};
#else
u8 D_us_801806B0[8] = { 0x10, 0xA, 6, 8, 5, 7, 0xFF, 0 };
u8 D_us_801806B8[0x18] = {
    2,
    1,
    6,
    0x13,
    7,
    0x14,
    8,
    0x15,
    9,
    0x16,
    6,
    0x17,
    6,
    0x18,
    7,
    0x19,
    7,
    0x1A,
    8,
    0x1B,
    4,
    0x1C,
    0xFF,
    0,
};
#endif
u8 D_us_801806D0[0x10] = { 4, 2, 4, 0x4A, 4, 0x4B, 4, 0x4C, 4, 0x4D, 4, 0x4E, 0xFF, 0, 0, 0 };
u8 D_us_801806E0[0x18] = {
    9,
    0x1D,
    4,
    0x1E,
    4,
    0x1F,
    4,
    0x20,
    3,
    0x21,
    3,
    0x22,
    3,
    0x23,
    3,
    0x24,
    3,
    0x25,
    4,
    0x26,
    8,
    0x27,
    0xFF,
    0,
};
u8 D_us_801806F8[0x10] = { 3, 0x25, 3, 0x28, 3, 0x29, 3, 0x2A, 3, 0x1F, 4, 0x1E, 5, 0x1D, 0xFF, 0 };
u8 D_us_80180708[0x10] = { 4, 0x2B, 4, 0x2C, 4, 0x2D, 4, 0x2E, 4, 0x2F, 4, 0x31, 4, 0x30, 0xFF, 0 };
u8 D_us_80180718[0x14] = {
    2,
    0x30,
    1,
    0x31,
    2,
    0x32,
    1,
    0x31,
    2,
    0x33,
    1,
    0x31,
    2,
    0x34,
    1,
    0x31,
    0,
    0,
    0,
    0,
};
u8 D_us_8018072C[0x18] = {
    1,
    0x33,
    4,
    0x31,
    1,
    0x34,
    4,
    0x31,
    1,
    0x30,
    4,
    0x31,
    4,
    0x35,
    4,
    0x36,
    4,
    0x37,
    4,
    0x38,
    0x10,
    0x39,
    0xFF,
    0,
};
u8 D_us_80180744[0x18] = {
    0x10,
    0x39,
    4,
    0x38,
    4,
    0x37,
    4,
    0x36,
    4,
    0x35,
    4,
    0x31,
    1,
    0x30,
    4,
    0x31,
    1,
    0x34,
    4,
    0x31,
    1,
    0x33,
    0xFF,
    0,
};
u8 D_us_8018075C[0x14] = {
    4,
    0x30,
    4,
    0x3A,
    4,
    0x3B,
    4,
    0x3C,
    4,
    0x3D,
    4,
    0x3E,
    4,
    0x3F,
    8,
    0x40,
    8,
    0x50,
    0xFF,
    0,
};
u8 D_us_80180770[0x14] = {
    5,
    0x41,
    5,
    0x42,
    5,
    0x43,
    5,
    0x44,
    5,
    0x45,
    5,
    0x46,
    5,
    0x47,
    5,
    0x48,
    5,
    0x49,
    0,
    0,
};
s32 D_us_80180784 = 0;
s32 D_us_80180788 = 0;
DeathPartInit D_us_8018078C[] = {
    {1, 0, -0x18, 1, 1, -0x100},
    {3, 1, 0x18, 7, 2, 0},
    {4, 3, 0x12, 8, 3, 0},
    {5, 4, 0, 0xA, 4, 0},
    {6, 1, 0x18, 0xB, -1, 0},
    {7, 6, 0x12, 0xC, -2, 0},
    {8, 7, 0, 0xE, -3, 0},
    {9, 0, 0x1A, 3, 1, 0},
    {0xA, 9, 0x12, 4, 2, 0},
    {0xB, 0xA, 0xC, 5, 3, 0},
    {0xC, 0xB, 0, 6, 4, 0},
    {0},
};
s16 D_us_8018081C[0xC] = { 1, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0 };
s16 D_us_80180834[8] = { 1, 3, 4, 6, 7, 5, 8, 0 };
s16 D_us_80180844[0xC] = { 1, 3, 4, 6, 7, 5, 8, 9, 0xA, 0xB, 0xC, 0 };
unkStr_801CDD80 D_us_801808A4[4] = {
    { 0x20, D_us_8018085C },
    { 0x40, D_us_80180874 },
    { 0x30, D_us_8018088C },
    { 0, NULL },
};
unkStr_801CDD80 D_us_801808E4[5] = {
    { 0x10, D_us_8018088C },
    { 0x18, D_us_801808C4 },
    { 0x10, D_us_8018088C },
    { 0x40, D_us_801808D4 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_8018095C[7] = {
    { 0x20, D_us_8018090C },
    { 0x20, D_us_8018091C },
    { 4, D_us_8018092C },
    { 2, D_us_8018093C },
    { 0x20, D_us_8018094C },
    { 0x40, D_us_8018088C },
    { 0, NULL },
};
unkStr_801CDD80 D_us_801809E4[6] = {
    { 0x20, D_us_80180994 },
    { 0x18, D_us_801809A4 },
    { 4, D_us_801809B4 },
    { 0x20, D_us_801809C4 },
    { 0x40, D_us_801809D4 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_80180A34[4] = {
    { 6, D_us_80180A14 },
    { 8, D_us_80180A24 },
    { 0x10, D_us_8018088C },
    { 0, NULL },
};
unkStr_801CDD80 D_us_80180A84[3] = { { 6, D_us_80180A54 }, { 0x40, D_us_80180A6C }, { 0, NULL } };
s16 D_us_80180A9C[6] = { 9, 0xA, 0xB, 0xC, 0, 0 };
static s16 D_us_80180AA8[] = {0x100, 0x120, 0x20, -0xC0};
static s16 D_us_80180AB0[] = {-0x80, 0, 0x180, 0x300};
unkStr_801CDD80 D_us_80180AB8[] = {
    {0x40, D_us_80180AA8},
    {0x40, D_us_80180AB0},
    {0, NULL},
};
EntityConfig D_us_80180B44[2] = {
    { 0x8006, 0x58, 4, 0x30, D_us_80180B24 },
    { 0x8005, 0x59, 8, 0x30, D_us_80180AEC },
};
// end pc_import_data.py
// clang-format on
#endif
