// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo3.h"

extern EInit g_EInitCommon;
extern EInit g_EInitInteractable;
extern EInit g_EInitSpawner;

u16 GetAngleBetweenEntities(Entity* a, Entity* b);
u16 LimitAngleChange(u16 delta, u16 base, u16 target);

static u8 D_us_801806E4[] = {0x04, 0x01, 0x04, 0x02, 0x00};
static u8 D_us_801806EC[] = {0x04, 0x00, 0x04, 0x00, 0x00};
static u8 D_us_801806F4[] = {
    0x05, 0x01, 0x05, 0x02, 0x05, 0x03, 0x05, 0x04, 0x00};
static u8 D_us_80180700[] = {
    0x05, 0x05, 0x05, 0x06, 0x05, 0x07, 0x05, 0x08, 0x00};
static u8 D_us_8018070C[] = {0x00, 0x00, 0x00, 0x00};
static u8 D_us_80180710[] = {
    0x05, 0x0D, 0x05, 0x0E, 0x05, 0x0F, 0x05, 0x10, 0x00};
static u8 D_us_8018071C[] = {0x05, 0x11, 0x05, 0x12, 0x05, 0x13, 0x00};
static u8 D_us_80180724[] = {0x05, 0x17, 0x00, 0x00};
static u8 D_us_80180728[] = {0x05, 0x16, 0x00, 0x00};
static u8 D_us_8018072C[] = {
    0x05, 0x14, 0xFF, 0xFF, 0x05, 0x15, 0x05, 0x15, 0xFF, 0x00, 0x00, 0x00};
static u8* anims[] = {
    D_us_801806E4, D_us_801806EC, D_us_801806F4, D_us_80180700, D_us_8018070C,
    D_us_80180710, D_us_8018071C, D_us_80180724, D_us_80180728, D_us_8018072C};

static u8 hitbox_heights[] = {
    0x08, 0x08, 0x28, 0x18, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00};
static u8 params_arr[] = {
    0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00};
static u16 palettes[] = {
    PAL_NULL, PAL_NULL, 0x228, 0x228, 0x228, 0x228, 0x228, 0x228, 0x228, 0x228};
static u16 anim_sets[] = {
    ANIMSET_DRA(0x3), ANIMSET_DRA(0x3), ANIMSET_OVL(0xB), ANIMSET_OVL(0xB),
    ANIMSET_OVL(0xB), ANIMSET_OVL(0xB), ANIMSET_OVL(0xB), ANIMSET_OVL(0xB),
    ANIMSET_OVL(0xB), ANIMSET_OVL(0xB)};
static u16 unk5a_arr[] = {0x0000, 0x007C, 0x005B, 0x005B, 0x005B,
                          0x005B, 0x005B, 0x005B, 0x005B, 0x005B};
static u8 blend_modes[] = {
    DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE,
    DRAW_TPAGE2 | DRAW_TPAGE};
static u16 hitbox_y_offsets[] = {0x0000, 0x0000, 0xFFE8, 0xFFF0, 0x0000,
                                 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

#define E_BREAKABLE_RELIC E_ID(UNK_43)

#include "../../st/e_breakable_no4.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C123C_from_no4);
#else
void func_us_801C123C_from_no4(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = ANIMSET_OVL(0xB);
        self->unk5A = 0x5B;
        self->palette = 0x228;
        self->animCurFrame = 0x15;
        self->zPriority = 0x6A;
        self->step = 0x100;
        break;
    }
}
#endif

extern u8 D_us_8018080C[];
extern u8 D_us_80180824[];
extern s16 D_us_80180830[];

void func_us_801C12B0_from_no4(Entity* self) {
    u32 var_s8;
    s16* ptr;
    s32 var_s6;
    u8* var_s5;
    s32 var_s4;
    s32 var_s3;
    s32 var_s2;
    s32 var_s1;

    Primitive* prim;
    s32 primIndex;
    s32 scrollX;
    s32 scrollY;
    s32 i;
    s32 xOffset;
    s32 sp38;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        self->ext.et_801C12B0.unk80 = 4;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 16);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        self->ext.et_801C12B0.unk82 = 0;
        while (prim != NULL) {
            prim->tpage = 0xF;
            prim->clut = 0x5E;
            prim->priority = 0x9C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        break;
    }

    prim = self->ext.et_801C12B0.prim;
    AnimateEntity(D_us_80180824, self);
    i = (self->params >> 8) & 0xFF;
    ptr = &D_us_80180830[(self->params & 0xFF) * 5];
    scrollX = g_Tilemap.scrollX.i.hi - 0x10;
    scrollY = g_Tilemap.scrollY.i.hi;
    xOffset = scrollX + 0x120;
    var_s5 = &D_us_8018080C[self->animCurFrame * 4];
    var_s8 = var_s5[1];

    for (; i > 0; i--) {
        var_s3 = *ptr++;
        var_s2 = var_s3 + *ptr++;
        if (scrollX >= var_s2 || xOffset < var_s3) {
            ptr += 3;
            continue;
        }

        var_s6 = *ptr++;
        if (scrollY - 4 > var_s6 || var_s6 > scrollY + 0xE0) {
            ptr += 2;
            continue;
        } else {
            ptr++;
            sp38 = *ptr++;
            var_s6 -= scrollY;
            if (var_s3 < scrollX) {
                var_s3 = scrollX;
            }
            if (xOffset < var_s2) {
                var_s2 = xOffset;
            }
            if (sp38 != 0) {
                self->ext.et_801C12B0.unk82 += sp38;
                var_s4 = (u32)(var_s3 - self->ext.et_801C12B0.unk82) % 126;
            } else {
                var_s4 = (u32)var_s3 % 126;
            }
            var_s2 -= var_s3;
            var_s3 -= scrollX + 0x10;
            var_s4 += var_s5[0];

            do {
                prim->u0 = prim->u2 = var_s4;
                var_s1 = 0x7E - (var_s4 - var_s5[0]);
                if (var_s2 < var_s1) {
                    var_s1 = var_s2;
                }
                prim->u1 = prim->u3 = var_s4 + var_s1;
                prim->v0 = prim->v1 = var_s8;
                prim->v2 = prim->v3 = var_s8 + 1;
                prim->x0 = prim->x2 = var_s3;
                var_s3 += var_s1;
                prim->x1 = prim->x3 = var_s3;
                var_s2 -= var_s1;
                var_s4 = var_s5[0];
                prim->y0 = prim->y1 = var_s6;
                prim->y2 = prim->y3 = var_s6 + 1;
                prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
                prim = prim->next;
            } while (var_s2 != 0);
        }
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

extern s16 D_us_8018092C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C15F8_from_no4);
#else
void func_us_801C15F8_from_no4(Entity* self) {
    s32 scrollX;
    s32 scrollY;
    s16* ptr;
    s32 var_s5;
    s32 var_s4;
    s32 var_s3;
    s32 var_s2;
    s32 var_s1;
    Primitive* prim;
    s32 primIndex;
    s32 xOffset;
    s32 yOffset;
    s32 i;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        self->ext.et_801C12B0.unk80 = 4;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 16);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->r0 = 0;
            prim->g0 = 0x10;
            prim->b0 = 0x20;
            prim->priority = 0x9B;
            prim = prim->next;
        }
        break;
    }

    prim = self->ext.et_801C12B0.prim;
    ptr = &D_us_8018092C[(self->params & 0xFF) * 4];
    i = (self->params >> 8) & 0xFF;
    scrollX = g_Tilemap.scrollX.i.hi - 0x10;
    scrollY = g_Tilemap.scrollY.i.hi - 0x10;
    xOffset = scrollX + 0x120;
    yOffset = scrollY + 0x100;

    for (; i > 0; i--) {
        var_s3 = *ptr++;
        var_s2 = var_s3 + *ptr++;
        if (scrollX >= var_s2 || xOffset < var_s3) {
            ptr += 2;
            continue;
        }

        var_s5 = *ptr++;
        var_s4 = *ptr++;
        if (var_s4 > scrollY && yOffset >= var_s5) {
            if (var_s3 < scrollX) {
                var_s3 = scrollX;
            }
            if (xOffset < var_s2) {
                var_s2 = xOffset;
            }

            var_s2 -= var_s3;
            var_s3 -= scrollX + 0x10;

            if (var_s5 < scrollY) {
                var_s5 = scrollY;
            }
            if (yOffset < var_s4) {
                var_s4 = yOffset;
            }

            var_s4 -= var_s5;
            var_s5 -= scrollY + 0x10;
            if (var_s4 >= 0x100) {
                var_s4 = 0xFF;
            }

            do {
                var_s1 = var_s2;
                if (var_s1 >= 0x100) {
                    var_s1 = 0xFF;
                }
                prim->u0 = var_s1;
                prim->v0 = var_s4;
                prim->x0 = var_s3;
                prim->y0 = var_s5;
                var_s3 += var_s1;
                var_s2 -= var_s1;
                prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
                prim = prim->next;
            } while (var_s2 != 0);
        }
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}
#endif

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C1844_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", EntityBgColumnsParallax_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C1EE4_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C21AC_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_8019E398);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C2850_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C2B78_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C2E60_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_8019F03C);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C34EC_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C37C8_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C3A04_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C3CC4_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C3FB0_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C4228_from_no4);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", EntityWaterBox);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A051C);

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A069C);

extern s16 D_us_80180C64;
extern s16 D_us_80180C68[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A071C);
#else
// Moves the water line. Three tables follow it: the surface line drawn by
// 0x1C (its entries 22 to 24, at 0x80180910), the dark water boxes drawn by
// 0x1D (entries 27 to 29, at 0x80180A08) and the areas func_us_801A09DC
// checks Alucard against.
void func_us_801A071C(void) {
    s16* lines = &D_us_80180830[22 * 5 + 2];
    s16* boxes = &D_us_8018092C[27 * 4 + 2];
    s16* areas = &D_us_80180C68[2];
    s16 y;

    y = 0xD8 - D_us_80180C64;
    if (y < 0x18) {
        y = 0;
    }
    areas[0] = y;
    boxes[0] = y;
    lines[0] = y;
    if (D_us_80180C64 < 0xC0) {
        y = 0xF0;
    } else {
        y = 0x1A8 - D_us_80180C64;
    }
    areas[4] = y;
    areas[8] = y;
    if (y > 0xE8) {
        y = 0xE8;
    }
    lines[5] = y;
    lines[10] = y;
    y++;
    boxes[4] = y;
    boxes[8] = y;
}
#endif

extern u16 D_us_80180C66;
extern s16 D_us_801B2870;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A07CC);
#else
// Raises the water when Alucard walks in and lowers it once Scylla is dead
void func_us_801A07CC(Entity* self) {
    D_us_801B2870 = 0;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        if (D_us_80180C64) {
            if (g_CastleFlags[SCYLLA_DEFEATED]) {
                self->step = 5;
            } else {
                self->step = 3;
            }
        } else if (g_CastleFlags[SCYLLA_DEFEATED]) {
            self->step = 6;
        }
        break;
    case 1:
        if (PLAYER.posX.i.hi >= 0x38) {
            self->ext.timer.t = 0x10;
            D_us_80180C66 |= 1;
            self->step++;
        }
        break;
    case 2:
        if (!--self->ext.timer.t) {
            self->step++;
        }
        break;
    case 3:
        if (self->ext.timer.t) {
            self->ext.timer.t--;
        } else {
            self->ext.timer.t = 3;
            if (D_us_80180C64 < 0xD0) {
                D_us_80180C64++;
                D_us_801B2870 = -1;
            } else {
                self->step++;
            }
        }
        func_us_801A071C();
        break;
    case 4:
        if (g_CastleFlags[SCYLLA_DEFEATED]) {
            D_us_80180C66 ^= 1;
            self->step++;
        }
        break;
    case 5:
        if (self->ext.timer.t) {
            self->ext.timer.t--;
        } else {
            self->ext.timer.t = 1;
            if (D_us_80180C64 > 0) {
                D_us_80180C64--;
                D_us_801B2870 = 1;
            } else {
                self->step++;
            }
        }
        func_us_801A071C();
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A09DC);
#else
// Like water_effects.h's func_801C4144 for the areas of D_us_80180C68
u16 func_us_801A09DC(s16 index, s16 x, s16 y, s16* out) {
    s16* area = &D_us_80180C68[index * 4];

    x -= area[0];
    if (x < 0) {
        return 0;
    }
    out[0] = x;
    x = area[1] - x;
    if (x <= 0) {
        return 0;
    }
    out[1] = x;
    if (y >= area[2] && y < area[3]) {
        return (y + 0x8000) - area[2];
    }
    return 0;
}
#endif

extern s16 D_us_801B2874;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A0A80);
#else
// A simpler water_effects.h EntityAlucardWaterEffect: splashes when Alucard
// falls in or jumps out, ripples while he walks in the water
void func_us_801A0A80(Entity* self) {
    s16 sp10[2];
    s16 posX;
    s16 posY;
    u16 inWater;
    Entity* newEntity;
    s32 status;
    s16 depth;
    s16 index;
    s16 i;
    s16 count;
    u16 params;
    s32 surfaceDepth;
    s32 bodyOffsetY;

    sp10[0] = sp10[1] = 0;
    inWater = 0;
    status = g_Player.status;
    posX = PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi;
    if (status & (PLAYER_STATUS_CROUCH | PLAYER_STATUS_TRANSFORM)) {
        if (status & PLAYER_STATUS_CROUCH) {
            surfaceDepth = 0x14;
            bodyOffsetY = 0x19;
            if (status & PLAYER_STATUS_WOLF_FORM) {
                surfaceDepth = 0xA;
            }
        } else if (status &
                   (PLAYER_STATUS_MIST_FORM | PLAYER_STATUS_BAT_FORM)) {
            surfaceDepth = 0xC;
            bodyOffsetY = 5;
        } else {
            surfaceDepth = 0x14;
            bodyOffsetY = 0x19;
        }
    } else {
        surfaceDepth = 0x28;
        bodyOffsetY = 0x19;
    }
    params = self->params;
    posY = bodyOffsetY + (PLAYER.posY.i.hi + g_Tilemap.scrollY.i.hi);
    count = (s16)params >> 8;
    index = params & 0xFF;
    for (i = 0; i < count; i++, index++) {
        inWater = func_us_801A09DC(index, posX, posY, sp10);
        if (inWater) {
            break;
        }
    }
    depth = inWater & 0x7FFF;
    if (self->step) {
        if (F(PLAYER.velocityY).i.hi &&
            !(status & (PLAYER_STATUS_MIST_FORM | PLAYER_STATUS_BAT_FORM))) {
            if (F(PLAYER.velocityY).i.hi < 0) {
                if (!inWater && D_us_801B2874 &&
                    (D_us_801B2874 & 0x7FFF) <= 0x10) {
                    if (self->ext.aluwater.unk8C < 14) {
                        params = (14 - self->ext.aluwater.unk8C) << 11;
                    } else if (self->ext.aluwater.unk8E < 14) {
                        params = (self->ext.aluwater.unk8E + 14) << 11;
                    } else {
                        params = 0;
                    }
                    newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                    if (newEntity != NULL) {
                        CreateEntityFromEntity(E_UNK_2F, &PLAYER, newEntity);
                        newEntity->posX.i.hi =
                            self->ext.aluwater.unk80 - g_Tilemap.scrollX.i.hi;
                        newEntity->posY.i.hi =
                            self->ext.aluwater.unk82 -
                            (D_us_801B2874 & 0x7FFF) - g_Tilemap.scrollY.i.hi;
                        newEntity->zPriority = PLAYER.zPriority;
                        if (PLAYER.velocityY > FIX(-4)) {
                            newEntity->params = params + 1;
                        } else {
                            newEntity->params = params;
                        }
                    }
                }
            } else if (inWater && depth < 9 && !D_us_801B2874) {
                if (sp10[0] < 14) {
                    params = (14 - sp10[0]) << 11;
                } else if (sp10[1] < 14) {
                    params = (sp10[1] + 14) << 11;
                } else {
                    params = 0;
                }
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_2F, &PLAYER, newEntity);
                    newEntity->params = params;
                    newEntity->posY.i.hi += bodyOffsetY - depth;
                    newEntity->zPriority = PLAYER.zPriority;
                }
            }
        } else if (!(status & PLAYER_STATUS_MIST_FORM) && inWater &&
                   !self->ext.aluwater.unk7E &&
                   posX != self->ext.aluwater.unk80 &&
                   depth <= surfaceDepth && sp10[0] >= 6 && sp10[1] >= 6) {
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_30, &PLAYER, newEntity);
                newEntity->posY.i.hi += bodyOffsetY - depth;
                if (PLAYER.velocityX != 0) {
                    newEntity->params = (surfaceDepth - depth) >> 3;
                    if (newEntity->params == 5) {
                        newEntity->params = 4;
                    }
                } else {
                    newEntity->params = 0;
                }
                newEntity->ext.waterEffects.unk8A = 0;
                newEntity->zPriority = PLAYER.zPriority;
                self->ext.aluwater.unk7E = 8;
            }
        }
        if (self->ext.aluwater.unk7E) {
            self->ext.aluwater.unk7E--;
        }
        g_unkGraphicsStruct.D_80097448 = depth;
        if (status & (PLAYER_STATUS_CROUCH | PLAYER_STATUS_TRANSFORM)) {
            if (status & PLAYER_STATUS_CROUCH) {
                if (status & PLAYER_STATUS_WOLF_FORM) {
                    if (depth > 4) {
                        g_unkGraphicsStruct.D_8009744C = depth - 4;
                    } else {
                        g_unkGraphicsStruct.D_8009744C = 0;
                    }
                    if (depth > 12) {
                        g_unkGraphicsStruct.D_80097450 = depth - 12;
                    } else {
                        g_unkGraphicsStruct.D_80097450 = 0;
                    }
                } else {
                    if (depth > 8) {
                        g_unkGraphicsStruct.D_8009744C = depth - 8;
                    } else {
                        g_unkGraphicsStruct.D_8009744C = 0;
                    }
                    if (depth > 0x18) {
                        g_unkGraphicsStruct.D_80097450 = depth - 0x18;
                    } else {
                        g_unkGraphicsStruct.D_80097450 = 0;
                    }
                }
            } else if (status &
                       (PLAYER_STATUS_MIST_FORM | PLAYER_STATUS_BAT_FORM)) {
                if (depth > 6) {
                    g_unkGraphicsStruct.D_8009744C = depth - 6;
                } else {
                    g_unkGraphicsStruct.D_8009744C = 0;
                }
                if (depth > 0x10) {
                    g_unkGraphicsStruct.D_80097450 = depth - 0x10;
                } else {
                    g_unkGraphicsStruct.D_80097450 = 0;
                }
            } else if (status & PLAYER_STATUS_WOLF_FORM) {
                if (depth > 8) {
                    g_unkGraphicsStruct.D_8009744C = depth - 8;
                } else {
                    g_unkGraphicsStruct.D_8009744C = 0;
                }
                if (depth > 0x18) {
                    g_unkGraphicsStruct.D_80097450 = depth - 0x18;
                } else {
                    g_unkGraphicsStruct.D_80097450 = 0;
                }
            }
        } else {
            if (depth > 0x10) {
                g_unkGraphicsStruct.D_8009744C = depth - 0x10;
            } else {
                g_unkGraphicsStruct.D_8009744C = 0;
            }
            if (depth > 0x30) {
                g_unkGraphicsStruct.D_80097450 = depth - 0x30;
            } else {
                g_unkGraphicsStruct.D_80097450 = 0;
            }
        }
        FntPrint("mizu %x\n", depth);
    } else {
        InitializeEntity(g_EInitSpawner);
    }
    self->ext.aluwater.unk80 = posX;
    self->ext.aluwater.unk82 = posY;
    self->ext.aluwater.unk8C = sp10[0];
    D_us_801B2874 = inWater;
    self->ext.aluwater.unk8E = sp10[1];
}
#endif

extern s32 D_us_80180C80[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A1120);
#else
// A simpler water_effects.h EntitySplashWater that rides the water line
void func_us_801A1120(Entity* self) {
    Primitive* prim;
    Primitive* prim2;
    Entity* newEntity;
    s32 primIndex;
    u16 params;
    u16 width;
    s16 selfX;
    s16 selfY;
    s16 pan;
    s32 i;

    params = self->params;
    width = params >> 11;
    params &= 0xF;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCommon);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 2);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->flags |= FLAG_HAS_PRIMS;
        selfX = self->posX.i.hi;
        selfY = self->posY.i.hi;
        self->ext.waterEffects.unk82 = selfY + g_Tilemap.scrollY.i.hi;
        for (i = 0; prim != NULL; i++, prim = prim->next) {
            if (i & 1) {
                prim->u0 = prim->u2 = prim2->u0;
                prim->u1 = prim->u3 = prim2->u1;
                prim->v0 = prim->v1 = prim2->v0;
                prim->v2 = prim->v3 = prim2->v2;
                prim->y2 = prim2->y2;
                prim->y3 = prim2->y3;
                prim->x0 = prim->x2 = prim2->x0;
                prim->x1 = prim->x3 = prim2->x1;
            } else {
                prim->u1 = prim->u3 = 0x20;
                prim->u0 = prim->u2 = 0;
                prim->v0 = prim->v1 = 0x60;
                prim->v2 = prim->v3 = 0x7C;
                prim->y2 = prim->y3 = selfY;
                prim->x0 = prim->x2 = selfX - 0xE;
                prim->x1 = prim->x3 = selfX + 0xE;
                if (width) {
                    if (width > 14) {
                        prim->u1 = prim->u3 = prim->u0 + (width << 5) / 28;
                        prim->x1 = prim->x3 = prim->x0 + width;
                    } else {
                        prim->u0 = prim->u2 += (width << 5) / 28;
                        prim->x0 = prim->x2 += width;
                    }
                }
            }
            PGREY(prim, 0) = PGREY(prim, 1) = 0xFF;
            PGREY(prim, 2) = PGREY(prim, 3) = 0x80;
            prim->clut = PAL_CC_MAGIC_HUD_EFFECT;
            prim->tpage = 0x1A;
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
            prim->priority = self->zPriority + 2;
            if (i & 1) {
                prim->clut = PAL_FILL_WHITE;
                PGREY(prim, 0) = PGREY(prim, 1) = 0x00;
                PGREY(prim, 2) = PGREY(prim, 3) = 0x60;
                prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                                 DRAW_UNK02 | DRAW_TRANSP;
                prim->priority += 2;
            }
            prim2 = prim;
        }
        pan = (self->posX.i.hi - 0x78) >> 4;
        if (pan < -8) {
            pan = -8;
        }
        if (pan > 8) {
            pan = 8;
        }
        g_api.PlaySfxVolPan(SFX_WATER_SPLASH_JUMP, 0x7F, pan);
        self->velocityY = D_us_80180C80[params * 2];
        self->ext.waterEffects.accelY = D_us_80180C80[params * 2 + 1];
        newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (newEntity != NULL) {
            CreateEntityFromCurrentEntity(E_WATER_DROP, newEntity);
            newEntity->velocityY = self->velocityY;
        }
        break;
    case 1:
        MoveEntity();
        self->velocityY += self->ext.waterEffects.accelY;
        if (self->velocityY > FIX(2.5)) {
            self->step++;
        }
        break;
    case 2:
        MoveEntity();
        prim = &g_PrimBuf[self->primIndex];
        if (prim->r0 < 9) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    selfY = self->ext.waterEffects.unk82 - self->posY.i.hi -
            g_Tilemap.scrollY.i.hi;
    for (i = 0, prim = &g_PrimBuf[self->primIndex]; prim != NULL; i++,
        prim = prim->next) {
        prim->y2 = prim->y3 += D_us_801B2870;
        prim->y0 = prim->y2 - selfY;
        prim->y1 = prim->y3 - selfY;
        if (i & 1) {
            if (prim->b3 >= 4) {
                prim->b3 -= 4;
            }
            PGREY(prim, 2) = PGREY(prim, 3);
        } else {
            if (prim->b3 >= 8) {
                prim->b3 -= 4;
            }
            PGREY(prim, 2) = PGREY(prim, 3);
            if (prim->b3 <= 8) {
                if (prim->b1 >= 8) {
                    prim->b1 -= 8;
                }
                PGREY(prim, 0) = PGREY(prim, 1);
            }
        }
    }
}
#endif

extern s16 D_us_80180CA0[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A16E4);
#else
// A simpler water_effects.h EntitySurfacingWater that rides the water line
void func_us_801A16E4(Entity* self) {
    Primitive* prim;
    s32 primIndex;
    s16 pan;
    s16 x;
    s16 y;
    s32 i;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCommon);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 2);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->primIndex = primIndex;
        self->flags |= FLAG_HAS_PRIMS;
        pan = (self->posX.i.hi - 0x78) >> 4;
        if (pan < -8) {
            pan = -8;
        }
        if (pan > 8) {
            pan = 8;
        }
        if (!(self->params & 0x8000)) {
            g_api.PlaySfxVolPan(SFX_WATER_SPLASH_MOVE, 0x7F, pan);
        }
        prim = &g_PrimBuf[primIndex];
        x = self->posX.i.hi;
        y = self->posY.i.hi;
        self->ext.waterEffects.unk82 = y + g_Tilemap.scrollY.i.hi;
        for (i = 0; i < 2; i++) {
            prim->clut = PAL_CC_MAGIC_HUD_EFFECT;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x1E;
            prim->v0 = prim->v1 = 0x60;
            prim->v2 = prim->v3 = 0x7C;
            prim->y2 = prim->y3 = y;
            prim->x0 = prim->x2 = x - 9;
            prim->x1 = prim->x3 = x + 9;
            PGREY(prim, 0) = PGREY(prim, 1) = 0xFF;
            PGREY(prim, 2) = PGREY(prim, 3) = 0x80;
            prim->tpage = 0x1A;
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
            prim->priority = self->zPriority + 2;
            if (i != 0) {
                prim->clut = PAL_CC_STONE_EFFECT;
                prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                                 DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
                prim->priority = self->zPriority + 4;
            }
            prim = prim->next;
        }
        self->ext.waterEffects.topY.i.hi =
            D_us_80180CA0[self->params & 0xFF] + 12 + (rand() & 1);
        break;
    case 1:
        self->ext.waterEffects.topY.val -= FIX(0.25);
        break;
    }

    x = self->posX.i.hi;
    self->posY.i.hi += D_us_801B2870;
    y = self->ext.waterEffects.unk82 - self->posY.i.hi - g_Tilemap.scrollY.i.hi;
    prim = &g_PrimBuf[self->primIndex];
    for (i = 0; i < 2; i++) {
        prim->y2 -= y;
        prim->y3 -= y;
        prim->y0 = prim->y2 - self->ext.waterEffects.topY.i.hi;
        prim->x0 = prim->x2 = x - 9;
        prim->x1 = prim->x3 = x + 9;
        prim->b1 -= 8;
        PGREY(prim, 0) = PGREY(prim, 1);
        prim->b3 -= 4;
        PGREY(prim, 2) = PGREY(prim, 3);
        prim->y1 = prim->y3 - self->ext.waterEffects.topY.i.hi;
        if (prim->r0 < 9) {
            DestroyEntity(self);
            return;
        }
        prim = prim->next;
    }
    self->ext.waterEffects.unk82 = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
}
#endif

INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801C5518_from_no4);

extern s16 D_us_80180CC4[];
extern u16 D_us_80180CD8[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801B5004_from_rbo5);
#else
void func_us_801B5004_from_rbo5(Entity* self, s32 index) {
    Tilemap* tilemap;
    s16 tilePos;
    u16* tileData;
    s32 i;

    tilemap = &g_Tilemap;
    tilePos = D_us_80180CC4[index >> 1];
    tileData = &D_us_80180CD8[index << 2];
    for (i = 0; i < 4; i++) {
        tilemap->fg[tilePos] = *tileData++;
        tilePos += tilemap->hSize << 4;
    }
}
#endif

extern u8 D_us_80180CD0[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A1BE8);
#else
// The gates that drop when a fight starts (D_us_80180C66 bit params)
void func_us_801A1BE8(Entity* self) {
    u16 params;
    s32 closed;
    s16 posY;

    params = self->params;
    closed = D_us_80180C66 & (1 << params);
    params <<= 1;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCommon);
        self->animSet = ANIMSET_OVL(1);
        self->animCurFrame = 0x25;
        self->zPriority = 0x9E;
        self->facingLeft = D_us_80180CD0[params >> 1];
        if (closed) {
            func_us_801B5004_from_rbo5(self, params + 1);
            self->step = 4;
            break;
        }
        func_us_801B5004_from_rbo5(self, params);
        self->posY.i.hi -= 0x40;
        break;
    case 1:
        if (closed) {
            self->velocityY = FIX(4.0);
            self->step++;
        }
        break;
    case 2:
        if (self->facingLeft) {
            self->posX.i.hi += 8;
        } else {
            self->posX.i.hi -= 8;
        }
        GetPlayerCollisionWith(self, 0x10, 0x20, 0xB);
        if (self->facingLeft) {
            self->posX.i.hi -= 8;
        } else {
            self->posX.i.hi += 8;
        }
        MoveEntity();
        posY = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
        if (posY >= 0x80) {
            g_api.PlaySfxVolPan(SFX_EXPLODE_B, 0x7F, -6);
            func_us_801B5004_from_rbo5(self, params + 1);
            self->velocityY = FIX(-1.0);
            self->step++;
        }
        break;
    case 3:
        self->velocityY += FIX(1.0 / 8);
        MoveEntity();
        posY = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
        if (posY >= 0x80) {
            self->posY.i.hi = 0x80 - g_Tilemap.scrollY.i.hi;
            self->step++;
        }
        break;
    case 4:
        if (!closed) {
            self->velocityY = FIX(-1.0);
            self->step++;
            func_us_801B5004_from_rbo5(self, params);
        }
        break;
    case 5:
        if (self->facingLeft) {
            self->posX.i.hi += 8;
        } else {
            self->posX.i.hi -= 8;
        }
        GetPlayerCollisionWith(self, 0x10, 0x20, 0xB);
        if (self->facingLeft) {
            self->posX.i.hi -= 8;
        } else {
            self->posX.i.hi += 8;
        }
        MoveEntity();
        posY = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
        if (posY <= 0x40) {
            self->step++;
        }
        break;
    }
}
#endif

extern s16 D_us_80180D38[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", EntitySplashWater);
#else
// water_effects.h's EntitySplashWater with BO3's tables
void EntitySplashWater(Entity* self) {
    s32 primIndex;
    Tilemap* tilemap = &g_Tilemap;
    u16 params;
    u16 index1;
    u16 width;
    s16 aspect;
    u16 index2;
    Primitive *prim, *prim2;
    Entity* newEntity;
    s16 selfY;
    s16 selfX;
    s32 i;

    params = self->params;
    width = params / 0x800;
    index1 = (params >> 8) & 7;
    index2 = (params >> 5) & 7;
    params &= 0xF;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCommon);
        if (width && index2 != 7) {
            primIndex = g_api.AllocPrimitives(PRIM_GT4, 4);
        } else {
            primIndex = g_api.AllocPrimitives(PRIM_GT4, 2);
        }
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        i = 0;
        selfX = self->posX.i.hi;
        selfY = self->posY.i.hi;
        self->ext.waterEffects.unk82 = selfY + tilemap->scrollY.i.hi;
        for (; prim != NULL; i++, prim = prim->next) {
            if (i & 1) {
                prim->u0 = prim->u2 = prim2->u0;
                prim->u1 = prim->u3 = prim2->u1;
                prim->v0 = prim->v1 = prim2->v0;
                prim->v2 = prim->v3 = prim2->v2;
                prim->y2 = prim2->y2;
                prim->y3 = prim2->y3;
                prim->x2 = prim->x0 = prim2->x0;
                prim->x3 = prim->x1 = prim2->x1;
            } else {
                prim->u0 = prim->u2 = 0;
                prim->u1 = prim->u3 = 0x20;
                prim->v0 = prim->v1 = 96;
                prim->v2 = prim->v3 = 0x7C;
                prim->y2 = prim->y3 = selfY;
                prim->x2 = prim->x0 = selfX - 0xE;
                prim->x3 = prim->x1 = selfX + 0xE;
                if (i > 1) {
                    aspect = D_us_80180D38[index2];
                    if (width > 14) {
                        prim->u0 = prim->u2 = prim2->u1;
                        prim->x0 = prim->x2 = prim2->x1;
                        prim->y2 = prim2->y3;
                        if (aspect) {
                            prim->y3 =
                                prim->y2 - (prim->x1 - prim->x0) / aspect;
                        } else {
                            prim->y3 = prim->y2;
                        }
                    } else {
                        prim->u1 = prim->u3 = prim2->u0;
                        prim->x1 = prim->x3 = prim2->x0;
                        prim->y3 = prim2->y2;
                        if (aspect) {
                            prim->y2 =
                                prim->y3 + (prim->x1 - prim->x0) / aspect;
                        } else {
                            prim->y2 = prim->y3;
                        }
                    }
                } else {
                    if (width) {
                        if (width > 14) {
                            prim->u1 = prim->u3 = prim->u0 + (width << 5) / 28;
                            prim->x1 = prim->x3 = prim->x0 + width;
                        } else {
                            prim->u0 = prim->u2 += (width << 5) / 28;
                            prim->x0 = prim->x2 += width;
                        }
                    }
                    if (index1) {
                        aspect = D_us_80180D38[index1];
                        if (aspect < 0) {
                            if (selfX >= prim->x1) {
                                prim->y2 += (prim->x1 - prim->x0) / aspect;
                            } else {
                                prim->y2 += (selfX - prim->x0) / aspect;
                                prim->y3 -= (prim->x1 - selfX) / aspect;
                            }
                        } else if (prim->x0 >= selfX) {
                            prim->y3 -= (prim->x1 - prim->x0) / aspect;
                        } else {
                            prim->y2 += (selfX - prim->x0) / aspect;
                            prim->y3 -= (prim->x1 - selfX) / aspect;
                        }
                    }
                }
            }
            PGREY(prim, 0) = PGREY(prim, 1) = 0xFF;
            PGREY(prim, 2) = PGREY(prim, 3) = 0x80;
            prim->clut = PAL_CC_MAGIC_HUD_EFFECT;
            prim->tpage = 0x1A;
            prim->priority = self->zPriority + 2;
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
            if (i & 1) {
                PGREY(prim, 0) = PGREY(prim, 1) = 0x00;
                PGREY(prim, 2) = PGREY(prim, 3) = 0x60;
                prim->clut = PAL_FILL_WHITE;
                prim->priority += 2;
                prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                                 DRAW_UNK02 | DRAW_TRANSP;
            }
            prim2 = prim;
        }

        aspect = (self->posX.i.hi - 120) >> 4;
        if (aspect < -8) {
            aspect = -8;
        }
        if (aspect > 8) {
            aspect = 8;
        }
        g_api.PlaySfxVolPan(SFX_WATER_SPLASH_JUMP, 0x7F, aspect);

        self->velocityY = D_us_80180C80[params * 2];
        self->ext.waterEffects.accelY = D_us_80180C80[params * 2 + 1];

        newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (newEntity != NULL) {
            CreateEntityFromCurrentEntity(E_WATER_DROP, newEntity);
            newEntity->velocityY = self->velocityY;
        }
        break;

    case 1:
        MoveEntity();
        self->velocityY += self->ext.waterEffects.accelY;
        if (self->velocityY > FIX(2.5)) {
            self->step++;
        }
        break;

    case 2:
        MoveEntity();
        prim = &g_PrimBuf[self->primIndex];
        if (prim->r0 < 9) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    selfY =
        self->ext.waterEffects.unk82 - self->posY.i.hi - tilemap->scrollY.i.hi;
    for (i = 0, prim = &g_PrimBuf[self->primIndex]; prim != NULL; i++,
        prim = prim->next) {
        prim->y0 = prim->y2 - selfY;
        prim->y1 = prim->y3 - selfY;
        if (i & 1) {
            if (prim->b3 >= 4) {
                prim->b3 -= 4;
            }
            PGREY(prim, 2) = PGREY(prim, 3);
        } else {
            if (prim->b3 >= 8) {
                prim->b3 -= 4;
            }
            PGREY(prim, 2) = PGREY(prim, 3);
            if (prim->b3 <= 8) {
                if (prim->b1 >= 8) {
                    prim->b1 -= 8;
                }
                PGREY(prim, 0) = PGREY(prim, 1);
            }
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", EntityWaterDrop);
#else
#include "../../st/entity_water_drop.h"
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A2A90);
#else
// Fades the two colours of a line; false once both are black
s32 func_us_801A2A90(Primitive* prim, s32 amount) {
    u8* color;
    s32 lit;
    s32 i;
    s32 j;
    s32 c;

    lit = 0;
    for (i = 0; i < 2; i++) {
        color = i ? &prim->r1 : &prim->r0;
        for (j = 0; j < 3; j++, color++) {
            c = *color - amount;
            if (c <= 0) {
                c = 0;
            } else {
                lit = 1;
            }
            *color = c;
        }
    }
    return lit;
}
#endif

extern s32 D_us_80180D48;
extern EInit g_EInitScylla;
extern u8 D_us_80180D4C[];
extern u8 D_us_80180D58[];
extern u8 D_us_80180D68[];
extern u8 D_us_80180D74[];
extern u8 D_us_80180D8C[];
extern u8 D_us_80180D9C[];
extern u8 D_us_80180DA8[];
extern u8 D_us_80180DB4[];
extern u8 D_us_80180DC8[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A2AEC);
#else
// Scylla. The next slot holds her wyrm (0x33); once the fight starts she
// fills the slots +2, +15 and +28 with three wyrms (0x3A) of 13 slots each.
void func_us_801A2AEC(Entity* self) {
    Entity* newEntity;
    Entity* entity;
    s32 i;
    s32 rnd;

    if (D_us_80180D48 & SCYLLA_REWARD_SPAWNED) {
        DestroyEntity(self);
        return;
    }
    if ((self->flags & FLAG_DEAD) && self->step != 6) {
        SetStep(6);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitScylla);
        entity = self + 1;
        self->zPriority = 0x88;
        self->hitboxOffX = 6;
        self->hitboxOffY = -0x30;
        self->hitboxWidth = 4;
        self->hitboxHeight = 0x1C;
        self->nextPart = entity;
        self->parent = NULL;
        CreateEntityFromEntity(E_UNK_33, self, entity);
        entity->nextPart = self;
        entity->parent = self;
        entity->zPriority = self->zPriority + 1;
        SetStep(1);
        // fallthrough
    case 1:
        AnimateEntity(D_us_80180D4C, self);
        if (GetDistanceToPlayerX() < 0xB0 || self->hitFlags) {
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_SCYLLA_DEFEAT, TIMEATTACK_SET_VISITED);
            D_us_80180C66 |= 0x30;
            entity = self + 2;
            CreateEntityFromEntity(E_UNK_3A, self, entity);
            entity->zPriority = self->zPriority;
            entity->posX.i.hi = 0x48 - g_Tilemap.scrollX.i.hi;
            entity->params = 1;
            entity->posY.i.hi = 0x90 - g_Tilemap.scrollY.i.hi;
            entity = self + 15;
            CreateEntityFromEntity(E_UNK_3A, self, entity);
            entity->zPriority = self->zPriority - 2;
            entity->posX.i.hi = 0x48 - g_Tilemap.scrollX.i.hi;
            entity->params = 2;
            entity->posY.i.hi = 0x90 - g_Tilemap.scrollY.i.hi;
            entity = self + 28;
            CreateEntityFromEntity(E_UNK_3A, self, entity);
            entity->zPriority = self->zPriority - 4;
            entity->posX.i.hi = 0x48 - g_Tilemap.scrollX.i.hi;
            D_us_80180D48 |= SCYLLA_FIGHT_STARTED;
            entity->params = 3;
            entity->posY.i.hi = 0x90 - g_Tilemap.scrollY.i.hi;
            SetStep(2);
        }
        break;
    case 2:
        if (!self->step_s) {
            self->ext.scylla.timer = 0x50;
            self->step_s++;
        }
        AnimateEntity(D_us_80180D4C, self);
        if (self->hitFlags & 3) {
            SetStep(5);
        }
        if (!--self->ext.scylla.timer) {
            SetStep(self->ext.scylla.nextAttack ? 4 : 3);
            self->ext.scylla.nextAttack ^= 1;
        }
        break;
    case 5:
        if (!self->step_s) {
            PlaySfxPositional(SFX_SCYLLA_PAIN);
            self->step_s++;
        }
        if (!AnimateEntity(D_us_80180DB4, self)) {
            SetStep(2);
            self->step_s = 1;
        }
        break;
    case 3: // fireball and water jets
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180D58, self)) {
                SetSubStep(1);
            }
            break;
        case 1:
            newEntity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_34, self, newEntity);
                newEntity->posX.i.hi += 0x14;
                newEntity->posY.i.hi -= 0x34;
                newEntity->ext.scylla.parent = self;
                newEntity->zPriority = self->zPriority - 1;
            }
            for (i = 0; i < 6; i++) {
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_35, self, newEntity);
                    newEntity->posY.i.hi = 0xD8 - g_Tilemap.scrollY.i.hi;
                    rnd = (Random() & 0x7F) + 0x30;
                    newEntity->params = i;
                    newEntity->posX.i.hi = self->posX.i.hi + rnd;
                }
            }
            self->ext.scylla.fireReady = 0;
            self->ext.scylla.timer = 0x50;
            PlaySfxPositional(SFX_SCYLLA_ATTACK_YELL);
            self->step_s++;
            // fallthrough
        case 2:
            AnimateEntity(D_us_80180D68, self);
            if (!--self->ext.scylla.timer) {
                SetSubStep(3);
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180D74, self)) {
                SetStep(2);
            }
            if (!self->poseTimer && self->pose == 4) {
                PlaySfxPositional(SFX_SCYLLA_ATTACK_DIE);
                self->ext.scylla.fireReady = 1;
            }
            break;
        }
        break;
    case 4: // ice shards
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180D8C, self)) {
                SetSubStep(1);
            }
            break;
        case 1:
            self->ext.scylla.timer = 0x40;
            self->step_s++;
            // fallthrough
        case 2:
            AnimateEntity(D_us_80180D9C, self);
            if (!(self->ext.scylla.timer & 7)) {
                newEntity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_36, self, newEntity);
                    newEntity->posY.i.hi = 0xBC - g_Tilemap.scrollY.i.hi;
                    newEntity->posX.i.hi += 0x30 + (Random() & 0x7F);
                }
            }
            if (!--self->ext.scylla.timer) {
                SetSubStep(3);
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180DA8, self)) {
                SetStep(2);
            }
            break;
        }
        break;
    case 6:
        switch (self->step_s) {
        case 0:
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_SCYLLA_DEFEAT, TIMEATTACK_SET_RECORD);
            g_CastleFlags[SCYLLA_DEFEATED] = 1;
            D_us_80180D48 |= SCYLLA_DEFEATED;
            PlaySfxPositional(SFX_SCYLLA_DEATH);
            self->hitboxState = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_80180DC8, self)) {
                SetSubStep(2);
            }
            break;
        case 2:
            self->ext.scylla.timer = 0x80;
            g_api.PlaySfx(SFX_BOSS_LARGE_FLAMES);
            D_us_80180D48 |= SCYLLA_FLAMES_1_START;
            self->step_s++;
            // fallthrough
        case 3:
            if (!(g_Timer & 0x1F)) {
                g_api.PlaySfx(SFX_FIREBALL_SHOT_A);
            }
            for (i = 0; i < 4; i++) {
                newEntity = AllocEntity(&g_Entities[64], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_3D, self, newEntity);
                    newEntity->zPriority = 0x98;
                    newEntity->params = 1;
                    rnd = Random();
                    newEntity->posY.i.hi = 0xD8;
                    newEntity->posX.i.hi = newEntity->posX.i.hi - 0x40 + rnd;
                    if (rnd < 0x40) {
                        newEntity->posY.i.hi = 0xB0;
                    }
                }
            }
            if (!--self->ext.scylla.timer) {
                D_us_80180D48 |= SCYLLA_FLAMES_1_END;
                self->step_s++;
            }
            break;
        case 4:
            self->ext.scylla.timer = 0x50;
            D_us_80180D48 |= SCYLLA_FLAMES_2_START;
            PlaySfxPositional(SFX_BOSS_LARGE_FLAMES);
            self->step_s++;
            // fallthrough
        case 5:
            if (!(g_Timer & 0x1F)) {
                g_api.PlaySfx(SFX_FIREBALL_SHOT_B);
            }
            for (i = 0; i < 2; i++) {
                newEntity = AllocEntity(&g_Entities[64], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_3D, self, newEntity);
                    newEntity->zPriority = 0x98;
                    newEntity->params = 2;
                    rnd = Random() & 0x3F;
                    newEntity->posY.i.hi = 0xC0;
                    newEntity->posX.i.hi = newEntity->posX.i.hi - 0x20 + rnd;
                }
            }
            if (!--self->ext.scylla.timer) {
                D_us_80180D48 |= SCYLLA_FLAMES_2_END;
                self->step_s++;
            }
            break;
        case 6:
            self->ext.scylla.timer = 0x60;
            PlaySfxPositional(SFX_BOSS_LARGE_FLAMES);
            self->step_s++;
            // fallthrough
        case 7:
            if (!(g_Timer & 0x1F)) {
                g_api.PlaySfx(SFX_FIREBALL_SHOT_C);
            }
            newEntity = AllocEntity(&g_Entities[64], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_3D, self, newEntity);
                newEntity->zPriority = 0x98;
                newEntity->params = 3;
                rnd = Random() & 0x1F;
                newEntity->posY.i.hi = 0xB0;
                newEntity->posX.i.hi = newEntity->posX.i.hi - 0x10 + rnd;
            }
            if (!--self->ext.scylla.timer) {
                self->step_s++;
            }
            break;
        case 8:
            D_us_80180C66 &= ~0x30;
            D_us_80180D48 |= SCYLLA_REWARD_SPAWNED;
            newEntity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_LIFE_UP_SPAWN, self, newEntity);
                newEntity->posX.i.hi = 0xC0 - g_Tilemap.scrollX.i.hi;
                newEntity->params = 3;
                newEntity->posY.i.hi = 0x80 - g_Tilemap.scrollY.i.hi;
                DestroyEntity(self);
                return;
            }
            break;
        }
        break;
    case 0xFF:
        FntPrint("charal %x\n", self->animCurFrame);
        if (g_pads[1].pressed & PAD_SQUARE) {
            if (self->params) {
                break;
            }
            self->animCurFrame++;
            self->params |= 1;
        } else {
            self->params = 0;
        }
        if (g_pads[1].pressed & PAD_CIRCLE) {
            if (self->step_s) {
                break;
            }
            self->animCurFrame--;
            self->step_s |= 1;
        } else {
            self->step_s = 0;
        }
        self->posX.i.hi = 0x80;
        self->posY.i.hi = 0x80;
        break;
    }
}
#endif

extern EInit D_us_801805AC;
extern u8 D_us_80180DD4[];
extern u8 D_us_80180DE0[];
extern u8 D_us_80180DE8[];
extern u8 D_us_80180E04[];
extern u8 D_us_80180E18[];
extern u8 D_us_80180E5C[];
extern u8 D_us_80180E6C[];
extern u8 D_us_80180E80[];
extern u8 D_us_80180E94[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A365C);
#else
// Scylla's wyrm, in the slot after her: spits homing bubbles and rams the
// wall to drop rocks
void func_us_801A365C(Entity* self) {
    Entity* newEntity;
    s32 animating;
    s32 i;

    if ((self->flags & FLAG_DEAD) && self->step != 6) {
        SetStep(6);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801805AC);
        self->hitboxOffX = 0x14;
        self->hitboxOffY = -0xE;
        self->hitboxWidth = 0x18;
        self->hitboxHeight = 0x14;
        SetStep(1);
        // fallthrough
    case 1:
        AnimateEntity(D_us_80180DD4, self);
        if (D_us_80180D48 & SCYLLA_FIGHT_STARTED) {
            SetStep(2);
        }
        break;
    case 2:
        switch (self->step_s) {
        case 0:
            self->ext.scylla.timer = 0x60;
            self->step_s++;
            // fallthrough
        case 1:
            AnimateEntity(D_us_80180DD4, self);
            if (!(Random() & 0x7F)) {
                SetSubStep(2);
            }
            if (!--self->ext.scylla.timer) {
                SetStep(self->ext.scylla.nextAttack ? 4 : 3);
                self->ext.scylla.nextAttack ^= 1;
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80180DE8, self)) {
                SetSubStep(1);
            }
            break;
        }
        break;
    case 5:
        switch (self->step_s) {
        case 0:
            self->ext.scylla.timer = 0x20;
            PlaySfxPositional(SFX_WARG_GROWL);
            self->step_s++;
            // fallthrough
        case 1:
            AnimateEntity(D_us_80180DE0, self);
            if (!--self->ext.scylla.timer) {
                SetSubStep(2);
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80180DE8, self)) {
                SetStep(2);
            }
            break;
        }
        break;
    case 3: // bubble
        switch (self->step_s) {
        case 0:
            self->ext.scylla.timer = 0x28;
            PlaySfxPositional(SFX_WARG_GROWL);
            self->step_s++;
            // fallthrough
        case 1:
            AnimateEntity(D_us_80180DE0, self);
            if (!--self->ext.scylla.timer) {
                SetSubStep(2);
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80180E04, self)) {
                SetSubStep(3);
            }
            break;
        case 3:
            newEntity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_37, self, newEntity);
                newEntity->posX.i.hi += 0x30;
                newEntity->posY.i.hi -= 8;
                newEntity->zPriority = self->zPriority + 1;
            }
            self->step_s++;
            // fallthrough
        case 4:
            if (!AnimateEntity(D_us_80180E18, self)) {
                SetStep(2);
            }
            break;
        }
        break;
    case 4: // rams the wall three times
        animating = 1;
        switch (self->step_s) {
        case 0:
            animating = AnimateEntity(D_us_80180E5C, self);
            if (!self->poseTimer && self->pose == 1) {
                g_api.PlaySfx(SFX_WARG_DEATH_HOWL);
            }
            if (!animating) {
                SetSubStep(1);
            }
            break;
        case 1:
            animating = AnimateEntity(D_us_80180E6C, self);
            if (!self->poseTimer && self->pose == 1) {
                g_api.PlaySfx(SFX_WARG_DEATH_HOWL);
            }
            if (!animating) {
                SetSubStep(2);
            }
            break;
        case 2:
            animating = AnimateEntity(D_us_80180E80, self);
            if (!self->poseTimer && self->pose == 1) {
                g_api.PlaySfx(SFX_WARG_DEATH_HOWL);
            }
            if (!animating) {
                SetSubStep(3);
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180E94, self)) {
                SetStep(2);
                return;
            }
            break;
        }
        if (!animating) {
            g_api.PlaySfx(SFX_SCYLLA_WYRM_WALL_DEBRIS);
            g_api.ShakeCamera(1);
            for (i = 0; i < 8; i++) {
                newEntity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_38, self, newEntity);
                    newEntity->posY.i.hi = 0;
                    newEntity->posX.i.hi =
                        self->posX.i.hi + (Random() & 0x7F);
                    newEntity->zPriority = self->zPriority + 2;
                }
            }
        }
        break;
    case 6:
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            PlaySfxPositional(SFX_WARG_GROWL);
            self->step_s++;
            // fallthrough
        case 1:
            AnimateEntity(D_us_80180DE0, self);
            if (D_us_80180D48 & SCYLLA_FLAMES_2_START) {
                self->step_s++;
            }
            break;
        case 2:
            self->scaleX = self->scaleY = 0x100;
            self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY;
            self->step_s++;
            // fallthrough
        case 3:
            AnimateEntity(D_us_80180DE0, self);
            if (self->scaleX > 0x80) {
                self->scaleX = --self->scaleY;
            }
            break;
        }
        if (D_us_80180D48 & SCYLLA_FLAMES_2_END) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern EInit D_us_801805B8;
extern ScyllaRoomPiece D_us_80180F4C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A3CD8);
#else
void func_us_801A3CD8(Entity* self) {
    ScyllaRoomPiece* piece;

    if (D_us_80180D48 & SCYLLA_FLAMES_1_END) {
        DestroyEntity(self);
        return;
    }
    piece = &D_us_80180F4C[self->params];
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801805B8);
        self->zPriority = piece->zPriority;
        self->posX.i.hi = piece->posX - g_Tilemap.scrollX.i.hi;
        self->posY.i.hi = piece->posY - g_Tilemap.scrollY.i.hi;
        // fallthrough
    case 1:
        AnimateEntity(piece->anim, self);
        if (D_us_80180D48 & SCYLLA_DEFEATED) {
            self->step++;
        }
        break;
    case 2:
        switch (self->step_s) {
        case 0:
            if (D_us_80180D48 & SCYLLA_FLAMES_1_START) {
                self->step_s++;
            }
            break;
        case 1:
            self->scaleX = self->scaleY = 0x100;
            self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY;
            self->step_s++;
            // fallthrough
        case 2:
            if (self->scaleX > 0x80) {
                self->scaleX = --self->scaleY;
            }
            break;
        }
        AnimateEntity(piece->anim, self);
        break;
    }
}
#endif

extern EInit D_us_801805C4;
extern u8 D_us_80180ED8[];
extern SVECTOR D_us_80180FA0;
extern SVECTOR D_us_80180FA8;
extern SVECTOR D_us_80180FB0;
extern SVECTOR D_us_80180FB8;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A3EE0);
#else
// The fireball Scylla holds until she throws it (fireReady)
void func_us_801A3EE0(Entity* self) {
    SVECTOR rot = {0, 0, 0};
    VECTOR trans;
    MATRIX m;
    long p;
    long flag;
    Primitive* prim;
    s32 primIndex;
    s16 angle;

    if (D_us_80180D48 & SCYLLA_DEFEATED) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801805C4);
        self->ext.scylla.timer = 0x20;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleX = self->scaleY = 0;
        // fallthrough
    case 1:
        if (self->ext.scylla.timer) {
            self->ext.scylla.timer--;
            break;
        }
        AnimateEntity(D_us_80180ED8, self);
        if (self->scaleX < 0x100) {
            self->scaleX = self->scaleY += 8;
        }
        if (self->ext.scylla.parent->ext.scylla.fireReady) {
            SetStep(2);
        }
        break;
    case 2:
        self->animCurFrame = 0;
        self->zPriority = 0x8C;
        angle = ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                       PLAYER.posX.i.hi - self->posX.i.hi);
        self->velocityX = rcos(angle) * 0x48;
        self->velocityY = rsin(angle) * 0x48;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 2);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.scylla.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        UnkPolyFunc2(prim);
        prim->tpage = 0x12;
        prim->clut = 0x203;
        prim->u0 = prim->u2 = 0x60;
        prim->u1 = prim->u3 = 0x7F;
        prim->v0 = prim->v1 = 0xE0;
        prim->v2 = prim->v3 = 0xFF;
        prim->drawMode = DRAW_UNK02;
        prim->priority = self->zPriority;
        self->step++;
        // fallthrough
    case 3:
        MoveEntity();
        prim = self->ext.scylla.prim;
        self->rotate += 0x280;
        SetGeomScreen(0x400);
        SetGeomOffset(self->posX.i.hi, self->posY.i.hi);
        RotMatrix(&rot, &m);
        RotMatrixZ(self->rotate, &m);
        RotMatrixX(0x2C0, &m);
        SetRotMatrix(&m);
        trans.vx = 0;
        trans.vy = 0;
        trans.vz = 0x400;
        TransMatrix(&m, &trans);
        SetTransMatrix(&m);
        RotAverage4(&D_us_80180FA0, &D_us_80180FA8, &D_us_80180FB0,
                    &D_us_80180FB8, (s32*)&prim->x0, (s32*)&prim->x1,
                    (s32*)&prim->x2, (s32*)&prim->x3, &p, &flag);
        if (!((g_Timer + 2) & 7)) {
            PlaySfxPositional(SFX_WHIP_TWIRL_SWISH);
        }
        if (self->velocityX > 0) {
            if (self->posX.i.hi > 0x100) {
                DestroyEntity(self);
            }
        } else if (self->posX.i.hi < 0) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A42A8);
#else
// A water jet that rises from the surface and arcs to Scylla's mouth,
// leaving a fading trail of lines
void func_us_801A42A8(Entity* self) {
    Primitive* prim;
    Primitive* trail;
    s32 primIndex;
    s32 done;

    if (D_us_80180D48 & SCYLLA_DEFEATED) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.func_800EDB58(PRIM_LINE_G2_ALT, 0x12);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.scylla.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->r0 = 0x20;
            prim->g0 = 0x28;
            prim->b0 = 0xC0;
            prim->r1 = 0x10;
            prim->g1 = 0x10;
            prim->b1 = 0x60;
            prim->priority = 0x98;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->ext.scylla.timer = (Random() & 7) * self->params;
        // fallthrough
    case 1:
        if (self->ext.scylla.timer) {
            self->ext.scylla.timer--;
            break;
        }
        self->velocityX =
            ((s16)(0x5C - g_Tilemap.scrollX.i.hi - self->posX.i.hi) << 16) /
            32;
        prim = self->ext.scylla.prim;
        self->velocityY = FIX(-8.25);
        self->ext.scylla.trail = prim;
        prim->x0 = prim->x1 = self->posX.i.hi;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
        prim->y0 = prim->y1 = self->posY.i.hi;
        self->ext.scylla.timer = 0x20;
        self->step++;
        // fallthrough
    case 2:
        switch (self->step_s) {
        case 0:
            MoveEntity();
            self->velocityY += FIX(0.25);
            if (self->velocityY > 0) {
                self->step_s++;
            }
            break;
        case 1:
            self->step_s++;
            self->velocityY =
                ((0x54 - g_Tilemap.scrollY.i.hi - self->posY.i.hi) << 16) /
                self->ext.scylla.timer;
            // fallthrough
        case 2:
            MoveEntity();
            break;
        }
        trail = self->ext.scylla.trail;
        trail->x0 = self->posX.i.hi;
        trail->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
        trail->y0 = self->posY.i.hi;
        if (!--self->ext.scylla.timer) {
            self->step++;
        } else if (!(self->ext.scylla.timer & 3)) {
            trail = trail->next;
            self->ext.scylla.trail = trail;
            trail->x0 = trail->x1 = self->posX.i.hi;
            trail->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            trail->y0 = trail->y1 = self->posY.i.hi;
        }
        // fallthrough
    case 3:
        prim = self->ext.scylla.prim;
        done = 1;
        while (prim->next != NULL) {
            if (prim->drawMode != DRAW_HIDE) {
                if (func_us_801A2A90(prim, 1)) {
                    done = 0;
                } else {
                    prim->drawMode = DRAW_HIDE;
                }
            }
            prim = prim->next;
        }
        prim->x0 = prim->y0 = prim->x1 = prim->y1 = 0;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
        if (done) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

INCLUDE_RODATA("boss/bo3/nonmatchings/unk_1CEEC", D_us_8019CA94);

extern EInit D_us_801805D0;
extern u8 D_us_80180F0C[];
extern u8 D_us_80180F18[];
extern u8 D_us_80180F30[];
extern u8 D_us_80180F40[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A4680);
#else
// An ice shard: rises out of the water, bobs, then flies at Alucard
void func_us_801A4680(Entity* self) {
    Entity* newEntity;
    s32 i;
    s16 angle;

    if (D_us_80180D48 & SCYLLA_DEFEATED) {
        self->flags |= FLAG_DEAD;
    }
    if (self->flags & FLAG_DEAD) {
        for (i = 0; i < 5; i++) {
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_39, self, newEntity);
                newEntity->params = 1;
                newEntity->zPriority = self->zPriority;
            }
        }
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801805D0);
        self->zPriority = 0x8C;
        self->hitboxState = 0;
        // fallthrough
    case 1:
        if (!AnimateEntity(D_us_80180F0C, self)) {
            self->velocityY = FIX(-4);
            self->ext.scylla.timer = 0x50;
            self->hitboxState = 2;
            SetStep(2);
        }
        if (!self->poseTimer && self->pose == 3) {
            PlaySfxPositional(SFX_BONE_SWORD_SWISH_C);
        }
        break;
    case 2:
        AnimateEntity(D_us_80180F18, self);
        MoveEntity();
        self->velocityY -= self->velocityY >> 4;
        if (!--self->ext.scylla.timer) {
            self->ext.scylla.timer = 0x80;
            self->rotate = Random() * 16;
            self->step++;
        }
        break;
    case 3:
        AnimateEntity(D_us_80180F18, self);
        MoveEntity();
        self->rotate += 0x20;
        self->velocityY = rsin(self->rotate) * 8;
        if (!--self->ext.scylla.timer) {
            SetStep(4);
        }
        break;
    case 4:
        if (!AnimateEntity(D_us_80180F30, self)) {
            SetStep(5);
        }
        break;
    case 5:
        angle = ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                       PLAYER.posX.i.hi - self->posX.i.hi);
        self->velocityX = rcos(angle) * 0x70;
        self->velocityY = rsin(angle) * 0x70;
        if (self->velocityX > 0) {
            self->facingLeft = 0;
        } else {
            self->facingLeft = 1;
        }
        self->hitboxState = 3;
        PlaySfxPositional(SFX_ARROW_SHOT_A);
        self->step++;
        // fallthrough
    case 6:
        AnimateEntity(D_us_80180F40, self);
        MoveEntity();
        break;
    }
}
#endif

extern EInit D_us_801805DC;
extern u8 D_us_80180EE4[];
extern u8 D_us_80180EF4[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A4988);
#else
// A homing bubble spat by Scylla's wyrm
void func_us_801A4988(Entity* self) {
    Entity* newEntity;
    s32 i;
    s16 angle;

    if (D_us_80180D48 & SCYLLA_DEFEATED) {
        self->flags |= FLAG_DEAD;
    }
    if (self->flags & FLAG_DEAD) {
        for (i = 0; i < 0x10; i++) {
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_UNK_39, self, newEntity);
                newEntity->zPriority = self->zPriority;
            }
        }
        PlaySfxPositional(SFX_SCYLLA_BUBBLE_BURST);
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801805DC);
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        self->hitPoints = 0;
        // fallthrough
    case 1:
        if (!AnimateEntity(D_us_80180EF4, self)) {
            SetStep(2);
        }
        break;
    case 2:
        angle = ratan2(PLAYER.posY.i.hi - self->posY.i.hi,
                       PLAYER.posX.i.hi - self->posX.i.hi);
        if (angle > 0x200) {
            angle = 0x200;
        }
        if (angle < -0x180) {
            angle = -0x180;
        }
        self->velocityX = rcos(angle) * 0x50;
        self->velocityY = rsin(angle) * 0x50;
        self->ext.scylla.angle = angle;
        PlaySfxPositional(SFX_EXPLODE_B);
        self->step++;
        // fallthrough
    case 3:
        AnimateEntity(D_us_80180EE4, self);
        MoveEntity();
        angle = LimitAngleChange(8, self->ext.scylla.angle,
                                 GetAngleBetweenEntities(self, &PLAYER));
        self->velocityX = rcos(angle) * 0x50;
        self->velocityY = rsin(angle) * 0x50;
        self->ext.scylla.angle = angle;
        break;
    }
}
#endif

extern EInit D_us_801805E8;
extern s16 D_us_80180F88[];
extern s16 D_us_80180F90[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A4C0C);
#else
// A rock knocked off the ceiling when the wyrm rams the wall
void func_us_801A4C0C(Entity* self) {
    Entity* newEntity;

    if (D_us_80180D48 & SCYLLA_DEFEATED) {
        self->flags |= FLAG_DEAD;
    }
    if (self->flags & FLAG_DEAD) {
        newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (newEntity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
            newEntity->params = 0x11;
        }
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801805E8);
        self->drawFlags = ENTITY_ROTATE;
        self->params = Random() & 3;
        self->animCurFrame = self->params + 0x6F;
        self->rotate = rand() & 0xFFF;
        PlaySfxPositional(SFX_WALL_DEBRIS_B);
        // fallthrough
    case 1:
        self->rotate += 0x80;
        if (self->posY.i.hi < 0x80) {
            MoveEntity();
            self->velocityY += FIX(1.0 / 8);
            break;
        }
        if (self->posY.i.hi + D_us_80180F88[self->params] +
                    g_Tilemap.scrollY.i.hi >=
                0xD9 &&
            !self->ext.scylla.splashed) {
            self->ext.scylla.splashed = 1;
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_SPLASH_WATER, self, newEntity);
                newEntity->params = 0;
                newEntity->posY.i.hi = 0xD8 - g_Tilemap.scrollY.i.hi;
            }
        }
        if (UnkCollisionFunc3(D_us_80180F90) & 1) {
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity == NULL) {
                FntPrint("no WORK !!\n");
            } else {
                CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
                newEntity->params = 0x11;
            }
            DestroyEntity(self);
            return;
        }
        self->velocityY -= FIX(1.0 / 8);
        break;
    }
}
#endif

extern EInit D_us_801805F4;
extern u8 D_us_80180F24[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A4E24);
#else
// A droplet of a burst bubble (params 1: of a shattered ice shard)
void func_us_801A4E24(Entity* self) {
    s16 scale;
    s16 angle;
    s32 speed;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801805F4);
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        if (!self->params) {
            scale = (Random() & 0x7F) + 0x40;
        } else {
            scale = (Random() & 0x3F) + 0x20;
        }
        self->scaleX = self->scaleY = scale;
        self->blendMode = BLEND_TRANSP;
        self->facingLeft = Random() & 1;
        speed = (Random() & 0x1F) + 0x10;
        angle = (Random() * 6) + 0x900;
        self->velocityX = (speed * rcos(angle)) >> 1;
        self->velocityY = speed * rsin(angle);
        if (self->params) {
            self->velocityX >>= 1;
            self->velocityY >>= 1;
        }
        self->posX.val += self->velocityX * 4;
        self->posY.val += self->velocityY * 4;
        self->ext.scylla.timer = (Random() & 0x1F) + 0x10;
        // fallthrough
    case 1:
        AnimateEntity(D_us_80180F24, self);
        MoveEntity();
        if (self->velocityY < FIX(3)) {
            self->velocityY += FIX(0.078125);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A4FB8);
#else
// Draws the wyrm's neck: a quad between each pair of segment midpoints
void func_us_801A4FB8(void) {
    Entity* next;
    Primitive* prim;
    s32 x;
    s32 y;
    s32 midX;
    s32 midY;
    s32 dx;
    s32 dy;
    s16 angle;
    s32 i;

    next = g_CurrentEntity + 1;
    x = g_CurrentEntity->posX.val;
    y = g_CurrentEntity->posY.val;
    prim = g_CurrentEntity->ext.scyllaWyrm.prim;
    prim->x1 = prim->x3 = x;
    prim->y1 = prim->y3 = y;
    prim->drawMode = DRAW_HIDE;
    for (i = 0; i < 12; i++, next++) {
        angle = ratan2(next->posY.val - y, next->posX.val - x);
        if (!g_CurrentEntity->params) {
            angle += 0x400;
        } else {
            angle -= 0x400;
        }
        midX = ((x + next->posX.val) / 2) >> 16;
        midY = ((y + next->posY.val) / 2) >> 16;
        dx = (rcos(angle) * 16) >> 12;
        dy = (rsin(angle) * 16) >> 12;
        prim->x0 = midX + dx;
        prim->y0 = midY + dy;
        prim->x2 = midX - dx;
        prim->y2 = midY - dy;
        if (g_CurrentEntity->flags & 0xF) {
            prim->clut = g_CurrentEntity->palette & 0xFFF;
        } else {
            prim->clut = 0x205;
        }
        prim = prim->next;
        prim->x1 = midX + dx;
        prim->y1 = midY + dy;
        prim->x3 = midX - dx;
        prim->y3 = midY - dy;
        y = next->posY.val;
        x = next->posX.val;
    }
    prim->drawMode = DRAW_HIDE;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A516C);
#else
// Pulls each neck segment toward the middle of its neighbours
void func_us_801A516C(void) {
    Entity* seg;
    s32 x;
    s32 y;
    s32 midX;
    s32 midY;
    s32 i;

    seg = g_CurrentEntity + 1;
    x = g_CurrentEntity->posX.val;
    y = g_CurrentEntity->posY.val;
    for (i = 0; i < 11; i++, seg++) {
        midX = (x + seg[1].posX.val) >> 1;
        midY = (y + seg[1].posY.val) >> 1;
        x = ((midX - seg->posX.val) >> 2) + midX;
        y = ((midY - seg->posY.val) >> 2) + midY;
        seg->posX.val = x;
        seg->posY.val = y;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A51E4);
#else
// Keeps the neck segments at most 32 px apart and makes the neck wave
void func_us_801A51E4(void) {
    Entity* seg;
    s32 x;
    s32 y;
    s32 dx;
    s32 dy;
    s32 len;
    s16 angle;
    u16 wave;
    s32 i;

    seg = g_CurrentEntity + 1;
    x = g_CurrentEntity->posX.val;
    y = g_CurrentEntity->posY.val;
    wave = g_CurrentEntity->ext.scyllaWyrm.angle;
    for (i = 0; i < 11; i++, seg++) {
        dx = seg->posX.val - x;
        dy = seg->posY.val - y;
        angle = ratan2(dy, dx);
        dx >>= 16;
        dy >>= 16;
        angle += (rcos((s16)wave) << 8) >> 12;
        len = SquareRoot0(dx * dx + dy * dy);
        if (len > 0x20) {
            len = 0x20;
        }
        x += len * rcos(angle) * 16;
        seg->posX.val = x;
        wave += 0x180;
        seg->rotate = angle;
        y += len * rsin(angle) * 16;
        seg->posY.val = y;
    }
}
#endif

extern EInit g_EInitScyllawyrm;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A5338);
#else
// A wyrm: params 0 is the Scylla Wyrm miniboss, 1 to 3 are Scylla's.
// Its 11 neck segments (0x3B) and head (0x3C) follow it.
void func_us_801A5338(Entity* self) {
    Entity* entity;
    Primitive* prim;
    s32 primIndex;
    s32 i;
    u8 u;

    if (self->params && (D_us_80180D48 & SCYLLA_DEFEATED)) {
        self->flags |= FLAG_DEAD;
    }
    if ((self->flags & FLAG_DEAD) && self->step == 2) {
        SetStep(3);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitScyllawyrm);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 13);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.scyllaWyrm.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        for (i = 0, u = 0xFF; i < 13; i++, u -= 0xE) {
            prim->u0 = prim->u2 = u;
            prim->tpage = 0x13;
            prim->clut = 0x205;
            prim->v0 = prim->v1 = 0x60;
            prim->v2 = prim->v3 = 0x7F;
            prim->u1 = prim->u3 = u - 0xE;
            prim->priority = 0x88;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        }
        if (!self->params && g_CastleFlags[SCYLLA_WYRM_DEFEATED]) {
            DestroyEntity(self);
            return;
        }
        entity = self + 1;
        self->nextPart = entity;
        self->parent = NULL;
        for (i = 0; i < 11; i++, entity++) {
            CreateEntityFromEntity(E_UNK_3B, self, entity);
            entity->parent = self;
            entity->nextPart = entity + 1;
            entity->params = i & 3;
        }
        entity = self + 12;
        CreateEntityFromEntity(E_UNK_3C, self, entity);
        entity->params = self->params;
        entity->parent = self;
        entity->nextPart = self;
        entity->zPriority = self->zPriority + 1;
        self->ext.scyllaWyrm.timer = self->params * 0x30;
        // fallthrough
    case 1:
        if (!self->params) {
            self->ext.scyllaWyrm.waiting = 1;
            if (GetDistanceToPlayerX() < 0xC0) {
                self->ext.scyllaWyrm.waiting = 0;
                D_us_80180C66 |= 0xE;
                SetStep(2);
            }
        } else {
            self->ext.scyllaWyrm.waiting = 1;
            if (!--self->ext.scyllaWyrm.timer) {
                self->ext.scyllaWyrm.waiting = 0;
                SetStep(2);
            }
        }
        break;
    case 2:
        func_us_801A516C();
        func_us_801A51E4();
        func_us_801A4FB8();
        self->ext.scyllaWyrm.angle += 0x40;
        break;
    case 3:
        func_us_801A516C();
        func_us_801A51E4();
        func_us_801A4FB8();
        self->ext.scyllaWyrm.angle += 0x80;
        switch (self->step_s) {
        case 0:
            self->ext.scyllaWyrm.segments = 11;
            self->ext.scyllaWyrm.timer = 1;
            if (!self->params) {
                g_CastleFlags[SCYLLA_WYRM_DEFEATED] |= 1;
                D_us_80180C66 &= ~0xE;
            }
            self->step_s++;
            // fallthrough
        case 1:
            if (!--self->ext.scyllaWyrm.timer) {
                self->ext.scyllaWyrm.timer = 0x10;
                prim = self->ext.scyllaWyrm.prim;
                entity = self + 1;
                for (i = 0; i < self->ext.scyllaWyrm.segments; i++) {
                    prim = prim->next;
                    entity++;
                }
                prim->next->drawMode = DRAW_HIDE;
                entity->ext.scyllaWyrm.burst = 1;
                if (!--self->ext.scyllaWyrm.segments) {
                    self->step_s++;
                }
            }
            break;
        case 2:
            for (i = 0, entity = self + 1; i < 12; i++, entity++) {
                DestroyEntity(entity);
            }
            DestroyEntity(self);
            return;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A57A4);
#else
// A neck segment of a wyrm; bursts into flames when the wyrm dies
void func_us_801A57A4(Entity* self) {
    Entity* newEntity;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitScyllawyrm);
        self->animSet = 2;
        self->animCurFrame = 0;
        self->unk5A = 0;
        self->palette = 0;
        // fallthrough
    case 1:
        if ((g_Timer & 3) == self->params) {
            self->hitboxState = 3;
        } else {
            self->hitboxState = 0;
        }
        if (self->ext.scyllaWyrm.burst) {
            SetStep(2);
        }
        break;
    case 2:
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            self->ext.scyllaWyrm.timer = 0x10;
            self->step_s++;
            // fallthrough
        case 1:
            if (!(g_Timer & 0xF)) {
                PlaySfxPositional(SFX_FIREBALL_SHOT_C);
            }
            if (!(self->ext.scyllaWyrm.timer & 3)) {
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(E_UNK_3D, self, newEntity);
                    newEntity->params = 0;
                    newEntity->zPriority = 0x89;
                    newEntity->rotate = self->rotate - 0x400;
                }
            }
            if (!--self->ext.scyllaWyrm.timer) {
                self->step_s++;
            }
            break;
        }
        break;
    }
}
#endif

extern u8 D_us_80180FCC[];
extern u8 D_us_80180FD8[];
extern u8 D_us_80180FE0[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_1CEEC", func_us_801A5948);
#else
// The head of a wyrm, 12 slots after its body: lunges at Alucard and
// swings back to the body
void func_us_801A5948(Entity* self) {
    Entity* body;
    Entity* newEntity;
    s32 dx;
    s16 angle;

    body = self - 12;
    if (self->params && (D_us_80180D48 & SCYLLA_DEFEATED)) {
        self->flags |= FLAG_DEAD;
    }
    if ((self->flags & FLAG_DEAD) && self->step < 6) {
        SetStep(6);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitScyllawyrm);
        self->animCurFrame = 0x49;
        self->drawFlags = ENTITY_ROTATE;
        if (!self->params) {
            self->facingLeft = 1;
        }
        // fallthrough
    case 1:
        if (body->ext.scyllaWyrm.waiting) {
            break;
        }
        self->step = 2;
        // fallthrough
    case 2:
        if (!AnimateEntity(D_us_80180FCC, self)) {
            angle = ratan2(PLAYER.posY.val - self->posY.val,
                           PLAYER.posX.val - self->posX.val);
            self->velocityX = rcos(angle) << 5;
            self->velocityY = rsin(angle) << 5;
            self->ext.scyllaWyrm.timer = 0x40;
            self->animCurFrame = 0x49;
            SetStep(3);
        }
        break;
    case 3:
        if (!self->step_s) {
            if (!self->params) {
                g_api.PlaySfx(SFX_SCYLLA_WYRM_WALL_DEBRIS);
            }
            self->step_s++;
        }
        MoveEntity();
        if (GetDistanceToPlayerX() < 0x38) {
            AnimateEntity(D_us_80180FD8, self);
            if (!self->poseTimer && self->pose == 2) {
                PlaySfxPositional(SFX_SCYLLA_WYRM_ATTACK);
            }
        }
        if (!--self->ext.scyllaWyrm.timer) {
            SetStep(4);
        }
        if (!self->params && !(g_Timer & 7)) {
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromCurrentEntity(
                    E_BREAKABLE_WALL_DEBRIS, newEntity);
                newEntity->posX.i.hi = 0xE0;
                newEntity->posY.i.hi = 0x24;
                angle = ((Random() & 0x3F) * 16) | 0x400;
                newEntity->velocityX = rcos(angle) * 16;
                newEntity->velocityY = rsin(angle) * 16;
            }
        }
        break;
    case 4:
        if (!AnimateEntity(D_us_80180FE0, self)) {
            SetStep(5);
        }
        break;
    case 5:
        dx = body->posX.val - self->posX.val;
        angle = ratan2(body->posY.val - self->posY.val, dx);
        angle += (rsin(self->ext.scyllaWyrm.angle) << 9) >> 12;
        self->velocityX = rcos(angle) << 5;
        self->velocityY = rsin(angle) << 5;
        MoveEntity();
        self->animCurFrame = 0x49;
        self->ext.scyllaWyrm.angle += 0x80;
        if (self->velocityY > FIX(0.5)) {
            self->animCurFrame = 0x4A;
        }
        if (self->velocityY < FIX(0.5)) {
            self->animCurFrame = 0x4B;
        }
        if (abs(dx >> 16) < 0x40) {
            SetStep(2);
        }
        break;
    case 6:
        switch (self->step_s) {
        case 0:
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
                newEntity->params = 3;
                newEntity->zPriority = self->zPriority + 1;
            }
            self->animCurFrame = 0;
            self->hitboxState = 0;
            angle = ratan2(body->posY.val - self->posY.val,
                           body->posX.val - self->posX.val);
            self->velocityX = rcos(angle) * 12;
            self->velocityY = rsin(angle) * 12;
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            if (abs(body->posX.val - self->posX.val) < 0x40) {
                self->velocityX = 0;
                self->velocityY = 0;
            }
            break;
        }
        break;
    }
    angle = ratan2(self->posY.val - self[-1].posY.val,
                   self->posX.val - self[-1].posX.val);
    self->rotate = angle;
    if (!self->params) {
        self->rotate = 0x800 - angle;
    }
}
#endif

#ifdef VERSION_PC
s16 D_us_801B2870; // how far the water line moved this frame
s16 D_us_801B2874; // func_us_801A0A80: Alucard's depth last frame
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180EA0[];
extern u8 D_us_80180EAC[];
extern u8 D_us_80180EB8[];
extern u8 D_us_80180EC4[];
extern u8 D_us_80180ECC[];
u8 D_us_80180EA0[] ALIGNED4 = {0x14, 0x3D, 0x14, 0x3E, 0x14, 0x3F, 0x14, 0x3E, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180EAC[] ALIGNED4 = {0x04, 0x3E, 0x14, 0x3F, 0x14, 0x3E, 0x14, 0x3D, 0x10, 0x3E, 0x00, 0x00};
u8 D_us_80180EB8[] ALIGNED4 = {0x14, 0x40, 0x14, 0x41, 0x14, 0x42, 0x14, 0x41, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180EC4[] ALIGNED4 = {0x14, 0x43, 0x14, 0x44, 0x14, 0x45, 0x00, 0x00};
u8 D_us_80180ECC[] ALIGNED4 = {0x14, 0x46, 0x14, 0x47, 0x14, 0x48, 0x14, 0x47, 0x00, 0x00, 0x00, 0x00};
u8 D_us_8018080C[0x14] = {
    0x81,
    0x81,
    0x7E,
    6,
    0x81,
    0x89,
    0x7E,
    6,
    0x81,
    0x91,
    0x7E,
    6,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
u8 D_us_80180824[0xC] = { 0xA, 0, 0xA, 1, 0xA, 2, 0xA, 1, 0, 0, 0, 0 };
s16 D_us_80180830[0x7E] = {
    0xD0,
    0xF0,
    0xB0,
    0xF0,
    -2,
    0x60,
    0xC0,
    0x5B0,
    0x5F0,
    0,
    0xB80,
    0x60,
    0xB0,
    0xF0,
    0,
    0xA40,
    0xA0,
    0xB0,
    0xE0,
    0,
    0x540,
    0x2B0,
    0xB0,
    0x130,
    0,
    0x4D0,
    0x70,
    0xB0,
    0xF0,
    -1,
    0x260,
    0x230,
    0x120,
    0x180,
    0,
    0x1D0,
    0x4C,
    0xB0,
    0xF0,
    1,
    0x50,
    0x180,
    0xB0,
    0x1A0,
    0,
    0x510,
    0x50,
    0xB0,
    0xF0,
    0,
    0x840,
    0x90,
    0x100,
    0x1D0,
    0,
    0x2A0,
    0x160,
    0xB0,
    0x170,
    0,
    0xA0,
    0x150,
    0xB0,
    0x130,
    0,
    0x620,
    0xB0,
    0xB0,
    0xC0,
    0,
    0x70,
    0xC10,
    0xB0,
    0x160,
    0,
    0x60,
    0xD0,
    0xB0,
    0xE0,
    0,
    0x50,
    0x78,
    0xB8,
    0xE0,
    0,
    0x50,
    0x70,
    0xB0,
    0xF0,
    -2,
    0x50,
    0x70,
    0xB0,
    0xE0,
    0,
    0x30,
    0x7C,
    0xB8,
    0xE0,
    0,
    0x58,
    0x148,
    0xB0,
    0xE8,
    0,
    0x220,
    0xD0,
    0xB0,
    0xE8,
    0,
    0,
    0x4D0,
    0xD8,
    0xE8,
    0,
    0x68,
    0xB0,
    0xF0,
    0xF0,
    0,
    0x340,
    0x74,
    0xF0,
    0xF0,
    0,
    0,
};
s16 D_us_8018092C[0x19C] = {
    0xD0,
    0xF0,
    0xB1,
    0xF0,
    0x60,
    0xC0,
    0x5B1,
    0x5F0,
    0xB80,
    0x60,
    0xB1,
    0xF0,
    0xA40,
    0xA0,
    0xB1,
    0xE0,
    0x540,
    0x2B0,
    0xB1,
    0x130,
    0x4D0,
    0x70,
    0xB1,
    0xF0,
    0x260,
    0x230,
    0x121,
    0x180,
    0x1D0,
    0x48,
    0xB1,
    0xF0,
    0x50,
    0x180,
    0xB1,
    0x1A0,
    0x510,
    0x50,
    0xB1,
    0xF0,
    0x840,
    0x90,
    0x101,
    0x1D0,
    0x2A0,
    0x1D2,
    0xB1,
    0x170,
    0xA0,
    0x150,
    0xB1,
    0x130,
    0x620,
    0xB0,
    0xB1,
    0xC0,
    0x560,
    0x1B0,
    0xC0,
    0xF0,
    0x530,
    0x1E2,
    0xF0,
    0x1D2,
    0x712,
    0x12E,
    0x120,
    0x1A2,
    0x10,
    0x90,
    0x100,
    0x1E0,
    0xA0,
    0xF2,
    0x130,
    0x1E0,
    0x10,
    0xC70,
    0xB1,
    0x160,
    0x60,
    0xD0,
    0xB1,
    0xE0,
    0x50,
    0x78,
    0xB9,
    0xE0,
    0x50,
    0x70,
    0xB1,
    0xF0,
    0x50,
    0x70,
    0xB1,
    0xE0,
    0x30,
    0x7C,
    0xB9,
    0xE0,
    0x58,
    0x148,
    0xB1,
    0xE8,
    0x220,
    0xD0,
    0xB1,
    0xE8,
    0,
    0x4D0,
    0xD9,
    0xE8,
    0x68,
    0xB0,
    0xF0,
    0xF0,
    0x340,
    0x74,
    0xF0,
    0xF0,
    0x128,
    0x158,
    0x58,
    0xAC,
    0x168,
    0x198,
    0x68,
    0xAC,
    0x568,
    0x598,
    0x58,
    0xAC,
    0x5B0,
    0x5D8,
    0x68,
    0xAC,
    0x5E8,
    0x658,
    0x58,
    0xAC,
    0x6E8,
    0x758,
    0x58,
    0xAC,
    0x768,
    0x798,
    0x58,
    0xAC,
    0x7B0,
    0x7D8,
    0x68,
    0xAC,
    0x8F0,
    0x918,
    0x68,
    0xAC,
    0xA68,
    0xA98,
    0x58,
    0xAC,
    0x68,
    0x98,
    0x58,
    0xAC,
    0xB0,
    0xD8,
    0x68,
    0xAC,
    0x130,
    0x198,
    0x58,
    0xAC,
    0x70,
    0xD8,
    0x58,
    0xAC,
    0x70,
    0x92,
    0x58,
    0x88,
    0xF0,
    0x118,
    0x68,
    0xAC,
    0x1A8,
    0x1D8,
    0x58,
    0xAC,
    0x1F0,
    0x218,
    0x68,
    0xAC,
    0x68,
    0x98,
    0x58,
    0xAC,
    0xB0,
    0xD8,
    0x68,
    0xAC,
    0xE8,
    0x158,
    0x58,
    0xAC,
    0x168,
    0x198,
    0x58,
    0xAC,
    0x1B0,
    0x1D8,
    0x68,
    0xAC,
    0x2A8,
    0x318,
    0x58,
    0xAC,
    0x330,
    0x358,
    0x68,
    0xAC,
    0x368,
    0x398,
    0x58,
    0xAC,
    0x3B0,
    0x3D8,
    0x68,
    0xAC,
    0x430,
    0x458,
    0x68,
    0xA8,
    0x470,
    0x498,
    0x58,
    0x9C,
    0x4F0,
    0x518,
    0x68,
    0xAC,
    0x70,
    0x98,
    0x68,
    0xAC,
    0xA8,
    0xD8,
    0x58,
    0xAC,
    0xF0,
    0x118,
    0x68,
    0xAC,
    0x128,
    0x158,
    0x58,
    0xAC,
    0x1E8,
    0x258,
    0x58,
    0xAC,
    0x368,
    0x398,
    0x58,
    0xAC,
    0x3B0,
    0x3D8,
    0x68,
    0xAC,
    0x4B0,
    0x4D8,
    0x68,
    0xAC,
    0x568,
    0x5D8,
    0x58,
    0xAC,
    0x5E8,
    0x618,
    0x58,
    0xAC,
    0x630,
    0x658,
    0x68,
    0xAC,
    0x7F0,
    0x818,
    0x68,
    0xAC,
    0x928,
    0x959,
    0x58,
    0xAC,
    0x971,
    0x998,
    0x68,
    0xAC,
    0x9A8,
    0xA18,
    0x58,
    0xAC,
    0xA28,
    0xA58,
    0x58,
    0xAC,
    0xA70,
    0xA98,
    0x68,
    0xAC,
    0xB68,
    0xB98,
    0x58,
    0xAC,
    0xC28,
    0xC58,
    0x58,
    0xAC,
    0xC70,
    0xC98,
    0x68,
    0xAC,
    0x68,
    0x98,
    0x58,
    0xAC,
    0xB0,
    0xD8,
    0x68,
    0xAC,
    0xE8,
    0x158,
    0x58,
    0xAC,
    0x22,
    0x23,
    3,
    0x48,
    0x22,
    1,
    0x70,
    0x21,
    2,
    0xCE,
    0x24,
    1,
    0x122,
    0x23,
    3,
    0x148,
    0x22,
    1,
    0x170,
    0x21,
    2,
    0x1CE,
    0x24,
    1,
    0x10,
    0x32,
    0x70,
    0x9E,
    0xDF,
    0x110,
    0x132,
    0x170,
    0x19E,
    0x1DF,
    0x210,
    0,
    0,
    0xA,
    4,
    8,
    1,
    0xC,
    5,
    9,
    2,
    6,
    0xB,
    3,
    7,
    0,
    0x58,
    0x68,
    0x30,
    -2,
    0,
    4,
    8,
    1,
    5,
    2,
    6,
    3,
    7,
    0,
    0x100,
    0x240,
    0x2E0,
    0x300,
    0x320,
    0x7FFF,
    0x701,
    0x801,
    0x901,
    0,
    0x30,
    0x10,
    0x28,
    0x10,
    0x18,
    0x10,
};
s16 D_us_80180C64 = 0;
u16 D_us_80180C66 = 0;
s16 D_us_80180C68[0xC] = { 0, 0x4D0, 0xD8, 0xE8, 0x68, 0xB0, 0xF0, 0xF0, 0x340, 0x74, 0xF0, 0xF0 };
s32 D_us_80180C80[8] = { -0x68000, 0x4800, -0x58000, 0x4400, -0x48000, 0x4000, -0x38000, 0x3C00 };
s16 D_us_80180CA0[0x12] = {
    0,
    2,
    4,
    6,
    8,
    0xA,
    0x260,
    0x20,
    0xD0,
    0x80,
    0x470,
    0x20,
    0xD0,
    0x80,
    0xC0,
    0x2C,
    0xE0,
    0x500,
};
s16 D_us_80180CC4[6] = { 0x1E0, 0x60, 0x61, 0x6E, 0x1FF, 0x1E0 };
u8 D_us_80180CD0[8] = { 0, 0, 0, 1, 1, 0, 0, 0 };
u16 D_us_80180CD8[0x30] = {
    0x373,
    0,
    0,
    0,
    0x75E,
    1,
    1,
    1,
    0x375,
    0,
    0,
    0,
    0x765,
    1,
    1,
    1,
    0x3F2,
    0,
    0,
    0,
    0x75C,
    1,
    1,
    1,
    0x72D,
    0,
    0,
    0,
    0x75D,
    1,
    1,
    1,
    0x375,
    0,
    0,
    0x969,
    0x762,
    1,
    1,
    0x764,
    0x933,
    0,
    0,
    0x951,
    0x761,
    1,
    1,
    0x763,
};
s16 D_us_80180D38[8] = { 0, 1, 2, 4, -1, -2, -4, 0 };
s32 D_us_80180D48 = 0;
u8 D_us_80180D4C[0xC] = { 0x12, 1, 0x12, 2, 0x12, 3, 0x12, 2, 0, 0, 0, 0 };
u8 D_us_80180D58[0x10] = { 4, 1, 7, 5, 7, 6, 7, 7, 7, 8, 7, 9, 7, 0xA, 0xFF, 0 };
u8 D_us_80180D68[0xC] = { 8, 7, 8, 8, 8, 9, 8, 0xA, 0, 0, 0, 0 };
u8 D_us_80180D74[0x18] = {
    4,
    9,
    4,
    0xA,
    2,
    7,
    2,
    6,
    1,
    0xB,
    1,
    0xC,
    1,
    0xD,
    0x30,
    0xE,
    6,
    0xF,
    6,
    0x10,
    2,
    1,
    0xFF,
    0,
};
u8 D_us_80180D8C[0x10] = { 2, 1, 0xA, 0x14, 0xA, 0x15, 8, 0x16, 8, 0x17, 8, 0x18, 1, 0x19, 0xFF, 0 };
u8 D_us_80180D9C[0xC] = { 8, 0x16, 8, 0x17, 8, 0x18, 8, 0x19, 0, 0, 0, 0 };
u8 D_us_80180DA8[0xC] = { 2, 0x16, 8, 0x17, 0xA, 0x1A, 0xA, 0x1B, 8, 1, 0xFF, 0 };
u8 D_us_80180DB4[0x14] = {
    1,
    1,
    3,
    0x11,
    3,
    0x12,
    0x10,
    0x13,
    0x10,
    0x13,
    6,
    0x12,
    6,
    0x11,
    1,
    1,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180DC8[0xC] = { 1, 1, 4, 0x11, 4, 0x12, 0x40, 0x13, 0xFF, 0, 0, 0 };
u8 D_us_80180DD4[0xC] = { 0x14, 0x1C, 0x14, 0x1D, 0x14, 0x1E, 0x14, 0x1D, 0, 0, 0, 0 };
u8 D_us_80180DE0[8] = { 1, 0x1C, 1, 0x1F, 0, 0, 0, 0 };
u8 D_us_80180DE8[0x1C] = {
    1,
    0x1C,
    0xA,
    0x20,
    0x10,
    0x21,
    0xA,
    0x22,
    0x10,
    0x23,
    0xA,
    0x24,
    0x10,
    0x25,
    8,
    0x26,
    8,
    0x27,
    8,
    0x28,
    8,
    0x29,
    8,
    0x1C,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180E04[0x14] = {
    2,
    0x1C,
    6,
    0x2E,
    6,
    0x2F,
    6,
    0x30,
    6,
    0x31,
    0x16,
    0x32,
    8,
    0x20,
    1,
    0x21,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180E18[0x44] = {
    4,
    0x21,
    6,
    0x2A,
    6,
    0x2B,
    6,
    0x2C,
    6,
    0x2D,
    2,
    0x1C,
    0xFF,
    0,
    0,
    0,
    0x20,
    0x1C,
    4,
    0x24,
    6,
    0x25,
    4,
    0x33,
    6,
    0x34,
    4,
    0x35,
    0x1A,
    0x36,
    6,
    0x35,
    6,
    0x34,
    4,
    0x33,
    4,
    0x22,
    6,
    0x23,
    6,
    0x37,
    4,
    0x38,
    0x1A,
    0x39,
    6,
    0x38,
    6,
    0x37,
    4,
    0x20,
    6,
    0x21,
    6,
    0x3A,
    4,
    0x3B,
    0x1A,
    0x3C,
    6,
    0x3B,
    6,
    0x3A,
    2,
    0x1C,
    0xFF,
    0,
};
u8 D_us_80180E5C[0x10] = { 0x10, 0x1C, 4, 0x24, 6, 0x25, 4, 0x33, 6, 0x34, 4, 0x35, 0xA, 0x36, 0xFF, 0 };
u8 D_us_80180E6C[0x14] = {
    0x10,
    0x36,
    6,
    0x35,
    6,
    0x34,
    4,
    0x33,
    4,
    0x22,
    6,
    0x23,
    6,
    0x37,
    4,
    0x38,
    0xA,
    0x39,
    0xFF,
    0,
};
u8 D_us_80180E80[0x14] = {
    0x10,
    0x39,
    6,
    0x38,
    6,
    0x37,
    4,
    0x20,
    6,
    0x21,
    6,
    0x3A,
    4,
    0x3B,
    0xA,
    0x3C,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180E94[0xC] = { 0x10, 0x3C, 6, 0x3B, 6, 0x3A, 2, 0x1C, 0xFF, 0, 0, 0 };
u8 D_us_80180ED8[0xC] = { 8, 0x50, 8, 0x51, 8, 0x52, 8, 0x53, 0, 0, 0, 0 };
u8 D_us_80180EE4[0x10] = { 1, 0x55, 1, 0x58, 1, 0x56, 1, 0x59, 1, 0x57, 1, 0x5A, 0, 0, 0, 0 };
u8 D_us_80180EF4[0x18] = {
    4,
    0x5B,
    4,
    0x5C,
    4,
    0x5D,
    4,
    0x5E,
    1,
    0x55,
    1,
    0x58,
    1,
    0x56,
    1,
    0x59,
    1,
    0x57,
    1,
    0x5A,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180F0C[0xC] = { 7, 0x5F, 7, 0x60, 7, 0x61, 7, 0x62, 7, 0x63, 0xFF, 0 };
u8 D_us_80180F18[0xC] = { 8, 0x64, 8, 0x65, 8, 0x64, 8, 0x66, 0, 0, 0, 0 };
u8 D_us_80180F24[0xC] = { 4, 0x64, 4, 0x65, 4, 0x64, 4, 0x66, 0, 0, 0, 0 };
u8 D_us_80180F30[0x10] = { 2, 0x64, 8, 0x67, 8, 0x68, 8, 0x69, 8, 0x6A, 8, 0x6B, 0x10, 0x6C, 0xFF, 0 };
u8 D_us_80180F40[0xC] = { 8, 0x6D, 8, 0x6C, 8, 0x6E, 8, 0x6C, 0, 0, 0, 0 };
ScyllaRoomPiece D_us_80180F4C[5] = {
    { D_us_80180EA0, 0x86, 0x78, 0xB0 },
    { D_us_80180EAC, 0x84, 0x88, 0xB0 },
    { D_us_80180EB8, 0x90, 0x50, 0xAC },
    { D_us_80180EC4, 0x86, 0x24, 0x90 },
    { D_us_80180ECC, 0x84, 0x20, 0x88 },
};
s16 D_us_80180F88[4] = { 0x10, 8, 8, 8 };
s16 D_us_80180F90[8] = { 0, 8, 0, 4, 4, -4, -8, 0 };
SVECTOR D_us_80180FA0 = { -0x10, -0x10, 0, 0 };
SVECTOR D_us_80180FA8 = { 0x10, -0x10, 0, 0 };
SVECTOR D_us_80180FB0 = { -0x10, 0x10, 0, 0 };
SVECTOR D_us_80180FB8 = { 0x10, 0x10, 0, 0 };
u8 D_us_80180FCC[0xC] = { 4, 0x49, 8, 0x4C, 8, 0x49, 8, 0x4D, 4, 0x49, 0xFF, 0 };
u8 D_us_80180FD8[8] = { 8, 0x49, 8, 0x4E, 8, 0x4F, 0xFF, 0 };
u8 D_us_80180FE0[8] = { 4, 0x4F, 8, 0x4E, 8, 0x49, 0xFF, 0 };
// end pc_import_data.py
// clang-format on
#endif
