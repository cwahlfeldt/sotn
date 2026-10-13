// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rlib.h"

extern EInit D_us_801806B8;
extern u8 D_us_80180BEC[];
extern u8 D_us_80180BFC[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rlib/nonmatchings/unk_228C0", func_us_801BAF60_from_lib);
#else
// LIB's EntityCandleTable for the inverted castle
void func_us_801BAF60_from_lib(Entity* self) {
    Entity* newEntity;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801806B8);
        self->zPriority = 0x74;
        if (self->params & 0x400) {
            self->animCurFrame = 0x11;
            self->step = 4;
            break;
        }
        if (self->params & 0x200) {
            self->animCurFrame = 0x12;
            self->step = 4;
            break;
        }
        if (self->params & 0x100) {
            self->blendMode = BLEND_TRANSP | BLEND_QUARTER;
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0xC0;
        } else {
            self->zPriority += 4;
            self->hitboxWidth = 8;
            self->hitboxHeight = 12;
            self->hitboxOffX = 0;
            self->hitboxOffY = 10;
            self->hitboxState = 2;
            newEntity = self + 1;
            CreateEntityFromEntity(E_UNK_29, self, newEntity);
            newEntity->params = 0x100;
        }
        // fallthrough
    case 1:
        if (self->params & 0x100) {
            AnimateEntity(D_us_80180BFC, self);
            break;
        }
        AnimateEntity(D_us_80180BEC, self);
        if (self->hitFlags) {
            SetStep(2);
        }
        break;
    case 2:
        newEntity = self + 1;
        CreateEntityFromEntity(E_PERSISTENT_ITEM_DROP, self, newEntity);
        newEntity->params = 6;
        newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (newEntity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
            newEntity->posY.i.hi -= 8;
            newEntity->params = 2;
        }
        self->hitboxState = 0;
        self->step++;
        // fallthrough
    case 3:
        self->animCurFrame = 8;
        // fallthrough
    case 4:
        break;
    case 255:
#include "../pad2_anim_debug.h"
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u8 D_us_80180BEC[0x10] = { 3, 1, 3, 2, 3, 3, 3, 4, 3, 5, 3, 6, 3, 7, 0, 0 };
u8 D_us_80180BFC[0x18] = {
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
    3,
    0xE,
    3,
    0xF,
    3,
    0x10,
    0,
    0,
    0,
    0,
    3,
    0,
    0,
    0,
};
// end pc_import_data.py
// clang-format on
#endif
