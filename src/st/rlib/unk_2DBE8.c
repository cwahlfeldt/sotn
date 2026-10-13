// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rlib.h"

extern EInit g_EInitLion;
extern s16 D_us_80181750[];
extern s16 D_us_80181760[];
extern u8 D_us_80181768[];
extern u8 D_us_80181778[];
extern u8 D_us_801817A8[];
extern u8 D_us_801817C0[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_2DBE8", func_us_801ADBE8);
#else
// The Lion: keeps its distance while Alucard faces it, charges when he
// turns away, and swipes when he is close
void func_us_801ADBE8(Entity* self) {
    Entity* newEntity;
    s32 i;

    if ((self->flags & FLAG_DEAD) && self->step != 6) {
        PlaySfxPositional(SFX_UNK_RLIB_732);
        self->hitboxState = 0;
        PlaySfxPositional(SFX_FM_STUTTER_EXPLODE);
        SetStep(6);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitLion);
        CreateEntityFromCurrentEntity(E_HARPY_KICK, self + 1);
        // fallthrough
    case 1:
        if (UnkCollisionFunc3(D_us_80181750) & 1) {
            SetStep(2);
        }
        break;
    case 2:
        AnimateEntity(D_us_801817C0, self);
        self->facingLeft = GetSideToPlayer() & 1;
        if (GetDistanceToPlayerX() < 0x60) {
            SetStep(3);
        }
        break;
    case 3:
        AnimateEntity(D_us_80181768, self);
        if (UnkCollisionFunc2(D_us_80181760) & 0x60) {
            self->posX.val -= self->velocityX;
        }
        if (self->facingLeft != self->ext.rlibEnemy.flag) {
            self->velocityX = FIX(-0.75);
        } else {
            self->velocityX = FIX(0.75);
        }
        if (self->ext.rlibEnemy.flag) {
            self->velocityX -= self->velocityX / 4;
        }
        switch (self->step_s) {
        case 0:
            self->facingLeft = GetSideToPlayer() & 1;
            self->ext.rlibEnemy.flag = 1;
            self->ext.rlibEnemy.angle = 0x60;
            self->step_s++;
            // fallthrough
        case 1:
            self->facingLeft = GetSideToPlayer() & 1;
            if (PLAYER.facingLeft != self->facingLeft) {
                self->ext.rlibEnemy.angle--;
            }
            if (GetDistanceToPlayerX() <= 0x60 ||
                PLAYER.facingLeft != self->facingLeft) {
                if (!self->ext.rlibEnemy.angle) {
                    self->step_s++;
                }
                if (GetDistanceToPlayerX() < 0x28) {
                    SetStep(5);
                }
            } else {
                SetStep(4);
            }
            break;
        case 2:
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            self->ext.rlibEnemy.flag = 0;
            if (GetDistanceToPlayerX() > 0xA0) {
                SetStep(2);
            }
            break;
        }
        if (self->hitFlags & 3) {
            self->facingLeft = GetSideToPlayer() & 1;
            SetStep(5);
        }
        break;
    case 4:
        AnimateEntity(D_us_80181768, self);
        if (UnkCollisionFunc2(D_us_80181760) & 0x60) {
            self->posX.val -= self->velocityX;
        }
        self->facingLeft = GetSideToPlayer() & 1;
        if (PLAYER.facingLeft != self->facingLeft) {
            self->ext.rlibEnemy.angle--;
            self->ext.rlibEnemy.flag = 1;
            if (!self->ext.rlibEnemy.angle) {
                SetStep(3);
                break;
            }
        } else {
            self->ext.rlibEnemy.angle = 0x20;
            self->ext.rlibEnemy.flag = 0;
        }
        if (self->facingLeft != self->ext.rlibEnemy.flag) {
            self->velocityX = FIX(-0.75);
        } else {
            self->velocityX = FIX(0.75);
        }
        if (self->ext.rlibEnemy.flag) {
            self->velocityX -= self->velocityX / 4;
        }
        switch (self->step_s) {
        case 0:
            self->facingLeft = GetSideToPlayer() & 1;
            self->ext.rlibEnemy.timer = 0x30;
            self->step_s++;
            // fallthrough
        case 1:
            self->velocityX -= self->velocityX / 2;
            if (g_Timer & 1) {
                self->poseTimer++;
            }
            if (!--self->ext.rlibEnemy.timer) {
                self->step_s++;
            }
            // fallthrough
        case 2:
            if (GetDistanceToPlayerX() < 0x40) {
                SetStep(5);
            }
            break;
        }
        if (self->hitFlags & 3) {
            self->facingLeft = GetSideToPlayer() & 1;
            SetStep(5);
        }
        break;
    case 5:
        if (!self->poseTimer && self->pose == 6) {
            PlaySfxPositional(SFX_BOSS_WING_FLAP);
        }
        if (!AnimateEntity(D_us_80181778, self)) {
            SetStep(3);
        }
        break;
    case 6:
        switch (self->step_s) {
        case 0:
            if (!(g_Timer & 3)) {
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(
                        E_INTENSE_EXPLOSION, self, newEntity);
                    newEntity->posX.i.hi += 0x20 - (Random() & 0x2F);
                    newEntity->posY.i.hi += 0x20 - (Random() & 0x3F);
                }
            }
            if (!(g_Timer & 7)) {
                PlaySfxPositional(SFX_SMALL_FLAME_IGNITE);
            }
            if (!AnimateEntity(D_us_801817A8, self)) {
                self->step_s++;
            }
            break;
        case 1:
            PlaySfxPositional(SFX_FLEA_ARMOR_EXPLODE);
            for (i = 0; i < 6; i++) {
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_1E, self, newEntity);
                    newEntity->params = i;
                    newEntity->facingLeft = self->facingLeft;
                }
            }
            DestroyEntity(self);
            return;
        }
        break;
    case 0xFF:
#include "../pad2_anim_debug.h"
    }
}
#endif

extern EInit D_us_8018064C;
extern s8 g_HarpyKickHitboxes[];
extern u8 g_HarpyKickHitboxIdx[];

void EntityHarpyKick(Entity* self) {
    s32 animFrame;
    s8* hitbox;
    Entity* harpy;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_8018064C);
        /* fall through */
    case 1:
        harpy = self - 1;
        self->facingLeft = harpy->facingLeft;
        self->posX.val = harpy->posX.val;
        self->posY.val = harpy->posY.val;
        animFrame = harpy->animCurFrame;
        hitbox = g_HarpyKickHitboxes;
        animFrame = g_HarpyKickHitboxIdx[animFrame];
        hitbox += animFrame * 4;
        self->hitboxOffX = *hitbox++;
        self->hitboxOffY = *hitbox++;
        self->hitboxWidth = *hitbox++;
        self->hitboxHeight = *hitbox++;
        if (harpy->entityId != E_UNK_1C) {
            DestroyEntity(self);
        }
        break;
    }
}

extern EInit D_us_80180658;
extern s32 D_us_801817CC[];

void func_us_801AE414(Entity* self) {
    switch (self->step_s) {
    case 0:
        InitializeEntity(D_us_80180658);
        self->animCurFrame = self->params + 0x1C;
        /* fall through */
    case 1:
        MoveEntity();
        self->velocityY += (u32)self->ext.et_801AE414.accelY;
        self->ext.et_801AE414.accelY += D_us_801817CC[self->params];
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_2DBE8", func_us_801AE4B4);
#else
// BO0's func_us_801B15BC: moves a value toward a target by step; true
// once there
s32 func_us_801AE4B4(s16* value, s16 target, s16 step) {
    s16 current = *value;

    if (abs(current - target) < step) {
        *value = target;
        return 1;
    }
    if (target < current) {
        *value = current - step;
    }
    if (*value < target) {
        *value += step;
    }
    return 0;
}
#endif

extern EInit g_EInitTinman;
extern s16 D_us_80181820[];
extern s16 D_us_80181830[];
extern u8 D_us_80181838[];
extern u8 D_us_80181844[];
extern u8 D_us_80181880[];
extern u8 D_us_801818B8[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_2DBE8", func_us_801AE534);
#else
// The Tin Man: rolls at Alucard swinging its arm (0x20); low on health it
// falls over, loses its armor and shoots fire from where it lies
void func_us_801AE534(Entity* self) {
    Entity* newEntity;
    s32 atTopSpeed;
    s32 spawn;
    s32 side;
    s32 i;

    if (self->step && self->hitPoints < g_api.enemyDefs[264].hitPoints / 4 &&
        self->step < 5) {
        SetStep(5);
    }
    if (self->hitFlags & 3) {
        PlaySfxPositional(SFX_UNK_RLIB_73D);
    }
    if ((self->flags & FLAG_DEAD) && self->step != 7) {
        SetStep(7);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitTinman);
        self->hitboxOffY = 9;
        CreateEntityFromCurrentEntity(E_UNK_20, self + 1);
        self[1].zPriority = self->zPriority + 1;
        // fallthrough
    case 1:
        if (UnkCollisionFunc3(D_us_80181820) & 1) {
            SetStep(2);
        }
        break;
    case 2:
        if (GetDistanceToPlayerX() < 0x60) {
            SetStep(3);
        }
        break;
    case 3:
        AnimateEntity(D_us_80181838, self);
        atTopSpeed = 0;
        self->ext.rlibEnemy.flag = 0;
        if (self->facingLeft) {
            self->velocityX += FIX(1.0 / 32);
            if (self->velocityX > FIX(1.5)) {
                self->velocityX = FIX(1.5);
                atTopSpeed = 1;
            }
        } else {
            self->velocityX -= FIX(1.0 / 32);
            if (self->velocityX < FIX(-1.5)) {
                self->velocityX = FIX(-1.5);
                atTopSpeed = 1;
            }
        }
        if (atTopSpeed) {
            spawn = !(Random() & 0x1F);
            if (self->poseTimer) {
                self->poseTimer--;
            }
        } else {
            spawn = !(g_Timer & 0xF);
        }
        if (spawn) {
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_22, self, newEntity);
                newEntity->ext.et_801AEFE0.parent = self;
                newEntity->facingLeft = self->facingLeft;
                newEntity->zPriority = self->zPriority + 1;
            }
        }
        if (UnkCollisionFunc2(D_us_80181830) & 0x60) {
            self->posX.val -= self->velocityX;
            self->velocityX = -self->velocityX;
        }
        if (GetDistanceToPlayerX() < 0x50) {
            self->ext.rlibEnemy.angle =
                (self->ext.rlibEnemy.angle - 0x100) & 0xFFF;
        } else {
            func_us_801AE4B4(&self->ext.rlibEnemy.angle, 0, 0x80);
        }
        side = GetSideToPlayer() & 1;
        if (side == self->facingLeft && GetDistanceToPlayerX() > 0x30) {
            SetStep(4);
        }
        break;
    case 4:
        self->velocityX -= self->velocityX / 32;
        if (UnkCollisionFunc2(D_us_80181830) & 0x60) {
            self->posX.val -= self->velocityX;
            self->velocityX = -self->velocityX;
        }
        switch (self->step_s) {
        case 0:
            self->ext.rlibEnemy.flag = 0;
            if (func_us_801AE4B4(&self->ext.rlibEnemy.angle, 0, 0x80)) {
                self->step_s++;
            }
            break;
        case 1:
            self->ext.rlibEnemy.flag = 1;
            if (!AnimateEntity(D_us_801818B8, self)) {
                self->ext.rlibEnemy.flag = 0;
                self->animCurFrame = 1;
                self->facingLeft ^= 1;
                SetStep(3);
            }
            break;
        }
        break;
    case 5:
        switch (self->step_s) {
        case 0:
            if (GetSideToPlayer() & 1) {
                self->velocityX = FIX(1);
            } else {
                self->velocityX = FIX(-1);
            }
            self->velocityY = FIX(-2);
            self->drawFlags = ENTITY_ROTATE;
            self->step_s++;
            // fallthrough
        case 1:
            func_us_801AE4B4(&self->rotate, 0x80, 0x10);
            if (UnkCollisionFunc3(D_us_80181820) & 1) {
                self->step_s++;
            }
            break;
        case 2:
            if (func_us_801AE4B4(&self->rotate, 0, 0x10)) {
                self->ext.rlibEnemy.armorLost = 1;
                self->drawFlags = ENTITY_DEFAULT;
                self->rotate = 0;
                self->step_s++;
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80181844, self)) {
                self->hitboxWidth = 0xF;
                self->hitboxHeight = 0x11;
                self->hitboxOffX = 2;
                self->hitboxOffY = 0xF;
                self->ext.rlibEnemy.timer = Random() & 0xF;
                SetStep(6);
            }
            break;
        }
        break;
    case 6:
        AnimateEntity(D_us_80181880, self);
        if (!(self->ext.rlibEnemy.timer & 7)) {
            PlaySfxPositional(SFX_EXPLODE_E);
        }
        if (!(++self->ext.rlibEnemy.timer & 3)) {
            newEntity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_21, self, newEntity);
                newEntity->facingLeft = self->facingLeft;
                newEntity->posY.i.hi -=
                    7 + ((u16)(self->ext.rlibEnemy.timer & 7) >> 1);
                if (self->facingLeft) {
                    newEntity->posX.i.hi -= 2;
                } else {
                    newEntity->posX.i.hi += 2;
                }
            }
        }
        break;
    case 7:
        UnkCollisionFunc3(D_us_80181820);
        func_us_801AE4B4(&self->rotate, 0, 0x20);
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            if (self->ext.rlibEnemy.armorLost) {
                self->step_s = 2;
            } else {
                self->step_s = 1;
            }
            break;
        case 1:
            if (!AnimateEntity(D_us_80181844, self)) {
                self->step_s++;
            }
            break;
        case 2:
            for (i = 0; i < 11; i++) {
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_23, self, newEntity);
                    newEntity->params = i + 0x3B;
                    newEntity->facingLeft = self->facingLeft;
                }
            }
            PlaySfxPositional(SFX_FLEA_ARMOR_EXPLODE);
            DestroyEntity(self);
            return;
        }
        break;
    case 0xFF:
#include "../pad2_anim_debug.h"
    }
}
#endif

extern EInit D_us_80180670;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_2DBE8", func_us_801AED4C);
#else
// The Tin Man's arm, in the slot after it: swings with ext.angle; when the
// armor comes off it breaks into pieces and becomes a hitbox for the body
void func_us_801AED4C(Entity* self) {
    Entity* tinman = self - 1;
    Entity* newEntity;
    s32 i;
    s16 angle;

    self->posX.val = tinman->posX.val;
    self->facingLeft = tinman->facingLeft;
    self->posY.val = tinman->posY.val;
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180670);
        self->drawFlags = ENTITY_ROTATE;
        // fallthrough
    case 1:
        if (tinman->ext.rlibEnemy.flag == 1) {
            self->hitboxState = 0;
            self->animCurFrame = 0;
        } else {
            self->animCurFrame = 5;
            self->rotate = tinman->ext.rlibEnemy.angle;
            self->hitboxState = 1;
            angle = self->rotate + 0x200;
            self->hitboxOffX = (rcos(angle) * 7) >> 10;
            self->hitboxOffY = (rsin(angle) * 7) >> 10;
            if (self->rotate >= 0xF00) {
                PlaySfxPositional(SFX_WHIP_TWIRL_SWISH);
            }
        }
        if (tinman->ext.rlibEnemy.armorLost || (tinman->flags & FLAG_DEAD)) {
            self->hitboxState = 0;
            self->step++;
        }
        break;
    case 2:
        newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (newEntity != NULL) {
            CreateEntityFromEntity(E_UNK_23, self, newEntity);
            newEntity->facingLeft = self->facingLeft;
            newEntity->rotate = self->rotate;
            newEntity->params = 5;
            newEntity->zPriority = self->zPriority;
        }
        for (i = 0; i < 8; i++) {
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_23, self, newEntity);
                newEntity->params = 0;
                newEntity->zPriority = self->zPriority;
            }
        }
        PlaySfxPositional(SFX_WALL_DEBRIS_C);
        self->hitboxWidth = 0xF;
        self->hitboxHeight = 0xB;
        self->hitboxOffX = 0x17;
        self->hitboxOffY = -5;
        self->parent = tinman;
        self->animCurFrame = 0;
        self->hitboxState = tinman->hitboxState;
        self->attackElement = tinman->attackElement;
        self->attack = tinman->attack;
        self->step++;
        break;
    case 3:
        if (tinman->entityId != E_UNK_1F || (tinman->flags & FLAG_DEAD)) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern EInit D_us_80180688;
extern AnimationFrame D_us_80181888;

void func_us_801AEFE0(Entity* self) {
    Entity* parent;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180688);
        self->blendMode = BLEND_TRANSP;
        PlaySfxPositional(SFX_NOISE_SWEEP_DOWN_B);
        /* fall through */
    case 1:
        parent = self->ext.et_801AEFE0.parent;
        self->posX.val = parent->posX.val;
        self->posY.val = parent->posY.val;
        if (AnimateEntity(&D_us_80181888, self) == 0) {
            DestroyEntity(self);
        }
        break;
    }
}

extern EInit D_us_8018067C;
extern AnimationFrame D_us_8018189C;

void func_us_801D8D44_from_no4(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_8018067C);
        if (self->facingLeft) {
            self->velocityX = FIX(6);
        } else {
            self->velocityX = FIX(-6);
        }
        /* fall through */
    case 1:
        MoveEntity();
        AnimateEntity(&D_us_8018189C, self);
        if (self->flags & FLAG_DEAD) {
            DestroyEntity(self);
        }
        break;
    }
}

extern u8 D_us_801818A8[];
extern RlibTinmanPiece D_us_801818D0[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_2DBE8", func_us_801AF11C);
#else
// A piece of the Tin Man: params 0 a bolt, 0x3B-0x45 the armor when it
// dies, anything else a spinning part of the arm
void func_us_801AF11C(Entity* self) {
    Entity* newEntity;
    RlibTinmanPiece* piece;
    s32 speed;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180688);
        if (!self->params) {
            self->step = 1;
            speed = (Random() & 0x1F) + 0x10;
            angle = Random() * 6 + 0x900;
            self->velocityX = speed * rcos(angle);
            self->velocityY = speed * rsin(angle);
            break;
        }
        self->animCurFrame = self->params;
        if (self->params >= 0x3B) {
            piece = &D_us_801818D0[self->params - 0x3B];
            self->velocityX = piece->velocityX;
            if (self->facingLeft) {
                self->velocityX = -piece->velocityX;
            }
            self->velocityY = piece->velocityY;
            self->ext.rlibEnemy.timer = piece->timer;
            self->step = 3;
            self->ext.rlibEnemy.falls = piece->falls;
            break;
        }
        self->drawFlags = ENTITY_ROTATE;
        self->step = 2;
        break;
    case 1:
        AnimateEntity(D_us_801818A8, self);
        MoveEntity();
        self->velocityY += FIX(0.09375);
        break;
    case 2:
        MoveEntity();
        self->velocityY += FIX(0.09375);
        self->rotate += 0x80;
        break;
    case 3:
        switch (self->step_s) {
        case 0:
            if (!--self->ext.rlibEnemy.timer) {
                self->ext.rlibEnemy.timer = 0x20;
                self->step_s++;
            }
            break;
        case 1:
            MoveEntity();
            if (self->ext.rlibEnemy.falls) {
                self->velocityY += FIX(0.125);
            }
            if (!--self->ext.rlibEnemy.timer) {
                PlaySfxPositional(SFX_EXPLODE_B);
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
                    newEntity->params = 1;
                    newEntity->posY.i.hi += 0x10;
                }
                DestroyEntity(self);
            }
            break;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_2DBE8", func_us_801AF3C8);
#else
// a copy of func_us_801AE4B4
s32 func_us_801AF3C8(s16* value, s16 target, s16 step) {
    s16 current = *value;

    if (abs(current - target) < step) {
        *value = target;
        return 1;
    }
    if (target < current) {
        *value = current - step;
    }
    if (*value < target) {
        *value += step;
    }
    return 0;
}
#endif

extern EInit g_EInitScarecrow;
extern s16 D_us_80181984[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_2DBE8", func_us_801AF448);
#else
// The Scarecrow: hops about on its pole, its four parts (0x25) swinging
// after it. One in 16 is a bare pole without them.
void func_us_801AF448(Entity* self) {
    Collider collider;
    Entity* entity;
    s32 side;
    s32 distance;
    s32 radius;
    s32 dx;
    s32 dy;
    s16 angle;
    s32 i;

    if ((self->flags & FLAG_DEAD) && self->step != 3) {
        SetStep(3);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitScarecrow);
        self->drawFlags = ENTITY_ROTATE;
        self->animCurFrame = 2;
        self->hitboxWidth = 0xC;
        self->hitboxHeight = 8;
        self->hitboxOffY = -6;
        if (!(Random() & 0xF)) {
            self->params |= 1;
        }
        if (self->params) {
            self->animCurFrame = 7;
            self->hitboxWidth = 9;
            self->hitboxHeight = 0xC;
            self->hitboxOffY = -6;
            self->palette--;
            break;
        }
        entity = self + 1;
        for (i = 0; i < 4; i++, entity++) {
            CreateEntityFromEntity(E_UNK_25, self, entity);
            entity->params = i;
            entity->nextPart = entity - 1;
            entity->parent = self;
        }
        self->nextPart = self + 4;
        // fallthrough
    case 1:
        switch (self->step_s) {
        case 0:
            self->velocityY = FIX(-2.5);
            if (self->facingLeft) {
                self->velocityX = FIX(1.5);
            } else {
                self->velocityX = FIX(-1.5);
            }
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY += FIX(0.125);
            if (self->velocityY > 0) {
                func_us_801AF3C8(&self->rotate, 0x100, 8);
            } else {
                func_us_801AF3C8(&self->rotate, -0x100, 0x10);
            }
            g_api.CheckCollision(
                self->posX.i.hi, self->posY.i.hi + 0x30, &collider, 0);
            if (!(collider.effects & EFFECT_SOLID)) {
                break;
            }
            self->step_s = 0;
            PlaySfxPositional(SFX_STOMP_HARD_D);
            g_api.CheckCollision(
                self->posX.i.hi, self->posY.i.hi + 0x28, &collider, 0);
            if (collider.effects & EFFECT_SOLID) {
                self->facingLeft ^= 1;
                break;
            }
            self->posY.i.hi += collider.unk18;
            side = (GetSideToPlayer() & 1) ^ 1;
            distance = GetDistanceToPlayerX();
            if (side != self->facingLeft && distance > 0x40) {
                self->facingLeft ^= 1;
                if (!(Random() & 3)) {
                    SetStep(2);
                }
            }
            break;
        }
        break;
    case 2:
        if (!self->step_s) {
            self->velocityX = 0;
            self->velocityY = 0;
            self->ext.rlibEnemy.timer = (Random() & 0x1F) + 0x20;
            self->step_s++;
        }
        i = UnkCollisionFunc2(D_us_80181984);
        if (self->facingLeft) {
            self->velocityX += FIX(1.0 / 16);
        } else {
            self->velocityX -= FIX(1.0 / 16);
        }
        if (i == 0xFF) {
            self->ext.rlibEnemy.timer = 1;
        }
        if (!--self->ext.rlibEnemy.timer) {
            self->facingLeft = GetSideToPlayer() & 1;
            SetStep(1);
        }
        break;
    case 3:
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, entity);
            entity->params = 3;
        }
        angle = self->rotate + 0x400;
        if (self->facingLeft) {
            angle = 0x800 - angle;
        }
        radius = 0x30;
        for (i = 0; i < 8; i++) {
            dx = (radius * rcos(angle)) >> 12;
            dy = (radius * rsin(angle)) >> 12;
            radius -= 8;
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_EXPLODE_PUFF_OPAQUE, self, entity);
                entity->params = 3;
                entity->rotate = self->rotate;
                entity->posY.i.hi += dy;
                entity->posX.i.hi += dx;
            }
        }
        PlaySfxPositional(SFX_FIRE_SHOT);
        DestroyEntity(self);
        return;
    case 0xFF:
#include "../pad2_anim_debug.h"
    }
}
#endif

extern EInit D_us_801806A0;
extern RlibScarecrowPart D_us_80181954[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_2DBE8", func_us_801AF9E8);
#else
// A part of the Scarecrow, params + 1 slots after it: follows the pole and
// swings against its motion
void func_us_801AF9E8(Entity* self) {
    Entity* scarecrow;
    RlibScarecrowPart* part;
    s32 offX;
    s32 offY;
    s32 c;
    s32 s;
    s32 dx;
    s16 angle;

    scarecrow = self - (self->params + 1);
    part = &D_us_80181954[self->params];
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801806A0);
        self->drawFlags |= ENTITY_ROTATE;
        self->animCurFrame = part->animCurFrame;
        self->zPriority = scarecrow->zPriority + part->zOffset;
        self->ext.rlibScarecrowPart.lastVelocityX = scarecrow->velocityX;
        self->ext.rlibScarecrowPart.lastVelocityY = scarecrow->velocityY;
        // fallthrough
    case 1:
        self->posX.i.hi = scarecrow->posX.i.hi;
        self->posY.i.hi = scarecrow->posY.i.hi;
        self->facingLeft = scarecrow->facingLeft;
        self->palette = scarecrow->palette;
        offX = part->offsetX;
        angle = scarecrow->rotate;
        offY = part->offsetY;
        s = rsin(angle);
        c = rcos(angle);
        dx = (offX * c - offY * s) >> 12;
        if (self->facingLeft) {
            self->posX.i.hi -= dx;
        } else {
            self->posX.i.hi += dx;
        }
        self->posY.i.hi += (offX * s + offY * c) >> 12;
        dx = scarecrow->velocityX - self->ext.rlibScarecrowPart.lastVelocityX;
        if (self->facingLeft) {
            dx = -dx;
        }
        self->rotate += (dx >> 8) + ((self->ext.rlibScarecrowPart.lastVelocityY -
                                      scarecrow->velocityY) >>
                                     8);
        func_us_801AF3C8(&self->rotate, 0, 0x10);
        if (self->rotate > part->rotateMax) {
            self->rotate = part->rotateMax;
        }
        if (self->rotate < part->rotateMin) {
            self->rotate = part->rotateMin;
        }
        self->ext.rlibScarecrowPart.lastVelocityX = scarecrow->velocityX;
        self->ext.rlibScarecrowPart.lastVelocityY = scarecrow->velocityY;
        if (scarecrow->entityId != E_UNK_24) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern EInit g_EInitSchmoo;
extern AnimationFrame D_us_8018198C;
extern AnimationFrame D_us_80181998;

void func_us_801AFC88(Entity* self) {
    Entity* entity;
    s32 sideToPlayer;
    s32 distanceToPlayer;

    if (self->flags & FLAG_DEAD) {
        PlaySfxPositional(SFX_EXPLODE_B);
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
        InitializeEntity(g_EInitSchmoo);
        self->drawFlags = ENTITY_ROTATE;
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        /* fall through */
    case 1:
        AnimateEntity(&D_us_8018198C, self);
        MoveEntity();
        sideToPlayer = GetSideToPlayer();

        if (self->facingLeft) {
            self->velocityX += FIX(0.046875);
            if (self->velocityX > FIX(3)) {
                self->velocityX = FIX(3);
            }
        } else {
            self->velocityX -= FIX(0.046875);
            if (self->velocityX < FIX(-3)) {
                self->velocityX = FIX(-3);
            }
        }

        if (sideToPlayer & 2) {
            self->velocityY -= FIX(0.046875);
            if (self->velocityY < FIX(-1.5)) {
                self->velocityY = FIX(-1.5);
            }
        } else {
            self->velocityY += FIX(0.046875);
            if (self->velocityY > FIX(1.5)) {
                self->velocityY = FIX(1.5);
            }
        }

        sideToPlayer = (sideToPlayer & 1) ^ 1;
        distanceToPlayer = GetDistanceToPlayerX();
        if (sideToPlayer != self->facingLeft && distanceToPlayer >= 0x59) {
            SetStep(2);
        }
        break;

    case 2:
        MoveEntity();
        switch (self->step_s) {
        case 0:
            self->velocityX -= self->velocityX >> 4;
            self->velocityY -= self->velocityY >> 4;
            if (abs(self->velocityX) < FIX(0.25)) {
                self->step_s++;
            }
            break;

        case 1:
            self->velocityX -= self->velocityX >> 8;
            self->velocityY -= self->velocityY >> 8;
            if (AnimateEntity(&D_us_80181998, self) == 0) {
                SetStep(1);
            }
            if (!self->poseTimer && self->pose == 2) {
                self->facingLeft ^= 1;
            }
            break;
        }
        break;
    }

    self->rotate = -abs(self->velocityX >> 8);
}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s16 D_us_80181750[8] = { 0, 0x1E, 0, 4, 0x18, -4, -0x30, 0 };
s16 D_us_80181760[4] = { 0, 0x1E, 0x18, 0 };
u8 D_us_80181768[0x10] = { 0xA, 1, 7, 2, 6, 3, 9, 4, 6, 3, 7, 2, 0, 0, 0, 0 };
u8 D_us_80181778[0x30] = {
    0x1C,
    1,
    8,
    5,
    8,
    6,
    8,
    7,
    8,
    8,
    0x1F,
    7,
    3,
    9,
    3,
    0xA,
    2,
    0xB,
    2,
    0xC,
    2,
    0xD,
    1,
    0xE,
    2,
    0xF,
    1,
    0x10,
    2,
    0x11,
    8,
    0x12,
    8,
    0x13,
    8,
    0xA,
    8,
    9,
    8,
    6,
    8,
    5,
    8,
    1,
    0,
    0,
    0,
    0,
};
u8 D_us_801817A8[0x18] = {
    0x28,
    1,
    5,
    5,
    5,
    6,
    5,
    7,
    5,
    0x14,
    7,
    0x17,
    7,
    0x18,
    7,
    0x19,
    7,
    0x1A,
    0x31,
    0x1B,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801817C0[0xC] = { 0x10, 1, 5, 0x15, 0xD, 0x16, 7, 0x15, 7, 1, 0, 0 };
s32 D_us_801817CC[6] = { 0x200, 0x1C0, 0x100, 0x180, 0x300, 0x380 };
s16 D_us_80181820[8] = { 0, 0x22, 0, 4, 0x18, -4, -0x30, 0 };
s16 D_us_80181830[4] = { 0, 0x22, 0x18, 0 };
u8 D_us_80181838[0xC] = { 3, 1, 3, 2, 3, 3, 3, 4, 0, 0, 0, 0 };
u8 D_us_80181844[0x3C] = {
    0x20,
    1,
    4,
    6,
    3,
    7,
    2,
    8,
    2,
    9,
    8,
    0xA,
    2,
    0xB,
    2,
    0xC,
    2,
    0xD,
    2,
    0xE,
    2,
    0xF,
    2,
    0x10,
    2,
    0x11,
    2,
    0x12,
    8,
    0x11,
    2,
    0x13,
    2,
    0x15,
    2,
    0x14,
    8,
    0x15,
    3,
    0x16,
    3,
    0x17,
    3,
    0x18,
    3,
    0x19,
    3,
    0x1A,
    3,
    0x1B,
    3,
    0x1C,
    3,
    0x1D,
    3,
    0x1E,
    0x20,
    0x15,
    0xFF,
    0,
};
u8 D_us_80181880[8] = { 2, 0x15, 2, 0x1F, 0, 0, 0, 0 };
AnimationFrame D_us_80181888 = { 0x2003, 0x2103 };
AnimationFrame D_us_8018189C = { 0x2A02, 0x2B02 };
u8 D_us_801818A8[0x10] = { 2, 0x2F, 2, 0x30, 2, 0x31, 2, 0x32, 2, 0x33, 2, 0x34, 0, 0, 0, 0 };
u8 D_us_801818B8[0x18] = {
    0x11,
    0x35,
    3,
    0x36,
    3,
    0x37,
    3,
    0x38,
    3,
    0x39,
    0x11,
    0x3A,
    3,
    0x39,
    3,
    0x38,
    3,
    0x37,
    3,
    0x36,
    0xFF,
    0,
    0,
    0,
};
RlibTinmanPiece D_us_801818D0[0xB] = {
    { 0x34, 0, -0x18000, 0 },
    { 0x1C, 1, 0xC000, -0x14000 },
    { 0x10, 1, 0x10000, -0x40000 },
    { 0x18, 1, 0x8000, -0x18000 },
    { 0x30, 1, 0x6000, -0x10000 },
    { 0x20, 1, 0xE000, -0x10000 },
    { 0x28, 0, 0x30000, 0 },
    { 0x28, 1, 0, -0x30000 },
    { 0x34, 0, -0xC000, 0 },
    { 8, 1, 0x8000, -0x20000 },
    { 4, 1, -0x4000, -0x10000 },
};
RlibScarecrowPart D_us_80181954[4] = {
    { 3, -1, 5, -0xB, -0x400, 0x180 },
    { 4, -1, -9, -6, -0xFFF, 0xFFF },
    { 5, 1, -9, -6, -0xFFF, 0xFFF },
    { 6, 1, 5, -0xB, -0x400, 0x100 },
};
s16 D_us_80181984[4] = { 0, 0x30, 1, 0 };
AnimationFrame D_us_8018198C = { 0x805, 0x905 };
AnimationFrame D_us_80181998 = { 0xC08, 0xD08 };
s8 g_HarpyKickHitboxes[0x20] = {
    0,
    0,
    0,
    0,
    -0x3D,
    0x9E,
    0,
    0,
    0x14,
    -0x15,
    0x10,
    0x1F,
    0x19,
    -4,
    0xF,
    0xE,
    0x35,
    -1,
    0x21,
    0x1A,
    0x52,
    -9,
    4,
    0x11,
    0x2D,
    -0xA,
    0x21,
    0x1A,
    0x4A,
    -3,
    4,
    0x11,
};
u8 g_HarpyKickHitboxIdx[0x1C] = {
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
    2,
    3,
    4,
    5,
    6,
    7,
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
};
// end pc_import_data.py
// clang-format on
#endif
