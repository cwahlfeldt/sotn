// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #58. Used by weapons:
// Alucart shield, Unknown#215
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "gen/w_058_1.h"
#include "gen/w_058_2.h"
#define g_Animset w_058_1
#define g_Animset2 w_058_2
#include "shared.h"
#include "sfx.h"

#ifdef VERSION_PC
extern s8 D_19A000_8017AB3C[];
#else
extern s8 D_19A000_8017AB3C;
#endif
#ifdef VERSION_PC
extern AnimationFrame* D_19A000_8017AB84[];
#else
extern AnimationFrame* D_19A000_8017AB84;
#endif
extern s32 D_19A000_8017BB40;

static void EntityWeaponAttack(Entity* self) {
    s32 anim1;
    s32 anim2;
    s32 attackButton;

    anim1 = 0;
    anim2 = 0;
    if (g_HandId) {
        attackButton = PAD_CIRCLE;
    } else {
        attackButton = PAD_SQUARE;
    }
    if (!(attackButton & g_Player.padPressed) && (self->step < 3)) {
        self->poseTimer = 0;
        self->pose = 0;
        self->step = 3;
    }
    if (self->step != 4) {
        self->posX.val = PLAYER.posX.val;
        self->posY.val = PLAYER.posY.val;
        self->facingLeft = PLAYER.facingLeft;
    }
    if ((g_Player.status & PLAYER_STATUS_UNK10000) && (self->step != 4)) {
        self->zPriority = PLAYER.zPriority + 2;
        self->step = 4;
        if (g_Player.vram_flag & TOUCHING_GROUND) {
            self->velocityX = PLAYER.velocityX;
        } else {
            self->velocityX = PLAYER.velocityX * 2;
        }
        self->velocityY = FIX(-3.5);
        self->ext.timer.t = 0x80;
        self->flags = FLAG_POS_CAMERA_LOCKED;
        self->animCurFrame = 0x3E;
    }
    if ((PLAYER.step == Player_Crouch) && (PLAYER.step_s != 2)) {
        anim2++;
    }
    switch (self->step) {
    case 0:
        SetSpriteBank1(g_Animset);
        if (g_HandId != 0) {
            self->animSet = ANIMSET_OVL(0x12);
            self->palette = PAL_UNK_128;
            self->unk5A = 0x66;
        } else {
            self->animSet = ANIMSET_OVL(0x10);
            self->palette = PAL_UNK_110;
            self->unk5A = 0x64;
        }
        self->flags = FLAG_POS_PLAYER_LOCKED | FLAG_UNK_20000;
        self->zPriority = PLAYER.zPriority - 2;
        g_Player.unk48 = 1;
        SetWeaponProperties(self, 0);
        D_19A000_8017BB40 = self->attack;
        self->attack = 1;
        self->step++;
        /* fallthrough */
    case 1:
        self->ext.shield.anim = anim2 + 10;
        if (self->poseTimer < 0) {
            self->step++;
        }
        break;
    case 2:
        switch (PLAYER.ext.player.anim) {
        case 9:
        case 10:
        case 11:
            anim1++;
            /* fallthrough */
        case 7:
            anim1++;
            /* fallthrough */
        case 8:
            anim1++;
            /* fallthrough */
        case 12:
            anim1++;
            /* fallthrough */
        case 13:
            anim1++;
            /* fallthrough */
        case 24:
        case 25:
            anim1++;
            /* fallthrough */
        case 14:
        case 15:
            anim1++;
            /* fallthrough */
        case 26:
            anim1 += 2;
            self->pose = PLAYER.pose;
            break;
        default:
            self->pose = 0;
            anim1 += anim2;
            break;
        }
        self->ext.shield.anim = anim1;
        self->poseTimer = 2;
        break;
    case 3:
        g_Player.unk48 = 0;
        self->ext.shield.anim = anim2 + 12;
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    case 4:
        self->hitboxState = 0;
        g_Player.unk48 = 0;
        self->drawFlags |= ENTITY_ROTATE;
        self->posY.val += self->velocityY;
        self->posX.val += self->velocityX;
        self->velocityY += FIX(20.0 / 128);
        self->rotate += ROT(11.25);
        if (--self->ext.timer.t < 0x10) {
            self->drawFlags |= ENTITY_BLINK;
        }
        if (--self->ext.timer.t == 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    if (self->step != 4) {
        g_api.PlayAnimation(&D_19A000_8017AB3C, &D_19A000_8017AB84);
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

s32 GetWeaponId(void) { return 52; }

static void EntityWeaponShieldSpell(Entity* self) {
    Primitive* prim;
    s16 primRight;
    s16 primLeft;
    s16 selfX;
    s16 selfY;
    bool showQuestionMark;

    // This block sets variables that are unused
    s16 temp_s1;
    s16 temp_s5;

    temp_s1 = 0x1C;
    if (PLAYER.facingLeft) {
        temp_s1 = -temp_s1;
    }
    temp_s5 = 0x30;

    showQuestionMark = false;
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 2);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        SetSpriteBank1(g_Animset);
        if (g_HandId != 0) {
            self->animSet = ANIMSET_OVL(0x12);
            self->palette = PAL_UNK_128;
            self->ext.shield.unk86 = 0x129;
            self->unk5A = 0x66;
            self->ext.shield.unk7C = 0;
            self->ext.shield.unk7D = 0x80;
        } else {
            self->animSet = ANIMSET_OVL(0x10);
            self->palette = PAL_UNK_110;
            self->ext.shield.unk86 = 0x111;
            self->unk5A = 0x64;
            self->ext.shield.unk7C = 0x80;
            self->ext.shield.unk7D = 0;
        }

        self->posY.i.hi -= 8;
        self->flags =
            FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_HAS_PRIMS | FLAG_UNK_10000;
        self->zPriority = PLAYER.zPriority - 2;
        self->facingLeft = PLAYER.facingLeft;
        self->animCurFrame = 0x3E;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleX = self->scaleY = 0;
        // The primitive for the main shield appearing up behind Alucard
        prim = &g_PrimBuf[self->primIndex];
        prim->clut = PAL_UNK_19F;
        prim->tpage = 0x19;
        prim->u0 = prim->u2 = 0x80;
        prim->u1 = prim->u3 = 0x80 + 0x4F;
        prim->v0 = prim->v1 =
            self->ext.shield.unk7C + 0; // PSP needs +0 which is interesting
        prim->v2 = prim->v3 = self->ext.shield.unk7C + 0x4F;
        self->ext.shield.unk82 = 0;
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3 =
                0x80;
        prim->priority = self->zPriority - 4;
        prim->drawMode =
            DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        prim = prim->next;
        // The primitive for the little question mark above Alucard
        prim->clut = PAL_UNK_190;
        prim->tpage = 0x1E;
        prim->u0 = prim->u2 = 0x78;
        prim->u1 = prim->u3 = 0x78 + 7;
        prim->v0 = prim->v1 = 8;
        prim->v2 = prim->v3 = 8 + 7;
        prim->priority = self->zPriority + 4;
        prim->drawMode = DRAW_HIDE;
        SetSpeedX(FIX(-2));
        self->velocityY = 0;
        DestroyEntityWeapon(1);
        g_api.PlaySfx(SFX_ANIME_SWORD_C);
        g_unkGraphicsStruct.unk28 = 1;
        self->step++;
        break;
    case 1:
        DecelerateX(FIX(0.0625));
        self->velocityY -= FIX(0.15625);
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->scaleX += 12;
        self->scaleY = self->scaleX;
        self->ext.shield.unk82 = self->scaleX * 0x28 / 256;
        if (self->scaleX >= 0x100) {
            self->ext.shield.unk82 = 0x28;
            self->scaleY = self->scaleX = 0x100;
            self->ext.shield.unk80 = 0x40;
            self->step++;
        }
        break;
    case 2:
        if (--self->ext.shield.unk80 == 0) {
            self->ext.shield.unk80 = 0x10;
            g_api.PlaySfx(SFX_TRANSFORM);
            self->step++;
        }
        break;
    case 3:
        prim = &g_PrimBuf[self->primIndex];
        if (!(self->ext.shield.unk80 & 1)) {
            prim->priority = 0x1FC;
        } else {
            prim->priority = self->zPriority - 2;
        }
        self->scaleX -= 0x10;
        if (self->scaleX <= 0) {
            self->scaleX = 0;
        }
        self->scaleY = self->scaleX;
        if (--self->ext.shield.unk80 == 0) {
            self->animCurFrame = 0;
            self->ext.shield.unk80 = 0x20;
            self->step++;
        }
        break;
    case 4:
        self->ext.shield.unk82 += 8;
        self->scaleX += 0x10;
        if (self->scaleX >= 0x100) {
            self->scaleX = 0x100;
        }
        self->ext.shield.childPalette = self->scaleX * 36 / 256;
        self->ext.shield.unk8A = self->scaleX * 28 / 256;
        prim = &g_PrimBuf[self->primIndex];
        if (prim->b3 > 8) {
            prim->b3 -= 4;
        }
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3;
        self->ext.shield.unk86 += 4;
        self->ext.shield.unk92 += self->ext.shield.unk86;
        if (--self->ext.shield.unk80 == 0) {
            self->ext.shield.unk80 = 0x28;
            self->step++;
        }
        break;
    case 5:
        showQuestionMark = true;
        if (--self->ext.shield.unk80 == 0) {
            self->step++;
        }
        break;
    case 6:
        g_unkGraphicsStruct.unk28 = 0;
        DestroyEntity(self);
        return;
    }
    selfX = self->posX.i.hi;
    selfY = self->posY.i.hi;
    prim = &g_PrimBuf[self->primIndex];
    if (prim->b3 > 8) {
        if (g_Timer & 1) {
            prim->drawMode |= DRAW_HIDE;
        } else {
            prim->drawMode &= ~DRAW_HIDE;
        }
    }
    prim->x0 = prim->x2 = selfX - self->ext.shield.unk82;
    prim->x1 = prim->x3 = (selfX + self->ext.shield.unk82) - 1;
    prim->y0 = prim->y1 = selfY - self->ext.shield.unk82;
    prim->y2 = prim->y3 = (selfY + self->ext.shield.unk82) - 1;
    prim = prim->next;
    if (PLAYER.facingLeft) {
        prim->x0 = prim->x2 = PLAYER.posX.i.hi + 2;
        prim->x1 = prim->x3 = prim->x0 + 7;

    } else {
        prim->x0 = prim->x2 = PLAYER.posX.i.hi - 6;
        prim->x1 = prim->x3 = prim->x0 + 7;
    }

    prim->y0 = prim->y1 = PLAYER.posY.i.hi - 0x22;
    prim->y2 = prim->y3 = prim->y0 + 7;
    if (showQuestionMark) {
        prim->drawMode = DRAW_DEFAULT;
    } else {
        prim->drawMode = DRAW_HIDE;
    }
}

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
extern u8 D_19A000_8017A808[];
extern u8 D_19A000_8017A988[];
extern u8 D_19A000_8017A990[];
extern u8 D_19A000_8017A998[];
extern u8 D_19A000_8017A9C4[];
extern u8 D_19A000_8017AA08[];
extern u8 D_19A000_8017AA40[];
extern u8 D_19A000_8017AA60[];
extern u8 D_19A000_8017AAA4[];
extern u8 D_19A000_8017AAB4[];
extern u8 D_19A000_8017AAC4[];
extern u8 D_19A000_8017AAD4[];
extern u8 D_19A000_8017AAE4[];
extern u8 D_19A000_8017AAF0[];
extern u8 D_19A000_8017AAFC[];
u8 D_19A000_8017A808[] ALIGNED4 = {0x00, 0x00, 0x63, 0x90, 0x97, 0x82, 0x3F, 0xB3, 0x0F, 0x80, 0x16, 0x80, 0x3C, 0x84, 0x1F, 0xA1, 0xC6, 0x9C, 0x29, 0xAD, 0x8C, 0xB9, 0xEF, 0xC5, 0x52, 0xD2, 0xB5, 0xE2, 0x18, 0xEF, 0x9C, 0xFB, 0x00, 0x00, 0x21, 0x84, 0x44, 0x84, 0x67, 0x84, 0xAB, 0x84, 0xED, 0x88, 0x30, 0x8D, 0x73, 0x95, 0x94, 0x99, 0xB6, 0x9D, 0xF7, 0xA1, 0x19, 0xA6, 0xF4, 0xE2, 0x7C, 0xAE, 0x52, 0xCA, 0xDF, 0xB6, 0x00, 0x00, 0x21, 0x84, 0x6A, 0xA8, 0xCD, 0xB8, 0x6F, 0xCD, 0x4F, 0xE2, 0x2F, 0x8D, 0xD6, 0x91, 0x9B, 0x92, 0x84, 0x90, 0x08, 0x9D, 0x6C, 0xA9, 0xF0, 0xB9, 0x54, 0xC6, 0xD8, 0xD2, 0x5C, 0xE3, 0x00, 0x00, 0x21, 0x84, 0xA3, 0x98, 0x2A, 0xAD, 0x52, 0xCA, 0x61, 0xAC, 0x03, 0xC5, 0xA7, 0xD9, 0xAF, 0xE2, 0x84, 0x90, 0x08, 0x9D, 0x6C, 0xA9, 0xF0, 0xB9, 0x1F, 0x80, 0xD8, 0xD2, 0x5C, 0xE3, 0x00, 0x00, 0x22, 0x88, 0xE0, 0x83, 0xE5, 0xEC, 0x8A, 0xF6, 0x48, 0x80, 0x4C, 0x88, 0xB5, 0x88, 0x39, 0x8D, 0x1C, 0x82, 0xA4, 0x9C, 0x29, 0xAD, 0xAD, 0xBD, 0x31, 0xD2, 0xB5, 0xE6, 0x7A, 0xEB, 0x00, 0x00, 0x21, 0x98, 0x2F, 0x8D, 0x94, 0x95, 0x19, 0xA2, 0xFF, 0x92, 0x99, 0x90, 0xDF, 0x98, 0xE8, 0xB4, 0x84, 0x90, 0x09, 0xA1, 0x6C, 0xAD, 0x11, 0xBA, 0x54, 0xC6, 0xD8, 0xD2, 0x5C, 0xE3, 0x00, 0x00, 0x21, 0x84, 0x84, 0x90, 0xE5, 0xA8, 0x46, 0xC1, 0x53, 0xFF, 0x84, 0x90, 0x84, 0x90, 0x84, 0x90, 0x84, 0x90, 0x84, 0x90, 0x84, 0x90, 0x84, 0x90, 0x84, 0x90, 0x84, 0x90, 0x84, 0x90, 0x00, 0x00, 0x09, 0x95, 0x42, 0x88, 0xC7, 0x98, 0x08, 0xA5, 0x6B, 0xB1, 0xCD, 0xBD, 0x92, 0xD2, 0x16, 0xE3, 0x79, 0xF3, 0xC2, 0x88, 0xE4, 0x98, 0x64, 0xA1, 0xCA, 0x84, 0x91, 0x91, 0x34, 0xAA, 0x00, 0x00, 0x21, 0x84, 0x07, 0x90, 0x69, 0x98, 0xA7, 0x94, 0x0B, 0x99, 0x2F, 0x8D, 0xD6, 0x91, 0x9B, 0x92, 0x84, 0x90, 0x08, 0x9D, 0x6C, 0xA9, 0xF0, 0xB9, 0x54, 0xC6, 0xD8, 0xD2, 0x5C, 0xE3, 0x00, 0x00, 0x1C, 0xB3, 0x19, 0x82, 0xF6, 0x80, 0x0E, 0x80, 0xA0, 0x80, 0x80, 0x9D, 0xA0, 0xC5, 0xA0, 0xCE, 0x21, 0x84, 0x6A, 0x80, 0x51, 0x81, 0x99, 0x82, 0x8C, 0xB1, 0x94, 0xD2, 0x19, 0x80, 0x00, 0x00, 0x17, 0xD7, 0x72, 0xC2, 0xEE, 0xB5, 0x4B, 0x9D, 0xC8, 0x98, 0x85, 0x8C, 0x42, 0x88, 0xE2, 0x90, 0x46, 0xA5, 0xD6, 0xDA, 0xA9, 0x8C, 0x53, 0x99, 0xB6, 0x99, 0x59, 0xA6, 0x9C, 0xF3, 0x00, 0x00, 0x21, 0x84, 0xA5, 0x90, 0x2A, 0xA1, 0xAE, 0xB1, 0x33, 0xC2, 0xD8, 0xD2, 0x8B, 0x90, 0x13, 0x80, 0x1F, 0x80, 0x70, 0x9D, 0xFE, 0xA6, 0x5F, 0xAF, 0x1D, 0x92, 0x5D, 0x96, 0xBE, 0x9E};
u8 D_19A000_8017A988[] ALIGNED4 = {0x02, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017A990[] ALIGNED4 = {0x02, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017A998[] ALIGNED4 = {0x02, 0x00, 0x03, 0x06, 0x02, 0x00, 0x04, 0x06, 0x02, 0x00, 0x05, 0x06, 0x02, 0x00, 0x06, 0x06, 0x02, 0x00, 0x07, 0x06, 0x02, 0x00, 0x08, 0x06, 0x02, 0x00, 0x09, 0x08, 0x02, 0x00, 0x0A, 0x02, 0x02, 0x00, 0x0B, 0x02, 0x02, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017A9C4[] ALIGNED4 = {0x03, 0x00, 0x0D, 0x02, 0x03, 0x00, 0x0E, 0x0E, 0x03, 0x00, 0x0F, 0x10, 0x03, 0x00, 0x10, 0x12, 0x03, 0x00, 0x11, 0x14, 0x03, 0x00, 0x12, 0x16, 0x03, 0x00, 0x13, 0x18, 0x03, 0x00, 0x14, 0x1A, 0x03, 0x00, 0x15, 0x1C, 0x03, 0x00, 0x16, 0x1A, 0x03, 0x00, 0x17, 0x18, 0x03, 0x00, 0x18, 0x16, 0x03, 0x00, 0x19, 0x14, 0x03, 0x00, 0x1A, 0x12, 0x03, 0x00, 0x1B, 0x10, 0x03, 0x00, 0x1C, 0x0E, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017AA08[] ALIGNED4 = {0x01, 0x00, 0x24, 0x02, 0x01, 0x00, 0x25, 0x02, 0x01, 0x00, 0x26, 0x02, 0x01, 0x00, 0x27, 0x02, 0x01, 0x00, 0x28, 0x02, 0x01, 0x00, 0x29, 0x02, 0x01, 0x00, 0x2A, 0x02, 0x01, 0x00, 0x2B, 0x02, 0x01, 0x00, 0x2C, 0x02, 0x01, 0x00, 0x2D, 0x02, 0x01, 0x00, 0x2E, 0x02, 0x01, 0x00, 0x2F, 0x02, 0x01, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017AA40[] ALIGNED4 = {0x01, 0x00, 0x2A, 0x02, 0x01, 0x00, 0x2B, 0x02, 0x01, 0x00, 0x2C, 0x02, 0x01, 0x00, 0x2D, 0x02, 0x01, 0x00, 0x2E, 0x02, 0x01, 0x00, 0x2F, 0x02, 0x01, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017AA60[] ALIGNED4 = {0x02, 0x00, 0x21, 0x02, 0x02, 0x00, 0x22, 0x02, 0x02, 0x00, 0x23, 0x02, 0x02, 0x00, 0x24, 0x02, 0x02, 0x00, 0x25, 0x02, 0x02, 0x00, 0x26, 0x02, 0x02, 0x00, 0x27, 0x02, 0x02, 0x00, 0x28, 0x02, 0x02, 0x00, 0x29, 0x02, 0x02, 0x00, 0x2A, 0x02, 0x02, 0x00, 0x2B, 0x02, 0x02, 0x00, 0x2C, 0x02, 0x02, 0x00, 0x2D, 0x02, 0x02, 0x00, 0x2E, 0x02, 0x02, 0x00, 0x2F, 0x02, 0x02, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017AAA4[] ALIGNED4 = {0x02, 0x00, 0x31, 0x02, 0x02, 0x00, 0x32, 0x02, 0x02, 0x00, 0x33, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_19A000_8017AAB4[] ALIGNED4 = {0x02, 0x00, 0x34, 0x04, 0x02, 0x00, 0x35, 0x04, 0x02, 0x00, 0x36, 0x04, 0xFF, 0xFF, 0x00, 0x00};
u8 D_19A000_8017AAC4[] ALIGNED4 = {0x02, 0x00, 0x33, 0x02, 0x02, 0x00, 0x32, 0x02, 0x02, 0x00, 0x31, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_19A000_8017AAD4[] ALIGNED4 = {0x02, 0x00, 0x36, 0x04, 0x02, 0x00, 0x35, 0x04, 0x02, 0x00, 0x34, 0x04, 0xFF, 0xFF, 0x00, 0x00};
u8 D_19A000_8017AAE4[] ALIGNED4 = {0x04, 0x00, 0x1D, 0x02, 0x04, 0x00, 0x1E, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017AAF0[] ALIGNED4 = {0x02, 0x00, 0x1F, 0x0C, 0x02, 0x00, 0x20, 0x0C, 0x00, 0x00, 0x00, 0x00};
u8 D_19A000_8017AAFC[] ALIGNED4 = {0x03, 0x00, 0x37, 0x22, 0x03, 0x00, 0x38, 0x22, 0x03, 0x00, 0x38, 0x22, 0x03, 0x00, 0x38, 0x20, 0x03, 0x00, 0x38, 0x1E, 0x03, 0x00, 0x39, 0x02, 0x03, 0x00, 0x3A, 0x02, 0x03, 0x00, 0x3B, 0x02, 0x03, 0x00, 0x3C, 0x02, 0x03, 0x00, 0x3D, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
s8 D_19A000_8017AB3C[0x48] = {
    0,
    0,
    0,
    0,
    0xE,
    -7,
    4,
    0xC,
    0xE,
    0xD,
    4,
    0xC,
    -0xB,
    -7,
    4,
    0xC,
    1,
    -5,
    4,
    0xC,
    -0x20,
    -0x1F,
    0,
    0,
    0x16,
    -6,
    4,
    0xC,
    0xF,
    -6,
    4,
    0xC,
    0x10,
    -5,
    4,
    0xC,
    0x11,
    -4,
    4,
    0xC,
    0x12,
    -3,
    4,
    0xC,
    0x13,
    -2,
    4,
    0xC,
    0x14,
    -1,
    4,
    0xC,
    0x15,
    0,
    4,
    0xC,
    0x16,
    1,
    4,
    0xC,
    0xC,
    -5,
    4,
    0xC,
    0xA,
    -3,
    4,
    0xC,
    8,
    -1,
    4,
    0xC,
};
AnimationFrame* D_19A000_8017AB84[0x10] = {
    &D_19A000_8017A988,
    &D_19A000_8017A990,
    D_19A000_8017AAF0,
    D_19A000_8017AAFC,
    D_19A000_8017AAE4,
    D_19A000_8017AA60,
    D_19A000_8017AA40,
    D_19A000_8017AA08,
    D_19A000_8017A9C4,
    D_19A000_8017A998,
    D_19A000_8017AAA4,
    D_19A000_8017AAB4,
    D_19A000_8017AAC4,
    D_19A000_8017AAD4,
    NULL,
    NULL,
};
s32 D_19A000_8017BB40;
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[1] = { D_19A000_8017A808 };
// end pc_import_data.py
// clang-format on
#endif
