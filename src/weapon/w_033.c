// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #33. Used by weapons:
// Nunchaku
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "shared.h"
#include "gen/w_033_1.h"
#include "gen/w_033_2.h"
#define g_Animset w_033_1
#define g_Animset2 w_033_2

extern WeaponAnimation D_EB000_8017AC3C[2];

void EntityWeaponAttack(Entity* self) {
    WeaponAnimation* anim;
    s8 animIndex;

    self->posX.val = PLAYER.posX.val;
    self->posY.val = PLAYER.posY.val;
    self->facingLeft = PLAYER.facingLeft;
    animIndex = (self->params & 0x7fff) >> 8;
    anim = &D_EB000_8017AC3C[animIndex];

    if (PLAYER.ext.player.anim >= anim->frameStart &&
        PLAYER.ext.player.anim < (anim->frameStart + 7) && g_Player.unk46) {

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

        self->ext.weapon.anim = PLAYER.ext.player.anim - anim->frameStart;
        if (PLAYER.poseTimer == 1 && PLAYER.pose == anim->soundFrame) {
            g_api.PlaySfx(anim->soundId);
        }

        if (PLAYER.pose == 12 && PLAYER.poseTimer == 1) {
            g_api.PlaySfx(anim->soundId);
            g_api.PlaySfx(SFX_WEAPON_SWISH_B);
        }
        if (g_api.UpdateUnarmedAnim(anim->frameProps, anim->frames) < 0) {
            DestroyEntity(self);
            return;
        }
    } else {
        DestroyEntity(self);
        return;
    }

    self->drawFlags = PLAYER.drawFlags;
    self->scaleY = PLAYER.scaleY;
    self->rotPivotY = PLAYER.rotPivotY;
}

s32 func_ptr_80170004(Entity* self) {}

static void func_ptr_80170008(Entity* self) {}

static void func_ptr_8017000C(Entity* self) {}

static s32 func_ptr_80170010(Entity* self) {}

static s32 func_ptr_80170014(Entity* self) {}

static int GetWeaponId(void) { return 33; }

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
extern u8 D_EB000_8017AA9C[];
u8 D_EB000_8017AA9C[] ALIGNED4 = {0x00, 0x00, 0x21, 0x04, 0xC6, 0x18, 0x6B, 0x2D, 0x10, 0x42, 0xB5, 0x56, 0x5A, 0x6B, 0xFF, 0x7F, 0x42, 0x7C, 0x84, 0x10, 0xEB, 0x10, 0x73, 0x01, 0x18, 0x12, 0xDF, 0x1E, 0xDF, 0x18, 0xA4, 0x13, 0x00, 0x00, 0x40, 0x08, 0x40, 0x10, 0x40, 0x18, 0x40, 0x20, 0x40, 0x28, 0x40, 0x30, 0x40, 0x38, 0x40, 0x40, 0x40, 0x48, 0x40, 0x50, 0x40, 0x58, 0x40, 0x60, 0x40, 0x68, 0x40, 0x70, 0x40, 0x78, 0x00, 0x00, 0x80, 0x08, 0x80, 0x10, 0x80, 0x18, 0x80, 0x20, 0x80, 0x28, 0x80, 0x30, 0x80, 0x38, 0x80, 0x40, 0x80, 0x48, 0x80, 0x50, 0x80, 0x58, 0x80, 0x60, 0x80, 0x68, 0x80, 0x70, 0x80, 0x78, 0x00, 0x00, 0xC0, 0x08, 0xC0, 0x10, 0xC0, 0x18, 0xC0, 0x20, 0xC0, 0x28, 0xC0, 0x30, 0xC0, 0x38, 0xC0, 0x40, 0xC0, 0x48, 0xC0, 0x50, 0xC0, 0x58, 0xC0, 0x60, 0xC0, 0x68, 0xC0, 0x70, 0xC0, 0x78, 0x00, 0x00, 0x00, 0x09, 0x00, 0x11, 0x00, 0x19, 0x00, 0x21, 0x00, 0x29, 0x00, 0x31, 0x00, 0x39, 0x00, 0x41, 0x00, 0x49, 0x00, 0x51, 0x00, 0x59, 0x00, 0x61, 0x00, 0x69, 0x00, 0x71, 0x00, 0x79};
WeaponAnimation D_EB000_8017AC3C[2];
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[2] = { D_EB000_8017AA9C, D_EB000_8017AA9C };
// end pc_import_data.py
// clang-format on
#endif
