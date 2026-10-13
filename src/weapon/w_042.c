// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #42. Used by weapons:
// Monster vial 3
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "shared.h"
#include "gen/w_042_1.h"
#include "gen/w_042_2.h"
#define g_Animset w_042_1
#define g_Animset2 w_042_2
#include "sfx.h"

// data
extern AnimationFrame D_12A000_8017A604[];
extern AnimationFrame D_12A000_8017A620[];
extern AnimationFrame D_12A000_8017A684[];
extern AnimationFrame D_12A000_8017A6B4[];
extern AnimationFrame D_12A000_8017A6AC[];
extern AnimationFrame D_12A000_8017A6BC[];
extern FrameProperty D_12A000_8017A6DC[];
extern s32 D_12A000_8017A6F4[];
extern AnimationFrame D_12A000_8017A704[];
extern s32 D_12A000_8017A740[];
extern s32 D_12A000_8017A74C[];

// bss
static s32 g_DebugWaitInfoTimer;
static s32 D_12A000_8017B5F0;

static void DebugShowWaitInfo(const char* msg) {
    g_CurrentBuffer = g_CurrentBuffer->next;
    FntPrint(msg);
    if (g_DebugWaitInfoTimer++ & 4) {
        FntPrint("\no\n");
    }
    DrawSync(0);
    VSync(0);
    PutDrawEnv(&g_CurrentBuffer->draw);
    PutDispEnv(&g_CurrentBuffer->disp);
    FntFlush(-1);
}

static void DebugInputWait(const char* msg) {
    while (PadRead(0))
        DebugShowWaitInfo(msg);
    while (!PadRead(0))
        DebugShowWaitInfo(msg);
}

void func_12A000_8017AC08(void) {
    RECT rect;
    u16* dst;
    s16 color;

    color = (g_GameTimer >> 1) % 2 ? 0x039C : 0x199D;
    dst = (u16*)g_Clut + (g_HandId * N_WEAPON_PAL + 0x110) * COLORS_PER_PAL;
    dst[8] = color;

    rect.x = 0;
    rect.y = 0xF1;
    rect.w = 0x100;
    rect.h = 3;
    dst = (u16*)g_Clut + 0x1100;
    LoadImage(&rect, (u_long*)dst);
}

static void EntityWeaponAttack(Entity* self) {
    Collider col;
    s32 var_a2;
    s16 xMod;

    switch (self->step) {
    case 0:
        SetSpriteBank1(g_Animset);
        self->animSet = ANIMSET_OVL(16);
        self->palette = PAL_UNK_110;
        self->unk5A = 0x64;
        if (g_HandId != 0) {
            self->palette += 0x18;
            self->unk5A += 2;
            self->animSet += 2;
        }

        self->zPriority = PLAYER.zPriority + 2;
        self->facingLeft = PLAYER.facingLeft;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
        self->anim = D_12A000_8017A6B4;
        self->posY.i.hi -= 4;

        D_12A000_8017B5F0 %= 4;
        SetSpeedX(D_12A000_8017A6F4[D_12A000_8017B5F0]);
        self->velocityY = -FIX(2.5);
        g_Player.timers[10] = 4;
        D_12A000_8017B5F0++;
        self->step++;
        break;
    case 1:
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityY += FIX(0.15625);
        g_api.CheckCollision(self->posX.i.hi, self->posY.i.hi, &col, 0);
        if (col.effects & EFFECT_SOLID) {
            self->posY.i.hi += col.unk18;
            self->anim = D_12A000_8017A684;
            self->poseTimer = 0;
            self->pose = 0;
            self->blendMode = BLEND_TRANSP | BLEND_ADD;
            g_api.PlaySfxVolPan(SFX_GLASS_BREAK_A, 0x50, 0);
            // TODO: FACTORY()
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(56, 0), 0);
            self->step++;
            return;
        }

        if (self->velocityX < 0) {
            xMod = -4;
        } else {
            xMod = 4;
        }
        g_api.CheckCollision(
            (s16)(xMod + self->posX.i.hi), self->posY.i.hi, &col, 0);
        if (col.effects & EFFECT_SIDE) {
            if (xMod < 0) {
                self->posX.i.hi += col.unkC;
            } else {
                self->posX.i.hi += col.unk4;
            }
            self->velocityX /= -2;
        }

        g_api.CheckCollision(
            self->posX.i.hi, (s16)(self->posY.i.hi - 8), &col, 0);
        if (col.effects & EFFECT_SOLID) {
            self->posY.i.hi += col.unk20 + 1;
            self->velocityY = FIX(1);
            self->velocityX /= 2;
            return;
        }
        break;
    case 2:
        if (self->poseTimer < 0) {
            DestroyEntity(self);
        }
        break;
    }
}

s32 func_ptr_80170004(Entity* self) {
    s32 unk;

    switch (self->step) {
    case 0:
        self->animSet = self->ext.weapon.parent->animSet;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->palette = self->ext.weapon.parent->palette;
        self->facingLeft = (self->facingLeft + 1) & 1;
        self->flags = FLAG_POS_CAMERA_LOCKED;
        self->zPriority = self->ext.weapon.parent->zPriority - 2;
        self->anim = D_12A000_8017A604;
        self->drawFlags |= ENTITY_SCALEY | ENTITY_SCALEX;
        self->scaleY = 0;
        self->scaleX = 0;
        self->rotPivotY = 0x14;
        self->posY.i.hi -= 0x14;
        self->step++;
        break;
    case 1:
        self->scaleX += 4;
        if (self->scaleX >= 0x100) {
            self->scaleX = 0x100;
            self->anim = D_12A000_8017A620;
            self->pose = 0;
            self->poseTimer = 0;
            self->ext.weapon.equipId =
                self->ext.weapon.parent->ext.weapon.equipId;
            SetWeaponProperties(self, 0);
            self->step++;
        }
        self->scaleY = self->scaleX;
        break;
    case 2:
        if (self->poseTimer == 1) {
            if (self->pose == 6 || self->pose == 0xC || self->pose == 0x12) {
                unk = 0x3E;
                if (g_api.CreateEntFactoryFromEntity(
                        self, WFACTORY(unk, self->ext.weapon.unk7E), 0) !=
                    NULL) {
                    self->ext.weapon.unk7E++;
                }
            }
        }
        if (self->poseTimer < 0) {
            self->anim = D_12A000_8017A6BC;
            self->pose = 0;
            self->poseTimer = 0;
            self->step++;
        }
        break;
    case 3:
        if (self->scaleX == 0x40) {
            // TODO: FACTORY()
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(70, 0), 0);
        }
        if (self->pose != 0) {
            self->scaleX -= 4;
        }
        if (self->scaleX < 0) {
            DestroyEntity(self);
            return;
        }
        self->scaleY = self->scaleX;
        break;
    }
    g_api.UpdateAnim(D_12A000_8017A6DC, NULL);
    func_12A000_8017AC08();
}

static void func_ptr_80170008(Entity* self) {
    s16 unk;
    s32 modX;

    unk = self->params & 0x7F00;
    unk >>= 8;
    switch (self->step) {
    case 0:
        if (self->ext.weapon.parent->entityId == 0) {
            DestroyEntity(self);
            return;
        }
        self->animSet = self->ext.weapon.parent->animSet;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->palette = self->ext.weapon.parent->palette;
        self->flags = FLAG_POS_CAMERA_LOCKED;
        self->drawFlags = ENTITY_ROTATE;
        self->zPriority = self->ext.weapon.parent->zPriority - 2;
        self->posY.i.hi -= 0x10;
        if (self->facingLeft == 0) {
            modX = -8;
        } else {
            modX = 8;
        }
        self->posX.i.hi = modX + self->posX.i.hi;

        SetSpeedX(D_12A000_8017A740[unk]);
        self->velocityY = D_12A000_8017A74C[unk];
        self->anim = D_12A000_8017A6AC;
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        g_api.PlaySfx(SFX_ARROW_SHOT_D);
        self->step++;
        break;
    case 1:
        self->rotate -= ROT(8.4375);
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityY += FIX(0.15625);
        if (self->hitFlags != 0) {
            self->animSet = 2;
            self->palette = PAL_FLAG(PAL_UNK_170);
            self->anim = D_12A000_8017A704;
            self->unk5A = 0;
            self->poseTimer = 0;
            self->pose = 0;
            self->drawFlags = ENTITY_DEFAULT;
            self->velocityY = -FIX(0.5);
            self->step++;
        }
        break;
    case 2:
        self->posY.val += self->velocityY;
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    g_api.UpdateAnim(D_12A000_8017A6DC, NULL);
}

static void func_ptr_8017000C(Entity* self) {
    if (self->step == 0) {
        self->animSet = self->ext.weapon.parent->animSet;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->palette = self->ext.weapon.parent->palette;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
        self->zPriority = self->ext.weapon.parent->zPriority + 2;
        self->anim = D_12A000_8017A684;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        self->posY.i.hi += 0x10;
        self->step++;
    } else if (self->poseTimer < 0) {
        DestroyEntity(self);
    }
}

static s32 func_ptr_80170010(Entity* self) {}

static s32 func_ptr_80170014(Entity* self) {}

static int GetWeaponId(void) { return 42; }

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
extern u8 D_12A000_8017A3E4[];
u8 D_12A000_8017A3E4[] ALIGNED4 = {0x00, 0x00, 0x63, 0x0C, 0x07, 0x25, 0xAD, 0x35, 0x31, 0x46, 0xB5, 0x56, 0x5A, 0x6B, 0xA6, 0x35, 0xFD, 0x01, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x07, 0x11, 0x00, 0x1B, 0x00, 0xE0, 0x03, 0x21, 0x04, 0x00, 0x00, 0xC6, 0xA4, 0xE7, 0xA8, 0x29, 0xAD, 0x6B, 0xB1, 0xAD, 0xB5, 0xCE, 0xB9, 0xEF, 0xBD, 0x10, 0xC2, 0x31, 0xC6, 0x52, 0xCA, 0x73, 0xCE, 0x94, 0xD2, 0xB5, 0xD6, 0xD6, 0xDA, 0xF6, 0xE2, 0x00, 0x00, 0x65, 0x1C, 0x65, 0x20, 0x86, 0x28, 0xA7, 0x30, 0xC8, 0x34, 0xE9, 0x3C, 0x0A, 0x45, 0x2B, 0x4D, 0x8C, 0x55, 0x0E, 0x5E, 0x70, 0x66, 0xF1, 0x6E, 0x53, 0x77, 0xD5, 0x7F, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80};
AnimationFrame D_12A000_8017A604[7] = {
    { 6, 0x201 },
    { 4, 0x202 },
    { 4, 0x203 },
    { 6, 0x204 },
    { 5, 0x205 },
    { 6, 0x206 },
    { 0, 0 },
};
AnimationFrame D_12A000_8017A620[0x19] = {
    { 5, 0x201 },
    { 5, 0x202 },
    { 5, 0x207 },
    { 6, 0x208 },
    { 5, 0x209 },
    { 5, 0x20A },
    { 5, 0x20B },
    { 0x10, 0x20C },
    { 5, 0x20D },
    { 5, 0x20E },
    { 5, 0x209 },
    { 5, 0x20A },
    { 5, 0x20B },
    { 0x10, 0x20C },
    { 5, 0x20D },
    { 5, 0x20E },
    { 5, 0x209 },
    { 5, 0x20A },
    { 5, 0x20B },
    { 0x1F, 0x20C },
    { 4, 0x204 },
    { 4, 0x205 },
    { 4, 0x206 },
    { 0x40, 0x201 },
    { 0xFFFF, 0 },
};
AnimationFrame D_12A000_8017A684[0xA] = {
    { 2, 0x17 },
    { 1, 0x18 },
    { 2, 0x19 },
    { 1, 0x1A },
    { 1, 0x1B },
    { 1, 0x1C },
    { 1, 0x1D },
    { 1, 0x19 },
    { 1, 0x18 },
    { 0xFFFF, 0 },
};
AnimationFrame D_12A000_8017A6AC[2] = { { 0x80, 0xA15 }, { 0xFFFF, 0 } };
AnimationFrame D_12A000_8017A6B4[2] = { { 0x80, 0x16 }, { 0xFFFF, 0 } };
AnimationFrame D_12A000_8017A6BC[8] = {
    { 8, 0x201 },
    { 6, 6 },
    { 5, 5 },
    { 6, 4 },
    { 4, 3 },
    { 4, 2 },
    { 6, 1 },
    { 0, 1 },
};
FrameProperty D_12A000_8017A6DC[6] = {
    { 0, 0, 0, 0 },
    { -1, 0, 4, 0x12 },
    { 0xC0, -0x28, 0, 0 },
    { 0xC0, 0xC0, 0, 0 },
    { 0, 0, 7, 7 },
    { 0, 0, 3, 3 },
};
s32 D_12A000_8017A6F4[4] = { 0xE000, 0x10000, 0xC000, 0x12000 };
AnimationFrame D_12A000_8017A704[0xF] = {
    { 2, 1 },
    { 2, 2 },
    { 2, 3 },
    { 2, 4 },
    { 2, 5 },
    { 2, 4 },
    { 2, 3 },
    { 2, 4 },
    { 2, 3 },
    { 2, 4 },
    { 2, 5 },
    { 1, 6 },
    { 1, 7 },
    { 1, 8 },
    { 0xFFFF, 0 },
};
s32 D_12A000_8017A740[3] = { -0x10000, -0x18000, -0x20000 };
s32 D_12A000_8017A74C[3] = { -0x40000, -0x48000, -0x50000 };
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[1] = { D_12A000_8017A3E4 };
// end pc_import_data.py
// clang-format on
#endif
