// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #39. Used by weapons:
// Were Bane, Unknown#190
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "shared.h"
#include "gen/w_039_1.h"
#include "gen/w_039_2.h"
#define g_Animset w_039_1
#define g_Animset2 w_039_2
#include "sfx.h"

extern WeaponAnimation D_115000_8017ADC0[2];

static void EntityWeaponAttack(Entity* self) {
    WeaponAnimation* anim;
    s8 animIndex;

    self->posX.val = PLAYER.posX.val;
    self->posY.val = PLAYER.posY.val;
    self->facingLeft = PLAYER.facingLeft;
    animIndex = (self->params & 0x7fff) >> 0x8;
    anim = &D_115000_8017ADC0[animIndex];

    if (PLAYER.ext.weapon.anim < anim->frameStart ||
        PLAYER.ext.weapon.anim >= (anim->frameStart + 7) || !g_Player.unk46) {
        DestroyEntity(self);
        return;
    }

    if (self->step == 0) {
        SetSpriteBank1(g_Animset);
        self->animSet = ANIMSET_OVL(0x10);
        self->palette = PAL_UNK_110;
        self->unk5A = 0x64;

        if (g_HandId != 0) {
            self->animSet += 2;
            self->palette += 0x18;
            self->unk5A += 2;
        }

        self->palette += anim->palette;
        self->flags = FLAG_UNK_20000 | FLAG_POS_PLAYER_LOCKED;
        self->zPriority = PLAYER.zPriority - 2;
        SetWeaponProperties(self, 0);

        self->step++;
    }

    self->ext.weapon.anim = PLAYER.ext.weapon.anim - anim->frameStart;

    if (PLAYER.poseTimer == 1 && PLAYER.pose == anim->soundFrame) {
        g_api.PlaySfx(anim->soundId);
    }

    if (g_api.UpdateUnarmedAnim(anim->frameProps, anim->frames) < 0) {
        DestroyEntity(self);
        return;
    }

    self->drawFlags = PLAYER.drawFlags;
    self->scaleY = PLAYER.scaleY;
    self->rotPivotY = PLAYER.rotPivotY;

    if (animIndex != 0 && PLAYER.pose == 4 && PLAYER.poseTimer == 1) {
        g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x38, 0), 0);
    }
}

extern s32 D_115000_8017BA28;
extern s16 D_115000_8017BA2C[];
extern s16 D_115000_8017BA6C[];

s32 func_ptr_80170004(Entity* self) {
    const int PrimCount = 32;
    Primitive* prim;
    s32 i;
    s16 x, y;
    s16 xPos, yPos;
    s16 xOffset, yOffset;
    s16 params;

    params = (self->params & 0x7fff) >> 8; // unused (for PSP)

    self->posX.val = PLAYER.posX.val;
    self->posY.val = PLAYER.posY.val;
    self->facingLeft = PLAYER.facingLeft;

    if (!g_Player.unk46 || PLAYER.step_s != 0x5A) {
        DestroyEntity(self);
        return;
    }

    if (self->step == 0) {
        self->primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, PrimCount);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }

        prim = &g_PrimBuf[self->primIndex];

        for (i = 0; i < PrimCount; i++) {
#ifdef VERSION_PSP
            prim->r0 = 0xBF;
            prim->r1 = i;
            prim->g0 = 0x3F;
            prim->g1 = i;
            prim->b0 = 0xFF;
            prim->b1 = i;
#else
            prim->r0 = 0xBF;
            prim->r1 = 0xBF;
            prim->g0 = 0x3F;
            prim->g1 = 0x3F;
            prim->b0 = 0xFF;
            prim->b1 = 0xFF;
#endif
            prim->r2 = (i * 2) + 1;
            prim->priority = PLAYER.zPriority + 4;
            prim->drawMode = DRAW_UNK_200 | DRAW_UNK_100 | DRAW_TPAGE2 |
                             DRAW_TPAGE | DRAW_HIDE | DRAW_TRANSP;
            prim = prim->next;
        }

        self->flags = FLAG_HAS_PRIMS | FLAG_POS_PLAYER_LOCKED | FLAG_UNK_20000;
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        self->enemyId = self->ext.weapon.parent->enemyId;
        D_115000_8017BA28 = 40;
        self->hitboxWidth = 24;
        self->hitboxHeight = 10;
        self->hitboxOffX = 38;
        self->hitboxOffY = -12;
        self->step++;
    } else {
        if ((D_115000_8017BA28 & 7) == 7) {
            g_api.PlaySfx(SFX_ANIME_SWORD_B);
        }

        if (--D_115000_8017BA28 == 0) {
            g_api.PlaySfx(SFX_VO_ALU_ATTACK_C);
            DestroyEntity(self);
            return;
        }
    }

    prim = &g_PrimBuf[self->primIndex];

    xPos = 34;
    yPos = -14;

    if (PLAYER.facingLeft) {
        xPos = -xPos;
    }

    x = PLAYER.posX.i.hi + xPos;
    y = PLAYER.posY.i.hi + yPos;

    if (PLAYER.drawFlags & ENTITY_SCALEY) {
        y -= 3;
    }

    for (i = 0; i < PrimCount; i++) {
        switch (prim->r3) {
        case 0:
            if (--prim->r2 == 0) {
                if (i == 0) {
                    xPos = x + (rand() % 24) - 12;
                    yPos = (y + (rand() % 18)) - 9;

                    if (!PLAYER.facingLeft) {
                        prim->x0 = xPos - 16;
                        prim->x1 = xPos + 16;
                    } else {
                        prim->x0 = xPos + 16;
                        prim->x1 = xPos - 16;
                    }
                    prim->y1 = yPos;
                    prim->y0 = yPos;
                    D_115000_8017BA2C[i] = prim->x1;
                    D_115000_8017BA6C[i] = prim->y1;
                    prim->drawMode &= ~DRAW_HIDE;
                    prim->r2 = 9;
                    prim->r3++;
                } else {
                    if (i & 1) {
                        xPos = x + (rand() % 24) - 12;
                        yPos = y + (rand() % 18) - 9;
                        if (!PLAYER.facingLeft) {
                            prim->x0 = D_115000_8017BA2C[i - 1];
                            prim->x1 = xPos - 16;
                        } else {
                            prim->x0 = D_115000_8017BA2C[i - 1];
                            prim->x1 = xPos + 16;
                        }
                        prim->y0 = D_115000_8017BA6C[i - 1];
                        prim->y1 = yPos;
                        D_115000_8017BA2C[i] = prim->x1;
                        D_115000_8017BA6C[i] = prim->y1;
                        prim->drawMode &= ~DRAW_HIDE;
                        prim->r2 = 9;
                        prim->r3++;
                    } else {
                        xPos = x + (rand() % 24) - 12;
                        yPos = y + (rand() % 18) - 9;
                        if (!PLAYER.facingLeft) {
                            prim->x0 = D_115000_8017BA2C[i - 1];
                            prim->x1 = xPos + 16;
                        } else {
                            prim->x0 = D_115000_8017BA2C[i - 1];
                            prim->x1 = xPos - 16;
                        }
                        prim->y1 = yPos;
                        prim->y0 = D_115000_8017BA6C[i - 1];

                        D_115000_8017BA2C[i] = prim->x1;
                        D_115000_8017BA6C[i] = prim->y1;
                        prim->drawMode &= ~DRAW_HIDE;
                        prim->r2 = 9;
                        prim->r3++;
                    }
                }
            }
            break;
        case 1:
            if (--prim->r2 == 0) {
                prim->r3++;
                prim->drawMode |= DRAW_HIDE;
            }
            break;
        }
        prim = prim->next;
    }
}

static void func_ptr_80170008(Entity* self) {}

static void func_ptr_8017000C(Entity* self) {}

static s32 func_ptr_80170010(Entity* self) {}

static s32 func_ptr_80170014(Entity* self) {}

static int GetWeaponId(void) { return 39; }

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
extern u8 D_115000_8017AC08[];
extern u8 D_115000_8017AD5C[];
extern void* D_115000_8017ADA4[];
extern u8 D_115000_8017ACA8[];
extern u8 D_115000_8017ACC8[];
extern u8 D_115000_8017ACE8[];
extern u8 D_115000_8017AD08[];
extern u8 D_115000_8017AD24[];
extern u8 D_115000_8017AD40[];
u8 D_115000_8017AC08[] ALIGNED4 = {0x00, 0x00, 0x21, 0x04, 0xC6, 0x18, 0x6B, 0x2D, 0x10, 0x42, 0xB5, 0x56, 0x39, 0x67, 0xFF, 0x7F, 0x73, 0x21, 0x18, 0x2E, 0x3C, 0x43, 0xE5, 0x70, 0x0A, 0x6D, 0x2F, 0x6D, 0x54, 0x69, 0x9A, 0x69, 0x00, 0x00, 0xC0, 0x08, 0xC0, 0x10, 0xC0, 0x18, 0xC0, 0x20, 0xC0, 0x28, 0xC0, 0x30, 0xC0, 0x38, 0xC0, 0x40, 0xC0, 0x48, 0xC0, 0x50, 0xC0, 0x58, 0xC0, 0x60, 0xC0, 0x68, 0xC0, 0x70, 0xC0, 0x78, 0x00, 0x00, 0xC0, 0x08, 0xC0, 0x10, 0xC0, 0x18, 0xC0, 0x20, 0xC0, 0x28, 0xC0, 0x30, 0xC0, 0x38, 0xC0, 0x40, 0xC0, 0x48, 0xC0, 0x50, 0xC0, 0x58, 0xC0, 0x60, 0xC0, 0x68, 0xC0, 0x70, 0xC0, 0x78, 0x00, 0x00, 0xC0, 0x08, 0xC0, 0x10, 0xC0, 0x18, 0xC0, 0x20, 0xC0, 0x28, 0xC0, 0x30, 0xC0, 0x38, 0xC0, 0x40, 0xC0, 0x48, 0xC0, 0x50, 0xC0, 0x58, 0xC0, 0x60, 0xC0, 0x68, 0xC0, 0x70, 0xC0, 0x78, 0x00, 0x00, 0x00, 0x09, 0x00, 0x11, 0x00, 0x19, 0x00, 0x21, 0x00, 0x29, 0x00, 0x31, 0x00, 0x39, 0x00, 0x41, 0x00, 0x49, 0x00, 0x51, 0x00, 0x59, 0x00, 0x61, 0x00, 0x69, 0x00, 0x71, 0x00, 0x79};
u8 D_115000_8017AD5C[] ALIGNED4 = {0x00, 0x00, 0x00, 0x00, 0xE0, 0xE1, 0x00, 0x00, 0x03, 0xE8, 0x05, 0x04, 0x10, 0xF2, 0x20, 0x03, 0x1A, 0xF2, 0x17, 0x02, 0xFB, 0xFB, 0x05, 0x04, 0x09, 0x03, 0x21, 0x03, 0x14, 0x03, 0x16, 0x02, 0x05, 0x09, 0x0D, 0x0A, 0x10, 0x13, 0x12, 0x08, 0xFD, 0xEB, 0x06, 0x07, 0x0E, 0xF0, 0x20, 0x03, 0x19, 0xF0, 0x16, 0x03, 0xF7, 0xE9, 0x05, 0x08, 0x10, 0xF1, 0x1B, 0x04, 0x17, 0xF2, 0x14, 0x03, 0x12, 0x01, 0x17, 0x0A, 0x18, 0x04, 0x12, 0x0A};
void* D_115000_8017ADA4[] = {D_115000_8017ACA8, D_115000_8017ACA8, D_115000_8017ACC8, D_115000_8017ACE8, D_115000_8017AD08, D_115000_8017AD24, D_115000_8017AD40};
u8 D_115000_8017ACA8[] ALIGNED4 = {0x01, 0x02, 0x02, 0x04, 0x03, 0x04, 0x04, 0x04, 0x05, 0x06, 0x06, 0x08, 0x07, 0x08, 0x08, 0x08, 0x09, 0x08, 0x0A, 0x02, 0x0B, 0x02, 0x0C, 0x02, 0x0D, 0x02, 0x0E, 0x02, 0x0F, 0x02, 0xFF, 0xFF};
u8 D_115000_8017ACC8[] ALIGNED4 = {0x10, 0x02, 0x11, 0x0A, 0x12, 0x0A, 0x13, 0x0A, 0x14, 0x0C, 0x15, 0x0E, 0x16, 0x0E, 0x17, 0x0E, 0x18, 0x0E, 0x19, 0x02, 0x1A, 0x02, 0x25, 0x02, 0x26, 0x02, 0x27, 0x02, 0x28, 0x02, 0xFF, 0xFF};
u8 D_115000_8017ACE8[] ALIGNED4 = {0x10, 0x02, 0x1B, 0x0A, 0x1C, 0x0A, 0x1D, 0x0A, 0x1E, 0x10, 0x1F, 0x12, 0x20, 0x12, 0x21, 0x12, 0x22, 0x12, 0x23, 0x02, 0x24, 0x02, 0x25, 0x02, 0x26, 0x02, 0x27, 0x02, 0x28, 0x02, 0xFF, 0xFF};
u8 D_115000_8017AD08[] ALIGNED4 = {0x29, 0x02, 0x2A, 0x14, 0x2B, 0x14, 0x2C, 0x14, 0x2D, 0x16, 0x2E, 0x18, 0x2F, 0x18, 0x30, 0x18, 0x31, 0x18, 0x32, 0x02, 0x33, 0x02, 0x34, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_115000_8017AD24[] ALIGNED4 = {0x35, 0x02, 0x36, 0x1A, 0x37, 0x1A, 0x38, 0x1A, 0x39, 0x1C, 0x3A, 0x1E, 0x3B, 0x1E, 0x3C, 0x1E, 0x3D, 0x1E, 0x3E, 0x02, 0x3F, 0x02, 0x47, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_115000_8017AD40[] ALIGNED4 = {0x35, 0x02, 0x36, 0x1A, 0x37, 0x1A, 0x38, 0x1A, 0x40, 0x20, 0x41, 0x22, 0x42, 0x22, 0x43, 0x22, 0x44, 0x22, 0x45, 0x02, 0x46, 0x02, 0x47, 0x02, 0xFF, 0xFF, 0x00, 0x00};
WeaponAnimation D_115000_8017ADC0[2] = {
    { D_115000_8017ADA4, D_115000_8017AD5C, 0, 0x60B, 0xAE, 4, 0 },
    { D_115000_8017ADA4, D_115000_8017AD5C, 0, 0x60B, 0xAE, 4, 0 },
};
s32 D_115000_8017BA28;
s16 D_115000_8017BA2C[0x1f];
s16 D_115000_8017BA6C[0xaca];
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[2] = { D_115000_8017AC08, D_115000_8017AC08 };
// end pc_import_data.py
// clang-format on
#endif
