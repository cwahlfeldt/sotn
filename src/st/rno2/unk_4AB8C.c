// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rno2.h"

extern EInit g_EInitGhostDancer;
extern s16 D_us_801822A8[];
extern s16 D_us_801822B8[];
extern u8 D_us_801822C0[];
extern u8 D_us_801822D0[];
extern u8 D_us_801822DC[];
extern s16 D_us_801822E4[][4];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno2/nonmatchings/unk_4AB8C", EntityGhostDancer);
#else
// A dancer that walks back and forth and leaps at Alucard, spinning; it
// bursts into six pieces (params 0x100 + i) when it dies
void EntityGhostDancer(Entity* self) {
    Entity* entity;
    s16* piece;
    s32 i;
    s32 collision;

    if (self->flags & FLAG_DEAD) {
        for (i = 0; i < 6; i++) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                piece = D_us_801822E4[i];
                CreateEntityFromEntity(E_GHOST_DANCER, self, entity);
                entity->params = i + 0x100;
                entity->facingLeft = self->facingLeft;
                if (self->facingLeft) {
                    entity->posX.i.hi -= *piece++;
                } else {
                    entity->posX.i.hi += *piece++;
                }
                entity->posY.i.hi += *piece++;
                entity->ext.ghostDancer.spin = piece[0];
                entity->ext.ghostDancer.timer = piece[1];
                entity->velocityY = -((Random() & 0x1F) << 12);
                entity->velocityX = (Random() & 0x3F) * 0xC00;
                if (!(GetSideToPlayer() & 1)) {
                    entity->velocityX = -entity->velocityX;
                }
            }
        }
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, entity);
            entity->params = 2;
        }
        PlaySfxPositional(SFX_SKELETON_DEATH_B);
        DestroyEntity(self);
        return;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitGhostDancer);
        self->hitboxOffY = 2;
        if (Random() & 1) {
            self->palette++;
        }
        if (self->params & 0x100) {
            self->step = 6;
            self->hitboxState = 0;
            self->drawFlags |= ENTITY_ROTATE;
            self->animCurFrame = (self->params & 0xF) + 0x10;
            self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA | FLAG_UNK_00200000 |
                           FLAG_UNK_2000;
            return;
        }
        self->animCurFrame = 1;
        self->ext.ghostDancer.timer = 0;
        break;

    case 1:
        if (UnkCollisionFunc3(D_us_801822A8) & 1) {
            self->step++;
        }
        break;

    case 2:
        if (self->facingLeft) {
            self->velocityX = FIX(1.5);
        } else {
            self->velocityX = FIX(-1.5);
        }
        collision = UnkCollisionFunc2(D_us_801822B8);
        if (collision == 0x80) {
            SetStep(4);
        }
        if (collision == 0xFF) {
            SetStep(3);
        }
        if (!AnimateEntity(D_us_801822C0, self) &&
            self->facingLeft == (GetSideToPlayer() & 1)) {
            SetStep(3);
        }
        if (self->ext.ghostDancer.timer) {
            self->ext.ghostDancer.timer--;
        } else if (self->facingLeft != (GetSideToPlayer() & 1) &&
                   GetDistanceToPlayerX() < 0x40) {
            SetStep(4);
        }
        break;

    case 3:
        if (!AnimateEntity(D_us_801822D0, self)) {
            self->animCurFrame = 1;
            self->facingLeft ^= 1;
            SetStep(2);
        }
        break;

    case 4:
        switch (self->step_s) {
        case 0:
            self->hitboxWidth = 8;
            self->hitboxHeight = 15;
            self->hitboxOffX = -2;
            self->hitboxOffY = 13;
            if (!AnimateEntity(D_us_801822DC, self)) {
                self->step_s++;
            }
            break;
        case 1:
            if (self->facingLeft) {
                self->velocityX = FIX(2.0);
            } else {
                self->velocityX = FIX(-2.0);
            }
            self->velocityY = FIX(-6.5);
            self->animCurFrame = 0xF;
            self->rotate = ROT(45);
            self->hitboxOffY = -8;
            self->drawFlags |= ENTITY_ROTATE;
            self->step_s++;
            // fallthrough
        case 2:
            self->rotate -= 0x10;
            if (UnkCollisionFunc3(D_us_801822A8) & 1) {
                PlaySfxPositional(SFX_STOMP_HARD_C);
                self->hitboxOffY = 13;
                self->animCurFrame = 0xE;
                self->drawFlags = ENTITY_DEFAULT;
                self->ext.ghostDancer.timer = 16;
                self->step_s++;
            }
            break;
        case 3:
            if (!--self->ext.ghostDancer.timer) {
                self->hitboxWidth = 5;
                self->hitboxHeight = 25;
                self->hitboxOffY = 2;
                self->hitboxOffX = 0;
                self->ext.ghostDancer.timer = 32;
                SetStep(2);
            }
            break;
        }
        break;

    case 6:
        MoveEntity();
        self->velocityY += FIX(0.125);
        self->rotate += self->ext.ghostDancer.spin;
        if (!--self->ext.ghostDancer.timer) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_EXPLOSION, self, entity);
                entity->params = 0;
            }
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s16 D_us_801822A8[8] = { 0, 0x1D, 0, 4, 8, -4, -0x10, 0 };
s16 D_us_801822B8[4] = { 0, 0x1D, 6, 0 };
u8 D_us_801822C0[0x10] = { 4, 1, 4, 2, 4, 3, 5, 4, 5, 5, 5, 6, 5, 7, 0, 0 };
u8 D_us_801822D0[0xC] = { 3, 8, 3, 9, 3, 0xA, 3, 0xB, 0xFF, 0, 0, 0 };
u8 D_us_801822DC[8] = { 4, 0xC, 4, 0xD, 4, 0xE, 0xFF, 0 };
s16 D_us_801822E4[6][4] = {
    { 0, 0, 0x40, 0x20 },
    { 0, 0, 0x20, 0x12 },
    { 0, 0, 0x30, 0x18 },
    { 0, 0, 0x10, 0xC },
    { 0, 0, 0x40, 0x14 },
    { 0, 0, 8, 0x10 },
};
// end pc_import_data.py
// clang-format on
#endif
