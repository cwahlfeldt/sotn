// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #47. Used by weapons:
// Dynamite
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "shared.h"
#include "gen/w_047_1.h"
#include "gen/w_047_2.h"
#define g_Animset w_047_1
#define g_Animset2 w_047_2
#include "sfx.h"

extern AnimationFrame D_14D000_8017A528[];
extern s16 D_14D000_8017A468[0x20];
extern s16 D_14D000_8017A488[0x20];
extern unsigned long D_14D000_8017A4A8[]; // image data
extern s32 D_14D000_8017B6A4;

void EntityWeaponAttack(Entity* self) {
    Collider collider;
    RECT rect;
    Primitive* prim;
    s32 unk84;
    s16 offsetX, offsetY;
    s32 i;
    s16 xVar, yVar;

    switch (self->step) {
    case 0:
        SetSpriteBank1(g_Animset);
        self->animSet = ANIMSET_OVL(0x10);
        self->unk5A = 0x65;
        self->palette = PAL_UNK_110;
        self->ext.weapon_047.unk88 = 0;
        if (g_HandId != 0) {
            self->palette += 0x18;
            self->unk5A += 2;
            self->animSet += 2;
            self->ext.weapon_047.unk88 = 0x80;

            rect.x = 0x260;
            rect.w = 4;
            rect.h = 0x10;
            rect.y = 0x180;
            LoadImage(&rect, D_14D000_8017A4A8);
        } else {
            rect.x = 0x260;
            rect.w = 4;
            rect.h = 0x10;
            rect.y = 0x100;
            LoadImage(&rect, D_14D000_8017A4A8);
        }
        self->facingLeft = PLAYER.facingLeft;
        self->zPriority = PLAYER.zPriority - 6;
        self->flags = FLAG_POS_CAMERA_LOCKED;
        if (rand() & 7) {
            self->primIndex = g_api.AllocPrimitives(PRIM_TILE, 0x20);
            if (self->primIndex != -1) {
                self->flags |= FLAG_HAS_PRIMS;
            }
        }
        if (self->flags & FLAG_HAS_PRIMS) {
            prim = &g_PrimBuf[self->primIndex];
            while (prim != NULL) {
                prim->r0 = 0x80;
                prim->g0 = 0x40;
                prim->b0 = 0;
                prim->u0 = prim->v0 = 1;
                prim->x1 = rand() & 0xF;
                prim->x2 = 0;
                prim->x3 = rand() & 0xF;
                prim->priority = self->zPriority - 4;
                prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_HIDE |
                                 DRAW_UNK02 | DRAW_TRANSP;
                prim = prim->next;
            }
        }
        self->velocityY = -FIX(2.5);
        SetSpeedX(rand() + FIX(1.5));
        self->posY.i.hi = PLAYER.posY.i.hi + PLAYER.hitboxOffY - 8;
        if (PLAYER.step != Player_Crouch) {
            self->posY.i.hi -= 6;
        }
        DestroyEntityWeapon(true);
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        self->ext.weapon_047.unk7C = 0x80;
        D_14D000_8017B6A4 = 0;
        g_Player.timers[10] = 4;
        self->step++;
        return;
    case 1:
        self->animCurFrame = 1;
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        if (self->velocityY < FIX(7)) {
            self->velocityY += FIX(0.125);
        }
        if (self->hitFlags) {
            self->velocityX = (self->velocityX * 2) / 3;
            self->hitFlags = 0;
            return;
        }

        offsetX = 4;
        if (self->velocityX < 0) {
            offsetX = -offsetX;
        }
        offsetY = 0;

        xVar = self->posX.i.hi + offsetX;
        yVar = self->posY.i.hi + offsetY;
        g_api.CheckCollision(xVar, yVar, &collider, 0);
        if (collider.effects & EFFECT_SIDE) {
            if (offsetX < 0) {
                self->posX.i.hi += collider.unkC;
            } else {
                self->posX.i.hi += collider.unk4;
            }
            self->velocityX = -(self->velocityX / 2);
        }

        offsetX = 0;
        offsetY = -4;

        xVar = self->posX.i.hi + offsetX;
        yVar = self->posY.i.hi + offsetY;
        g_api.CheckCollision(xVar, yVar, &collider, 0);
        if (collider.effects & EFFECT_SOLID) {
            self->posY.i.hi += collider.unk20 + 1;
            self->velocityY = FIX(1);
            self->velocityX = self->velocityX / 2;
        }

        offsetX = 0;
        offsetY = 4;

        xVar = self->posX.i.hi + offsetX;
        yVar = self->posY.i.hi + offsetY;
        g_api.CheckCollision(xVar, yVar, &collider, 0);
        if (collider.effects & EFFECT_SOLID) {
            if (D_14D000_8017B6A4 == 0) {
                g_api.PlaySfx(SFX_STOMP_SOFT_A);
                D_14D000_8017B6A4++;
            }
            self->posY.i.hi += collider.unk18;
            self->velocityX = 0;
            self->velocityY = 0;
        }

        if (--self->ext.weapon_047.unk7C == 0) {
            self->ext.weapon_047.unk7C = 0x28;
            if (self->flags & FLAG_HAS_PRIMS) {
                g_api.ShakeCamera(SHAKE_Y_HEAVY);
                self->step++;

                prim = &g_PrimBuf[self->primIndex];
                while (prim != NULL) {
                    prim->drawMode |= DRAW_HIDE;
                    prim = prim->next;
                }

                prim = &g_PrimBuf[self->primIndex];
                for (i = 0; prim != NULL; i++) {
                    prim->u0 = (rcos(i << 7) >> 4 << 6 >> 8) + 0xC0;
                    prim->v0 = (self->ext.weapon_047.unk88 + 0x40) -
                               ((rsin(i << 7) >> 4 << 6) >> 8);
                    if (prim->u0 < 4) {
                        prim->u0 = 0xFF;
                    }
                    prim->u1 = (rcos((i + 1) << 7) >> 4 << 6 >> 8) + 0xC0;
                    prim->v1 = (self->ext.weapon_047.unk88 + 0x40) -
                               ((rsin((i + 1) << 7) >> 4 << 6) >> 8);
                    if (prim->u1 < 4) {
                        prim->u1 = 0xFF;
                    }
                    if (self->ext.weapon_047.unk88 != 0) {
                        if (prim->v1 < 4) {
                            prim->v1 = 0xFF;
                        }
                        if (prim->v0 < 4) {
                            prim->v0 = 0xFF;
                        }
                    }

                    prim->u2 = prim->u3 = 0xC0;
                    prim->v2 = prim->v3 = self->ext.weapon_047.unk88 + 0x40;
                    prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 =
                        prim->b1 = 0;
                    prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 =
                        prim->b3 = 0xFF;
                    prim->clut = PAL_FILL_ORANGE;
                    prim->tpage = 0x19;
                    prim->priority = PLAYER.zPriority + 8;
                    prim->type = PRIM_GT4;
                    prim = prim->next;
                }

                g_api.PlaySfx(SFX_EXPLODE_A);
                self->hitboxWidth = 4;
                self->hitboxHeight = 4;
                SetWeaponProperties(self, 0);
                g_api.func_80118894(self);
                return;
            }
            self->step = 4;
            self->ext.weapon_047.unk7C = 0x18;
        } else {
            xVar = self->posX.i.hi;
            yVar = self->posY.i.hi;
            if (self->flags & FLAG_HAS_PRIMS) {
                if (!(g_GameTimer & 7)) {
                    g_api.PlaySfx(SFX_RAPID_SCRAPE_3X);
                }

                prim = &g_PrimBuf[self->primIndex];
                while (prim != NULL) {
                    if (prim->x3 == 0) {
                        if (self->facingLeft) {
                            prim->x0 =
                                xVar - 8 -
                                ((D_14D000_8017A468[prim->x1] * prim->x2) / 15);
                        } else {
                            prim->x0 =
                                xVar + 8 +
                                ((D_14D000_8017A468[prim->x1] * prim->x2) / 15);
                        }
                        prim->y0 =
                            yVar +
                            (D_14D000_8017A488[prim->x1] * prim->x2) / 15;
                        prim->x2++;
                        if (prim->x2 > 15) {
                            prim->x2 = 0;
                            prim->x1 = rand() & 0xF;
                        }
                        prim->drawMode &= ~DRAW_HIDE;
                    } else {
                        prim->x3--;
                    }
                    prim = prim->next;
                }
            }
        }
        break;
    case 2:
        if (self->hitboxWidth < 0x60) {
            self->hitboxWidth += 6;
            self->hitboxHeight += 6;
        }
        self->animCurFrame = 0;
        if (self->ext.weapon_047.unk84 < 0x8000) {
            self->ext.weapon_047.unk84 += 0x480;
        }
        xVar = self->posX.i.hi;
        yVar = self->posY.i.hi;
        unk84 = self->ext.weapon_047.unk84 >> 8;
        i = 0;
        prim = &g_PrimBuf[self->primIndex];
        if (prim->b3 > 0x58) {
            g_api.CreateEntFactoryFromEntity(
                self, ((g_HandId + 1) << 0xC) + 0x3A, 0);
        }
        if (prim->b3 < 5) {
            DestroyEntity(self);
            break;
        }
        while (prim != NULL) {
            prim->x0 = xVar + (((rcos(i << 7) >> 8) * unk84) >> 4);
            prim->y0 = yVar - (((rsin(i << 7) >> 8) * unk84) >> 4);
            prim->x1 = xVar + (((rcos((i + 1) << 7) >> 8) * unk84) >> 4);
            prim->y1 = yVar - (((rsin((i + 1) << 7) >> 8) * unk84) >> 4);
            prim->x2 = prim->x3 = xVar;
            prim->y2 = prim->y3 = yVar;
            prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                             DRAW_UNK02 | DRAW_TRANSP;

            if (prim->b3 >= 4) {
                prim->b3 -= 4;
            }
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3;
            prim = prim->next;
            i++;
        }
        break;
    case 3:
        break;
    case 4:
        self->drawFlags |= ENTITY_BLINK;
        if (--self->ext.weapon_047.unk7C == 0) {
            DestroyEntity(self);
        }
        break;
    }
}

s32 func_ptr_80170004(Entity* self) {
    s16 speed;

    if (self->step == 0) {
        if (self->ext.factory.parent->entityId == 0) {
            DestroyEntity(self);
        } else {
            SetSpriteBank2(g_Animset2);
            self->animSet = ANIMSET_OVL(0x11);
            self->unk5A = 0x64;
            self->palette = PAL_UNK_111;
            if (g_HandId != 0) {
                self->palette = 0x129;
                self->unk5A += 2;
                self->animSet += 2;
            }
            self->anim = D_14D000_8017A528;
            self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
            self->zPriority = PLAYER.zPriority + 2;
            speed = (rand() % 1535) + 0x100;
            self->velocityX = rcos((s32)speed) << 7;
            self->velocityY = -(rsin((s32)speed) << 7);
            self->posX.val += self->velocityX * 2;
            self->posY.val += self->velocityY * 2;
            self->blendMode = BLEND_TRANSP | BLEND_ADD;
            if (rand() & 1) {
                self->blendMode = BLEND_TRANSP | BLEND_QUARTER;
            }
            self->step += 1;
        }
        return;
    }
    self->posY.val += self->velocityY;
    self->posX.val += self->velocityX;
    if (self->poseTimer < 0) {
        DestroyEntity(self);
    }
}

static void func_ptr_80170008(Entity* self) {}

static void func_ptr_8017000C(Entity* self) {}

static s32 func_ptr_80170010(Entity* self) {}

static s32 func_ptr_80170014(Entity* self) {}

static int GetWeaponId(void) { return 47; }

static void EntityWeaponShieldSpell(Entity* self) {}

static void func_ptr_80170024(Entity* self) {}

static void func_ptr_80170028(Entity* self) {}

static void WeaponUnused2C(void) {}

static void WeaponUnused30(void) {}

static void WeaponUnused34(void) {}

static void WeaponUnused38(void) {}

static void WeaponUnused3C(void) {}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_14D000_8017A228[];
u8 D_14D000_8017A228[] ALIGNED4 = {0x00, 0x00, 0x25, 0x8C, 0x68, 0x9C, 0xCD, 0xA0, 0x30, 0xAD, 0x93, 0xAD, 0xF6, 0xAD, 0x99, 0xC6, 0x00, 0xE8, 0x00, 0x80, 0x07, 0xA5, 0x49, 0xB1, 0xAC, 0xBD, 0x0F, 0xCA, 0x72, 0xD6, 0xD5, 0xE2, 0x00, 0x00, 0xAB, 0x84, 0xD0, 0x84, 0x15, 0x85, 0x5A, 0x85, 0x9D, 0x89, 0xDF, 0x8D, 0x3F, 0x96, 0x9F, 0x9E, 0xFF, 0xA6, 0x5F, 0xAF, 0xBF, 0xB7, 0xFF, 0xC3, 0xFF, 0xD7, 0xFF, 0xEB, 0xFF, 0xFF, 0x00, 0x00, 0xEF, 0x8C, 0x30, 0x8D, 0x71, 0x91, 0xB3, 0x95, 0xF4, 0x99, 0x35, 0x9E, 0x77, 0xA2, 0xB8, 0xA6, 0xFA, 0xAA, 0x1B, 0xB7, 0x5C, 0xC3, 0x7C, 0xD3, 0x9D, 0xDF, 0xDE, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0xEF, 0x8C, 0x10, 0x8D, 0x51, 0x91, 0x92, 0x95, 0xB3, 0x99, 0xF4, 0x9D, 0x35, 0xA2, 0x77, 0xA6, 0x97, 0xAA, 0xB8, 0xAE, 0xD9, 0xB2, 0xF9, 0xB6, 0x1A, 0xBB, 0x3B, 0xBF, 0x5C, 0xC3, 0x00, 0x00, 0xB0, 0x88, 0xF1, 0x88, 0x33, 0x8D, 0x75, 0x91, 0xB7, 0x95, 0xF8, 0x95, 0x3A, 0x9A, 0x7C, 0x9E, 0xBE, 0xA2, 0xDE, 0xB2, 0x1E, 0xC3, 0x5E, 0xD3, 0x9E, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0xB0, 0x88, 0xD1, 0x88, 0x13, 0x8D, 0x35, 0x8D, 0x77, 0x91, 0x99, 0x91, 0xDB, 0x95, 0x1D, 0x9A, 0x9D, 0xA2, 0x1E, 0xAF, 0x7E, 0xBB, 0x9E, 0xCB, 0xBE, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0xB0, 0x88, 0xD1, 0x88, 0xF2, 0x88, 0x14, 0x8D, 0x35, 0x8D, 0x77, 0x91, 0x98, 0x91, 0xBA, 0x95, 0xDB, 0x95, 0x1D, 0x9A, 0x5D, 0xA2, 0x9D, 0xAA, 0xFD, 0xB6, 0x3D, 0xBF, 0x9E, 0xCB, 0x00, 0x00, 0xEF, 0x8C, 0x30, 0x8D, 0x71, 0x91, 0xB2, 0x95, 0xF3, 0x99, 0x35, 0x9E, 0x58, 0x9E, 0x7B, 0x9E, 0xBE, 0xA2, 0xDE, 0xAE, 0x1E, 0xBF, 0x5E, 0xCF, 0x9E, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0x30, 0x81, 0x75, 0x89, 0xBA, 0x91, 0xFF, 0x9D, 0x1F, 0xAA, 0x1F, 0xB2, 0x3F, 0xBE, 0x5F, 0xCA, 0x9F, 0xD2, 0xDF, 0xDA, 0x1F, 0xE3, 0x3F, 0xE7, 0x7F, 0xEF, 0xBF, 0xF7, 0xFF, 0xFF, 0x00, 0x00, 0x1F, 0x80, 0x9F, 0x80, 0x3F, 0x81, 0xBF, 0x81, 0x5F, 0x82, 0xDF, 0x82, 0x7F, 0x83, 0xFF, 0x83, 0xFF, 0x93, 0xFF, 0xA7, 0xFF, 0xB7, 0xFF, 0xCB, 0xFF, 0xDB, 0xFF, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0x08, 0xB9, 0x09, 0xBD, 0x2B, 0xC1, 0x4D, 0xC5, 0x4E, 0xC9, 0x70, 0xCD, 0x92, 0xD1, 0xB4, 0xD5, 0xD5, 0xD9, 0x16, 0xE2, 0x58, 0xE6, 0x99, 0xEE, 0xDB, 0xF2, 0x1C, 0xFB, 0x5E, 0xFF, 0x00, 0x00, 0x08, 0xC9, 0x29, 0xD1, 0x6A, 0xD9, 0x8C, 0xE1, 0xCD, 0xE5, 0xEF, 0xED, 0x10, 0xF2, 0x51, 0xFA, 0x93, 0xFE, 0xB4, 0xFE, 0xF5, 0xFE, 0x16, 0xFF, 0x57, 0xFF, 0x78, 0xFF, 0xBA, 0xFF, 0x00, 0x00, 0xA0, 0x91, 0xC0, 0x95, 0xE0, 0x99, 0x00, 0xA2, 0x20, 0xA6, 0x40, 0xAE, 0x60, 0xB2, 0x80, 0xBA, 0xA0, 0xBE, 0xC0, 0xC6, 0x05, 0xCF, 0x4B, 0xDB, 0x70, 0xE3, 0xB6, 0xEF, 0xFC, 0xFB, 0x00, 0x00, 0x8B, 0xB5, 0x8D, 0xB1, 0xB0, 0xB1, 0xD2, 0xB1, 0xF5, 0xB1, 0x17, 0xB2, 0x3A, 0xB2, 0x5D, 0xB2, 0x9D, 0xBA, 0xDD, 0xC6, 0x1E, 0xD3, 0x3E, 0xDF, 0x7E, 0xEB, 0xBE, 0xF7, 0xFF, 0xFF, 0x00, 0x00, 0x8B, 0xB5, 0x8D, 0xB1, 0xAF, 0xB1, 0xD2, 0xB1, 0xF3, 0xAD, 0x34, 0xAA, 0x76, 0xA6, 0xB8, 0xA6, 0xD9, 0xB2, 0x1A, 0xBF, 0x3B, 0xCB, 0x7C, 0xDB, 0x9D, 0xE7, 0xDE, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x12, 0xB1, 0x30, 0xB5, 0x4E, 0xBD, 0x8D, 0xC5, 0xAB, 0xC9, 0xEA, 0xD1, 0x08, 0xDA, 0x47, 0xE2, 0x8A, 0xDE, 0xCE, 0xDA, 0x11, 0xD7, 0x35, 0xD7, 0x78, 0xD3, 0xBC, 0xCF, 0xFF, 0xCF, 0x00, 0x00, 0x08, 0xC9, 0x26, 0xC5, 0x65, 0xC1, 0xA4, 0xC1, 0xC3, 0xBD, 0x02, 0xBE, 0x41, 0xBA, 0x80, 0xBA, 0xA4, 0xBA, 0xE9, 0xBE, 0x2D, 0xC3, 0x52, 0xC3, 0x96, 0xC7, 0xDB, 0xCB, 0xFF, 0xCF, 0x00, 0x00, 0x29, 0xAD, 0x4A, 0xB1, 0x6B, 0xB5, 0x8C, 0xB9, 0xAD, 0xBD, 0xCE, 0xC5, 0xEF, 0xC9, 0x10, 0xCE, 0x31, 0xD2, 0x52, 0xD6, 0x73, 0xDE, 0x94, 0xE2, 0xB5, 0xE6, 0xD6, 0xEA, 0x18, 0xF3};
s16 D_14D000_8017A468[0x20];
s16 D_14D000_8017A488[0x20];
unsigned long D_14D000_8017A4A8[0x20] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x0C00DEA5,
    0x30000000,
    0x09CD9937,
    0x63000000,
    0x990092,
    0x25200000,
    9,
    0x92530000,
    0,
    0x09263000,
    0,
    0x936300,
    0,
    0x92590,
    0,
    0x9900,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
AnimationFrame D_14D000_8017A528[8] = {
    { 2, 1 },
    { 2, 2 },
    { 2, 3 },
    { 2, 4 },
    { 2, 5 },
    { 2, 6 },
    { 2, 7 },
    { 0xFFFF, 0 },
};
s32 D_14D000_8017B6A4;
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[2] = { D_14D000_8017A228, D_14D000_8017A228 };
// end pc_import_data.py
// clang-format on
#endif
