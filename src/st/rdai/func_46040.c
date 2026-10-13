// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

extern EInit D_us_80180920;
extern s16 D_us_80181AC0[];
extern s16 D_us_80181ACC[];
extern s16 D_us_80181ADC[];
extern u8 D_us_80181AE4[];
extern u8 D_us_80181AF8[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/func_46040", func_us_801C6040);
#else
u8 CheckColliderOffsets(s16* arg0, u8 facing);
void UnkPrimHelper(Primitive* prim);
void func_us_801C4C4C(void);
void func_us_801C5AA0(void);

// The Black Panther. It waits until Alucard is near, then runs at him,
// leaping over gaps and turning back at walls. Attacked from the front, it
// sinks into its shadow, slides toward him and rises again behind it.
void func_us_801C6040(Entity* self) {
    Collider col;
    Entity* other;
    Primitive* prim;
    s16 x;

    if ((self->flags & FLAG_DEAD) && self->step < 16) {
        self->hitboxState = 0;
        SetStep(16);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180920);
        self->hitboxOffX = 2;
        self->hitboxOffY = -4;
        self->animCurFrame = 1;
        // fallthrough
    case 1:
        if (UnkCollisionFunc3(D_us_80181ACC) & 1) {
            self->step++;
        }
        break;

    case 2:
        self->animCurFrame = 1;
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        if (GetDistanceToPlayerX() < 0x70) {
            self->step++;
        }
        break;

    case 3:
        // runs
        UnkCollisionFunc2(D_us_80181ADC);
        self->velocityX = self->facingLeft ? FIX(2.5) : FIX(-2.5);
        if (!AnimateEntity(D_us_80181AE4, self)) {
            PlaySfxPositional(SFX_STOMP_HARD_D);
            if (CheckColliderOffsets(D_us_80181AC0, self->facingLeft ^ 1) !=
                1) {
                SetStep(4);
            }
        }
        if (PLAYER.facingLeft != (GetSideToPlayer() & 1) &&
            GetDistanceToPlayerX() < 0x50 &&
            (g_Player.status & (PLAYER_STATUS_UNK400 | PLAYER_STATUS_SUBWPN |
                                PLAYER_STATUS_SPELLCAST))) {
            SetStep(7);
        }
        if (self->facingLeft) {
            x = self->posX.i.hi + 0x18;
        } else {
            x = self->posX.i.hi - 0x18;
        }
        g_api.CheckCollision(x, self->posY.i.hi, &col, 0);
        if (col.effects) {
            SetStep(6);
        }
        break;

    case 4:
        // leaps over a gap
        switch (self->step_s) {
        case 0:
            self->velocityX = self->facingLeft ? FIX(2) : FIX(-2);
            self->velocityY = FIX(-3.5);
            self->step_s++;
            break;
        case 1:
            AnimateEntity(D_us_80181AF8, self);
            if (UnkCollisionFunc3(D_us_80181ACC) & 1) {
                self->step_s++;
            }
            if (self->facingLeft) {
                x = self->posX.i.hi + 0x18;
            } else {
                x = self->posX.i.hi - 0x18;
            }
            g_api.CheckCollision(x, self->posY.i.hi, &col, 0);
            if (col.effects) {
                SetStep(6);
            }
            break;
        case 2:
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            SetStep(3);
            break;
        }
        break;

    case 5:
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 9;
            self->velocityX = 0;
            self->velocityY = FIX(-4);
            self->step_s++;
            break;
        case 1:
            if (UnkCollisionFunc3(D_us_80181ACC) & 1) {
                self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
                SetStep(3);
            }
            break;
        }
        break;

    case 6:
        // bounces back off a wall
        if (!self->step_s) {
            self->animCurFrame = 0xB;
            self->velocityX = self->facingLeft ? FIX(-2.5) : FIX(2.5);
            self->velocityY = FIX(-3);
            self->rotate = 0;
            self->drawFlags |= ENTITY_ROTATE;
            self->step_s++;
        }
        self->rotate += 0x100;
        if (UnkCollisionFunc3(D_us_80181ACC) & 1) {
            self->drawFlags &= ~ENTITY_ROTATE;
            SetStep(3);
        } else {
            self->velocityY -= FIX(1.0 / 16);
        }
        break;

    case 7:
        func_us_801C4C4C();
        if (self->step_s == 4) {
            SetStep(8);
        }
        break;

    case 8:
        // slides along the floor as a shadow
        if (!self->step_s) {
            self->velocityX = self->facingLeft ? FIX(4) : FIX(-4);
            self->ext.blackPanther.timer = 0x30;
            self->step_s++;
            break;
        }
        UnkCollisionFunc2(D_us_80181ADC);
        prim = self->ext.blackPanther.shadow;
        if (self->facingLeft) {
            prim->next->x1 = self->posX.i.hi - 0x12;
        } else {
            prim->next->x1 = self->posX.i.hi + 0x12;
        }
        prim->next->y0 = self->posY.i.hi + 2;
        UnkPrimHelper(prim);
        if (!--self->ext.blackPanther.timer) {
            SetStep(9);
        }
        break;

    case 9:
        func_us_801C5AA0();
        if (self->step_s == 7) {
            SetStep(3);
        }
        break;

    case 16:
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 0xB;
            self->rotate = 0;
            self->drawFlags |= ENTITY_ROTATE;
            self->velocityX = self->facingLeft ? FIX(2) : FIX(-2);
            self->velocityY = FIX(-3.5);
            self->step_s++;
            break;
        case 1:
            self->rotate -= 0x80;
            if (UnkCollisionFunc3(D_us_80181ACC) & 1) {
                self->step_s++;
            }
            break;
        case 2:
            other = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (other != NULL) {
                CreateEntityFromEntity(E_EXPLOSION, self, other);
                other->params = 2;
            }
            PlaySfxPositional(SFX_EXPLODE_SMALL);
            DestroyEntity(self);
            return;
        }
        break;

    case 32:
#include "../pad2_anim_debug.h"
    }

    if (self->animCurFrame == 1) {
        self->hitboxOffX = 9;
        self->hitboxOffY = 2;
        self->hitboxWidth = 0x11;
        self->hitboxHeight = 4;
    }
    if (self->animCurFrame == 2) {
        self->hitboxOffX = 5;
        self->hitboxOffY = -5;
        self->hitboxWidth = 0x10;
        self->hitboxHeight = 0xA;
    }
    if (self->animCurFrame >= 3) {
        self->hitboxOffX = 4;
        self->hitboxOffY = -0xC;
        self->hitboxWidth = 0x17;
        self->hitboxHeight = 6;
    }
    if (self->animCurFrame >= 6) {
        self->hitboxOffX = -2;
        self->hitboxOffY = -4;
        self->hitboxWidth = 0xB;
        self->hitboxHeight = 0xB;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
EInit D_us_80180920 = { 0x8009, 0, 0x4A, 0x238, 0x25 };
s16 D_us_80181AC0[6] = { -0x40, 0, -0x40, 0x10, 0xFF, 0 };
s16 D_us_80181ACC[8] = { 0, 8, 0, 4, 8, -4, -0x10, 0 };
s16 D_us_80181ADC[4] = { 0, 8, 0x10, 0 };
u8 D_us_80181AE4[0x14] = { 6, 2, 3, 3, 5, 4, 3, 3, 3, 6, 3, 7, 2, 8, 2, 9, 2, 0xA, 0, 0 };
u8 D_us_80181AF8[0xC] = { 3, 2, 2, 3, 3, 4, 4, 5, 2, 3, 0xFF, 0 };
// end pc_import_data.py
// clang-format on
#endif
