// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo8.h"

extern EInit g_EInitBreakable;
extern AnimationFrame* D_us_80180BBC[];
extern u8 D_us_80180BDC[];
extern u16 D_us_80180BEC[];
extern u8 D_us_80180BE4[];
extern u8 D_us_80180BFC[];

void EntityBreakable(Entity* self) {
    Entity* entity;
    u16 breakableType;

    breakableType = self->params >> 12;
    if (self->step) {
        AnimateEntity(D_us_80180BBC[breakableType], self);
        if (self->hitParams) {
            g_api.PlaySfx(SFX_CANDLE_HIT);
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_EXPLOSION, entity);
                entity->params = D_us_80180BE4[breakableType];
            }
            ReplaceBreakableWithItemDrop(self);
        }
    } else {
        InitializeEntity(g_EInitBreakable);
        self->zPriority = g_unkGraphicsStruct.g_zEntityCenter - 20;
        self->blendMode = D_us_80180BFC[breakableType];
        self->hitboxHeight = D_us_80180BDC[breakableType];
        self->animSet = D_us_80180BEC[breakableType];
    }
}

#include "func_us_801955a0.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_1546C", func_us_801955F8);
#else
// 2 when a step that way would leave the room's floor
s32 func_us_801955F8(Entity* self, s32 facing) {
    s32 x;

    x = g_Tilemap.scrollX.i.hi + self->posX.i.hi;
    if (facing) {
        x += 0x50;
    } else {
        x -= 0x50;
    }
    if (x < 0x30 || x > 0x1F0) {
        return 2;
    }
    return 0;
}
#endif

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

void func_801CE228(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        g_CurrentEntity->ext.GH_Props.unkB0[i] = 0;
        g_CurrentEntity->ext.GH_Props.unkB4[i] = 0;
    }
}

void polarPlacePartsList(s16* offsets) {
    Entity* entity;

    while (*offsets) {
        entity = g_CurrentEntity + *offsets;
        if (!entity->ext.GH_Props.unkA8) {
            polarPlacePart(entity);
        }
        offsets++;
    }
}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180B74[];
extern u8 D_us_80180B7C[];
extern u8 D_us_80180B84[];
extern u8 D_us_80180B8C[];
extern u8 D_us_80180B94[];
extern u8 D_us_80180B9C[];
extern u8 D_us_80180BA4[];
extern u8 D_us_80180BAC[];
u8 D_us_80180B74[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180B7C[] ALIGNED4 = {0x04, 0x07, 0x04, 0x08, 0x04, 0x09, 0x00, 0x00};
u8 D_us_80180B84[] ALIGNED4 = {0x04, 0x1D, 0x04, 0x1E, 0x04, 0x1F, 0x00, 0x00};
u8 D_us_80180B8C[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180B94[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180B9C[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180BA4[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180BAC[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
AnimationFrame* D_us_80180BBC[8] = {
    &D_us_80180B74,
    D_us_80180B7C,
    D_us_80180B84,
    &D_us_80180B8C,
    &D_us_80180B94,
    &D_us_80180B9C,
    &D_us_80180BA4,
    D_us_80180BAC,
};
u8 D_us_80180BDC[8] = { 8, 8, 8, 8, 8, 8, 8, 8 };
u8 D_us_80180BE4[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u16 D_us_80180BEC[8] = { 3, 0x8001, 0x8001, 3, 3, 3, 3, 3 };
u8 D_us_80180BFC[0x10] = { 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0, 0, 0, 0, 0, 0, 0, 0 };
// end pc_import_data.py
// clang-format on
#endif
