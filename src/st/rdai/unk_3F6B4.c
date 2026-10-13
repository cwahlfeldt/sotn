// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#include "../e_spear_guard_collision.h"

#include "../approach_s16.h"

extern EInit g_EInitRdaiUnk1F;
extern u8 D_us_801818B4[];
extern u8 D_us_801818BC[];
extern u8 D_us_801818C8[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_3F6B4", func_us_801BF830);
#else
// Rolls back and forth carrying the archer (0x1C, four slots on) and its
// three parts (0x1F); breaks apart once the archer is gone
void func_us_801BF830(Entity* self) {
    Entity* entity;
    u8* anim;
    s32 animating;
    s32 i;

    if (self->step && self[4].entityId != E_UNK_1C && self->step != 3) {
        SetStep(3);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitRdaiUnk1F);
        entity = self + 1;
        for (i = 0; i < 3; i++, entity++) {
            CreateEntityFromEntity(E_UNK_1F, self, entity);
            entity->params = i;
        }
        CreateEntityFromEntity(E_UNK_1C, self, self + 4);
        SetStep(1);
        break;
    case 1:
        if (!self->step_s) {
            self->ext.rdaiArcher.timer = 0x80;
            self->step_s++;
        }
        if (!self->ext.rdaiArcher.movingLeft) {
            self->velocityX += FIX(1.0 / 64);
            if (self->velocityX > FIX(0.75)) {
                self->velocityX = FIX(0.75);
            }
        } else {
            self->velocityX -= FIX(1.0 / 64);
            if (self->velocityX < FIX(-0.75)) {
                self->velocityX = FIX(-0.75);
            }
        }
        AnimateEntity(D_us_801818B4, self);
        SpearGuardCheckCollision();
        if (!(g_Timer & 7)) {
            PlaySfxPositional(SFX_UNK_RDAI_76A);
        }
        if (!--self->ext.rdaiArcher.timer) {
            SetStep(2);
        }
        break;
    case 2:
        switch (self->step_s) {
        case 0:
            SpearGuardCheckCollision();
            self->velocityX -= self->velocityX >> 8;
            if (abs(self->velocityX) < FIX(0.5)) {
                self->velocityX = 0;
                self->ext.rdaiArcher.movingLeft ^= 1;
                self->step_s++;
            }
            break;
        case 2:
            if (!self->ext.rdaiArcher.movingLeft) {
                self->velocityX += FIX(1.0 / 64);
                if (self->velocityX > FIX(0.75)) {
                    self->velocityX = FIX(0.75);
                }
            } else {
                self->velocityX -= FIX(1.0 / 64);
                if (self->velocityX < FIX(-0.75)) {
                    self->velocityX = FIX(-0.75);
                }
            }
            // fallthrough
        case 1:
            SpearGuardCheckCollision();
            anim = D_us_801818BC;
            if (!self->ext.rdaiArcher.movingLeft) {
                anim = D_us_801818C8;
            }
            animating = AnimateEntity(anim, self);
            if (self->pose >= 3) {
                self->step_s = 2;
            }
            if (!animating) {
                SetStep(1);
            }
            break;
        }
        break;
    case 3:
        PlaySfxPositional(SFX_FLEA_ARMOR_EXPLODE);
        for (i = 0; i < 9; i++) {
            if (i == 7) {
                continue;
            }
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_21, self, entity);
                entity->params = i + 1;
            }
        }
        PreventEntityFromRespawning(self);
        DestroyEntity(self);
        return;
    case 0xFF:
#include "../pad2_anim_debug.h"
    }
}
#endif

// Child parts share one implementation; only the fixed EInit address is local.
#include "../e_rdai_unk1f.h"

#include "../e_rdai_archer.h"

extern u8 D_us_801818D4[];
extern u8 D_us_801818DC[];
extern u8 D_us_801818E4[];
extern u8 D_us_801818F4[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_3F6B4", func_us_801C0240);
#else
// The archer's parts, params slots after it: 1 the bow arm, which shoots
// the arrows (0x20); 2 and 3 follow its pose
void func_us_801C0240(Entity* self) {
    Entity* archer;
    Entity* arrow;
    u8 mode;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitRdaiUnk1F);
        self->drawFlags = ENTITY_ROTATE;
        self->hitboxState = 0;
        self->step = self->params;
        if (self->params == 1) {
            arrow = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (arrow != NULL) {
                CreateEntityFromEntity(E_UNK_20, self, arrow);
                arrow->ext.rdaiArrow.arm = self;
                arrow->zPriority = self->zPriority - 1;
            }
        }
        return;
    case 1:
        archer = self - 1;
        self->posX.i.hi = archer->posX.i.hi;
        self->posY.i.hi = archer->posY.i.hi;
        self->rotate = archer->rotate;
        mode = self->ext.rdaiArcher.mode;
        self->zPriority = archer->zPriority + 3;
        if (mode == 1) {
            AnimateEntity(D_us_801818DC, self);
            if (!self->poseTimer && self->pose == 1) {
                PlaySfxPositional(SFX_BONE_CREAK);
            }
        } else if (mode == 2) {
            self->animCurFrame = 0x1B;
            self->pose = 0;
            self->poseTimer = 0;
        } else if (mode == 3) {
            AnimateEntity(D_us_801818F4, self);
            if (!self->poseTimer && self->pose == 6) {
                arrow = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (arrow != NULL) {
                    CreateEntityFromEntity(E_UNK_20, self, arrow);
                    arrow->ext.rdaiArrow.arm = self;
                    arrow->zPriority = self->zPriority - 1;
                }
            }
        } else {
            self->animCurFrame = 0x19;
            self->pose = 0;
            self->poseTimer = 0;
        }
        break;
    case 2:
        archer = self - 2;
        self->animCurFrame = 0x1C;
        self->posX.i.hi = archer->posX.i.hi;
        self->zPriority = archer->zPriority - 1;
        self->rotate = archer->rotate;
        self->posY.i.hi = archer->posY.i.hi;
        if (archer->entityId != E_UNK_1C) {
            DestroyEntity(self);
        }
        return;
    case 3:
        archer = self - 3;
        mode = self->ext.rdaiArcher.mode;
        self->posX.i.hi = archer->posX.i.hi;
        self->posY.i.hi = archer->posY.i.hi;
        self->zPriority = archer->zPriority + 1;
        self->rotate = archer->rotate;
        if (mode == 2) {
            AnimateEntity(D_us_801818E4, self);
        } else if (mode == 1) {
            AnimateEntity(D_us_801818D4, self);
            if (archer->ext.rdaiArcher.mode == 2) {
                self->pose = 0;
                self->poseTimer = 0;
                self->ext.rdaiArcher.mode = 2;
            }
        } else {
            self->animCurFrame = 0xB;
            self->pose = 0;
            self->poseTimer = 0;
        }
        break;
    default:
        return;
    }
    self->ext.rdaiArcher.mode = archer->ext.rdaiArcher.mode;
    if (archer->entityId != E_UNK_1C) {
        DestroyEntity(self);
    }
}
#endif

extern EInit D_us_80180884;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_3F6B4", func_us_801C0528);
#else
// The archer's arrow: rides the bow arm until it shoots (mode 2), then
// flies; sticks to Alucard for a moment when it hits him. params 1 and 2
// are the halves of a broken arrow.
void func_us_801C0528(Entity* self) {
    Entity* arm;
    Entity* half;
    s32 length;
    s16 angle;
    s32 i;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180884);
        self->animCurFrame = 0x16;
        self->hitboxState = 0;
        self->drawFlags |= ENTITY_ROTATE;
        if (self->params) {
            self->animCurFrame += self->params;
            self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA |
                           FLAG_DESTROY_IF_BARELY_OUT_OF_CAMERA |
                           FLAG_UNK_00200000 | FLAG_UNK_2000;
            if (self->params == 1) {
                self->velocityX = FIX(0.5);
            } else {
                self->velocityX = FIX(-0.5);
            }
            self->step = 4;
            break;
        }
        // fallthrough
    case 1:
        arm = self->ext.rdaiArrow.arm;
        self->posX.i.hi = arm->posX.i.hi;
        self->posY.i.hi = arm->posY.i.hi;
        angle = arm->rotate;
        self->rotate = angle;
        length = 0x18;
        if (arm->animCurFrame == 0x1A) {
            length = 0xE;
        }
        if (arm->animCurFrame == 0x1B) {
            length = 8;
        }
        self->posX.i.hi += (length * rcos(angle)) >> 12;
        self->posY.i.hi += (length * rsin(angle)) >> 12;
        if (arm->entityId != E_UNK_1D) {
            DestroyEntity(self);
            return;
        }
        if (arm->ext.rdaiArcher.mode == 2) {
            self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
            self->velocityX = (rcos(angle) * FIX(5)) >> 12;
            self->velocityY = (rsin(angle) * FIX(5)) >> 12;
            self->hitboxState = 3;
            self->hitboxOffX = rcos(angle) >> 9;
            self->hitboxOffY = rsin(angle) >> 9;
            self->hitboxWidth = self->hitboxHeight = 4;
            self->step = 2;
        }
        break;
    case 2:
        MoveEntity();
        if (self->flags & FLAG_DEAD) {
            for (i = 0; i < 2; i++) {
                half = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (half != NULL) {
                    CreateEntityFromEntity(E_UNK_20, self, half);
                    half->params = i + 1;
                    half->rotate = self->rotate;
                }
            }
            DestroyEntity(self);
            return;
        }
        if (self->hitFlags & 0x80) {
            self->ext.rdaiArrow.offsetX = PLAYER.posX.i.hi - self->posX.i.hi;
            self->ext.rdaiArrow.timer = 0x20;
            self->hitboxState = 0;
            self->step++;
            self->ext.rdaiArrow.offsetY = PLAYER.posY.i.hi - self->posY.i.hi;
        }
        break;
    case 3:
        self->posX.i.hi = PLAYER.posX.i.hi - self->ext.rdaiArrow.offsetX;
        self->posY.i.hi = PLAYER.posY.i.hi - self->ext.rdaiArrow.offsetY;
        if (!--self->ext.rdaiArrow.timer) {
            DestroyEntity(self);
        }
        break;
    case 4:
        MoveEntity();
        self->velocityY += FIX(1.0 / 8);
        if (self->params == 1) {
            self->rotate += 0x20;
        } else {
            self->rotate -= 0x20;
        }
        break;
    }
}
#endif

extern EInit D_us_80180890;
extern s16* sprites_rdai_2[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_3F6B4", func_us_801C0898);
#else
// A piece of the broken archer and its carrier; animCurFrame is params.
// It flies away from the centre of its sprite (sprite bank 2, which PSX
// names D_us_801ADD28) and explodes. 19-21 are the large pieces.
void func_us_801C0898(Entity* self) {
    SpritePart* part;
    Entity* explosion;
    s16 x;
    s16 y;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180890);
        self->animCurFrame = self->params;
        part = ((SpriteParts*)sprites_rdai_2[self->params])->parts;
        x = part->offsetx + part->width / 2;
        y = part->offsety + part->height / 2;
        self->ext.rdaiArrow.offsetX = x;
        self->ext.rdaiArrow.offsetY = y;
        self->ext.rdaiArrow.timer = (Random() & 0xF) + 8;
        angle = ratan2(y, x);
        self->velocityX = (rcos(angle) << 17) >> 12;
        self->velocityY = (rsin(angle) << 17) >> 12;
        explosion = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (explosion != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, explosion);
            explosion->params = 1;
            explosion->posX.i.hi += x;
            explosion->posY.i.hi += y;
        }
        if (self->params >= 0x13) {
            self->step = 2;
        }
        break;
    case 1:
        MoveEntity();
        self->velocityY += FIX(1.0 / 8);
        if (!--self->ext.rdaiArrow.timer) {
            PlaySfxPositional(SFX_EXPLODE_D);
            goto explode;
        }
        break;
    case 2:
        switch (self->params) {
        case 19:
            if (!self->step_s) {
                self->velocityY = FIX(-2.5);
                self->ext.rdaiArrow.timer = 0x10;
                self->step_s++;
            }
            MoveEntity();
            self->velocityY += FIX(1.0 / 8);
            if (!--self->ext.rdaiArrow.timer) {
                goto explode;
            }
            break;
        case 20:
            if (!self->step_s) {
                self->ext.rdaiArrow.timer = 0x18;
                self->velocityX = 0;
                self->velocityY = 0;
                self->step_s++;
            }
            MoveEntity();
            self->velocityY += FIX(1.0 / 32);
            if (!--self->ext.rdaiArrow.timer) {
                goto explode;
            }
            break;
        case 21:
            if (!self->step_s) {
                self->ext.rdaiArrow.timer = 0x40;
                self->velocityY = 0;
            }
            MoveEntity();
            self->velocityY += FIX(0.1875);
            break;
        }
        break;
    }
    return;

explode:
    explosion = AllocEntity(&g_Entities[224], &g_Entities[256]);
    if (explosion != NULL) {
        CreateEntityFromEntity(E_EXPLOSION, self, explosion);
        explosion->posX.i.hi += self->ext.rdaiArrow.offsetX;
        explosion->params = 1;
        explosion->posY.i.hi += self->ext.rdaiArrow.offsetY;
    }
    DestroyEntity(self);
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s16 g_RdaiArcherOffsets[0x4D][2] = {
    { 3, -4 },
    { 4, -5 },
    { -5, 4 },
    { -9, 6 },
    { 0x100, 0 },
    { 1, 0 },
    { 0x804, 0x704 },
    { 0x610, 0xFF },
    { 0x80C, 0x70C },
    { 0x620, 0xFF },
    { 0x620, 0x710 },
    { 0x810, 0xFF },
    { 0x102, 0x204 },
    { 0x302, 0x401 },
    { 0x502, 0x601 },
    { 0x701, 0x802 },
    { 0x902, 0x804 },
    { 0xA02, 0xB06 },
    { 0xC02, 0x601 },
    { 0x501, 0x401 },
    { 0x301, 0x201 },
    { 0, 0 },
    { 0x201, 0x501 },
    { 0x801, 0xB01 },
    { 0x501, 0x101 },
    { 0x201, 0x301 },
    { 0x401, 0x501 },
    { 0x601, 0x701 },
    { 0x801, 0x901 },
    { 0x802, 0xA01 },
    { 0xB02, 0xC01 },
    { 0x601, 0x501 },
    { 0x401, 0x301 },
    { 0x201, 0 },
    { 0xD21, 0xE11 },
    { 0xF01, 0x1001 },
    { 0xF01, 0x1001 },
    { 0xF02, 0x1002 },
    { 0xF03, 0x1014 },
    { 0xFF, 0 },
    { 0x1202, 0x1304 },
    { 0x1401, 0x1501 },
    { 0x1602, 0x1701 },
    { 0x1801, 0x1901 },
    { 0x1A01, 0x1B01 },
    { 0x1C01, 0x1D01 },
    { 0x1E01, 0x1F01 },
    { 0x2001, 0x2104 },
    { 0x2201, 0x2301 },
    { 0x1D01, 0x1C01 },
    { 0x1B01, 0x1901 },
    { 0x1701, 0x1601 },
    { 0x1501, 0x1401 },
    { 0x1301, 0 },
    { 0x2402, 0x2504 },
    { 0x2601, 0x2701 },
    { 0x2802, 0x2901 },
    { 0x2A01, 0x2B01 },
    { 0x2C01, 0x2D01 },
    { 0x2E01, 0x2F01 },
    { 0x3001, 0x3101 },
    { 0x3201, 0x3304 },
    { 0x3401, 0x3501 },
    { 0x2F01, 0x2E01 },
    { 0x2D01, 0x2B01 },
    { 0x2901, 0x2801 },
    { 0x2701, 0x2601 },
    { 0x2501, 0 },
    { 0x3608, 0x3708 },
    { 0, 0 },
    { 0x3B01, 0x3F01 },
    { 0x4001, 0x3C01 },
    { 0x3F01, 0x4001 },
    { 0x3D01, 0x3F01 },
    { 0x4001, 0x3E01 },
    { 0x3F01, 0x4001 },
    { 0, 0 },
};
u8 D_us_801818B4[8] = { 8, 0x1E, 8, 0x1F, 8, 0x20, 0, 0 };
u8 D_us_801818BC[0xC] = { 1, 0x1F, 2, 0x21, 0x10, 0x22, 6, 0x21, 1, 0x1F, 0xFF, 0 };
u8 D_us_801818C8[0xC] = { 1, 0x1F, 2, 0x23, 0x10, 0x24, 6, 0x23, 1, 0x1F, 0xFF, 0 };
u8 D_us_801818D4[8] = { 0x16, 0xB, 8, 0xC, 0x10, 0xD, 0xFF, 0 };
u8 D_us_801818DC[8] = { 0x16, 0x19, 8, 0x1A, 0x10, 0x1B, 0xFF, 0 };
u8 D_us_801818E4[0x10] = { 0xF, 0xD, 5, 0xC, 5, 0xE, 5, 0xF, 2, 0xE, 2, 0xF, 2, 0xB, 0xFF, 0 };
u8 D_us_801818F4[0x10] = {
    0x16,
    0x19,
    6,
    0x10,
    6,
    0x11,
    0x16,
    0x12,
    6,
    0x11,
    6,
    0x10,
    0x16,
    0x19,
    0xFF,
    0,
};
// end pc_import_data.py
// clang-format on
#endif
