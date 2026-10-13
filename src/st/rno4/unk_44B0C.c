// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rno4.h"

#if defined(VERSION_PC) || defined(M2CTX)
// file scope, for pc_import_data.py (it misses block-scope externs)
extern u16 g_FloatingIcePlatformHitbox[];
extern s16 g_GreyPuffScale[];
extern s32 g_GreyPuffVelocity[];
extern s16 g_BreakableCrystalFloorTiles[];
#endif

extern EInit g_EInitBreakable;
extern u8* D_us_80180DA4[];
extern u8 D_us_80180DCC[];
extern u8 D_us_80180DD8[];
extern u16 D_us_80180DE4[];
extern u16 D_us_80180DF8[];
extern u16 D_us_80180E0C[];
extern u8 D_us_80180E20[];
extern s16 D_us_80180E2C[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", EntityBreakable);
#else
// NO4's breakables (e_breakable_no4.h) as RNO4 has them: the candles
// (type 0) hang upside down, and type 8 drops its params' item like 7
void EntityBreakable(Entity* self) {
    Entity* entity;
    u16 breakableType;

    breakableType = self->params >> 12;
    if (!self->step) {
        InitializeEntity(g_EInitBreakable);
        self->zPriority = g_unkGraphicsStruct.g_zEntityCenter - 0x14;
        self->blendMode = D_us_80180E20[breakableType];
        self->hitboxHeight = D_us_80180DCC[breakableType];
        self->animSet = D_us_80180DF8[breakableType];
        self->unk5A = D_us_80180E0C[breakableType];
        self->palette = D_us_80180DE4[breakableType];
        self->hitboxOffY = D_us_80180E2C[breakableType];
        if (!breakableType) {
            self->drawFlags = ENTITY_ROTATE;
            self->rotate = 0x800;
        }
    }

    AnimateEntity(D_us_80180DA4[breakableType], self);

    if (self->hitParams) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromCurrentEntity(E_EXPLOSION, entity);
            entity->params = D_us_80180DD8[breakableType];
        }

        switch (breakableType) {
        case 0:
            self->drawFlags = 0;
            self->rotate = 0;
            g_api.PlaySfx(SFX_CANDLE_HIT);
            break;

        case 2:
        case 3:
            break;

        case 9:
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_UNK_3C, entity);
                entity->params = 0x100;
            }
            g_api.PlaySfx(SFX_GLASS_BREAK_E);
            break;

        case 7:
        case 8:
            g_api.PlaySfx(SFX_GLASS_BREAK_E);
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_PERSISTENT_ITEM_DROP, self, entity);
                entity->params = self->params & 0x1FF;
            }
            PreventEntityFromRespawning(self);
            DestroyEntity(self);
            return;

        default:
            g_api.PlaySfx(SFX_CANDLE_HIT);
            break;
        }

        ReplaceBreakableWithItemDrop(self);
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C123C_from_no4);
#else
// NO4's func_us_801C123C with RNO4's sprite set and palette
void func_us_801C123C_from_no4(Entity* self) {
    u32 pad[10];
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = ANIMSET_OVL(9);
        self->unk5A = 0x5B;
        self->palette = 0x25D;
        self->animCurFrame = 0x15;
        self->zPriority = 0x6A;
        self->step = 0x100;
        break;
    }
}
#endif

extern s16 D_us_80181094[];
extern u8 D_us_80181070[];
extern u8 D_us_80181088[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C12B0_from_no4);
#else
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
            prim->priority = 0x9E;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        break;
    }

    prim = self->ext.et_801C12B0.prim;
    AnimateEntity(D_us_80181088, self);
    i = (self->params >> 8) & 0xFF;
    ptr = &D_us_80181094[(self->params & 0xFF) * 5];
    scrollX = g_Tilemap.scrollX.i.hi - 0x10;
    scrollY = g_Tilemap.scrollY.i.hi;
    xOffset = scrollX + 0x120;
    var_s5 = &D_us_80181070[self->animCurFrame * 4];
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
            // This fixes a bunch of registers on PSP
            // It should mean the else below is not necessary but
            // removing it reintroduces the issue.
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
#endif

extern s16 D_us_80181190[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C15F8_from_no4);
#else
// NO4's func_us_801C15F8 with RNO4's colour; its areas list the bottom
// edge first
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
            prim->r0 = 0x10;
            prim->g0 = 8;
            prim->b0 = 0x18;
            prim->priority = 0x9D;
            prim = prim->next;
        }
        break;
    }

    prim = self->ext.et_801C12B0.prim;
    ptr = &D_us_80181190[(self->params & 0xFF) * 4];
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

        // RNO4's table holds the bottom before the top
        var_s4 = *ptr++;
        var_s5 = *ptr++;
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

#ifdef VERSION_PC
// D_us_80181084 is the last entry of D_us_80181070, imported whole
extern u8 D_us_80181070[];
#define D_us_80181084 (D_us_80181070 + 0x14)
extern s16 D_us_80181280[];
#else
extern u8 D_us_80181084[2];
extern s16 D_us_80181280[50]; // unknown size?
#endif

void func_us_801C5364(Entity* self) {
    Primitive* prim;
    u32 subTileX;
    int bgColorValue;
    int tileY1Wrap;
    s32 tileY1;
    s32 tileX0;
    s32 tileX1;
    u8* tilePos;
    s32 tileX1Wrap;
    s32 primIndex;
    s32 x0;
    long y0;
    s32 tileY0;
    s32 x1;
    s32 y1;
    s32 i;
    s16* tileDimensions; // It holds some dimensions of something, I'm guessing
                         // some tile or entity sprite

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        self->ext.et_801C5364.unk80 = 4;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x10);
        if (primIndex == (-1)) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C5364.prim7C = prim;
        while (prim) {
            prim->tpage = 0xF;
            prim->clut = 0x85;
            prim->priority = 0x18;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }
    if (g_Timer & 0x80) {
        if (g_Timer & 0x40) {
            bgColorValue = 0x3f - (g_Timer & 0x3f);
        } else {
            bgColorValue = g_Timer & 0x3f;
        }
    } else {
        bgColorValue = 0;
    }
    bgColorValue = bgColorValue + 0x80;
    bgColorValue = 0x80;

    g_GpuBuffers[0].draw.r0 = ((bgColorValue * 24) / 0x80) & 0xf8;
    g_GpuBuffers[0].draw.g0 = 0;
    g_GpuBuffers[0].draw.b0 = ((bgColorValue * 8) / 128) & 0xf8;
    g_GpuBuffers[1].draw.r0 = ((bgColorValue * 24) / 128) & 0xf8;
    g_GpuBuffers[1].draw.g0 = 0;
    g_GpuBuffers[1].draw.b0 = ((bgColorValue * 8) / 0x80) & 0xf8;

    prim = self->ext.et_801C5364.prim7C;

    i = (self->params >> 8) & 0xff;
    tileDimensions = D_us_80181280 + ((self->params & 0xff) * 4);
    x0 = g_Tilemap.scrollX.i.hi - 0x10;
    y0 = g_Tilemap.scrollY.i.hi;
    x1 = x0 + 0x120;
    y1 = y0 + 0xe0;
    tilePos = D_us_80181084;
    tileY1Wrap = tilePos[1];
    while (i > 0) {
        tileX0 = *(tileDimensions++);
        tileX1 = *(tileDimensions++);
        if ((x0 >= tileX0) || (x1 < tileX1)) {
            tileDimensions = tileDimensions + 2;
        } else {
            tileY0 = *(tileDimensions++);
            tileY1 = *(tileDimensions++);
            if ((y0 < tileY0) && (!(y1 < tileY1))) {
                {
                    if (x0 > tileX1) {
                        tileX1 = x0;
                    }
                }
                if (tileX0 > x1) {
                    tileX0 = x1;
                }
                tileX0 -= tileX1;
                tileX1 -= (x0 + 0x10);
                subTileX = ((x0 / 4) + tileX1) % 126;
                if (tileY1 < y0) {
                    tileY1 = y0;
                }
                if (y1 < tileY0) {
                    tileY0 = y1;
                }
                tileY1Wrap = 0x63 - ((y0 - (g_Tilemap.height - 0x100)) / 4);
                y1 = y1 + 0x46;
                tileY1 = tileY1 - y0;
                tileY0 -= y0;
                if ((tileY1Wrap < tileY0) && (y1 >= tileY1)) {
                    if (tileY1 < tileY1Wrap) {
                        tileY1 = tileY1Wrap;
                    }
                    if (y1 < tileY0) {
                        tileY0 = y1;
                    }
                    tileY0 -= tileY1;
                    tileY1Wrap = ((y0 - (g_Tilemap.height - 0x100)) / 4) +
                                 (tileY1 - 0x63);
                    subTileX += tilePos[0];
                    do {
                        prim->u0 = prim->u2 = subTileX;
                        tileX1Wrap = 0x7e - (subTileX - tilePos[0]);
                        if (tileX0 < tileX1Wrap) {
                            tileX1Wrap = tileX0;
                        }
                        prim->u1 = prim->u3 = subTileX + tileX1Wrap;
                        prim->x0 = prim->x2 = tileX1;
                        tileX1 = tileX1 + tileX1Wrap;

                        prim->x1 = (prim->x3 = tileX1);
                        tileX0 = tileX0 - tileX1Wrap;
                        subTileX = tilePos[0];

                        // There's this empty statement after all,
                        // I missed it when I thought I'd found the solution
                        // I had permuter running it for ~20 hours and couldn't
                        // find anything else
                        tilePos = tilePos;

                        if (tileY1Wrap > 0x46) {
                            tileX1Wrap = 0;
                        } else {
                            tileX1Wrap = 0x46 - tileY1Wrap;
                        }
                        prim->v0 = prim->v1 = tilePos[1] + tileX1Wrap;
                        prim->v2 = prim->v3 = tilePos[1];
                        prim->y0 = prim->y1 = tileY1;
                        prim->y2 = prim->y3 = tileY1 + tileX1Wrap;
                        prim->drawMode = DRAW_COLORS;
                        prim->r0 = prim->r1 = prim->r2 = prim->r3 = prim->b0 =
                            prim->b1 = prim->b2 = prim->b3 = bgColorValue;
                        prim->g0 = prim->g1 = prim->g2 = prim->g3 = 0x80;
                        prim = prim->next;
                    } while (tileX0);
                }
            }
        }
        i -= 1;
    }

    while (prim) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

extern s16 D_us_80181428[][3];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", EntityBgColumnsParallax_from_no4);
#else
// NO4's background columns in parallax, upside down: their height counts
// from the bottom of the room, and an x of 0x7FFF ends the table
void EntityBgColumnsParallax_from_no4(Entity* self) {
    s32 primIndex;
    s32 scrollY;
    s32 scrollX;
    s32 flipXY;
    s16* ptr;
    s32 posYBottom;
    s32 posY;
    s32 posX;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 10);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->tpage = 0xE;
            prim->clut = 0x84;
            prim->u0 = prim->u2 = 0xE9;
            prim->u1 = prim->u3 = 0xFD;
            prim->v0 = prim->v1 = 0x7F;
            prim->v2 = prim->v3 = 1;
            prim->priority = 0x1C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    ptr = (s16*)D_us_80181428;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollX /= 2;
    scrollX &= 0xFF;
    scrollY = g_Tilemap.scrollY.i.hi;
    scrollY /= 2;
    while (1) {
        posX = *ptr++;
        posY = *ptr++;
        flipXY = *ptr++;
        posX -= scrollX;
        posY = (g_Tilemap.height - 0x100) / 2 + 0x7A - posY - scrollY;
        posYBottom = posY + 0x7E;
        if (posX < -20) {
            continue;
        }
        if (posX >= STAGE_WIDTH) {
            break;
        }
        if (posYBottom >= 0 && posY < 0xE0) {
            if (flipXY & 1) {
                prim->x0 = prim->x2 = posX + 0x14;
                prim->x1 = prim->x3 = posX;
            } else {
                prim->x0 = prim->x2 = posX;
                prim->x1 = prim->x3 = posX + 0x14;
            }
            if (flipXY & 2) {
                prim->y0 = prim->y1 = posY;
                prim->y2 = prim->y3 = posYBottom;
            } else {
                prim->y0 = prim->y1 = posYBottom;
                prim->y2 = prim->y3 = posY;
            }
            prim->drawMode = DRAW_DEFAULT;
            prim = prim->next;
        }
    }
    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}
#endif

extern s16 D_us_80181460[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C1EE4_from_no4);
#else
// NO4's func_us_801C1EE4 upside down: thin strips of falling water between
// heights measured from the bottom of the room. Unlike NO4, the first
// strip's v0 adds its length to the v0 the primitive kept from the last
// frame, which scrolls the texture.
void func_us_801C1EE4_from_no4(Entity* self) {
    s32 scrollX;
    s32 scrollY;
    s32 clut;
    s32 yTop;
    s32 yBottom;
    s32 yMid;
    s32 span;
    s32 first;
    s32 rest;
    s32 x;
    Primitive* prim;
    s32 primIndex;
    s16* ptr;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 16);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.et_801C12B0.clut = 0;
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->tpage = 0xE;
            prim->u0 = prim->u2 = 0xFC;
            prim->u1 = prim->u3 = 0xFE;
            prim->priority = 0x1A;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    ptr = D_us_80181460;
    prim = self->ext.et_801C12B0.prim;
    self->ext.et_801C12B0.clut++;
    if (self->ext.et_801C12B0.clut >= 0xE) {
        self->ext.et_801C12B0.clut = 0;
    }
    scrollX = g_Tilemap.scrollX.i.hi * 3 / 8;
    scrollX &= 0xFF;
    scrollY = g_Tilemap.scrollY.i.hi;
    yBottom = g_Tilemap.height - (scrollY + 0x50);
    yTop = g_Tilemap.height - (scrollY + 0xB0);
    if (yBottom > 0xE0) {
        yBottom = 0xE0;
    }
    if (yBottom >= 0) {
        scrollY = (scrollY * 3 / 8 + yTop) % 0x20;
        span = yBottom - yTop;
        if (scrollY + span > 0x60) {
            first = 0x60 - scrollY;
        } else {
            first = span;
        }
        rest = span - first;
        yMid = first + yTop;
        clut = self->ext.et_801C12B0.clut + 0x90;
        while (true) {
            x = *ptr++;
            x -= scrollX;
            if (x < -2) {
                continue;
            }
            if (x >= 0x100) {
                break;
            }
            prim->clut = clut;
            prim->v2 = prim->v3 = scrollY + 0x84;
            prim->x0 = prim->x2 = x;
            prim->x1 = prim->x3 = x + 2;
            prim->y0 = prim->y1 = yTop;
            prim->y2 = prim->y3 = yMid;
            prim->drawMode =
                DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
            prim->v0 = prim->v1 = prim->v0 + first;
            prim = prim->next;
            if (rest != 0) {
                prim->clut = clut;
                prim->v0 = prim->v1 = 0x84;
                prim->v2 = prim->v3 = rest + 0x84;
                prim->x0 = prim->x2 = x;
                prim->x1 = prim->x3 = x + 2;
                prim->y0 = prim->y1 = yMid;
                prim->y2 = prim->y3 = yMid + rest;
                prim->drawMode =
                    DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
                prim = prim->next;
            }
        }
    }
    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}
#endif

extern Tilemap* D_pspeu_0929B8B8; // This variable is assigned to a different
                                  // var that is never used

void func_us_801C5C78(Entity* self) {
    Primitive* prim;
    Tilemap** tilemap_pp_0;
    u32 primIndex;
    u32 xOffset;
    u16 params;
    u32 clut;
    s32 scrollY;
    s32 posY;
    u32 scrollYModulo;

    params = self->params;
    if (!self->step) {
        InitializeEntity(g_EInitParticle);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 2);
        if (primIndex == (-1)) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA |
                       FLAG_DESTROY_IF_BARELY_OUT_OF_CAMERA | FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C5C78.prim = prim;
        while (prim) {
            if (params) {
                prim->tpage = 0xF;
                prim->u0 = (prim->u2 = 0x82);
                prim->u1 = (prim->u3 = 0x9d);
            } else {
                prim->tpage = 0xE;
                prim->u0 = (prim->u2 = 0xe9);
                prim->u1 = (prim->u3 = 0xf7);
            }
            prim->priority = 0x62;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C5C78.prim;
    tilemap_pp_0 = &D_pspeu_0929B8B8;
    self->ext.et_801C5C78.unk84 += 1;

    if (self->ext.et_801C5C78.unk84 >= 0xE) {
        self->ext.et_801C5C78.unk84 = 0;
    }

    posY = self->posY.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;

    if (posY >= 0) {
        scrollY %= 32;
        if (params) {
            clut = 0x90;
        } else {
            clut = 0xB0;
        }
        clut = 0xB0;
        clut += self->ext.et_801C5C78.unk84;

        if (((scrollY + posY) - 0x50) > 0x60) {
            scrollYModulo = 0x60 - scrollY;
        }
        scrollYModulo = 0x60;

        prim->clut = clut;

        if (params) {
            xOffset = self->posX.i.hi - 0xD;
            prim->v2 = (prim->v3 = 3);
            prim->x1 = (prim->x3 = xOffset + 0x1B);
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_UNK02 | DRAW_TRANSP;
        } else {
            xOffset = self->posX.i.hi - 7;
            prim->v2 = (prim->v3 = 0x83);
            prim->x1 = (prim->x3 = xOffset + 0xe);
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_UNK02 | DRAW_TRANSP;
        }

        prim->v0 = (prim->v1 = prim->v2 + 0x60);
        prim->x0 = (prim->x2 = xOffset);
        prim->y0 = (prim->y1 = 0x4C);
        prim->y2 = (prim->y3 = 0xAC);
        prim = prim->next;
        prim->clut = clut;

        if (params) {
            prim->v0 = (prim->v1 = 0x63);
            prim->v2 = (prim->v3 = 0x2f);
            prim->x0 = (prim->x2 = xOffset);
            prim->x1 = (prim->x3 = xOffset + 0x1B);
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_UNK02 | DRAW_TRANSP;
        } else {
            prim->v0 = (prim->v1 = 0xe3);
            prim->v2 = (prim->v3 = 0xaf);
            prim->x0 = (prim->x2 = xOffset);
            prim->x1 = (prim->x3 = xOffset + 0xe);
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_UNK02 | DRAW_TRANSP;
        }

        prim->y0 = (prim->y1 = 0xAC);
        prim->y2 = (prim->y3 = 0xE0);
        prim = prim->next;
    }
}

void func_us_801C5EE4(Entity* self) {
    Primitive* prim;
    Entity* player;
    s32 scrollX;
    s32 clut;
    s32 upperBound;
    int playerPosRelTop;
    int playerPos;
    s32 scrollY;
    s32 scrollYWrap;
    s32 primIndex;

    player = g_Entities;
    scrollX = g_Tilemap.scrollX.i.hi;

    scrollY = g_Tilemap.scrollY.i.hi;
    scrollYWrap = ((0x500 - scrollY) / 24) + 0x4D;
    if (scrollYWrap > 0x7F) {
        scrollYWrap = 0x7F;
    }
    playerPos = (scrollX + player->posX.i.hi) - 0x128;
    if (playerPos > 0) {
        if (playerPos < 32) {
            playerPos = 0;
        } else {
            { playerPos = (playerPos - 32) / (-16); }
        }
        if (playerPos < (-8)) {
            playerPos = -8;
        }
    } else {
        if (playerPos > (-32)) {
            playerPos = 0;
        } else {
            playerPos = (playerPos + 32) / (-24);
        }
        if (playerPos > 8) {
            playerPos = 8;
        }
    }
    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 4);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.et_801C5EE4.unk84 = 0;
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = (&g_PrimBuf[primIndex]);
        self->ext.et_801C5EE4.prim7C = prim;

        while (prim) {
            prim->priority = 0x9C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }
    g_api.SetVolumeCommand22_23(scrollYWrap, playerPos);
    self->ext.et_801C5EE4.unk84 += 1;
    if (self->ext.et_801C5EE4.unk84 >= 0xE) {
        self->ext.et_801C5EE4.unk84 = 0;
    }
    clut = self->ext.et_801C5EE4.unk84 + 0xa0;
    prim = self->ext.et_801C5EE4.prim7C;
    playerPos = 0x50;
    upperBound = 0x550;
    upperBound = upperBound - scrollY;
    playerPos = playerPos - scrollY;
    if (playerPos < 0) {
        playerPos = 0;
    }
    if (upperBound > 0xF0) {
        upperBound = 0xF0;
    }
    playerPosRelTop = upperBound - playerPos;
    while (prim) {
        upperBound = playerPos + scrollY;
        {
            if (upperBound >= 0x452) {
                prim->tpage = 0xE;
                prim->u0 = (prim->u2 = 0x87);
                prim->u1 = (prim->u3 = 0xe7);
                prim->v2 = (prim->v3 = 0x550 - upperBound);
                prim->v0 = (prim->v1 = 0);
                scrollYWrap = 0x550 - upperBound;
                prim->x0 = (prim->x2 = 0x140 - scrollX);
                prim->x1 = (prim->x3 = 0xE0 - scrollX);
            } else {
                prim->tpage = 0xF;

                scrollYWrap = (0x452 - (playerPos + scrollY)) % 126;
                if (!scrollYWrap) {
                    scrollYWrap = 0x7E;
                }
                prim->u0 = (prim->u2 = 0xCC);
                prim->u1 = (prim->u3 = 0xFD);
                prim->v2 = (prim->v3 = scrollYWrap);
                if ((playerPos + 0x7E) < 0xF0) {
                    prim->v0 = (prim->v1 = 0);
                } else {
                    prim->v0 = (prim->v1 = playerPos - 0x72);
                };
                scrollYWrap = prim->v2 - prim->v0;
                if ((upperBound + scrollYWrap) > 0x452) {
                    scrollYWrap = upperBound - 0x452;
                    prim->v0 = (prim->v1 = prim->v2 - scrollYWrap);
                };
                prim->x0 = (prim->x2 = 0x140 - scrollX);
                prim->x1 = (prim->x3 = 0x10F - scrollX);
            }

            prim->y2 = (prim->y3 = playerPos);
            playerPos += scrollYWrap;
            playerPosRelTop -= scrollYWrap;
            prim->y0 = prim->y1 = playerPos;
#if defined(VERSION_PSP)
            prim->y2 -= 1;
            prim->y3 -= 1;
#endif
            prim->clut = clut;
            prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
        }
        prim = prim->next;
        if (playerPosRelTop <= 0) {
            break;
        }
    }

    while (prim) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C2850_from_no4);
#else
// NO4's func_us_801C2850 mirrored both ways: three gradient quads that
// glow over the waterfall, brightest at its foot (the top here)
void func_us_801C2850_from_no4(Entity* self) {
    Primitive* prim;
    s32 primIndex;
    s32 scrollX;
    s32 scrollY;
    s32 var_s4;
    s32 var_s3;
    s32 var_s2;
    s32 var_s1;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_G4, 3);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x9E;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (scrollY < 0x410) {
        var_s4 = 0x20 - scrollY;
        var_s3 = 0x220 - scrollY;
        var_s1 = scrollY & 7;
        if (var_s4 < -0x10) {
            var_s4 = (-var_s1 & 7) - 0x10;
        }
        if (var_s3 > 0xF0) {
            var_s3 = (-var_s1 & 7) + 0xF0;
        }
        var_s1 = 0x220 - (var_s4 + scrollY);
        var_s2 = 0x220 - (var_s3 + scrollY);
        if (self->ext.et_801C12B0.unk80 & 1) {
            var_s1 *= 0x13;
            var_s1 /= 40;
            var_s2 *= 0x13;
            var_s2 /= 40;
        } else {
            var_s1 *= 0x12;
            var_s1 /= 40;
            var_s2 *= 0x12;
            var_s2 /= 40;
        }
        // Prim 1
        prim->r0 = prim->r1 = prim->b0 = prim->b1 = prim->g0 = prim->g1 =
            var_s1;
        prim->r2 = prim->r3 = prim->b2 = prim->b3 = prim->g2 = prim->g3 =
            var_s2;
        prim->x0 = prim->x2 = 0x180 - scrollX;
        prim->x1 = prim->x3 = 0xE0 - scrollX;
        prim->y0 = prim->y1 = var_s4;
        prim->y2 = prim->y3 = var_s3;
        prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                         DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim = prim->next;
        // Prim 2
        prim->r0 = prim->r2 = prim->b0 = prim->b2 = prim->g0 = prim->g2 = 0;
        prim->r1 = prim->b1 = prim->g1 = var_s1;
        prim->r3 = prim->b3 = prim->g3 = var_s2;
        prim->x0 = prim->x2 = 0x1E0 - scrollX;
        prim->x1 = prim->x3 = 0x180 - scrollX;
        prim->y0 = prim->y1 = var_s4;
        prim->y2 = prim->y3 = var_s3;
        prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                         DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim = prim->next;
        // Prim 3
        prim->r0 = prim->b0 = prim->g0 = var_s1;
        prim->r2 = prim->b2 = prim->g2 = var_s2;
        prim->r1 = prim->b1 = prim->g1 = prim->r3 = prim->b3 = prim->g3 = 0;
        prim->x0 = prim->x2 = 0xE0 - scrollX;
        prim->x1 = prim->x3 = 0x80 - scrollX;
        prim->y0 = prim->y1 = var_s4;
        prim->y2 = prim->y3 = var_s3;
        prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                         DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim = prim->next;
    }
    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C2B78_from_no4);
#else
// NO4's func_us_801C2B78 upside down: the droplets spray upward
void func_us_801C2B78_from_no4(Entity* self) {
    Primitive* prim;
    s32 i;
    s32 var_s2;
    s32 var_s3;
    s32 scrollX;
    s32 primIndex;
    s32 scrollY;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 128);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x6C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (scrollY > 0x12F) {
        var_s3 = 0x50;
        var_s3 -= scrollY;
        for (i = 0; i < 4; i++) {
            while (prim != NULL) {
                if (prim->drawMode == DRAW_HIDE) {
                    prim->r0 = prim->b0 = prim->g0 = 0x80;
                    prim->x0 = ((((rand() % 11) * 4) + 0x115) - scrollX) + i;
                    prim->y0 = var_s3 - 4 + (rand() & 7);
                    prim->x1 = 0;
                    LOH(prim->r1) = 0;
                    prim->u0 = prim->v0 = 2;
                    LOW(prim->r2) = FIX(4);
                    LOW(prim->u1) = 0;
                    LOW(prim->x3) = (0x4000 - (rand() & 0xFF0));
                    LOW(prim->r3) = ((rand() & 0x1F00) - 0xF80);
                    prim->clut = 0x20;
                    prim->drawMode = DRAW_UNK02;
                    break;
                }

                prim = prim->next;
            }
        }
    }

    // This is probably an alternate Primitive
    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            LOH(prim->b1) = prim->x0;
            prim->y1 = prim->y0;
            LOW(prim->x1) -= LOW(prim->r2);
            LOW(prim->r1) += LOW(prim->u1);
            prim->x0 = LOH(prim->b1);
            prim->y0 = prim->y1;

            var_s2 = 0;
            if (LOW(prim->r2) < 0) {

            } else {
                var_s2 = 1;
            }
            LOW(prim->r2) -= LOW(prim->x3);
            LOW(prim->u1) -= LOW(prim->r3);

            if (!--prim->clut) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}
#endif

extern u16 D_us_80181478[13];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C2E60_from_no4);
#else
// NO4's func_us_801C2E60 upside down: the sprays fall from the ceiling
void func_us_801C2E60_from_no4(Entity* self) {
    s32 var_s5;
#ifdef VERSION_PSP
    u32 scrollX;
#else
    s32 scrollX;
#endif
    s32 scrollY;
    s32 var_s2;
    s32 primIndex;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 32);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->clut = PAL_CC_STONE_EFFECT;
            prim->tpage = 0x1A;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x1E;
            prim->v0 = prim->v1 = 0x7C;
            prim->v2 = prim->v3 = 0x60;
            prim->priority = 0x9D;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (self->ext.et_801C12B0.unk80 >= LEN(D_us_80181478)) {
        self->ext.et_801C12B0.unk80 = 0;
    }
    if (scrollY > 0x17F) {
        var_s2 = 0x50;
        var_s2 -= scrollY;

        while (prim != NULL) {
            if (prim->drawMode == DRAW_HIDE) {
                prim->y0 = prim->y1 = prim->y2 = prim->y3 = var_s2;
#ifdef VERSION_PSP
                var_s5 = (D_us_80181478[self->ext.et_801C12B0.unk80] * 4) +
                         0x111 - scrollX - 9;
                prim->x2 = prim->x0 = var_s5 + (rand() & 3);
#else
                var_s5 = (rand() & 3) - 9;
                prim->x2 = prim->x0 =
                    (D_us_80181478[self->ext.et_801C12B0.unk80] * 4) + 0x111 -
                    scrollX + var_s5;
#endif
                prim->x3 = prim->x1 = prim->x0 + 0x12;
                PGREY(prim, 0) = PGREY(prim, 1) = 0xFF;
                PGREY(prim, 2) = PGREY(prim, 3) = 0x80;
                prim->p1 = 0;
                prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                                 DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
                break;
            }

            prim = prim->next;
        }
    }
    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            prim->y2++;
            if (rand() & 7) {
                prim->y2++;
            }
            prim->y3 = prim->y2;
            prim->p1++;
            prim->b1 -= 0x10;
            PGREY(prim, 0) = PGREY(prim, 1);
            prim->b3 -= 8;
            PGREY(prim, 2) = PGREY(prim, 3);
            if (prim->r0 < 0x11) {
                prim->drawMode = DRAW_HIDE;
            }
        }

        prim = prim->next;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C3160_from_no4);
#else
// NO4's func_us_801C3160 mirrored both ways: spray around the waterfall
void func_us_801C3160_from_no4(Entity* self) {
    s32 scrollY;
    s32 primIndex;
    u16 iterations;
    u16 params;
    s32 scrollX;
    s32 i;
    s32 yOffset;
    s32 randVal;
    Primitive* prim;

    params = self->params;
    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        if (!params) {
            primIndex = g_api.AllocPrimitives(PRIM_TILE, 0x80);
        } else {
            primIndex = g_api.AllocPrimitives(PRIM_TILE, 8);
        }
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = (s32)primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x6C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    // This is probably an alternative Primitive type
    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (scrollY < g_Tilemap.height - 0xF0) {
        if (!params) {
            iterations = 4;
            yOffset = 0x550 - scrollY;
        } else {
            iterations = 1;
            yOffset = 0x12A - scrollY;
        }

        for (i = 0; i < iterations; i++) {
            while (prim != NULL) {
                if (prim->drawMode == DRAW_HIDE) {
                    if (!params) {
                        randVal = rand() & 0x1F;
                        prim->y0 = yOffset - randVal;
                        prim->x0 =
                            ((0x100 + (rand() % 40)) - scrollX) + randVal;
                        LOW(prim->u1) = 0x20000;
                        prim->u0 = prim->v0 = 2;
                        LOW(prim->r2) = 0;
                        prim->clut = 0x10;
                    } else {
                        prim->y0 = yOffset - (rand() & 3);
                        prim->x0 = (0x1F2 - scrollX) - (rand() & 3);
                        LOW(prim->u1) = -0x14000;
                        LOW(prim->r2) = ((rand() & 0x1F) << 0xB);
                        prim->u0 = prim->v0 = 1;
                        prim->clut = 8;
                    }
                    prim->r0 = prim->b0 = prim->g0 = 0x80;
                    prim->x1 = 0;
                    LOH(prim->r1) = 0;
                    LOW(prim->x3) = 0x4000;
                    LOW(prim->r3) = 0;
                    prim->drawMode = DRAW_UNK02;
                    break;
                }

                prim = prim->next;
            }
        }
    }

    // This is probably an alternate Primitive
    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            LOH(prim->b1) = prim->x0;
            prim->y1 = prim->y0;
            LOW(prim->x1) = LOW(prim->x1) - LOW(prim->r2);
            LOW(prim->r1) = LOW(prim->r1) + LOW(prim->u1);
            prim->x0 = LOH(prim->b1);
            prim->y0 = prim->y1;
            LOW(prim->r2) = LOW(prim->r2) + LOW(prim->x3);
            prim->r0 = prim->b0 = prim->g0 -= 8;
            if (!--prim->clut) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}
#endif

extern s16 D_us_80181494[][5];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C34EC_from_no4);
#else
void func_us_801C34EC_from_no4(Entity* self) {
    s32 scrollX;
    s16 xOffset;
    s16 randX;
    s16 randY;
    s16 tpage;
    s32 yOffset;
    s32 scrollY;
    s32 primIndex;
    s16* ptr;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 0x40);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x9B;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    ptr = D_us_80181494[self->params];
    xOffset = *ptr++;
    randX = *ptr++;
    yOffset = *ptr++;
    randY = *ptr++;
    tpage = *ptr;
    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (scrollY < 0xF0) {
        yOffset -= scrollY;

        while (prim != NULL) {
            if (prim->drawMode == DRAW_HIDE) {
                prim->r0 = prim->b0 = prim->g0 = 0x80;
                prim->y0 = yOffset + (rand() % randY);
                prim->x0 = (xOffset - scrollX) + (rand() % randX);
                prim->x1 = 0;
                LOH(prim->r1) = 0;
                prim->u0 = prim->v0 = 2;
                LOW(prim->u1) = 0;
                prim->tpage = tpage;
                prim->drawMode = DRAW_UNK02;
                break;
            }

            prim = prim->next;
        }
    }

    // This is probably an alternate Primitive
    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            LOH(prim->b1) = prim->x0;
            LOW(prim->r1) += LOW(prim->u1);
            prim->x0 = LOH(prim->b1);
            prim->r0 -= 8;
            prim->b0 = prim->g0 = prim->r0;
            if (prim->r0 < 8) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}
#endif

extern s16 D_us_80181094[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C37C8_from_no4);
#else
// NO4's func_us_801C37C8 for RNO4's layout: flickers the edges of six
// entries of func_us_801C12B0_from_no4's table and draws a wavering quad,
// upside down, in one of two places
void func_us_801C37C8_from_no4(Entity* self) {
    Primitive* prim;
    s32 scrollX;
    s32 scrollY;
    s32 xOffset;
    s32 primIndex;
    s32 clut;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.et_801C12B0.clut = 0;
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        prim->tpage = 0xF;
        prim->v0 = prim->v1 = 0x7F;
        prim->v2 = prim->v3 = 1;
        prim->priority = 0x9C;
        prim->drawMode = DRAW_HIDE;
    }

    scrollX = (rand() & 0x1F) - 0x10;
    D_us_80181094[40] = scrollX + 0xB30;
    D_us_80181094[41] = 0x180 - scrollX;
    D_us_80181094[36] = scrollX + 0x4C;
    D_us_80181094[25] = scrollX + 0x7C0;
    D_us_80181094[26] = 0x70 - scrollX;
    D_us_80181094[21] = scrollX + 0x2B0;
    self->ext.et_801C12B0.clut++;
    if (self->ext.et_801C12B0.clut >= 0xE) {
        self->ext.et_801C12B0.clut = 0;
    }

    clut = self->ext.et_801C12B0.clut + 0xA0;
    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;

    if (scrollX < 0x898) {
        xOffset = 0x828 - scrollX;
        if (xOffset < 0x100) {
            prim->u0 = prim->u2 = 0x11;
            prim->u1 = prim->u3 = 0x80;
        } else {
            prim->drawMode = DRAW_HIDE;
            return;
        }
    } else if (scrollX < 0xAE8) {
        xOffset = 0xA78 - scrollX;
        if (xOffset < 0x100) {
            prim->u0 = prim->u2 = 0x80;
            prim->u1 = prim->u3 = 0x11;
        } else {
            prim->drawMode = DRAW_HIDE;
            return;
        }
    } else {
        prim->drawMode = DRAW_HIDE;
        return;
    }

    prim->clut = clut;
    prim->x0 = prim->x2 = xOffset;
    prim->x1 = prim->x3 = xOffset + 0x6F;
    prim->y0 = prim->y1 = 0xD1 - scrollY;
    prim->y2 = prim->y3 = prim->y0 + 0x7F;
    prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C3A04_from_no4);
#else
// NO4's func_us_801C3A04 upside down: the droplets spray downward
void func_us_801C3A04_from_no4(Entity* self) {
    Primitive* prim;
    s32 i;
    s32 var_s2;
    s32 yOffset;
    s32 posX;
    s32 primIndex;
    s32 scrollY;

    if (!self->step) {
        InitializeEntity(g_EInitCommon);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 0x40);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;

        while (prim != NULL) {
            prim->priority = 0x6C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    posX = self->posX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    yOffset = 0xE0;
    yOffset -= scrollY;

    // This is probably an alternate Primitive
    for (i = 0; i < 2; i++) {
        while (prim != NULL) {
            if (prim->drawMode == DRAW_HIDE) {
                prim->r0 = prim->b0 = prim->g0 = 0x80;
                prim->x0 = ((rand() % 0x10) * 2) + i + posX;
                prim->y0 = yOffset - 4 + (rand() & 7);
                prim->x1 = 0;
                LOH(prim->r1) = 0;
                prim->u0 = prim->v0 = 2;
                LOW(prim->r2) = 0x40000;
                LOW(prim->u1) = 0;
                LOW(prim->x3) = 0x4000 - (rand() & 0xFF0);
                LOW(prim->r3) = (rand() & 0x1F00) - 0xF80;
                prim->clut = 0x20;
                prim->drawMode = DRAW_UNK02;
                break;
            }

            prim = prim->next;
        }
    }

    // This is probably an alternate Primitive
    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            LOH(prim->b1) = prim->x0;
            prim->y1 = prim->y0;
            LOW(prim->x1) -= LOW(prim->r2);
            LOW(prim->r1) += LOW(prim->u1);
            prim->x0 = LOH(prim->b1);
            prim->y0 = prim->y1;

            var_s2 = 0;
            if (LOW(prim->r2) >= 0) {
                var_s2 = 1;
            }
            LOW(prim->r2) -= LOW(prim->x3);
            LOW(prim->u1) -= LOW(prim->r3);
            if (!--prim->clut) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}
#endif

extern u16 D_us_801814A8[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C3CC4_from_no4);
#else
// NO4's func_us_801C3CC4 upside down: the sprays fall from 0xE0
void func_us_801C3CC4_from_no4(Entity* self) {
    s32 var_s5;
    s32 posX;
    s32 scrollY;
    s32 var_s2;
    s32 primIndex;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitParticle);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x20);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->clut = PAL_CC_STONE_EFFECT;
            prim->tpage = 0x1A;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x1E;
            prim->v0 = prim->v1 = 0x7C;
            prim->v2 = prim->v3 = 0x60;
            prim->priority = 0x9D;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    posX = self->posX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (self->ext.et_801C12B0.unk80 >= 9) {
        self->ext.et_801C12B0.unk80 = 0;
    }
    var_s2 = 0xE0;
    var_s2 -= scrollY;

    while (prim != NULL) {
        if (prim->drawMode == DRAW_HIDE) {
            prim->y0 = prim->y1 = prim->y2 = prim->y3 = var_s2;
#ifdef VERSION_PSP
            var_s5 = D_us_801814A8[self->ext.et_801C12B0.unk80] * 4;
            prim->x2 = prim->x0 = var_s5 + (rand() & 3) - 0x19 + posX;
#else
            var_s5 = rand() & 3;
            prim->x2 = prim->x0 =
                (D_us_801814A8[self->ext.et_801C12B0.unk80] * 4) + var_s5 -
                0x19 + posX;
#endif
            prim->x3 = prim->x1 = prim->x0 + 0x12;
            PGREY(prim, 0) = PGREY(prim, 1) = 0xFF;
            PGREY(prim, 2) = PGREY(prim, 3) = 0x80;
            prim->p1 = 0;
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
            break;
        }

        prim = prim->next;
    }

    prim = self->ext.et_801C12B0.prim;

    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            prim->y2++;
            if (rand() & 3) {
                prim->y2++;
            }
            prim->y3 = prim->y2;
            prim->p1++;
            prim->b1 -= 0x10;
            PGREY(prim, 0) = PGREY(prim, 1);
            prim->b3 -= 8;
            PGREY(prim, 2) = PGREY(prim, 3);
            if (prim->r0 < 0x11) {
                prim->drawMode = DRAW_HIDE;
            }
        }

        prim = prim->next;
    }
}
#endif

extern s16 D_us_801814BC[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C3FB0_from_no4);
#else
// NO4's func_us_801C3FB0 with its texture upside down
void func_us_801C3FB0_from_no4(Entity* self) {
    s32 primIndex;
    s32 scrollX;
    s32 scrollY;
    s16* ptr;
    s32 clut;
    s32 var_s4;
    s32 var_s3;
    s32 var_s2;
    s32 var_s1;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x10);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.et_801C12B0.clut = 0;
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;

        while (prim != NULL) {
            prim->tpage = 0xF;
            prim->u0 = prim->u2 = 0x82;
            prim->u1 = prim->u3 = 0x9D;
            prim->priority = 0xC0;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    ptr = D_us_801814BC;
    self->ext.et_801C12B0.clut++;
    if (self->ext.et_801C12B0.clut >= 0xE) {
        self->ext.et_801C12B0.clut = 0;
    }
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollX *= 5;
    scrollX /= 4;

    scrollY = g_Tilemap.scrollY.i.hi;
    scrollY *= 5;
    scrollY /= 4;
    scrollY %= 0x20;

    clut = self->ext.et_801C12B0.clut + 0x90;

    while (prim != NULL) {
        var_s3 = *ptr++;
        var_s3 -= scrollX;
        if (var_s3 < -0x1B) {
            continue;
        }

        if (var_s3 >= 0x100) {
            break;
        }

        var_s4 = scrollY + *ptr++;
        var_s2 = 0x60 - var_s4;
        var_s1 = 0;

        while (prim != NULL) {
            prim->clut = clut;
            prim->v0 = prim->v1 = 0x63 - var_s4;
            prim->v2 = prim->v3 = prim->v0 - var_s2;
            prim->x0 = prim->x2 = var_s3;
            prim->x1 = prim->x3 = var_s3 + 0x1B;
            prim->y0 = prim->y1 = var_s1;
            var_s1 += var_s2;
            prim->y2 = prim->y3 = var_s1;
            prim->drawMode =
                DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
            prim = prim->next;

            if (var_s1 >= 0xE8) {
                break;
            }

            var_s4 = 0;
            if (var_s1 > 0x88) {
                var_s2 = 0xE8 - var_s1;
            } else {
                var_s2 = 0x60;
            }
        }
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C4228_from_no4);
#else
// NO4's func_us_801C4228 mirrored left to right: a beam of light, drawn as
// four gradient quads that end at the left edge of the screen
void func_us_801C4228_from_no4(Entity* self) {
    s32 scrollY;
    s32 scrollX;
    s32 primIndex;
    s32 yOffset;
    s32 yMax;
    s32 xOffset;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_G4, 4);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0xC0;
            prim->r0 = prim->r2 = prim->g0 = prim->g2 = prim->r1 = prim->r3 =
                prim->g1 = prim->g3 = 0x10;
            prim->b0 = prim->b2 = prim->b1 = prim->b3 = 0;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    if (scrollX < 0x70) {
        xOffset = PLAYER.posX.i.hi - 0x10;
    } else {
        xOffset = 0xE0 - scrollX;
    }
    yOffset = 0x30 - scrollY;
    yMax = yOffset + 0x80;
    if (yMax > 0 && xOffset > -0x10) {
        // Prim 1
        prim->x0 = prim->x2 = xOffset;
        prim->x1 = prim->x3 = xOffset - 0x30;
        prim->y0 = prim->y1 = yOffset;
        prim->y2 = prim->y3 = yMax;
        prim->r1 = prim->r3 = prim->g1 = prim->g3 = prim->b1 = prim->b3 = 0xFF;
        prim->r0 = prim->r2 = prim->g0 = prim->g2 = prim->b0 = prim->b2 = 0;
        prim->drawMode =
            DRAW_UNK_40 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim = prim->next;
        // Prim 2
        prim->x0 = prim->x2 = xOffset - 1;
        xOffset -= 3;
        prim->x1 = prim->x3 = xOffset;
        prim->y0 = prim->y1 = yOffset;
        prim->y2 = prim->y3 = yMax;
        prim->r0 = prim->r2 = prim->g0 = prim->g2 = 7;
        prim->b0 = prim->b2 = prim->b1 = prim->b3 = 0;
        prim->r1 = prim->r3 = prim->g1 = prim->g3 = 8;
        prim->drawMode =
            DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim->priority = 0xC1;
        prim = prim->next;
        // Prim 3
        prim->x0 = prim->x2 = xOffset;
        xOffset -= 0x2D;
        prim->x1 = prim->x3 = xOffset;
        prim->y0 = prim->y1 = yOffset;
        prim->y2 = prim->y3 = yMax;
        prim->r0 = prim->r2 = prim->g0 = prim->g2 = prim->r1 = prim->r3 =
            prim->g1 = prim->g3 = 0x10;
        prim->b0 = prim->b2 = prim->b1 = prim->b3 = 0;
        prim->drawMode =
            DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim->priority = 0xC1;
        prim = prim->next;
        // Prim 4
        if (xOffset > -0x10) {
            prim->x0 = prim->x2 = -0x10;
            prim->x1 = prim->x3 = xOffset;
            prim->y0 = prim->y1 = yOffset;
            prim->y2 = prim->y3 = yMax;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        }
    }
    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}
#endif

void EntityWaterBox(Entity* self) {
    Entity* player;
    u16 collision;
    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = ANIMSET_OVL(1);
        self->animCurFrame = 6;
        if (g_CastleFlags[NO4_WATER_BLOCKED]) {
            self->posX.i.hi = 0x720 - g_Tilemap.scrollX.i.hi;
        } else {
            self->posX.i.hi = 0x760 - g_Tilemap.scrollX.i.hi;
        }
    }

    player = &PLAYER;
    collision = GetPlayerCollisionWith(self, 0x10, 0x11, 5);
    if ((collision & 1) && (g_Player.vram_flag & TOUCHING_GROUND)) {
        if (self->posX.i.hi > player->posX.i.hi) {
            if ((g_pads[0].pressed & PAD_RIGHT) && (PLAYER.step == 1)) {
                if (self->ext.timer.t) {
                    self->ext.timer.t--;
                    return;
                }
                if (self->posX.i.hi + g_Tilemap.scrollX.i.hi < 0x7A0) {
                    self->posX.i.hi++;
                    player->posX.i.hi++;
                }
                self->ext.timer.t = 3;
            }
        } else {
            if ((g_pads[0].pressed & PAD_LEFT) && (PLAYER.step == 1)) {
                if (self->ext.timer.t) {
                    self->ext.timer.t--;
                    return;
                }
                if (self->posX.i.hi + g_Tilemap.scrollX.i.hi > 0x720) {
                    self->posX.i.hi--;
                    player->posX.i.hi--;
                    if (self->posX.i.hi + g_Tilemap.scrollX.i.hi == 0x720) {
                        g_CastleFlags[NO4_WATER_BLOCKED] = 1;
                    }
                }
                self->ext.timer.t = 3;
            }
        }
    }
}

#ifdef VERSION_PSP
extern s32 E_ID(UNK_26);
extern s32 E_ID(UNK_27);
#endif
#ifdef VERSION_PC
extern u8 D_us_801814D4[]; // an animation
#else
extern s16 D_us_801814D4;
#endif

void func_us_801C81C8(Entity* self) {
    Entity* child;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = -0x7FFE;
        self->palette = 0x44;
        self->drawFlags = ENTITY_MASK_R;
        self->posX.i.hi = (0x1EF - g_Tilemap.scrollX.i.hi);
        child = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (child) {
            CreateEntityFromCurrentEntity(E_ID(UNK_27), child);
            child->params = 1;
        }
        self->ext.et_801C81C8.unk80 = child;
        child = AllocEntity(child, &g_Entities[256]);
        if (child) {
            CreateEntityFromCurrentEntity(E_ID(UNK_26), child);
            child->params = 1;
        }
        self->ext.et_801C81C8.unk84 = child;
        self->ext.et_801C81C8.unk7C = 0;
    }
    AnimateEntity(&D_us_801814D4, self);
}

void EntityFloatingIcePlatform(Entity* self) {
    extern u16 g_FloatingIcePlatformHitbox[];
    u16* hitboxPtr;
    u16 collision;
    Entity* player;
    s16 prevPosY;
    s16 dx, dy;
    u16 hitboxIndex;

    player = &PLAYER;
    hitboxIndex = self->params;

    if (!self->step) {
        InitializeEntity(g_EInitCommon);
        self->animSet = ANIMSET_OVL(1);
        self->animCurFrame = hitboxIndex + 25;
        self->drawFlags = ENTITY_ROTATE;
        self->ext.floatingIcePlatform.baseY =
            self->posY.i.hi + g_Tilemap.scrollY.i.hi;
    }

    hitboxPtr = &g_FloatingIcePlatformHitbox[hitboxIndex * 2];

    prevPosY = self->posY.i.hi;
    self->posY.i.hi =
        self->ext.floatingIcePlatform.baseY - g_Tilemap.scrollY.i.hi +
        self->ext.floatingIcePlatform.bobOffset;
#ifdef VERSION_PSP
    collision = GetPlayerCollisionWith(self, hitboxPtr[0], hitboxPtr[1], 4);
#else
    collision = GetPlayerCollisionWith(self, *hitboxPtr++, *hitboxPtr, 4);
#endif
    self->posY.i.hi = prevPosY;
    self->ext.floatingIcePlatform.previousBobOffset =
        self->ext.floatingIcePlatform.bobOffset;

    dx = self->posX.i.hi - player->posX.i.hi;

    if (collision) {
        if (self->ext.floatingIcePlatform.bobOffset < 4) {
            self->ext.floatingIcePlatform.bobOffset++;
        }
    } else {
        if (self->ext.floatingIcePlatform.bobOffset) {
            self->ext.floatingIcePlatform.bobOffset--;
        }
    }

    dy = self->ext.floatingIcePlatform.bobOffset;
    if (dx < 0) {
        prevPosY = (dx * dy * -0x100) / 56;
    } else {
        prevPosY = (dx * dy * 0x100) / 56;
    }

    self->posY.i.hi = self->ext.floatingIcePlatform.baseY -
                      g_Tilemap.scrollY.i.hi + (dy - prevPosY / 256);

    if (collision) {
        dy = dy - self->ext.floatingIcePlatform.previousBobOffset;
        player->posY.i.hi += dy;
        g_unkGraphicsStruct.shoveX.i.hi += dy;
    }

    prevPosY = -prevPosY;
    if (collision || dy) {
        if (dx < 0) {
            self->rotate = ratan2(prevPosY, -0x3800);
            self->rotate = (self->rotate - 0x800) & 0xFFF;
            return;
        }
        self->rotate = ratan2(prevPosY, 0x3800);
    } else {
        self->rotate = 0;
    }
}

extern bool D_us_8018104C;
extern s16 D_us_801814E8[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C4BD8_from_no4);
#else
void func_us_801C4BD8_from_no4(Entity* self) {
    Entity* player;
    s16* dataPtr;
    s32 volume;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
    }

    player = &PLAYER;
    dataPtr = &D_us_801814E8[self->params * 4];

    volume = player->posX.i.hi + g_Tilemap.scrollX.i.hi - *dataPtr++;
    volume = (volume * *dataPtr++) / 0x1000;
    volume += *dataPtr++;

    if (volume < 0) {
        volume = 0;
    } else if (volume > 0x7F) {
        volume = 0x7F;
    }

    if (!volume) {
        if (D_us_8018104C) {
            D_us_8018104C = false;
            g_api.PlaySfx(SET_UNK_A6);
            return;
        }
    }
    if (D_us_8018104C) {
        g_api.SetVolumeCommand22_23(volume, *dataPtr++);
        return;
    }

    g_api.PlaySfxVolPan(SFX_WATERFALL_LOOP, volume, *dataPtr++);
    D_us_8018104C = true;
}
#endif

void func_us_801C8668(Entity* self) {
    s32 i;
    u16* tile;
    Tilemap* tilemap;

    tilemap = &g_Tilemap;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        tile = tilemap->fg + 0x1052;

        for (i = 0; i < 5; ++i) {
            *tile = 0xac7;
            tile += 1;
        }
        *tile = 0x59D;
        tile = tilemap->fg + 0x1062;

        for (i = 0; i < 0xA; ++i) {
            *tile = 0xAC7;
            tile += 1;
        }
    }
}

void RNO4_Unused801C8704(void) {}

void func_us_801C870C(Entity* self) {
    s16 i;
    u16* tilePtr;

    if (self->params == 0) {
        tilePtr = g_Tilemap.fg + 0x143;
    } else {
        tilePtr = g_Tilemap.fg + 0x53;
    }

    for (i = 0; i < 0xA; ++i) {
        tilePtr[0] = 0;
        ++tilePtr;
    }
}

void RNO4_Unused801C8768(void) {}

void RNO4_Unused801C8770(void) {}

extern s16 D_us_80181530[];
extern s16 D_us_80181548[];
extern u16 D_us_80181508[];
extern u8 D_us_80181528[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", EntityBoatElevatorChains);
#else
void EntityBoatElevatorChains(Entity* self) {
    u32 primIndex;
    u32 scrollX;

    u32 scrollY;
    s16 cos;
    u8* ptr;
    s32 i;
    s16* ptrTwo;
    s16 sin;
    s16 xOffset;
    s16 yOffset;
    Primitive* prim;

    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 13);
        if (primIndex != -1) {
            self->flags |= FLAG_HAS_PRIMS;
            self->primIndex = primIndex;
            prim = &g_PrimBuf[primIndex];

            i = 0;
            while (prim != NULL) {
                prim->tpage = 0xF;
                prim->clut = 0x5F;
                ptr = *D_us_80181528;
                ptr += D_us_80181508[i] * 4;
                prim->u0 = prim->u2 = *ptr++;
                prim->u1 = prim->u3 = *ptr++;
                prim->v0 = prim->v1 = *ptr++;
                prim->v2 = prim->v3 = *ptr;
                prim->priority = 0x80;
                prim->drawMode = DRAW_HIDE;
                prim = prim->next;
                i++;
            }
            self->rotate = ROT(45);
        } else {
            self->step = 0;
            return;
        }
    }

    if (self->ext.boatElevator_child.unk7C) {
        if (self->ext.boatElevator_child.unk7C < 0) {
            self->ext.boatElevator_child.unk7E++;
            self->rotate += ROT(1.40625);
        } else {
            self->ext.boatElevator_child.unk7E--;
            self->rotate -= ROT(1.40625);
        }
    }
    self->ext.boatElevator_child.unk7E &= 0xF;
    prim = &g_PrimBuf[self->primIndex];
    i = 0;
    while (prim != NULL) {
        if (i < 3) {
            ptrTwo = &D_us_80181530[(self->params * 6) + (i * 2)];
            xOffset = *ptrTwo++ - scrollX;
            yOffset = *ptrTwo - scrollY;
            if (self->params) {
                sin = (rsin(-self->rotate) * 0x1A) >> 0xC;
                cos = (rcos(-self->rotate) * 0x1A) >> 0xC;
            } else {
                sin = (rsin(self->rotate) * 0x1A) >> 0xC;
                cos = (rcos(self->rotate) * 0x1A) >> 0xC;
            }

            prim->x0 = xOffset - cos;
            prim->x1 = xOffset + sin;
            prim->x2 = xOffset - sin;
            prim->x3 = xOffset + cos;
            prim->y0 = yOffset - sin;
            prim->y1 = yOffset - cos;
            prim->y2 = yOffset + cos;
            prim->y3 = yOffset + sin;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        } else {
            ptrTwo = &D_us_80181548[(self->params * 3) * 10 + ((i - 3) * 3)];
            sin = *ptrTwo++;
            xOffset = *ptrTwo++ - scrollX;
            yOffset = *ptrTwo - scrollY;
            switch (sin) {
            case 0:
                prim->x0 = prim->x2 = xOffset - 4;
                prim->x1 = prim->x3 = xOffset + 4;
                yOffset += self->ext.boatElevator_child.unk7E;
                prim->y0 = prim->y1 = yOffset;
                prim->y2 = prim->y3 = yOffset + 0x60;
                break;
            case 1:
                prim->x0 = prim->x2 = xOffset - 4;
                prim->x1 = prim->x3 = xOffset + 4;
                yOffset -= self->ext.boatElevator_child.unk7E;
                prim->y0 = prim->y1 = yOffset;
                prim->y2 = prim->y3 = yOffset + 0x60;
                break;
            case 2:
                xOffset -= self->ext.boatElevator_child.unk7E;
                prim->x0 = prim->x1 = xOffset;
                prim->x2 = prim->x3 = xOffset + 0x60;
                prim->y1 = prim->y3 = yOffset - 4;
                prim->y0 = prim->y2 = yOffset + 4;
                break;
            case 3:
                xOffset += self->ext.boatElevator_child.unk7E;
                prim->x0 = prim->x1 = xOffset;
                prim->x2 = prim->x3 = xOffset + 0x60;
                prim->y1 = prim->y3 = yOffset - 4;
                prim->y0 = prim->y2 = yOffset + 4;
                break;
            }
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        }
        i++;
    }
}
#endif

void RNO4_Unused801C8BD4(void) {}

void RNO4_Unused801C8BDC(void) {}

extern u16 D_us_801815C0[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", LoadFerrymanGateTiles);
#else
// NO4's ferryman gate tiles, at RNO4's position: seven rows of two tiles
void LoadFerrymanGateTiles(void) {
    u16* tileLayoutPtr;
    Tilemap* tileMap;
    s32 i;
    s16 offset;

    tileMap = &g_Tilemap;
    offset = 0xF89;
    tileLayoutPtr = D_us_801815C0;
    for (i = 0; i < 7; i++) {
        tileMap->fg[offset] = *tileLayoutPtr++;
        offset++;
        tileMap->fg[offset] = *tileLayoutPtr++;
        offset += 0xCF;
    }
}
#endif

void func_us_801C8C54(Entity* self) {
    LoadFerrymanGateTiles();
    DestroyEntity(self);
}

extern s16 D_us_801815DC;
extern s16 D_us_80181094[];
extern s16 D_us_80181190[];
extern s16 D_us_801815E4[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801A071C_from_bo3);
#else
// BO3's func_us_801A071C with RNO4's tables: moves the water line, which
// is entry 22 of func_us_801C12B0_from_no4's lines, entry 27 of
// func_us_801C15F8_from_no4's boxes and the area at D_us_801815E4
void func_us_801A071C_from_bo3(void) {
    s16* lines = &D_us_80181094[22 * 5 + 2];
    s16* boxes = &D_us_80181190[27 * 4 + 2];
    s16* areas = D_us_801815E4;
    s16 y;

    y = 0xD8 - D_us_801815DC;
    if (y < 0x18) {
        y = 0;
    }
    areas[0] = y;
    boxes[0] = y;
    lines[0] = y;
    if (D_us_801815DC < 0xC0) {
        y = 0xF0;
    } else {
        y = 0x1A8 - D_us_801815DC;
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

extern s16 D_us_801DCF54;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801A07CC_from_bo3);
#else
// BO3's water controller as RNO4 has it: the water rises when Alucard
// walks in and, once full, sets SCYLLA_DEFEATED itself before it drains
void func_us_801A07CC_from_bo3(Entity* self) {
    D_us_801DCF54 = 0;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        if (D_us_801815DC) {
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
            if (D_us_801815DC < 0xD0) {
                D_us_801815DC++;
                D_us_801DCF54 = -1;
            } else {
                self->ext.timer.t = 0x80;
                self->step++;
            }
        }
        func_us_801A071C_from_bo3();
        break;
    case 4:
        if (!--self->ext.timer.t) {
            g_CastleFlags[SCYLLA_DEFEATED] = 1;
            self->step++;
        }
        break;
    case 5:
        if (self->ext.timer.t) {
            self->ext.timer.t--;
        } else {
            self->ext.timer.t = 1;
            if (D_us_801815DC > 0) {
                D_us_801815DC--;
                D_us_801DCF54 = 1;
            } else {
                self->step++;
            }
        }
        func_us_801A071C_from_bo3();
        break;
    }
}
#endif

extern s16 D_us_801815F8[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", func_us_801C5518_from_no4);
#else
void func_us_801C5518_from_no4(Entity* self) {
    Entity* player;
    u16 diff;
    s16* dataPtr;

    player = &PLAYER;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
    }

    dataPtr = &D_us_801815F8[self->params * 4];

    diff = player->posX.i.hi + g_Tilemap.scrollX.i.hi - *dataPtr++;
    if (diff > *dataPtr++) {
        return;
    }
    diff = player->posY.i.hi + g_Tilemap.scrollY.i.hi - *dataPtr++;
    if (diff > *dataPtr++) {
        return;
    }
    if (player->velocityY < 0) {
        player->velocityY *= 7;
        player->velocityY /= 8;
    } else if (player->velocityY > 0) {
        player->nFramesInvincibility = 1;
    }
}
#endif

extern u16 D_us_80181610[];

static void func_us_801C9048(s32 arg0) {
    s16 tilePos;
    s32 i;
    u16* tileLayoutPtr;

    Tilemap* tilemap;
    tilemap = &g_Tilemap;

    tilePos = 0x1E0;
    tileLayoutPtr = &(D_us_80181610[arg0 * 4]);

    for (i = 0; i < 4; ++i) {
        tilemap->fg[tilePos] = *(tileLayoutPtr++);
        tilePos += 0x50;
    }
}

extern s16 D_us_801815DC;

void func_us_801C909C(Entity* self) {
    s16 screen_pos;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCommon);
        self->animSet = ANIMSET_OVL(1);
        self->animCurFrame = 0x25;
        if (D_us_801815DC) {
            func_us_801C9048(1);
            self->step = 0x4;
            break;
        }
        func_us_801C9048(0);
        self->posY.i.hi -= 0x40;
        break;

    case 1:
        if (D_us_801815DC) {
            self->step += 1;
            self->velocityY = FIX(4.0);
        }

        break;

    case 2:
        self->posX.i.hi -= 8;
        GetPlayerCollisionWith(self, 0x10, 0x20, 9);
        self->posX.i.hi += 8;
        MoveEntity();
        screen_pos = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
        if (screen_pos >= 0x80) {
            g_api.PlaySfxVolPan(SFX_EXPLODE_B, 0x7F, -6);
            func_us_801C9048(1);
            self->velocityY = FIX(-1.0);
            self->step += 1;
        }
        break;

    case 3:
        self->velocityY += FIX(0.125);
        MoveEntity();
        screen_pos = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
        if (screen_pos >= 0x80) {
            self->posY.i.hi = 0x80 - g_Tilemap.scrollY.i.hi;
            self->step = 6;
        }
        break;

    case 4:
        if (!D_us_801815DC) {
            self->step += 1;
            self->velocityY = FIX(-1.0);
        }
        break;

    case 5:
        self->posX.i.hi -= 8;
        GetPlayerCollisionWith(self, 0x10, 0x20, 0xB);
        self->posX.i.hi += 8;
        MoveEntity();
        screen_pos = self->posY.i.hi + g_Tilemap.scrollY.i.hi;
        if (screen_pos < 0x41) {
            func_us_801C9048(0);
            self->step += 1;
        }
        break;

    case 6:
        break;
    }
}

extern s32 g_ExplosionVariantVelocity[];
extern u8 g_ExplosionVariantFrames[];
extern u16 g_ExplosionVariantDurations[];

void EntityExplosionVariants(Entity* self) {
    if (!self->step) {
        self->velocityY =
            g_ExplosionVariantVelocity[self->ext.destructAnim.index];
        self->flags =
            FLAG_UNK_2000 | FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_POS_CAMERA_LOCKED;
        self->palette = PAL_FLAG(PAL_UNK_195);
        self->animSet = ANIMSET_DRA(2);
        self->animCurFrame = g_ExplosionVariantFrames[self->params];
        self->blendMode = BLEND_TRANSP;
        self->step++;
        return;
    }

    self->posY.val -= self->velocityY;
    ++self->poseTimer;
    if ((self->poseTimer % 2) == 0) {
        self->animCurFrame++;
    }
    if (self->poseTimer > g_ExplosionVariantDurations[self->params]) {
        DestroyEntity(self);
    }
}

void EntityGreyPuff(Entity* self) {
    extern s16 g_GreyPuffScale[];
    extern s32 g_GreyPuffVelocity[];

    if (!self->step) {
        self->flags =
            FLAG_UNK_2000 | FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_POS_CAMERA_LOCKED;
        self->palette = PAL_FLAG(PAL_UNK_195);
        self->animSet = ANIMSET_DRA(5);
        self->animCurFrame = 1;
        self->blendMode = BLEND_TRANSP;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleX = g_GreyPuffScale[self->params];
        self->scaleY = self->scaleX;
        self->velocityY = g_GreyPuffVelocity[self->params];
        self->step++;
    } else {
        self->posY.val -= self->velocityY;
        self->poseTimer++;
        if ((self->poseTimer % 2) == 0) {
            self->animCurFrame++;
        }
        if (self->poseTimer > 36) {
            DestroyEntity(self);
        }
    }
}

void EntityIntenseExplosion(Entity* self) {
    if (!self->step) {
        InitializeEntity(g_EInitParticle);
        self->palette = PAL_FLAG(PAL_UNK_170);
        self->animSet = 5;
        self->animCurFrame = 1;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        if (self->params & 0xF0) {
            self->palette = PAL_FLAG(PAL_UNK_195);
            self->blendMode = BLEND_TRANSP;
        }
        if (self->params & 0xFF00) {
            self->zPriority = (u16)((self->params & 0xFF00) >> 8);
        }
        self->zPriority += 8;
        return;
    }

    self->poseTimer++;
    self->posY.val -= FIX(0.25);

    if (!((u16)self->poseTimer & 1)) {
        self->animCurFrame++;
    }

    if (self->poseTimer >= 0x25) {
        DestroyEntity(self);
    }
}

void PlaySfxPositional(s16 sfxId) {
    s32 posX, posY;
    s16 sfxPan;
    s16 sfxVol;

    posX = g_CurrentEntity->posX.i.hi - 128;
    sfxPan = (abs(posX) - 32) >> 5;
    if (sfxPan > 8) {
        sfxPan = 8;
    } else if (sfxPan < 0) {
        sfxPan = 0;
    }
    if (posX < 0) {
        sfxPan = -sfxPan;
    }
    sfxVol = abs(posX) - 96;
    posY = abs(g_CurrentEntity->posY.i.hi - 128) - 112;
    if (posY > 0) {
        sfxVol += posY;
    }
    if (sfxVol < 0) {
        sfxVol = 0;
    }
    sfxVol = 127 - (sfxVol >> 1);
    if (sfxVol > 0) {
        g_api.PlaySfxVolPan(sfxId, sfxVol, sfxPan);
    }
}

void EntityBreakableCrystalFloor(Entity* self) {
    extern s16 g_BreakableCrystalFloorTiles[];
    Entity* newEntity;
    s16* tileLayoutPtr;
    s32 tilePos;
    s32 i;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->hitboxWidth = 0x10;
        self->hitboxHeight = 0x10;
        self->hitboxState = 2;
        if (g_CastleFlags[RNO4_SECRET_CEILING_OPEN]) {
            i = 0x12;
        } else {
            i = 0;
        }
        tileLayoutPtr = &g_BreakableCrystalFloorTiles[i];
        tilePos = 0x2C;
        tilePos = 0x2C;
        (&g_BgLayers[0].layout[tilePos])[-1] = 0x3FE;
        (&g_BgLayers[0].layout[tilePos])[-2] = 0x3FF;
        (&g_BgLayers[0].layout[tilePos])[-17] = 0x401;
        (&g_BgLayers[0].layout[tilePos])[-18] = 0x402;
        for (i = 0; i < 2; i++, tileLayoutPtr += 3) {
            (&g_Tilemap.fg[tilePos])[0] = tileLayoutPtr[0];
            (&g_Tilemap.fg[tilePos])[-1] = tileLayoutPtr[1];
            (&g_Tilemap.fg[tilePos])[-2] = tileLayoutPtr[2];
            tilePos -= 0x10;
        }
        if (g_CastleFlags[RNO4_SECRET_CEILING_OPEN]) {
            DestroyEntity(self);
            return;
        }
        /* fallthrough */
    case 1:
        if (self->hitFlags) {
            PlaySfxPositional(SFX_WALL_DEBRIS_B);
            self->step++;
        }
        break;
    case 2:
        self->ext.breakable.breakCount++;
        tileLayoutPtr =
            &g_BreakableCrystalFloorTiles[self->ext.breakable.breakCount * 6];
        tilePos = 0x2C;
        for (i = 0; i < 2; i++, tileLayoutPtr += 3) {
            (&g_Tilemap.fg[tilePos])[0] = tileLayoutPtr[0];
            (&g_Tilemap.fg[tilePos])[-1] = tileLayoutPtr[1];
            (&g_Tilemap.fg[tilePos])[-2] = tileLayoutPtr[2];
            tilePos -= 0x10;
        }
        newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (newEntity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
            newEntity->params = 0x11;
        }
        self->ext.breakable.resetTimer = 0x20;
        self->step += 1;
        if (self->ext.breakable.breakCount == 3) {
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
                newEntity->params = 0x13;
            }
            g_CastleFlags[RNO4_SECRET_CEILING_OPEN] = 1;
            g_api.RevealSecretPassageAtPlayerPositionOnMap(0xCA);
            DestroyEntity(self);
            return;
        }
        break;
    case 3:
        if (!--self->ext.breakable.resetTimer) {
            self->step = 1;
            return;
        }
        break;
    }
}

extern s16 D_us_801816EC[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rno4/nonmatchings/unk_44B0C", EntityBreakableWall);
#else
// NO4's breakable wall as RNO4 has it: one column of tiles, rising from
// 0x9F; it makes a sound on each hit and its debris flies the other way
void EntityBreakableWall(Entity* self) {
    Entity* newEntity;
    s32 i;
    s32 tilePos;
    s16* tileLayoutPtr;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->hitboxWidth = 8;
        self->hitboxHeight = 0x20;
        self->hitboxState = 2;
        if (g_CastleFlags[RNO4_SECRET_WALL_OPEN]) {
            i = 0x18;
        } else {
            i = 0;
        }
        tileLayoutPtr = &D_us_801816EC[i];
        tilePos = 0x9F;
        for (i = 0; i < 4; tilePos -= 0x10, i++, tileLayoutPtr += 2) {
            g_Tilemap.fg[tilePos] = tileLayoutPtr[0];
        }
        if (g_CastleFlags[RNO4_SECRET_WALL_OPEN]) {
            DestroyEntity(self);
            break;
        }
        // fallthrough
    case 1:
        if (self->hitFlags) {
            PlaySfxPositional(SFX_WALL_DEBRIS_B);
            self->step++;
        }
        break;
    case 2:
        self->ext.breakable.breakCount++;
        tileLayoutPtr = &D_us_801816EC[self->ext.breakable.breakCount * 8];
        tilePos = 0x9F;
        for (i = 0; i < 4; tilePos -= 0x10, i++, tileLayoutPtr += 2) {
            g_Tilemap.fg[tilePos] = tileLayoutPtr[0];
        }
        newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (newEntity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
            newEntity->params = 0x11;
        }
        self->ext.breakable.resetTimer = 0x14;
        self->step++;
        if (self->ext.breakable.breakCount == 3) {
            for (i = 0; i < 0x10; i++) {
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(
                        E_BREAKABLE_WALL_DEBRIS, self, newEntity);
                    newEntity->posX.i.hi += 4 - (Random() & 0xF);
                    newEntity->posY.i.hi += 0x1F - (i * 4);
                    newEntity->velocityX = FIX(0.5) - (Random() << 8);
                    newEntity->params = 1;
                }
            }
            g_api.PlaySfx(SFX_WALL_DEBRIS_B);
            g_CastleFlags[RNO4_SECRET_WALL_OPEN] = 1;
            g_api.RevealSecretPassageAtPlayerPositionOnMap(
                RNO4_SECRET_WALL_OPEN);
            DestroyEntity(self);
        }
        break;
    case 3:
        if (!--self->ext.breakable.resetTimer) {
            self->step = 1;
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180D50[];
extern u8 D_us_80180D58[];
extern u8 D_us_80180D60[];
extern u8 D_us_80180D6C[];
extern u8 D_us_80180D78[];
extern u8 D_us_80180D7C[];
extern u8 D_us_80180D88[];
extern u8 D_us_80180D90[];
extern u8 D_us_80180D94[];
extern u8 D_us_80180D98[];
u8 D_us_80180D50[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180D58[] ALIGNED4 = {0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180D60[] ALIGNED4 = {0x05, 0x01, 0x05, 0x02, 0x05, 0x03, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180D6C[] ALIGNED4 = {0x05, 0x05, 0x05, 0x06, 0x05, 0x07, 0x05, 0x08, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180D78[] ALIGNED4 = {0x00, 0x00, 0x00, 0x00};
u8 D_us_80180D7C[] ALIGNED4 = {0x05, 0x0D, 0x05, 0x0E, 0x05, 0x0F, 0x05, 0x10, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180D88[] ALIGNED4 = {0x05, 0x11, 0x05, 0x12, 0x05, 0x13, 0x00, 0x00};
u8 D_us_80180D90[] ALIGNED4 = {0x05, 0x17, 0x00, 0x00};
u8 D_us_80180D94[] ALIGNED4 = {0x05, 0x16, 0x00, 0x00};
u8 D_us_80180D98[] ALIGNED4 = {0x05, 0x14, 0xFF, 0xFF, 0x05, 0x15, 0x05, 0x15, 0xFF, 0x00, 0x00, 0x00};
u16 D_us_801815C0[0xE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x374, 0x374 };
s16 D_us_801815DC = 0;
u16 D_us_80181610[8] = { 0x373, 0, 0, 0, 0x75E, 1, 1, 1 };
u16 g_ExplosionVariantDurations[4] = { 0x10, 0x18, 0x2A, 0x2F };
u8 g_ExplosionVariantFrames[4] = { 1, 9, 0x15, 0x2B };
s32 g_ExplosionVariantVelocity[6] = { 0x800, 0x2800, 0x4800, 0x7000, 0xE000, 0x12000 };
s16 g_GreyPuffScale[8] = { 0x30, 0x50, 0x80, 0xB0, 0xD0, 0x100, 0x100, 0 };
s32 g_GreyPuffVelocity[6] = { 0x400, 0x2400, 0x3C00, 0x6000, 0x7800, 0xC000 };
u16 g_FloatingIcePlatformHitbox[6] = { 0x30, 0xF, 0x28, 0xF, 0x18, 0xF };
s16 g_BreakableCrystalFloorTiles[0x18] = {
    0x54C,
    0x54D,
    0x54E,
    0x180,
    0x551,
    0x552,
    0x54C,
    0x740,
    0x748,
    0x180,
    0x551,
    0x552,
    0x54C,
    0x74D,
    0x74E,
    0x180,
    0x551,
    0x552,
    0x55E,
    0,
    0,
    0x55F,
    0,
    0,
};
u8 D_us_80181070[0x18] = {
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
    1,
    0x81,
    0x7E,
    0x46,
};
u8 D_us_80181088[0xC] = { 0xA, 0, 0xA, 1, 0xA, 2, 0xA, 1, 0, 0, 0, 0 };
bool D_us_8018104C = 0;
s16 D_us_801814E8[0x10] = {
    0x100,
    0x800,
    0x4D,
    8,
    0x100,
    0x800,
    0x7F,
    8,
    0,
    -0x800,
    0x4D,
    -8,
    0,
    -0x800,
    0x7F,
    -8,
};
s16 D_us_801815F8[0xC] = { 0x260, 0x20, 0xD0, 0x80, 0x470, 0x20, 0xD0, 0x80, 0xC0, 0x2C, 0xE0, 0x500 };
u16 D_us_80181508[0x10] = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
u8 D_us_80181528[8] = { 0xC1, 0xDF, 0xE1, 0xFF, 0xF4, 0xFC, 0x9C, 0xFC };
s16 D_us_80181530[0xC] = {
    0x410,
    0x30,
    0x410,
    0x1A0,
    0x500,
    0x30,
    0x2E0,
    0x30,
    0x2E0,
    0x1A0,
    0x1F0,
    0x30,
};
s16 D_us_80181548[0x3C] = {
    0,
    0x422,
    -0x10,
    0,
    0x422,
    0x50,
    0,
    0x422,
    0xB0,
    0,
    0x422,
    0x110,
    1,
    0x3FE,
    0,
    1,
    0x3FE,
    0x60,
    1,
    0x3FE,
    0xC0,
    1,
    0x3FE,
    0x120,
    2,
    0x438,
    0x42,
    2,
    0x498,
    0x42,
    0,
    0x2CE,
    -0x10,
    0,
    0x2CE,
    0x50,
    0,
    0x2CE,
    0xB0,
    0,
    0x2CE,
    0x110,
    1,
    0x2F2,
    0,
    1,
    0x2F2,
    0x60,
    1,
    0x2F2,
    0xC0,
    1,
    0x2F2,
    0x120,
    3,
    0x200,
    0x40,
    3,
    0x260,
    0x40,
};
s16 D_us_80181094[0x7E] = {
    0xD0,
    0xF0,
    0xB0,
    0xF0,
    -2,
    0xE0,
    0xC0,
    0x4F,
    0x10,
    0,
    0x120,
    0x60,
    0x14F,
    0x110,
    0,
    0x220,
    0xA0,
    0x14F,
    0x120,
    0,
    0x510,
    0x2B0,
    0x14F,
    0xD0,
    0,
    0x7C0,
    0x70,
    0x14F,
    0x110,
    1,
    0x870,
    0x230,
    0xDF,
    0x80,
    0,
    0xAE4,
    0x4C,
    0x14F,
    0x110,
    -1,
    0xB30,
    0x180,
    0x14F,
    0x60,
    0,
    0x3A0,
    0x50,
    0x14F,
    0x110,
    0,
    0x30,
    0x90,
    0xFF,
    0x30,
    0,
    0x500,
    0x160,
    0x14F,
    0x90,
    0,
    0x710,
    0x150,
    0x14F,
    0xD0,
    0,
    0x230,
    0xB0,
    0x14F,
    0x140,
    0,
    0x80,
    0xC10,
    0x14F,
    0xA0,
    0,
    0xD0,
    0xD0,
    0x4F,
    0x20,
    0,
    0x40,
    0x70,
    0x47,
    0x20,
    0,
    0x38,
    0x78,
    0x4F,
    0x10,
    2,
    0x40,
    0x70,
    0x4F,
    0x20,
    0,
    0x54,
    0x7C,
    0x47,
    0x20,
    0,
    0x260,
    0x148,
    0x4F,
    0x18,
    0,
    0x110,
    0xD0,
    0x4F,
    0x18,
    0,
    0x30,
    0x4D0,
    0x27,
    0x18,
    0,
    0x3E8,
    0xB0,
    0xF,
    0x10,
    0,
    0x14C,
    0x74,
    0x27,
    0x18,
    0,
    0,
};
s16 D_us_80181494[2][5] = { { 0x40, 0x68, 0x20, 0x30, 2 }, { 0x200, 0x60, 0x118, 0x20, -1 } };
u16 D_us_80181478[13] = { 0, 0xA, 4, 8, 1, 0xC, 5, 9, 2, 6, 0xB, 3, 7 };
u16 D_us_801814A8[0xA] = { 0, 4, 8, 1, 5, 2, 6, 3, 7, 0 };
s16 D_us_80181190[0x78] = {
    0xD0,
    0xF0,
    0xB1,
    0xF0,
    0xE0,
    0xC0,
    0x50,
    0x10,
    0x120,
    0x60,
    0x150,
    0x110,
    0x220,
    0xA0,
    0x150,
    0x120,
    0x510,
    0x2B0,
    0x150,
    0xD0,
    0x7C0,
    0x68,
    0x150,
    0x110,
    0x870,
    0x230,
    0xE0,
    0x80,
    0xAE8,
    0x48,
    0x150,
    0x110,
    0xB30,
    0x180,
    0x150,
    0x60,
    0x3A0,
    0x50,
    0x150,
    0x110,
    0x30,
    0x90,
    0x100,
    0x30,
    0x48E,
    0x1D2,
    0x150,
    0x90,
    0x710,
    0x150,
    0x150,
    0xD0,
    0x230,
    0xB0,
    0x150,
    0x140,
    0x1F0,
    0x1B0,
    0x140,
    0x110,
    0x1EE,
    0x1E2,
    0x110,
    0x2E,
    0xC0,
    0x12E,
    0xE0,
    0x5E,
    0x860,
    0x90,
    0x100,
    0x20,
    0x76E,
    0xF2,
    0xD0,
    0x20,
    0x80,
    0xC70,
    0x150,
    0xA0,
    0xD0,
    0xD0,
    0x50,
    0x20,
    0x38,
    0x78,
    0x48,
    0x20,
    0x40,
    0x70,
    0x50,
    0x10,
    0x40,
    0x70,
    0x50,
    0x20,
    0x54,
    0x7C,
    0x48,
    0x20,
    0x260,
    0x148,
    0x50,
    0x18,
    0x110,
    0xD0,
    0x50,
    0x18,
    0x30,
    0x4D0,
    0x28,
    0x18,
    0x3E8,
    0xB0,
    0x10,
    0x10,
    0x14C,
    0x74,
    0x28,
    0x18,
};
s16 D_us_80181428[9][3] = {
    { 0x32, 0x24, 1 },
    { 0x90, 0x21, 2 },
    { 0xB8, 0x22, 1 },
    { 0xDE, 0x23, 3 },
    { 0x132, 0x24, 1 },
    { 0x190, 0x21, 2 },
    { 0x1B8, 0x22, 1 },
    { 0x1DE, 0x23, 3 },
    { 0x7FFF, 0, 0 },
};
s16 D_us_80181460[0xC] = { 0x10, 0x32, 0x70, 0x9E, 0xDF, 0x110, 0x132, 0x170, 0x19E, 0x1DF, 0x210, 0 };
s16 D_us_801814BC[0xC] = { 0xE0, 0, 0x100, 0x14, 0x120, 8, 0x1C0, 0, 0x300, 0x12, 0x7FFF, 0 };
s16 D_us_801815E4[0xA] = { 0xD8, 0xF0, 0x68, 0xB0, 0xD8, 0xF0, 0x340, 0x74, 0xD8, 0xF0 };
s16 D_us_801DCF54;
u8 D_us_801814D4[8] = { 1, 7, 1, 8, 1, 9, 0, 0 };
u8* D_us_80180DA4[0xA] = {
    D_us_80180D50,
    D_us_80180D58,
    D_us_80180D60,
    D_us_80180D6C,
    &D_us_80180D78,
    D_us_80180D7C,
    D_us_80180D88,
    D_us_80180D90,
    D_us_80180D94,
    D_us_80180D98,
};
u8 D_us_80180DCC[0xC] = { 8, 8, 0x28, 0x18, 0x10, 0x10, 8, 8, 8, 8, 8, 0 };
u8 D_us_80180DD8[0xC] = { 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0 };
u16 D_us_80180DE4[0xA] = { 0, 0, 0x25D, 0x25D, 0x25D, 0x25D, 0x25D, 0x25D, 0x25D, 0x25D };
u16 D_us_80180DF8[0xA] = { 3, 3, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009 };
u16 D_us_80180E0C[0xA] = { 0, 0x7C, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B };
u8 D_us_80180E20[0xC] = { 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0, 0 };
s16 D_us_80180E2C[0x16] = { 0, 0, -0x18, -0x10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3, 0, 1, 2, 3, 0 };
s16 D_us_80181280[0xD4] = {
    0xBD8,
    0xBA8,
    0x1A0,
    0x15C,
    0xB98,
    0xB68,
    0x190,
    0x15C,
    0x798,
    0x768,
    0x1A0,
    0x15C,
    0x750,
    0x728,
    0x190,
    0x15C,
    0x718,
    0x6A8,
    0x1A0,
    0x15C,
    0x618,
    0x5A8,
    0x1A0,
    0x15C,
    0x598,
    0x568,
    0x1A0,
    0x15C,
    0x550,
    0x528,
    0x190,
    0x15C,
    0x410,
    0x3E8,
    0x190,
    0x15C,
    0x298,
    0x268,
    0x1A0,
    0x15C,
    0x198,
    0x168,
    0x5A0,
    0x54C,
    0x150,
    0x128,
    0x590,
    0x54C,
    0xD0,
    0x68,
    0x5A0,
    0x54C,
    0x90,
    0x28,
    0xA0,
    0x4C,
    0x390,
    0x36E,
    0xA0,
    0x70,
    0x310,
    0x2E8,
    0x90,
    0x4C,
    0x258,
    0x228,
    0xA0,
    0x4C,
    0x210,
    0x1E8,
    0x90,
    0x4C,
    0x898,
    0x868,
    0x1A0,
    0x14C,
    0x850,
    0x828,
    0x190,
    0x14C,
    0x818,
    0x7A8,
    0x1A0,
    0x14C,
    0x798,
    0x768,
    0x1A0,
    0x14C,
    0x750,
    0x728,
    0x190,
    0x14C,
    0x658,
    0x5E8,
    0x1A0,
    0x14C,
    0x5D0,
    0x5A8,
    0x190,
    0x14C,
    0x598,
    0x568,
    0x1A0,
    0x14C,
    0x550,
    0x528,
    0x190,
    0x14C,
    0x4D0,
    0x4A8,
    0x190,
    0x150,
    0x490,
    0x468,
    0x1A0,
    0x15C,
    0x410,
    0x3E8,
    0x190,
    0x14C,
    0xC90,
    0xC68,
    0x190,
    0x14C,
    0xC58,
    0xC28,
    0x1A0,
    0x14C,
    0xC10,
    0xBE8,
    0x190,
    0x14C,
    0xBD8,
    0xBA8,
    0x1A0,
    0x14C,
    0xB18,
    0xAA8,
    0x1A0,
    0x14C,
    0x998,
    0x968,
    0x1A0,
    0x14C,
    0x950,
    0x928,
    0x190,
    0x14C,
    0x850,
    0x828,
    0x190,
    0x14C,
    0x798,
    0x728,
    0x1A0,
    0x14C,
    0x718,
    0x6E8,
    0x1A0,
    0x14C,
    0x6D0,
    0x6A8,
    0x190,
    0x14C,
    0x510,
    0x4E8,
    0x190,
    0x14C,
    0x3D8,
    0x3A7,
    0x1A0,
    0x14C,
    0x38F,
    0x368,
    0x190,
    0x14C,
    0x358,
    0x2E8,
    0x1A0,
    0x14C,
    0x2D8,
    0x2A8,
    0x1A0,
    0x14C,
    0x290,
    0x268,
    0x190,
    0x14C,
    0x198,
    0x168,
    0x1A0,
    0x14C,
    0xD8,
    0xA8,
    0x1A0,
    0x14C,
    0x90,
    0x68,
    0x190,
    0x14C,
    0x198,
    0x168,
    0xA0,
    0x4C,
    0x150,
    0x128,
    0x90,
    0x4C,
    0x118,
    0xA8,
    0xA0,
    0x4C,
};
s16 D_us_801816EC[0x20] = {
    0x30E,
    0x310,
    0x766,
    0x312,
    0x76B,
    0x319,
    0x323,
    0x310,
    0x76D,
    0x310,
    0x76E,
    0x312,
    0x76F,
    0x319,
    0x770,
    0x310,
    0x771,
    0x310,
    0x772,
    0x312,
    0x773,
    0x319,
    0x774,
    0x310,
    0x30F,
    0x310,
    0x334,
    0x314,
    0x327,
    0x31B,
    0x351,
    0x352,
};
// end pc_import_data.py
// clang-format on
#endif
