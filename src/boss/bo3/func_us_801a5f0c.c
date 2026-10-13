// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo3.h"

extern EInit g_EInitParticle;
extern ScyllaParticleParams D_us_8018105C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/func_us_801a5f0c", func_us_801A5F0C);
#else
// Flames: 1 a burst along a wyrm's neck, 2 to 4 rising flames of Scylla's
// death, from her body outward
void func_us_801A5F0C(Entity* self) {
    ScyllaParticleParams* params;
    s32 variant;
    u32 zPriority;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitParticle);
        variant = self->params & 0xF;
        params = &D_us_8018105C[variant];
        self->palette = params->palette + 0x2E0;
        self->blendMode = params->blendMode;
        self->animSet = params->animSet;
        self->unk5A = params->unk5A;
        self->step = variant + 1;
        self->ext.et_801A518C.anim = params->anim;
        zPriority = self->params & 0xFF00;
        if (zPriority) {
            self->zPriority = zPriority >> 8;
        }
        if (self->params & 0xF0) {
            self->palette = PAL_FLAG(PAL_UNK_19F);
            self->blendMode = BLEND_TRANSP;
            self->facingLeft = 1;
        }
        break;
    case 1:
        switch (self->step_s) {
        case 0:
            self->drawFlags = ENTITY_ROTATE;
            self->velocityX = rcos(self->rotate) * 16;
            self->velocityY = rsin(self->rotate) * 16;
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            if (!AnimateEntity(self->ext.et_801A518C.anim, self)) {
                DestroyEntity(self);
            }
            break;
        }
        break;
    case 2:
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0xC0;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 8) - FIX(0.5);
            self->velocityY = FIX(-0.75);
            self->ext.et_801A518C.acceleration = -(Random() << 4) - 0x4000;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += self->ext.et_801A518C.acceleration;
        self->opacity--;
        if (!AnimateEntity(self->ext.et_801A518C.anim, self)) {
            DestroyEntity(self);
        }
        break;
    case 3:
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0x80;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 10) - FIX(2);
            self->velocityY = FIX(-2.25);
            self->ext.et_801A518C.acceleration = -(Random() << 4) - 0x2000;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += self->ext.et_801A518C.acceleration;
        self->opacity--;
        if (!AnimateEntity(self->ext.et_801A518C.anim, self)) {
            DestroyEntity(self);
        }
        break;
    case 4:
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0x80;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 9) - FIX(1);
            self->velocityY = FIX(-2.5);
            self->ext.et_801A518C.acceleration = -(Random() << 4) - 0x1000;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += self->ext.et_801A518C.acceleration;
        self->opacity--;
        if (!AnimateEntity(self->ext.et_801A518C.anim, self)) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180FE8[];
extern u8 D_us_80181004[];
extern u8 D_us_80181020[];
u8 D_us_80180FE8[] ALIGNED4 = {0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0xFF, 0x00};
u8 D_us_80181004[] ALIGNED4 = {0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0xFF, 0x00};
u8 D_us_80181020[] ALIGNED4 = {0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x04, 0x02, 0x05, 0x02, 0x06, 0x02, 0x07, 0x02, 0x08, 0x02, 0x09, 0x02, 0x0A, 0x02, 0x0B, 0x02, 0x0C, 0x02, 0x0D, 0xFF, 0x00, 0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x04, 0x02, 0x05, 0x02, 0x06, 0x02, 0x07, 0x02, 0x08, 0x02, 0x09, 0x02, 0x0A, 0x02, 0x0B, 0x02, 0x0C, 0x02, 0x0D, 0x02, 0x0E, 0xFF, 0x00, 0x00, 0x00};
ScyllaParticleParams D_us_8018105C[4] = {
    { 0xE, 0x79, 0, 0x30, D_us_80180FE8 },
    { 0x8003, 0x54, 9, 0x30, D_us_80181020 },
    { 0x8003, 0x54, 9, 0x30, D_us_80181004 },
    { 0x8003, 0x54, 9, 0x30, D_us_80181004 },
};
// end pc_import_data.py
// clang-format on
#endif
