// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo6.h"

#ifdef VERSION_PC
s32 PrimDecreaseBrightness(Primitive* prim, u8 amount);
#endif

// a claw's hitbox for each frame of the arm
typedef struct {
    s8 offX;
    s8 offY;
    u8 width;
    u8 height;
} DraculaClawHitbox;

extern u8* D_us_8018060C[];
extern u8 D_us_8018062C[];
extern u8 D_us_80180634[];
extern u16 D_us_8018063C[];
extern u8 D_us_8018064C[];

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", EntityBreakable);
#else
// RBO6's breakable tables have no names yet
#define g_eBreakableAnimations D_us_8018060C
#define g_eBreakableHitboxes D_us_8018062C
#define g_eBreakableExplosionTypes D_us_80180634
#define g_eBreakableanimSets D_us_8018063C
#define blend_modes D_us_8018064C
#include "../../st/e_breakable.h"
#undef g_eBreakableAnimations
#undef g_eBreakableHitboxes
#undef g_eBreakableExplosionTypes
#undef g_eBreakableanimSets
#undef blend_modes
#endif

// decompiled in src/boss/bo1/e_explosion_flame.c
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_8019D260_from_rcen);
#else
// BO1's, spawning RBO6's flame (0x16)
void func_us_8019D260_from_rcen(void) {
    s16 temp_s3;
    s32 i;
    s8 temp_s4;
    Entity* entity;

    temp_s4 = Random() & 3;
    temp_s3 = ((Random() & 0xF) << 8) - 0x800;
    for (i = 0; i < 6; i++) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_ID(UNK_16), g_CurrentEntity, entity);
            entity->ext.et_801A518C.unk89 = 6 - i;
            entity->ext.et_801A518C.unk88 = temp_s4;
            entity->params = 2;
            entity->ext.et_801A518C.unk84 = temp_s3;
            entity->zPriority = g_CurrentEntity->zPriority + 1;
        }
    }
}
#endif

void func_us_8019D330_from_rcen(Entity* self) {
    typedef struct {
        u16 animSet;
        u16 unk2;
        u16 palette;
        u8 blendMode;
        u8 unused;
#ifdef VERSION_PC
        u8* animation;
#else
        u32 animation;
#endif
    } Borbo6ParticleData;
    extern Borbo6ParticleData D_us_801806B4[];
    extern u16 g_EInitParticle;

    s16 temp_v1_3;
    s16 var_s0;
    s32 temp_a0;
    u16 temp_v1;
    u16 var_s0_2;
    Borbo6ParticleData* temp_v1_2;

    switch (self->step) {
    case 0:
        InitializeEntity(&g_EInitParticle);
        temp_a0 = self->params & 0xF;
        temp_v1_2 = &D_us_801806B4[temp_a0];
        self->palette = temp_v1_2->palette + 0x2E0;
        self->blendMode = temp_v1_2->blendMode;
        self->animSet = temp_v1_2->animSet;
        self->unk5A = temp_v1_2->unk2;
        self->ext.et_801A518C.anim = (u8*)temp_v1_2->animation;
        self->step = temp_a0 + 1;
        temp_v1 = self->params;
        {
            u32 temp_v0;
            temp_v0 = temp_v1 & 0xFF00;
            if (temp_v0 != 0) {
                self->zPriority = temp_v0 >> 8;
            }
        }
        if (self->params & 0xF0) {
            self->palette = PAL_FLAG(PAL_UNK_19F);
            self->blendMode = BLEND_TRANSP;
            self->facingLeft = 1;
        }
        return;

    case 1:
        if (AnimateEntity(self->ext.et_801A518C.anim, self) == 0) {
            DestroyEntity(self);
        }
        break;

    case 2:
        MoveEntity();
        self->velocityY = FIX(-1.0);
        if (AnimateEntity(self->ext.et_801A518C.anim, self) == 0) {
            DestroyEntity(self);
        }
        break;

    case 3:
        if (self->step_s == 0) {
            self->drawFlags = ENTITY_OPACITY;
            self->drawFlags = ENTITY_OPACITY | ENTITY_ROTATE;
            self->opacity = 0x80;
            self->facingLeft = Random() & 1;
            self->rotate = rand() & 0xFFF;
            var_s0 = self->rotate;
            if (self->facingLeft) {
                var_s0 = -self->rotate;
            }
            self->velocityX = rsin((s32)var_s0) * 0x28;
            self->velocityY = -(rcos((s32)var_s0) * 0x28);
            self->ext.et_801A518C.acceleration = (Random() << 4) + 0x1000;
            self->step_s++;
        }
        MoveEntity();
        var_s0_2 = self->rotate;
        self->opacity += 0xFF;
        if (self->facingLeft) {
            var_s0_2 = -var_s0_2;
        }
        self->velocityX += (rsin((s32)(s16)var_s0_2) *
                            (s32)self->ext.et_801A518C.acceleration) >>
                           12;
        self->velocityY += (-(s32)self->ext.et_801A518C.acceleration *
                            rcos((s32)(s16)var_s0_2)) >>
                           12;
        if (AnimateEntity(self->ext.et_801A518C.anim, self) == 0) {
            DestroyEntity(self);
        }
        break;

    case 4:
        if (self->step_s == 0) {
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0x80;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 9) + 0xFFFF0000;
            self->velocityY = FIX(-2.5);
            self->ext.et_801A518C.accelerationY = -(Random() << 4) - 0x1000;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += (s32)self->ext.et_801A518C.accelerationY;
        self->opacity += 0xFF;
        if (AnimateEntity(self->ext.et_801A518C.anim, self) == 0) {
            DestroyEntity(self);
        }
        break;

    default:
        return;
    }
}

extern EInit g_EInitDraculaLvl98;
extern u32 g_CutsceneFlags;
extern s32 D_us_801806E4; // 1: dying, 2: in place for the ending
extern s32 D_us_801806E8; // the body is hidden (rising or sinking)
extern s32 D_us_801806EC; // hand lightning: 1 charge, 2 sphere ready,
extern s32 D_us_801806F0; // which sphere frame the next lightning uses
extern s32 D_us_801806F4; // the energy ball is thrown
extern s32 D_us_801806F8; // claw swipe: 1 left, 2 right, 3 done
extern s32 D_us_801806FC; // head lunges: 1..3 the head lunging, 4 done
extern s32 D_us_80180700; // head breath: 1 start, 4 all fired, 5 return
extern u16 D_us_80180704; // the body's palette, shared by the parts
extern u8 D_us_80180708[];
extern u8 D_us_80180710[];
extern u8 D_us_801807A0[];
extern u16 D_us_80180830[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_8019DB9C);
#else
                          // 3 throw, 4 released, 5 done

// Dracula's true form. His parts: the five pieces of his body (0x19,
// slots 81-85), the arms (0x1A, 86 and 88) with their claws (0x1B, 87 and
// 89), the chest (0x18, 90), three serpent heads (0x1C, 91, 97 and 103)
// with their necks (0x1D) and the wings (0x1E, 109)
void func_us_8019DB9C(Entity* self) {
    Entity* entity;
    s32 i;
    s32 dx;
    s32 dy;
    s16 angle;

    if ((self->hitFlags & 3) && !(Random() & 7)) {
        if (Random() & 1) {
            g_api.PlaySfx(0x85B);
        } else {
            g_api.PlaySfx(0x85C);
        }
    }
    if ((self->flags & FLAG_DEAD) && self->step != 12) {
        SetStep(12);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitDraculaLvl98);
        self->zPriority = 0x80;
        self->hitboxOffY = -4;
        for (i = 0; i < 5; i++) {
            entity = &g_Entities[81 + i];
            CreateEntityFromCurrentEntity(E_ID(UNK_19), entity);
            entity->params = i;
            entity->zPriority = self->zPriority + 1;
        }
        CreateEntityFromCurrentEntity(E_ID(UNK_1A), &g_Entities[86]);
        g_Entities[86].params = 0;
        g_Entities[86].zPriority = self->zPriority + 3;
        CreateEntityFromCurrentEntity(E_ID(UNK_1A), &g_Entities[88]);
        g_Entities[88].params = 1;
        g_Entities[88].zPriority = self->zPriority + 3;
        self->nextPart = &g_Entities[87];
        self->parent = NULL;
        CreateEntityFromCurrentEntity(E_ID(UNK_18), &g_Entities[90]);
        g_Entities[90].zPriority = self->zPriority + 1;
        for (i = 0; i < 3; i++) {
            entity = &g_Entities[91 + i * 6];
            CreateEntityFromCurrentEntity(E_ID(UNK_1C), entity);
            entity->params = i;
            entity->zPriority = self->zPriority + 6;
        }
        CreateEntityFromCurrentEntity(E_ID(UNK_1E), &g_Entities[109]);
        g_Entities[109].zPriority = self->zPriority;
        SetStep(1);
        // fallthrough
    case 1:
        // fade in during the conversation
        self->hitboxState = 0;
        D_us_801806E8 = 1;
        switch (self->step_s) {
        case 0:
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0;
            self->animCurFrame = 1;
            if (g_CutsceneFlags & 0x20) {
                self->ext.rboDracula.timer = 0x80;
                self->step_s++;
            }
            break;
        case 1:
            AnimateEntity(D_us_80180708, self);
            if (!--self->ext.rboDracula.timer) {
                self->step_s++;
            }
            break;
        case 2:
            AnimateEntity(D_us_80180708, self);
            self->opacity++;
            if (self->opacity >= 0x80) {
                self->drawFlags = 0;
                self->step_s++;
            }
            break;
        case 3:
            AnimateEntity(D_us_80180708, self);
            if (g_CutsceneFlags & 2) {
                g_api.TimeAttackController(
                    TIMEATTACK_EVENT_UNUSED_28, TIMEATTACK_SET_VISITED);
                g_api.PlaySfx(0x86B);
                SetStep(2);
            }
            break;
        }
        break;
    case 12:
        // death: move to the middle and stay for the ending
        switch (self->step_s) {
        case 0:
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_UNUSED_28, TIMEATTACK_SET_RECORD);
            self->palette = 0x200;
            self->hitboxState = 0;
            D_us_801806E4 |= 1;
            self->step_s++;
            self->flags &= ~0xF;
            break;
        case 1:
            CreateEntityFromCurrentEntity(E_ID(UNK_27), &g_Entities[200]);
            g_Entities[200].params = 1;
            g_unkGraphicsStruct.unk28 = 0xFF;
            g_Entities[200].flags |= FLAG_UNK_10000;
            self->step_s++;
            break;
        case 2:
            dx = 0x80 - self->posX.i.hi;
            dy = 0x8A - self->posY.i.hi;
            angle = ratan2(dy, dx);
            self->velocityX = (rcos(angle) << 15) >> 12;
            self->velocityY = (rsin(angle) << 15) >> 12;
            MoveEntity();
            if (SquareRoot0(dx * dx + dy * dy) < 3) {
                self->step_s++;
            }
            break;
        case 3:
            self->velocityX = 0;
            self->velocityY = 0;
            D_us_801806E4 |= 2;
            self->step_s++;
            break;
        case 4:
            break;
        }
        break;
    case 2:
        // sink away
        AnimateEntity(D_us_80180708, self);
        D_us_801806E8 = 1;
        self->hitboxState = 0;
        switch (self->step_s) {
        case 0:
            self->velocityY = FIX(6);
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY -= 0x4000;
            if (self->posY.i.hi < -0x60) {
                SetStep(3);
            }
            break;
        }
        break;
    case 3:
        // rise from below
        AnimateEntity(D_us_801807A0, self);
        D_us_801806E8 = 0;
        self->hitboxState = 2;
        switch (self->step_s) {
        case 0:
            self->velocityY = FIX(1.25);
            self->posY.i.hi = -0x50;
            self->velocityX = 0;
            self->posX.i.hi = 0x80;
            self->zPriority = 0x80;
            self->ext.rboDracula.timer = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (!(self->ext.rboDracula.timer & 0x3F)) {
                g_api.PlaySfx(0x7D3);
            }
            self->ext.rboDracula.timer++;
            MoveEntity();
            if (self->posY.i.hi > 0x90) {
                SetStep(4);
            }
            break;
        }
        break;
    case 4:
        // hover, then the next attack
        if (!self->step_s) {
            self->ext.rboDracula.timer = 0x40;
            self->step_s++;
        }
        AnimateEntity(D_us_801807A0, self);
        D_us_801806E8 = 0;
        self->hitboxState = 2;
        MoveEntity();
        self->ext.rboDracula.angle += 0x20;
        self->velocityY = (rcos(self->ext.rboDracula.angle) << 15) >> 12;
        if (!--self->ext.rboDracula.timer) {
            SetStep(D_us_80180830[self->ext.rboDracula.attack]);
            self->ext.rboDracula.attack = (self->ext.rboDracula.attack + 1) & 3;
        }
        break;
    case 5:
        // lightning between the hands, then a sphere of energy
        AnimateEntity(D_us_801807A0, self);
        D_us_801806E8 = 0;
        self->hitboxState = 2;
        MoveEntity();
        self->ext.rboDracula.angle += 0x20;
        self->velocityY = (rcos(self->ext.rboDracula.angle) << 15) >> 12;
        switch (self->step_s) {
        case 0:
            D_us_801806EC = 1;
            CreateEntityFromCurrentEntity(E_ID(UNK_20), &g_Entities[129]);
            g_Entities[129].zPriority = self->zPriority + 3;
            self->step_s++;
            // fallthrough
        case 1:
            if (D_us_801806EC == 2) {
                self->step_s++;
            }
            break;
        case 2:
            D_us_801806EC = 3;
            g_api.PlaySfx(0x854);
            self->step_s++;
            // fallthrough
        case 3:
            if (D_us_801806EC == 5) {
                self->ext.rboDracula.timer = 0xA0;
                self->step_s++;
            }
            break;
        case 4:
            if (!--self->ext.rboDracula.timer) {
                if (++D_us_801806F0 >= 8) {
                    D_us_801806F0 = 0;
                }
                D_us_801806EC = 0;
                SetStep(6);
            }
            break;
        }
        break;
    case 6:
        // fly up out of sight
        if (!self->step_s) {
            self->velocityX = 0;
            self->velocityY = FIX(1);
            g_api.PlaySfx(0x84F);
            self->step_s++;
        }
        AnimateEntity(D_us_801807A0, self);
        D_us_801806E8 = 0;
        self->hitboxState = 2;
        MoveEntity();
        self->velocityY -= 0x1000;
        if (self->posY.i.hi < -0x40) {
            SetStep(7);
        }
        break;
    case 7:
        // come down from above as a shadow
        if (!self->step_s) {
            self->posY.i.hi = -0x40;
            self->posX.i.hi = 0x80;
            self->velocityX = 0;
            self->velocityY = FIX(3);
            self->zPriority = 0x80;
            self->step_s++;
        }
        AnimateEntity(D_us_80180708, self);
        D_us_801806E8 = 1;
        self->hitboxState = 0;
        MoveEntity();
        self->velocityY -= self->velocityY >> 6;
        if (self->posY.i.hi > 0x70) {
            SetStep(8);
        }
        break;
    case 8:
        // throw a ball of energy
        if (!self->step_s) {
            D_us_801806F4 = 0;
            self->step_s++;
        }
        D_us_801806E8 = 1;
        self->hitboxState = 0;
        if (!AnimateEntity(D_us_80180710, self)) {
            SetStep(2);
        }
        if (!self->poseTimer && self->pose == 2) {
            CreateEntityFromEntity(E_ID(UNK_23), self, &g_Entities[132]);
            g_Entities[132].zPriority = self->zPriority + 1;
        }
        if (!self->poseTimer && self->pose == 3) {
            g_api.PlaySfx(0x859);
            D_us_801806F4 = 1;
        }
        break;
    case 9:
        // drift sideways and swipe with a claw, four times
        AnimateEntity(D_us_801807A0, self);
        D_us_801806E8 = 0;
        self->hitboxState = 2;
        MoveEntity();
        self->ext.rboDracula.angle += 0x20;
        self->velocityY = (rcos(self->ext.rboDracula.angle) << 15) >> 12;
        switch (self->step_s) {
        case 0:
            self->ext.rboDracula.sweeps = 4;
            self->step_s++;
            // fallthrough
        case 1:
            if (PLAYER.posX.i.hi < 0x80) {
                D_us_801806F8 = 1;
                self->velocityX = FIX(-0.5);
            } else {
                D_us_801806F8 = 2;
                self->velocityX = FIX(0.5);
            }
            self->ext.rboDracula.timer = 0x28;
            self->step_s++;
            // fallthrough
        case 2:
            self->velocityX -= self->velocityX >> 4;
            if (!--self->ext.rboDracula.timer) {
                if (D_us_801806F8 == 1) {
                    self->velocityX = FIX(2);
                } else {
                    self->velocityX = FIX(-2);
                }
                self->step_s++;
            }
            break;
        case 3:
            self->velocityX -= self->velocityX >> 4;
            if (D_us_801806F8 == 3) {
                self->step_s++;
            }
            break;
        case 4:
            dx = 0x80 - self->posX.i.hi;
            if (dx > 0) {
                self->velocityX = FIX(1.125);
            }
            if (dx < 0) {
                self->velocityX = FIX(-1.125);
            }
            if (abs(dx) < 3) {
                self->velocityX = 0;
                if (!--self->ext.rboDracula.sweeps) {
                    D_us_801806F8 = 0;
                    SetStep(4);
                } else {
                    self->step_s = 1;
                }
            }
            break;
        }
        break;
    case 10:
        // the heads lunge one after another
        AnimateEntity(D_us_801807A0, self);
        D_us_801806E8 = 0;
        self->hitboxState = 2;
        MoveEntity();
        self->ext.rboDracula.angle += 0x20;
        self->velocityY = (rcos(self->ext.rboDracula.angle) << 15) >> 12;
        switch (self->step_s) {
        case 0:
            D_us_801806FC = 1;
            self->step_s++;
            // fallthrough
        case 1:
            if (D_us_801806FC == 4) {
                D_us_801806FC = 0;
                SetStep(4);
            }
            break;
        }
        break;
    case 11:
        // the heads breathe, then triangles of light fly at Alucard
        AnimateEntity(D_us_801807A0, self);
        D_us_801806E8 = 0;
        self->hitboxState = 2;
        switch (self->step_s) {
        case 0:
            D_us_80180700 = 1;
            self->step_s++;
            // fallthrough
        case 1:
            if (D_us_80180700 == 4) {
                self->ext.rboDracula.timer = 0x40;
                self->step_s++;
            }
            break;
        case 2:
            if (!(self->ext.rboDracula.timer & 0xF)) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_24), entity);
                    entity->zPriority = self->zPriority + 8;
                }
            }
            if (!--self->ext.rboDracula.timer) {
                self->ext.rboDracula.timer = 0x40;
                self->step_s++;
            }
            break;
        case 3:
            if (!--self->ext.rboDracula.timer) {
                D_us_80180700 = 5;
                self->step_s++;
            }
            break;
        case 4:
            D_us_80180700 = 0;
            SetStep(4);
            break;
        }
        break;
    case 0xFF:
#include "../../st/pad2_anim_debug.h"
    }
    D_us_80180704 = self->palette;
}
#endif

extern u16 D_us_80180540;
extern u8 D_us_8018074C[];
extern u8 D_us_80180758[];
extern u8 D_us_80180764[];
extern u8 D_us_80180770[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_8019EADC);
#else
// an arm (params 0 left, 1 right); its claw (0x1B) is the next slot
void func_us_8019EADC(Entity* self) {
    self->posX.i.hi = g_Entities[80].posX.i.hi;
    self->posY.i.hi = g_Entities[80].posY.i.hi;
    if (!self->params) {
        self->posX.i.hi -= 0x34;
        self->posY.i.hi += 0x10;
    } else {
        self->posX.i.hi += 0x3B;
        self->posY.i.hi += 0x18;
    }
    self->palette = D_us_80180704;
    if ((D_us_801806E4 & 1) && self->step != 16) {
        SetStep(16);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180540);
        self->facingLeft = self->params;
        CreateEntityFromCurrentEntity(E_ID(UNK_1B), self + 1);
        (self + 1)->params = self->params;
        (self + 1)->zPriority = self->zPriority + 1;
        // fallthrough
    case 1:
        self->animCurFrame = 0xC;
        if (D_us_801806EC == 1) {
            SetStep(2);
        }
        if (D_us_801806F8 == self->params + 1) {
            SetStep(3);
        }
        break;
    case 2:
        // throw the lightning sphere
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 0xC;
            if (D_us_801806EC == 3) {
                self->step_s++;
            }
            break;
        case 1:
            if (!self->poseTimer && self->pose == 3) {
                D_us_801806EC = 4;
            }
            if (!AnimateEntity(D_us_8018074C, self)) {
                D_us_801806EC = 5;
                SetSubStep(2);
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80180770, self)) {
                SetStep(1);
            }
            break;
        }
        break;
    case 3:
        // swipe
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180758, self)) {
                SetSubStep(1);
            }
            if (!self->poseTimer && self->pose == 2) {
                PlaySfxPositional(0x662);
            }
            break;
        case 1:
            if (!AnimateEntity(D_us_80180770, self)) {
                D_us_801806F8 = 3;
                SetStep(1);
            }
            break;
        }
        break;
    case 8:
        if (!AnimateEntity(D_us_80180764, self)) {
            self->pose = 0;
            self->poseTimer = 0;
        }
        break;
    case 16:
        switch (self->step_s) {
        case 0:
            self->pose = 0x10 - self->animCurFrame;
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_80180770, self)) {
                self->pose = 0;
                self->poseTimer = 0;
                self->step_s++;
            }
            break;
        }
        break;
    }
    if (D_us_801806E8) {
        self->animCurFrame = 0;
        self->hitboxState = 0;
    }
}
#endif

extern EInit D_us_80180558;
extern u8 D_us_8018072C[];
extern u8 D_us_80180734[];
extern u8 D_us_80180744[];
extern s16 D_us_80180808[][2];
extern DraculaClawHitbox D_us_80180838[];
extern u8 D_us_80180854[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_8019EE30);
#else
// a claw, following its arm (the previous slot)
void func_us_8019EE30(Entity* self) {
    Entity* entity;
    s32 i;
    s32 frame;
    DraculaClawHitbox* hitbox;

    self->palette = D_us_80180704;
    if ((D_us_801806E4 & 1) && self->step != 16) {
        SetStep(16);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180558);
        self->parent = &g_Entities[80];
        self->facingLeft = self->params;
        if (!self->params) {
            self->nextPart = self + 2;
        } else {
            self->nextPart = &g_Entities[80];
        }
        // fallthrough
    case 1:
        AnimateEntity(D_us_8018072C, self);
        if (D_us_801806EC == 1) {
            SetStep(2);
        }
        if (D_us_801806F8 == self->params + 1) {
            self->animCurFrame = 0xA;
        }
        break;
    case 2:
        // lightning arcs between the hands
        switch (self->step_s) {
        case 0:
            for (i = 0; i < 2; i++) {
                entity = AllocEntity(&g_Entities[144], &g_Entities[152]);
                if (entity != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_1F), entity);
                    entity->params = self->params;
                    entity->zPriority = self->zPriority + 4;
                }
            }
            self->animCurFrame = 9;
            self->step_s++;
            // fallthrough
        case 1:
            if (D_us_801806EC == 3) {
                SetSubStep(2);
            }
            break;
        case 2:
            AnimateEntity(D_us_80180734, self);
            if (D_us_801806EC == 5) {
                SetSubStep(3);
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180744, self)) {
                SetStep(1);
            }
            break;
        }
        break;
    case 16:
        if (!self->step_s) {
            self->hitboxState = 0;
            self->animCurFrame = 9;
            self->step_s++;
        }
        break;
    }
    frame = (self - 1)->animCurFrame - 0xC;
    self->posX.i.hi = (self - 1)->posX.i.hi;
    self->posY.i.hi = (self - 1)->posY.i.hi;
    if (frame < 0) {
        frame = 0;
    }
    if (frame > 4) {
        frame = 4;
    }
    if (!self->params) {
        self->posX.i.hi += D_us_80180808[frame][0];
    } else {
        self->posX.i.hi -= D_us_80180808[frame][0];
    }
    self->posY.i.hi += D_us_80180808[frame][1];
    hitbox = &D_us_80180838[D_us_80180854[self->animCurFrame]];
    self->hitboxOffX = hitbox->offX;
    self->hitboxOffY = hitbox->offY;
    self->hitboxWidth = hitbox->width;
    self->hitboxHeight = hitbox->height;
    if (D_us_801806E8) {
        self->animCurFrame = 0;
        self->hitboxState = 0;
        return;
    }
    self->hitboxState = 3;
}
#endif

extern EInit D_us_8018054C;
extern u8 D_us_80180780[];
extern u8 D_us_80180788[];
extern u8 D_us_80180794[];
extern s16 D_us_8018081C[][2];
extern s16 D_us_80180828[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_8019F1CC);
#else
// flies back towards the head's place on the body at speed (rcos * speed)
#define DRACULA_HEAD_RETURN(speed)                                             \
    MoveEntity();                                                              \
    dx = g_Entities[80].posX.i.hi + D_us_8018081C[self->params][0] -           \
         self->posX.i.hi;                                                      \
    dy = g_Entities[80].posY.i.hi + D_us_8018081C[self->params][1] -           \
         self->posY.i.hi;                                                      \
    angle = ratan2(dy, dx);                                                    \
    self->velocityX = rcos(angle) * (speed);                                   \
    self->velocityY = rsin(angle) * (speed);

// a serpent head (params 0-2) with its five neck pieces (0x1D) in the
// next slots: lunges at Alucard, or breathes a stream (0x25)
void func_us_8019F1CC(Entity* self) {
    Entity* entity;
    Entity* neck;
    s32 i;
    s32 dx;
    s32 dy;
    s32 x;
    s32 y;
    s16 px;
    s16 py;
    s16 angle;

    self->palette = D_us_80180704;
    if ((D_us_801806E4 & 1) && self->step != 16) {
        SetStep(16);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_8018054C);
        for (i = 0, neck = self + 1; i < 5; i++, neck++) {
            CreateEntityFromCurrentEntity(E_ID(UNK_1D), neck);
            neck->zPriority = self->zPriority - i;
        }
        // fallthrough
    case 1:
        AnimateEntity(D_us_80180780, self);
        self->posX.i.hi =
            g_Entities[80].posX.i.hi + D_us_8018081C[self->params][0];
        self->posY.i.hi =
            g_Entities[80].posY.i.hi + D_us_8018081C[self->params][1];
        self->zPriority = g_Entities[80].zPriority + 2;
        self->hitboxState = 0;
        if (D_us_801806FC == self->params + 1) {
            self->zPriority = g_Entities[80].zPriority + 6;
            SetStep(2);
        }
        if (D_us_80180700 == 1) {
            self->zPriority = g_Entities[80].zPriority + 6;
            SetStep(3);
        }
        break;
    case 2:
        // lunge at Alucard
        self->hitboxState = 3;
        switch (self->step_s) {
        case 0:
            angle = ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                           PLAYER.posX.i.hi - self->posX.i.hi);
            self->velocityX = (rcos(angle) << 18) >> 12;
            self->velocityY = (rsin(angle) << 18) >> 12;
            PlaySfxPositional(0x866);
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            if (!AnimateEntity(D_us_80180788, self)) {
                SetSubStep(2);
            }
            break;
        case 2:
            AnimateEntity(D_us_80180794, self);
            DRACULA_HEAD_RETURN(32);
            if (abs(dx) < 3 && abs(dy) < 3) {
                D_us_801806FC++;
                SetStep(1);
            }
            break;
        }
        break;
    case 3:
        // breathe
        switch (self->step_s) {
        case 0:
            angle = D_us_80180828[self->params];
            self->velocityX = (rcos(angle) << 18) >> 12;
            self->velocityY = (rsin(angle) << 18) >> 12;
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            if (!AnimateEntity(D_us_80180788, self)) {
                g_api.PlaySfx(0x62F);
                entity = AllocEntity(&g_Entities[192], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_25), entity);
                    entity->ext.succubus.real = self;
                    entity->zPriority = self->zPriority + 1;
                }
                D_us_80180700++;
                self->step_s++;
            }
            break;
        case 2:
            if (D_us_80180700 == 5) {
                self->step_s++;
            }
            break;
        case 3:
            AnimateEntity(D_us_80180794, self);
            DRACULA_HEAD_RETURN(0x28);
            if (abs(dx) < 3 && abs(dy) < 3) {
                SetStep(1);
            }
            break;
        }
        break;
    case 16:
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            self->animCurFrame = 0x12;
            DRACULA_HEAD_RETURN(0x28);
            if (abs(dx) < 3 && abs(dy) < 3) {
                self->zPriority = g_Entities[80].zPriority + 2;
                self->step_s++;
            }
            break;
        case 1:
            self->posX.i.hi =
                g_Entities[80].posX.i.hi + D_us_8018081C[self->params][0];
            self->posY.i.hi =
                g_Entities[80].posY.i.hi + D_us_8018081C[self->params][1];
            break;
        }
        break;
    }

    // the neck follows: each piece eases towards the middle of its
    // neighbours, from the body out to the head
    x = (D_us_8018081C[self->params][0] << 16) + g_Entities[80].posX.val;
    y = (D_us_8018081C[self->params][1] << 16) + g_Entities[80].posY.val;
    neck = self + 5;
    for (i = 0; i < 5; i++, neck--) {
        dx = (x + (neck - 1)->posX.val) >> 1;
        dy = (y + (neck - 1)->posY.val) >> 1;
        x = dx + ((dx - neck->posX.val) >> 2);
        y = dy + ((dy - neck->posY.val) >> 2);
        neck->posX.val = x;
        neck->posY.val = y;
        neck->zPriority = self->zPriority + i - 5;
    }
    // then no piece may be more than 12 pixels from the previous one
    px = g_Entities[80].posX.i.hi + D_us_8018081C[self->params][0];
    py = g_Entities[80].posY.i.hi + D_us_8018081C[self->params][1];
    neck = self + 5;
    for (i = 0; i < 6; i++, neck--) {
        dx = neck->posX.i.hi - px;
        dy = neck->posY.i.hi - py;
        angle = ratan2(dy, dx);
        if (dx * dx + dy * dy > 0x90) {
            neck->posX.i.hi = px + ((rcos(angle) * 12) >> 12);
            neck->posY.i.hi = py + ((rsin(angle) * 12) >> 12);
        }
        px = neck->posX.i.hi;
        py = neck->posY.i.hi;
    }
    if (D_us_801806E8) {
        self->animCurFrame = 0;
        self->hitboxState = 0;
    }
}
#endif

void func_us_8019FA78(Entity* self) {
    extern u16 D_us_80180540;
    extern s32 D_us_801806E8;
    extern u16 D_us_80180704;

    self->palette = D_us_80180704;

    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180540);
        self->animCurFrame = 0x11;
        /* fall through */
    case 1:
        self->animCurFrame = 0x11;
        break;
    }

    if (D_us_801806E8 != 0) {
        self->animCurFrame = 0;
        self->hitboxState = 0;
    }
}

void func_us_8019FB04(Entity* self) {
    extern u16 D_us_80180704;
    extern s32 D_us_801806E8;
    extern u8* D_us_801807F4[];
    extern u16 D_us_80180540;
    extern void InitializeEntity(u16*);

    u16 value;

    self->palette = D_us_80180704;
    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180540);
        /* fall through */
    case 1:
        AnimateEntity(D_us_801807F4[self->params], self);
        value = g_Entities[E_BOSS_WEAPON].posX.i.hi;
        self->posX.i.hi = value;
        value = g_Entities[E_BOSS_WEAPON].posY.i.hi;
        self->posY.i.hi = value;
        break;
    }

    if (D_us_801806E8 != 0) {
        self->animCurFrame = 0;
        self->hitboxState = 0;
    }
}

extern u8 D_us_801807A8[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_8019FBC0);
#else
// the chest
void func_us_8019FBC0(Entity* self) {
    self->posX.i.hi = g_Entities[80].posX.i.hi;
    self->palette = D_us_80180704;
    self->posY.i.hi = g_Entities[80].posY.i.hi;
    if ((D_us_801806E4 & 1) && self->step != 16) {
        SetStep(16);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180540);
        // fallthrough
    case 1:
        AnimateEntity(D_us_801807A8, self);
        break;
    case 16:
        self->animCurFrame = 0x1B;
        break;
    }
    if (D_us_801806E8) {
        self->animCurFrame = 0;
        self->hitboxState = 0;
    }
}
#endif

extern EInit g_EInitInteractable;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_8019FCB4);
#else
// the wings: two pairs of quads beating slowly
void func_us_8019FCB4(Entity* self) {
    Primitive* prim;
    s32 i;
    s16 x;
    s16 y;
    s16 angle;
    s16 primIndex;
    u16 clut;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 4);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.rboDracula.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->tpage = 0x14;
            prim->clut = 0x201;
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority - 1;
            prim = prim->next;
        }
        prim = self->ext.rboDracula.prim;
        for (i = 0; i < 2; i++) {
            prim->u0 = 0xC1;
            prim->u1 = 0xFF;
            prim->u2 = 0xC1;
            prim->u3 = 0xFF;
            prim->v0 = 1;
            prim->v1 = 1;
            prim->v2 = 0x47;
            prim->v3 = 0x47;
            prim->priority = self->zPriority - 2;
            prim = prim->next;
            prim->u0 = 1;
            prim->u1 = 0xBF;
            prim->u2 = 1;
            prim->u3 = 0xBF;
            prim->v0 = 1;
            prim->v1 = 1;
            prim->v2 = 0xB4;
            prim->v3 = 0xB4;
            prim = prim->next;
        }
        // fallthrough
    case 1:
        self->ext.rboDracula.angle += 0x10;
        self->posX.i.hi = g_Entities[80].posX.i.hi;
        self->posY.i.hi = g_Entities[80].posY.i.hi;
        angle = (rsin(self->ext.rboDracula.angle) << 7) >> 12;
        self->rotate = angle;
        prim = self->ext.rboDracula.prim;
        for (i = 0; i < 2; i++) {
            // the inner wing
            if (!i) {
                x = self->posX.i.hi - 8;
            } else {
                x = self->posX.i.hi + 8;
            }
            y = self->posY.i.hi - 0x34;
            prim->x1 = prim->x3 = x;
            prim->y1 = y;
            prim->y3 = self->posY.i.hi + 0x12;
            if (!i) {
                x -= (rcos(angle) * 0x3E) >> 12;
            } else {
                x += (rcos(angle) * 0x3E) >> 12;
            }
            y += (rsin(angle) * 0x3E) >> 12;
            prim->x0 = x;
            prim->y0 = y;
            if (!i) {
                prim->x2 = x + ((rsin(angle) * 0x46) >> 12);
            } else {
                prim->x2 = x - ((rsin(angle) * 0x46) >> 12);
            }
            prim->y2 = y + ((rcos(angle) * 0x46) >> 12);
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
            // the outer wing
            if (!i) {
                x += 0x18;
            } else {
                x -= 0x18;
            }
            y -= 0x78;
            prim->x1 = x;
            prim->y1 = y;
            prim->x3 = x;
            prim->y3 = y + 0xB3;
            if (!i) {
                x -= (rcos(angle) * 0xBE) >> 12;
            } else {
                x += (rcos(angle) * 0xBE) >> 12;
            }
            y += (rsin(angle) * 0xBE) >> 12;
            prim->x0 = x;
            prim->y0 = y;
            if (!i) {
                prim->x2 = x + ((rsin(angle) * 0xB3) >> 12);
            } else {
                prim->x2 = x - ((rsin(angle) * 0xB3) >> 12);
            }
            prim->drawMode = DRAW_UNK02;
            prim->y2 = y + ((rcos(angle) * 0xB3) >> 12);
            prim = prim->next;
        }
        break;
    }
    prim = self->ext.rboDracula.prim;
    clut = D_us_80180704 & 0xFFF;
    if (clut == 0x200) {
        clut = 0x201;
    }
    while (prim != NULL) {
        prim->clut = clut;
        if (D_us_801806E8) {
            prim->drawMode |= DRAW_HIDE;
        }
        prim = prim->next;
    }
    FntPrint("color %x\n", D_us_80180704);
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_801A01A4);
#else
// lightning from one hand to the other (params 0 from the left, 1 from the
// right): a chain of quads that zigzags towards the other hand, redrawn
// every frame
void func_us_801A01A4(Entity* self) {
    Primitive* prim;
    Primitive* next;
    Entity* target;
    s32 i;
    s32 dx;
    s32 dy;
    s32 near;
    s32 turn;
    s16 primIndex;
    s16 angle;
    u16 a;

    if (D_us_801806E4 & 1) {
        DestroyEntity(self);
        return;
    }
    if (D_us_801806EC == 2) {
        DestroyEntity(self);
        return;
    }
    if (!self->params) {
        self->ext.rboDracula.src = &g_Entities[86];
        self->ext.rboDracula.dst = &g_Entities[88];
    } else {
        self->ext.rboDracula.dst = &g_Entities[86];
        self->ext.rboDracula.src = &g_Entities[88];
    }
    self->posX.i.hi = self->ext.rboDracula.src->posX.i.hi;
    self->posY.i.hi = self->ext.rboDracula.src->posY.i.hi;
    if (!(g_Timer & 0xF)) {
        g_api.PlaySfx(0x869);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x14);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.rboDracula.prim = prim;
        self->ext.rboDracula.cur = prim;
        self->flags |= FLAG_HAS_PRIMS;
        i = 0;
        while (prim != NULL) {
            PGREY(prim, 0) = 0x50;
            prim->tpage = 0x1A;
            prim->clut = 0x194;
            prim->u0 = prim->u1 = i * 16 - 0x70;
            prim->u2 = prim->u3 = i * 16 - 0x60;
            prim->v0 = prim->v2 = 0xD0;
            prim->v1 = prim->v3 = 0xC0;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority + 1;
            prim = prim->next;
            if (++i >= 6) {
                i = 0;
            }
        }
        // fallthrough
    case 1:
        prim = self->ext.rboDracula.prim;
        prim->x0 = self->posX.i.hi;
        prim->y0 = self->posY.i.hi;
        prim->x1 = prim->x0;
        // as on PSX, y1 rather than y0
        prim->y1 = prim->y1 - 0x10;
        prim->x2 = self->posX.i.hi;
        prim->y2 = self->posY.i.hi;
        prim->x3 = prim->x2;
        prim->y3 = prim->y2 - 0x10;
        self->ext.rboDracula.cur = prim;
        self->ext.rboDracula.angle = Random() * 16;
        while (prim != NULL) {
            PGREY(prim, 0) = 0x50;
            prim->drawMode = DRAW_HIDE;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim = prim->next;
        }
        self->ext.rboDracula.count = 0;
        self->step++;
        // fallthrough
    case 2:
        while (true) {
            prim = self->ext.rboDracula.cur;
            target = self->ext.rboDracula.dst;
            a = self->ext.rboDracula.angle;
            dx = target->posX.i.hi - prim->x2;
            dy = target->posY.i.hi - prim->y2;
            if (abs(dx) < 0x10 && abs(dy) < 0x10) {
                break;
            }
            near = abs(dx) < 0x20 && abs(dy) < 0x20;
            if (!self->ext.rboDracula.count) {
                self->ext.rboDracula.count = near ? 2 : 4;
                turn = (s16)(ratan2(-dy, dx) - a);
                if (turn > 0x800) {
                    turn -= 0x1000;
                }
                if (turn < -0x800) {
                    turn += 0x1000;
                }
                if (near) {
                    self->ext.rboDracula.turn = turn / 2;
                } else {
                    self->ext.rboDracula.turn = turn / 4;
                }
            }
            angle = a + self->ext.rboDracula.turn;
            if (!near) {
                angle = angle + 0x60 - ((Random() & 3) << 6);
            }
            angle &= 0xFFF;
            next = prim->next;
            if (next == NULL) {
                break;
            }
            LOW(next->x0) = LOW(prim->x2);
            LOW(next->x1) = LOW(prim->x3);
            self->ext.rboDracula.angle = angle;
            self->ext.rboDracula.cur = next;
            next->x2 = next->x0 + ((rcos(angle) * 12) >> 12);
            next->y2 = next->y0 - ((rsin(angle) * 12) >> 12);
            next->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                             DRAW_UNK02 | DRAW_TRANSP | DRAW_UNK_40;
            next->x3 = next->x2 + ((rcos(angle - 0x400) * 16) >> 12);
            next->y3 = next->y2 - ((rsin(angle - 0x400) * 16) >> 12);
            self->ext.rboDracula.count--;
        }
        self->step = 1;
        break;
    case 3:
        prim = self->ext.rboDracula.prim;
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->step = 1;
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_801A0710);
#else
// the sphere of energy forming between the hands: grows, then spawns the
// lightning sphere (0x21)
void func_us_801A0710(Entity* self) {
    if (D_us_801806E4 & 1) {
        DestroyEntity(self);
        return;
    }
    self->posX.i.hi = g_Entities[80].posX.i.hi;
    self->posY.i.hi = g_Entities[80].posY.i.hi + 0x10;
    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180540);
        self->animCurFrame = 0x2A;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleY = self->scaleX = 0;
        // fallthrough
    case 1:
        self->scaleY = self->scaleX = self->scaleY + 0x10;
        if (self->scaleX > 0x180) {
            self->step++;
        }
        break;
    case 2:
        CreateEntityFromCurrentEntity(E_ID(UNK_21), &g_Entities[130]);
        g_Entities[130].zPriority = self->zPriority + 1;
        self->step++;
        break;
    case 3:
        if (D_us_801806EC == 2) {
            self->step++;
        }
        break;
    case 4:
        self->scaleY = self->scaleX = self->scaleX - 0x20;
        if (self->scaleX < 0) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_801A0860);
#else
// the lightning sphere: fades in (then the hands may throw), bursts into
// sparks (0x22) when thrown
void func_us_801A0860(Entity* self) {
    Entity* entity;
    s32 i;
    s16 scale;

    if (D_us_801806E4 & 1) {
        DestroyEntity(self);
        return;
    }
    self->posX.i.hi = g_Entities[80].posX.i.hi;
    self->posY.i.hi = g_Entities[80].posY.i.hi + 0x10;
    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180540);
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleY = self->scaleX = 0;
        self->opacity = 0;
        self->animCurFrame = D_us_801806F0 + 0x2D;
        self->drawFlags |= ENTITY_OPACITY;
        // fallthrough
    case 1:
        self->scaleY = self->scaleX = self->scaleY + 8;
        if (self->scaleX >= 0x100) {
            self->scaleY = self->scaleX = 0x100;
            self->step++;
        }
        // fallthrough
    case 2:
        self->opacity += 2;
        if (self->opacity >= 0x80) {
            D_us_801806EC = 2;
            self->step = 3;
        }
        break;
    case 3:
        if (D_us_801806EC == 4) {
            g_api.PlaySfx(0x868);
            self->step++;
        }
        break;
    case 4:
        scale = self->scaleY - 0x18;
        self->scaleX += 8;
        if (scale < 0) {
            scale = 0;
        }
        self->scaleY = scale;
        for (i = 0; i < 0x10; i++) {
            entity = AllocEntity(&g_Entities[192], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_22), self, entity);
                entity->zPriority = self->zPriority + 1;
            }
        }
        if (!scale) {
            self->animCurFrame = 0;
            self->ext.rboDracula.timer = 0x50;
            self->step++;
        }
        break;
    case 5:
        if (!--self->ext.rboDracula.timer) {
            g_api.PlaySfx(0x672);
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern u8 D_us_801807E0[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_801A0AB4);
#else
// a spark of the burst sphere: flies out, hangs, then is drawn back into
// Dracula
void func_us_801A0AB4(Entity* self) {
    s32 angle;
    s32 speed;
    s32 dx;
    s32 dy;
    s16 a;

    if (D_us_801806E4 & 1) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(&D_us_80180540);
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleY = self->scaleX = (Random() & 0x7F) | 0x80;
        self->blendMode = BLEND_TRANSP;
        angle = rand() & 0xFFF;
        speed = (Random() & 0x1F) + 0x10;
        self->velocityX = speed * rcos(angle);
        self->velocityY = speed * rsin(angle);
        self->posX.val += self->velocityX * 4;
        self->posY.val += self->velocityY * 4;
        self->ext.rboDracula.timer = (Random() & 0x1F) + 0x10;
        // fallthrough
    case 1:
        MoveEntity();
        AnimateEntity(D_us_801807E0, self);
        self->velocityX -= self->velocityX >> 5;
        self->velocityY -= self->velocityY >> 5;
        if (D_us_801806EC == 5) {
            self->velocityX = 0;
            self->velocityY = 0;
            self->step++;
        }
        break;
    case 2:
        AnimateEntity(D_us_801807E0, self);
        if (!--self->ext.rboDracula.timer) {
            self->step++;
        }
        if (!(g_Timer & 7)) {
            g_api.PlaySfx(0x673);
        }
        break;
    case 3:
        MoveEntity();
        if (self->scaleX > 0x20) {
            self->scaleY = self->scaleX = self->scaleY - 2;
        }
        dy = g_Entities[80].posY.i.hi - (self->posY.i.hi + 0x18);
        dx = g_Entities[80].posX.i.hi - self->posX.i.hi;
        a = ratan2(dy, dx);
        if (self->ext.rboDracula.speed < 0x30000) {
            self->ext.rboDracula.speed += 0x1000;
        }
        speed = self->ext.rboDracula.speed;
        self->velocityX = (speed * rcos(a)) >> 12;
        self->velocityY = (speed * rsin(a)) >> 12;
        if (dx * dx + dy * dy < 0x40) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern EInit D_us_80180564;
extern u8 D_us_8018071C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_801A0DC0);
#else
// the ball of energy: grows in Dracula's hands, then flies to where
// Alucard was in 96 frames and hits if he is still within it
void func_us_801A0DC0(Entity* self) {
    Entity* entity;
    s32 radius;
    s16 scale;
    s16 dx;
    s16 dy;

    if (D_us_801806E4 & 1) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180564);
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->opacity = 0x60;
        self->scaleY = self->scaleX = 0;
        self->hitboxState = 0;
        self->blendMode |= BLEND_ADD | BLEND_TRANSP;
        self->drawFlags |= ENTITY_OPACITY;
        // fallthrough
    case 1:
        AnimateEntity(D_us_8018071C, self);
        scale = self->scaleX + 0x10;
        if (scale > 0xCC) {
            scale = 0xCC;
        }
        self->scaleY = self->scaleX = scale;
        if (D_us_801806F4) {
            self->ext.rboDracula.timer = 0x60;
            self->step++;
            self->velocityX = (PLAYER.posX.val - self->posX.val) / 96;
            self->velocityY = (PLAYER.posY.val - self->posY.val) / 96;
        }
        if (!(g_Timer & 0x1F)) {
            g_api.PlaySfxVolPan(0x6DE, 0x50, 0);
        }
        break;
    case 2:
        AnimateEntity(D_us_8018071C, self);
        MoveEntity();
        self->scaleY = self->scaleX =
            0x6000 / (self->ext.rboDracula.timer + 0x18);
        if (!(self->ext.rboDracula.timer & 3)) {
            entity = AllocEntity(&g_Entities[192], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_26), self, entity);
                entity->scaleY = entity->scaleX = self->scaleX;
                entity->zPriority = self->zPriority;
                entity->opacity = self->opacity;
            }
        }
        if (!(self->ext.rboDracula.timer & 0xF)) {
            g_api.PlaySfxVolPan(
                0x6E2, 0x80 - self->ext.rboDracula.timer / 2, 0);
        }
        if (!--self->ext.rboDracula.timer) {
            g_api.PlaySfx(0x661);
            radius = self->scaleX * 0x14 / 256;
            self->zPriority = g_unkGraphicsStruct.g_zEntityCenter + 8;
            dx = PLAYER.posX.i.hi - self->posX.i.hi;
            dy = PLAYER.posY.i.hi - self->posY.i.hi;
            if (dx * dx + dy * dy < radius * radius) {
                self->hitboxOffX = dx;
                self->hitboxOffY = dy;
                self->hitboxState = 1;
            }
            self->step++;
        }
        break;
    case 3:
        DestroyEntity(self);
        break;
    }
}
#endif

void func_us_801A1150(Entity* self) {
    extern u16 D_us_80180540;
    extern u8 D_us_80180724;
    s32 var_v0;

    if (!self->step) {
        InitializeEntity(&D_us_80180540);
        self->drawFlags = ENTITY_OPACITY | ENTITY_SCALEY | ENTITY_SCALEX;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
    }
    AnimateEntity(&D_us_80180724, self);
    var_v0 = self->opacity;
    var_v0 -= 2;
    if (var_v0 < 0) {
        var_v0 = 0;
    }
    self->opacity = (u8)var_v0;
    if (var_v0 == 0) {
        DestroyEntity(self);
    }
}

extern EInit D_us_80180570;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/unk_1D690", func_us_801A11DC);
#else
// the vertices' colours cycle through hues
#define DRACULA_TRIANGLE_HUES(prim, level)                                     \
    a = self->ext.rboDracula.hue[0] += 0x100;                                  \
    b = self->ext.rboDracula.hue[1] += 0x40;                                   \
    c = self->ext.rboDracula.hue[2] += 0x80;                                   \
    color = &(prim)->r0;                                                       \
    for (i = 0; i < 3; i++) {                                                  \
        color[0] = ((level) + (((level) * rcos(a)) >> 12)) / 2;                \
        color[1] = ((level) + (((level) * rsin(b)) >> 12)) / 2;                \
        color[2] = ((level) + (((level) * rcos(c)) >> 12)) / 2;                \
        a += 0x40;                                                             \
        b += 0x280;                                                            \
        c += 0x100;                                                            \
        color += OFF(Primitive, r1) - OFF(Primitive, r0);                      \
    }

// a triangle of light strung between the three heads: shrinks to a small
// spinning triangle and flies at Alucard
void func_us_801A11DC(Entity* self) {
    Primitive* prim;
    s16* xy;
    u8* color;
    s32 i;
    s32 x;
    s32 y;
    s32 sin;
    s32 cos;
    s16 pts[4][2];
    s16 primIndex;
    s16 angle;
    s16 scale;
    u16 a;
    u16 b;
    u16 c;
    u8 level;

    if (D_us_801806E4 & 1) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180570);
        self->hitboxState = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 10);
        if (primIndex == -1) {
            self->step = 0;
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.rboDracula.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        // its edges
        for (i = 0; i < 3; i++) {
            prim->type = PRIM_LINE_G2;
            PGREY(prim, 0) = 0xC0;
            PGREY(prim, 1) = 0xC0;
            prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            prim->priority = self->zPriority;
            prim = prim->next;
        }
        // its face
        prim->type = PRIM_GT3;
        PGREY(prim, 0) = 0x10;
        prim->tpage = 0x1A;
        prim->clut = 0x15F;
        prim->u0 = prim->v0 = 0x10;
        prim->u1 = 0x14;
        prim->v1 = 0x10;
        prim->u2 = prim->v2 = 0x14;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r0);
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 |
                         DRAW_TRANSP;
        prim->priority = self->zPriority;
        self->opacity = 0;
        // fallthrough
    case 1:
        // follow the heads while it brightens
        for (i = 0; i < 3; i++) {
            pts[i][0] = g_Entities[91 + i * 6].posX.i.hi;
            pts[i][1] = g_Entities[91 + i * 6].posY.i.hi + 0xC;
        }
        pts[3][0] = g_Entities[91].posX.i.hi;
        pts[3][1] = g_Entities[91].posY.i.hi + 0xC;
        prim = self->ext.rboDracula.prim;
        for (i = 0; i < 3; i++) {
            prim->x0 = pts[i][0];
            prim->y0 = pts[i][1];
            prim = prim->next;
        }
        prim = self->ext.rboDracula.prim;
        for (i = 1; i < 4; i++) {
            prim->x1 = pts[i][0];
            prim->y1 = pts[i][1];
            prim = prim->next;
        }
        prim->x0 = pts[0][0];
        prim->y0 = pts[0][1];
        prim->x1 = pts[1][0];
        prim->y1 = pts[1][1];
        prim->x2 = pts[2][0];
        prim->y2 = pts[2][1];
        self->opacity += 3;
        level = self->opacity;
        DRACULA_TRIANGLE_HUES(prim, level);
        if (self->opacity > 0x40) {
            x = 0;
            y = 0;
            for (i = 0; i < 3; i++) {
                x += pts[i][0];
                y += pts[i][1];
            }
            self->posX.i.hi = x / 3;
            self->posY.i.hi = y / 3;
            for (i = 0; i < 3; i++) {
                self->ext.rboDracula.ofs[i][0] = pts[i][0] - self->posX.i.hi;
                self->ext.rboDracula.ofs[i][1] = pts[i][1] - self->posY.i.hi;
            }
            self->scaleX = 0x100;
            self->step++;
        }
        break;
    case 2:
        // the edges fade, the face shrinks
        prim = self->ext.rboDracula.prim;
        for (i = 0; i < 3; i++) {
            if (!PrimDecreaseBrightness(prim, 8)) {
                prim->drawMode = DRAW_HIDE;
            }
            prim = prim->next;
        }
        scale = self->scaleX - 8;
        self->scaleX = scale;
        xy = &prim->x0;
        for (i = 0; i < 3; i++) {
            xy[0] = self->posX.i.hi + ((self->ext.rboDracula.ofs[i][0] * scale) >> 8);
            xy[1] = self->posY.i.hi + ((self->ext.rboDracula.ofs[i][1] * scale) >> 8);
            xy += (OFF(Primitive, x1) - OFF(Primitive, x0)) / sizeof(s16);
        }
        if (self->opacity < 0xC0) {
            self->opacity += 4;
        }
        level = self->opacity;
        DRACULA_TRIANGLE_HUES(prim, level);
        if (self->scaleX < 0x60) {
            // as on PSX, the last edge is left to its fade
            prim = self->ext.rboDracula.prim;
            for (i = 1; i < 3; i++) {
                prim->drawMode = DRAW_HIDE;
                prim = prim->next;
            }
            scale = self->scaleX;
            for (i = 0; i < 3; i++) {
                self->ext.rboDracula.ofs[i][0] =
                    (self->ext.rboDracula.ofs[i][0] * scale) >> 8;
                self->ext.rboDracula.ofs[i][1] =
                    (self->ext.rboDracula.ofs[i][1] * scale) >> 8;
            }
            self->step++;
        }
        break;
    case 3:
        angle = ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                       PLAYER.posX.i.hi - self->posX.i.hi);
        self->velocityX = (rcos(angle) << 18) >> 12;
        self->velocityY = (rsin(angle) << 18) >> 12;
        self->hitboxState = 1;
        g_api.PlaySfx(0x8BF);
        self->step++;
        // fallthrough
    case 4:
        MoveEntity();
        self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
        if (self->velocityX > 0) {
            self->rotate += 0x40;
        } else {
            self->rotate -= 0x40;
        }
        prim = self->ext.rboDracula.prim;
        for (i = 0; i < 3; i++) {
            prim = prim->next;
        }
        level = self->opacity;
        DRACULA_TRIANGLE_HUES(prim, level);
        xy = &prim->x0;
        x = self->posX.i.hi;
        y = self->posY.i.hi;
        sin = rsin(self->rotate);
        cos = rcos(self->rotate);
        for (i = 0; i < 3; i++) {
            xy[0] = x + ((self->ext.rboDracula.ofs[i][0] * cos -
                          self->ext.rboDracula.ofs[i][1] * sin) >>
                         12);
            xy[1] = y + ((self->ext.rboDracula.ofs[i][0] * sin +
                          self->ext.rboDracula.ofs[i][1] * cos) >>
                         12);
            xy += (OFF(Primitive, x1) - OFF(Primitive, x0)) / sizeof(s16);
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// upstream's matching version reads its data by PSX address
void func_us_801A1B38(Entity* self) {
    extern s32 D_us_801806E4;
    extern s32 D_us_80180700;
    extern u16 D_us_80180540;
    extern u8 D_us_801807EC[];
    Entity* real;

    if (D_us_801806E4 & 1) {
        DestroyEntity(self);
        return;
    }
    if (!self->step) {
        InitializeEntity(&D_us_80180540);
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
    }
    AnimateEntity(D_us_801807EC, self);
    real = self->ext.succubus.real;
    self->posX.i.hi = real->posX.i.hi;
    self->posY.i.hi = real->posY.i.hi + 0xC;
    if (D_us_80180700 == 5) {
        DestroyEntity(self);
    }
}
#else
void func_us_801A1B38(Entity* self) {
    Entity* real;

    if (*(s32*)0x801806E4 & 1) {
        DestroyEntity(self);
        return;
    }

    if (!self->step) {
        InitializeEntity((u16*)((u32)func_us_801A1B38 - 0x215F8));
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
    }

    AnimateEntity((u8*)((u32)func_us_801A1B38 - 0x2134C), self);

    real = self->ext.succubus.real;
    self->posX.i.hi = real->posX.i.hi;
    self->posY.i.hi = real->posY.i.hi + 0xC;

    if (*(s32*)0x80180700 == 5) {
        DestroyEntity(self);
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_801805FC[];
extern u8 D_us_80180604[];
u8 D_us_801805FC[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180604[] ALIGNED4 = {0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
u8* D_us_8018060C[8] = { D_us_801805FC, D_us_80180604, NULL, NULL, NULL, NULL, NULL, NULL };
u8 D_us_8018062C[8] = { 8, 8, 0, 0, 0, 0, 0, 0 };
u8 D_us_80180634[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u16 D_us_8018063C[8] = { 3, 3, 0, 0, 0, 0, 0, 0 };
u8 D_us_8018064C[0x10] = { 0x70, 0x30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s32 D_us_801806E4 = 0;
s32 D_us_801806E8 = 0;
s32 D_us_801806EC = 0;
s32 D_us_801806F0 = 0;
s32 D_us_801806F4 = 0;
s32 D_us_801806F8 = 0;
s32 D_us_801806FC = 0;
s32 D_us_80180700 = 0;
u16 D_us_80180704 = 0;
u8 D_us_80180708[8] = { 0x20, 1, 0x20, 2, 0, 0, 0, 0 };
u8 D_us_80180710[0xC] = { 0x40, 1, 8, 4, 0x48, 3, 2, 1, 0x30, 4, 0xFF, 0 };
u8 D_us_8018071C[8] = { 1, 5, 1, 6, 1, 7, 0, 0 };
u8 D_us_8018072C[8] = { 0x1E, 8, 0x1E, 9, 0, 0, 0, 0 };
u8 D_us_80180734[0x10] = { 4, 8, 4, 9, 4, 8, 4, 0xA, 0x20, 0xB, 0xC, 0xA, 0xFF, 0, 0, 0 };
u8 D_us_80180744[8] = { 8, 0xA, 8, 8, 8, 9, 0xFF, 0 };
u8 D_us_8018074C[0xC] = { 4, 0xC, 4, 0xD, 4, 0xE, 4, 0xF, 0x30, 0x10, 0xFF, 0 };
u8 D_us_80180758[0xC] = { 0x20, 0xC, 2, 0xD, 2, 0xE, 2, 0xF, 0x40, 0x10, 0xFF, 0 };
u8 D_us_80180764[0xC] = { 0x20, 0xC, 8, 0xD, 8, 0xE, 8, 0xF, 0x20, 0x10, 0xFF, 0 };
u8 D_us_80180770[0x10] = { 4, 0x10, 8, 0xF, 8, 0xE, 8, 0xD, 0xF, 0xC, 1, 0xC, 0xFF, 0, 0, 0 };
u8 D_us_80180780[8] = { 1, 0x12, 1, 0x13, 0, 0, 0, 0 };
u8 D_us_80180788[0xC] = { 2, 0x13, 2, 0x14, 2, 0x15, 0x40, 0x16, 0xFF, 0, 0, 0 };
u8 D_us_80180794[0xC] = { 2, 0x16, 4, 0x15, 4, 0x14, 2, 0x13, 0xFF, 0, 0, 0 };
u8 D_us_801807A0[8] = { 0x1E, 0x17, 0x1E, 0x18, 0, 0, 0, 0 };
u8 D_us_801807A8[0xC] = { 0x80, 0x19, 8, 0x1A, 0x80, 0x1B, 8, 0x1A, 0, 0, 0, 0 };
u8 D_us_801807E0[0xC] = { 6, 0x27, 6, 0x28, 6, 0x27, 6, 0x29, 0, 0, 0, 0 };
s16 D_us_80180808[5][2] = {
    {-0x20, -0x10}, {-0x10, 0x10}, {0, 0x10}, {0x20, 8}, {0x30, -0x10},
};
s16 D_us_8018081C[3][2] = {{-0x18, -0x10}, {-0x10, 0x10}, {0x18, -0x10}};
s16 D_us_80180828[4] = { -0x600, 0x2C0, -0x1C0, 0 };
u16 D_us_80180830[4] = { 9, 5, 0xA, 0xB };
DraculaClawHitbox D_us_80180838[7] = {
    { 0, 0, 0, 0 },
    { 0xC0, 0xAC, 0, 0 },
    { 0, 0, 0x10, 0x10 },
    { -0x12, 0, 0xC, 0x18 },
    { -0xA, 0, 0xC, 0x18 },
    { 0xB8, 0x70, 0, 0 },
    { -0xC, -0x11, 0, 0 },
};
u8 D_us_80180854[0x10] = { 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 0, 0 };
// end pc_import_data.py
// clang-format on
#endif
