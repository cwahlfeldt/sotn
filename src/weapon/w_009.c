// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #9. Used by weapons:
// Knight shield, Unknown#204
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "gen/w_009_1.h"
#include "gen/w_009_2.h"
#define g_Animset w_009_1
#define g_Animset2 w_009_2
#include "shared.h"
#include "sfx.h"

#ifdef VERSION_PC
extern s8 D_43000_8017A9F8[];
#else
extern s8 D_43000_8017A9F8;
#endif
#ifdef VERSION_PC
extern AnimationFrame* D_43000_8017AA40[];
#else
extern AnimationFrame* D_43000_8017AA40;
#endif
extern s32 D_43000_8017AA78[];

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
        g_api.PlayAnimation(&D_43000_8017A9F8, &D_43000_8017AA40);
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

static int GetWeaponId(void) { return 9; }

void EntityWeaponShieldSpell(Entity* self) {
    s16 back;
    s16 front;
    s16 top;
    s16 bottom;

    Entity* unusedEnt;
    Primitive* prim;
    Primitive* prevPrim;
    Primitive* nextPrim;
    s32 i;
    s16 selfY;
    s16 baseCoord; // used for multiple stuff. Sometimes selfX, but not always

    s16 angle;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 12);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        SetSpriteBank1(g_Animset);
        if (g_HandId != 0) {
            self->animSet = ANIMSET_OVL(0x12);
            self->palette = PAL_UNK_128;
            self->ext.shield.childPalette = 0x129;
            self->unk5A = 0x66;
            self->ext.shield.unk7C = 0;
            self->ext.shield.unk7D = 0x80;
        } else {
            self->animSet = ANIMSET_OVL(0x10);
            self->palette = PAL_UNK_110;
            self->ext.shield.childPalette = PAL_UNK_111;
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
        prim = &g_PrimBuf[self->primIndex];
        prim->clut = PAL_UNK_19F;
        prim->tpage = 0x19;

        prim->u0 = prim->u2 = 0x80;
        prim->u1 = prim->u3 = 0x80 + 0x4F;
        prim->v0 = prim->v1 = self->ext.shield.unk7C + 0;
        prim->v2 = prim->v3 = self->ext.shield.unk7C + 0x4F;

        self->ext.shield.unk82 = 0;
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3 =
                0x80;
        prim->priority = self->zPriority - 4;
        prim->drawMode =
            DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        prim = prim->next;
        prim->clut = self->ext.shield.childPalette;
        prim->tpage = 0x19;
        prim->u0 = prim->u2 = 0;
        prim->u1 = prim->u3 = 0x5F;
        prim->v0 = prim->v1 = self->ext.shield.unk7D + 0x30;
        prim->v2 = prim->v3 = self->ext.shield.unk7D + 0x7F;
        self->ext.shield.unk84 = self->ext.shield.unk86 = 0;
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3 =
                0x80;
        prim->priority = 0x1BC;
        prim->drawMode = DRAW_HIDE;
        for (i = 0; i < 10; i++) {
            prevPrim = prim;
            prim = prim->next;
            nextPrim = prim->next;
            *prim = *prevPrim;
            prim->next = nextPrim;
            prim->v0 = prim->v1 = (self->ext.shield.unk7D + 0x30) + i * 80 / 10;
            prim->v2 = prim->v3 =
                (self->ext.shield.unk7D + 0x30) + (i + 1) * 80 / 10 - 1;
            prim->clut = PAL_FILL_WHITE;
            prim->priority = 0x1BE;
            prim->drawMode |=
                (DRAW_TPAGE2 + DRAW_TPAGE + DRAW_COLORS + DRAW_TRANSP);
        }
        self->ext.shield.unk92 = self->ext.shield.unk94 = 0;
        self->ext.shield.unk8A = 0x40;
        self->ext.shield.unk90 = 0x100;
        SetSpeedX(FIX(-2));
        self->velocityY = 0;
        DestroyEntityWeapon(1);
        g_api.PlaySfx(SFX_ANIME_SWORD_C);
        g_unkGraphicsStruct.unk28 = 1;
        self->step++;
        break;
    case 1:
        DecelerateX(FIX(1.0 / 16));
        self->velocityY -= FIX(20.0 / 128);
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->scaleX += 12;
        self->scaleY = self->scaleX;

        self->ext.shield.unk82 = self->scaleX * 0x28 / 256;
        if (self->scaleX >= 0x100) {
            self->ext.shield.unk82 = 0x28;
            self->scaleY = self->scaleX = 0x100;
            self->ext.shield.unk80 = 8;
            self->step++;
        }
        break;
    case 2:
        if (--self->ext.shield.unk80 == 0) {
            self->ext.shield.unk80 = 0x10;
            self->step++;
        }
        break;
    case 3:
        prim = &g_PrimBuf[self->primIndex];
        if (!(self->ext.shield.unk80 & 1)) {
            prim->priority = 0x1BC;
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
            prim = prim->next;
            prim->drawMode &= ~DRAW_HIDE;
            prim = prim->next;
            for (i = 0; i < 10; i++) {
                prim->drawMode &= ~DRAW_HIDE;
                prim = prim->next;
            }
            self->ext.shield.unk80 = 0x20;
            g_api.PlaySfx(SFX_TRANSFORM_3X);
            self->step++;
        }
        break;
    case 4:
        self->ext.shield.unk82 += 8;
        self->scaleX += 16;
        if (self->scaleX >= 0x100) {
            self->scaleX = 0x100;
        }
        self->ext.shield.unk84 = self->scaleX * 48 / 256;
        self->ext.shield.unk86 = self->scaleX * 40 / 256;
        prim = &g_PrimBuf[self->primIndex];
        if (prim->b3 > 8) {
            prim->b3 -= 4;
        }
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3;
        if (--self->ext.shield.unk80 == 0) {
            self->step++;
        }
        break;
    case 5:
        self->ext.shield.unk90 -= 4;
        if (self->ext.shield.unk90 < 0) {
            self->ext.shield.unk90 = 0;
            self->step++;
        }
        break;
    case 6:
        self->ext.shield.unk90 += 0x10;
        if (self->ext.shield.unk90 > 0x120) {
            prim = &g_PrimBuf[self->primIndex];
            prim = prim->next;
            prim->drawMode |= DRAW_HIDE;
            // Blueprint 55 has child 39, EntityPlayerPinkEffect
            g_api.CreateEntFactoryFromEntity(self, FACTORY(55, 7), 0);
            self->step++;
        }
        break;
    case 7:
        self->ext.shield.unk90 -= 8;
        if (self->ext.shield.unk86 > 2) {
            self->ext.shield.unk86--;
        }
        self->ext.shield.unk92 += 6;
        self->ext.shield.unk8A += 4;
        if (self->ext.shield.unk90 < 4) {
            g_unkGraphicsStruct.unk28 = 0;
            DestroyEntity(self);
            return;
        }
        break;
    }
    baseCoord = self->posX.i.hi;
    selfY = self->posY.i.hi;
    prim = &g_PrimBuf[self->primIndex];
    if (prim->b3 > 8) {
        if (g_Timer & 1) {
            prim->drawMode |= DRAW_HIDE;
        } else {
            prim->drawMode &= ~DRAW_HIDE;
        }
    }
    prim->x0 = prim->x2 = baseCoord - self->ext.shield.unk82;
    prim->x1 = prim->x3 = (baseCoord + self->ext.shield.unk82) - 1;
    prim->y0 = prim->y1 = selfY - self->ext.shield.unk82;
    prim->y2 = prim->y3 = (selfY + self->ext.shield.unk82) - 1;
    prim = prim->next;
    top = prim->y0 = prim->y1 = selfY - self->ext.shield.unk86;
    bottom = prim->y2 = prim->y3 = (selfY + self->ext.shield.unk86) - 1;
    if (!self->facingLeft) {
        back = prim->x0 = prim->x2 = baseCoord - self->ext.shield.unk84;
        front = prim->x1 = prim->x3 = (baseCoord + self->ext.shield.unk84) - 1;
    } else {
        front = prim->x1 = prim->x3 = baseCoord - self->ext.shield.unk84;
        back = prim->x0 = prim->x2 = (baseCoord + self->ext.shield.unk84) - 1;
    }

    front++;
    bottom++;
    prim = prim->next;
    for (i = 0; i < 10; i++) {
        angle = D_43000_8017AA78[i];
        baseCoord = (rsin(angle) >> 5) * self->ext.shield.unk92 / 256;
        prim->x0 = back + baseCoord;
        prim->x1 = (front + baseCoord) - 1;
        angle = D_43000_8017AA78[(i + 1) % 10];
        baseCoord = (rsin(angle) >> 5) * self->ext.shield.unk92 / 256;
        prim->x2 = back + baseCoord;
        prim->x3 = (front + baseCoord) - 1;
        prim->y0 = prim->y1 = top + (((bottom - top) * i) / 10);
        prim->y2 = prim->y3 = top + (((bottom - top) * (i + 1)) / 10);
        angle = D_43000_8017AA78[(i + 6) % 10];
        baseCoord = (rsin(angle) >> 5) * self->ext.shield.unk94 / 256;
        prim->y0 += baseCoord;
        prim->y2 += baseCoord;
        angle = D_43000_8017AA78[(i + 7) % 10];
        baseCoord = (rsin(angle) >> 5) * self->ext.shield.unk94 / 256;
        prim->y1 += baseCoord;
        prim->y3 += baseCoord;
        angle = D_43000_8017AA78[i];
        prim->r0 = prim->r1 =
            ((rsin(angle) + 0x1000) >> 6) * self->ext.shield.unk90 / 256;
        angle = D_43000_8017AA78[(i + 4) % 10];
        prim->g0 = prim->g1 =
            ((rsin(angle) + 0x1000) >> 6) * self->ext.shield.unk90 / 256;
        angle = D_43000_8017AA78[(i + 8) % 10];
        prim->b0 = prim->b1 =
            ((rsin(angle) + 0x1000) >> 6) * self->ext.shield.unk90 / 256;
        angle = D_43000_8017AA78[(i + 1) % 10];
        prim->r2 = prim->r3 =
            ((rsin(angle) + 0x1000) >> 6) * self->ext.shield.unk90 / 256;
        angle = D_43000_8017AA78[(i + 5) % 10];
        prim->g2 = prim->g3 =
            ((rsin(angle) + 0x1000) >> 6) * self->ext.shield.unk90 / 256;
        angle = D_43000_8017AA78[(i + 9) % 10];
        prim->b2 = prim->b3 =
            ((rsin(angle) + 0x1000) >> 6) * self->ext.shield.unk90 / 256;
        D_43000_8017AA78[i] += self->ext.shield.unk8A;
        prim = prim->next;
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
extern u8 D_43000_8017A804[];
extern u8 D_43000_8017A844[];
extern u8 D_43000_8017A84C[];
extern u8 D_43000_8017A854[];
extern u8 D_43000_8017A880[];
extern u8 D_43000_8017A8C4[];
extern u8 D_43000_8017A8FC[];
extern u8 D_43000_8017A91C[];
extern u8 D_43000_8017A960[];
extern u8 D_43000_8017A970[];
extern u8 D_43000_8017A980[];
extern u8 D_43000_8017A990[];
extern u8 D_43000_8017A9A0[];
extern u8 D_43000_8017A9AC[];
extern u8 D_43000_8017A9B8[];
u8 D_43000_8017A804[] ALIGNED4 = {0x00, 0x00, 0x21, 0x84, 0x6A, 0xA8, 0xCD, 0xB8, 0x6F, 0xCD, 0x4F, 0xE2, 0x2F, 0x8D, 0xD6, 0x91, 0x9B, 0x92, 0x84, 0x90, 0x08, 0x9D, 0x6C, 0xA9, 0xF0, 0xB9, 0x54, 0xC6, 0xD8, 0xD2, 0x5C, 0xE3, 0x00, 0x00, 0x21, 0x84, 0x6A, 0xAC, 0x15, 0xB5, 0x22, 0x83, 0xC9, 0x8C, 0x51, 0x8D, 0xF6, 0xA9, 0xBA, 0xC6, 0x84, 0x90, 0xE7, 0x9C, 0x49, 0xA9, 0xAC, 0xB5, 0x30, 0xC6, 0xB4, 0xD6, 0x58, 0xE7};
u8 D_43000_8017A844[] ALIGNED4 = {0x02, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A84C[] ALIGNED4 = {0x02, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A854[] ALIGNED4 = {0x02, 0x00, 0x03, 0x06, 0x02, 0x00, 0x04, 0x06, 0x02, 0x00, 0x05, 0x06, 0x02, 0x00, 0x06, 0x06, 0x02, 0x00, 0x07, 0x06, 0x02, 0x00, 0x08, 0x06, 0x02, 0x00, 0x09, 0x08, 0x02, 0x00, 0x0A, 0x02, 0x02, 0x00, 0x0B, 0x02, 0x02, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A880[] ALIGNED4 = {0x03, 0x00, 0x0D, 0x02, 0x03, 0x00, 0x0E, 0x0E, 0x03, 0x00, 0x0F, 0x10, 0x03, 0x00, 0x10, 0x12, 0x03, 0x00, 0x11, 0x14, 0x03, 0x00, 0x12, 0x16, 0x03, 0x00, 0x13, 0x18, 0x03, 0x00, 0x14, 0x1A, 0x03, 0x00, 0x15, 0x1C, 0x03, 0x00, 0x16, 0x1A, 0x03, 0x00, 0x17, 0x18, 0x03, 0x00, 0x18, 0x16, 0x03, 0x00, 0x19, 0x14, 0x03, 0x00, 0x1A, 0x12, 0x03, 0x00, 0x1B, 0x10, 0x03, 0x00, 0x1C, 0x0E, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A8C4[] ALIGNED4 = {0x01, 0x00, 0x24, 0x02, 0x01, 0x00, 0x25, 0x02, 0x01, 0x00, 0x26, 0x02, 0x01, 0x00, 0x27, 0x02, 0x01, 0x00, 0x28, 0x02, 0x01, 0x00, 0x29, 0x02, 0x01, 0x00, 0x2A, 0x02, 0x01, 0x00, 0x2B, 0x02, 0x01, 0x00, 0x2C, 0x02, 0x01, 0x00, 0x2D, 0x02, 0x01, 0x00, 0x2E, 0x02, 0x01, 0x00, 0x2F, 0x02, 0x01, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A8FC[] ALIGNED4 = {0x01, 0x00, 0x2A, 0x02, 0x01, 0x00, 0x2B, 0x02, 0x01, 0x00, 0x2C, 0x02, 0x01, 0x00, 0x2D, 0x02, 0x01, 0x00, 0x2E, 0x02, 0x01, 0x00, 0x2F, 0x02, 0x01, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A91C[] ALIGNED4 = {0x02, 0x00, 0x21, 0x02, 0x02, 0x00, 0x22, 0x02, 0x02, 0x00, 0x23, 0x02, 0x02, 0x00, 0x24, 0x02, 0x02, 0x00, 0x25, 0x02, 0x02, 0x00, 0x26, 0x02, 0x02, 0x00, 0x27, 0x02, 0x02, 0x00, 0x28, 0x02, 0x02, 0x00, 0x29, 0x02, 0x02, 0x00, 0x2A, 0x02, 0x02, 0x00, 0x2B, 0x02, 0x02, 0x00, 0x2C, 0x02, 0x02, 0x00, 0x2D, 0x02, 0x02, 0x00, 0x2E, 0x02, 0x02, 0x00, 0x2F, 0x02, 0x02, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A960[] ALIGNED4 = {0x02, 0x00, 0x31, 0x02, 0x02, 0x00, 0x32, 0x02, 0x02, 0x00, 0x33, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_43000_8017A970[] ALIGNED4 = {0x02, 0x00, 0x34, 0x04, 0x02, 0x00, 0x35, 0x04, 0x02, 0x00, 0x36, 0x04, 0xFF, 0xFF, 0x00, 0x00};
u8 D_43000_8017A980[] ALIGNED4 = {0x02, 0x00, 0x33, 0x02, 0x02, 0x00, 0x32, 0x02, 0x02, 0x00, 0x31, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_43000_8017A990[] ALIGNED4 = {0x02, 0x00, 0x36, 0x04, 0x02, 0x00, 0x35, 0x04, 0x02, 0x00, 0x34, 0x04, 0xFF, 0xFF, 0x00, 0x00};
u8 D_43000_8017A9A0[] ALIGNED4 = {0x04, 0x00, 0x1D, 0x02, 0x04, 0x00, 0x1E, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A9AC[] ALIGNED4 = {0x02, 0x00, 0x1F, 0x0C, 0x02, 0x00, 0x20, 0x0C, 0x00, 0x00, 0x00, 0x00};
u8 D_43000_8017A9B8[] ALIGNED4 = {0x03, 0x00, 0x37, 0x22, 0x03, 0x00, 0x38, 0x22, 0x03, 0x00, 0x38, 0x22, 0x03, 0x00, 0x38, 0x20, 0x03, 0x00, 0x38, 0x1E, 0x03, 0x00, 0x39, 0x02, 0x03, 0x00, 0x3A, 0x02, 0x03, 0x00, 0x3B, 0x02, 0x03, 0x00, 0x3C, 0x02, 0x03, 0x00, 0x3D, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
s8 D_43000_8017A9F8[0x48] = {
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
AnimationFrame* D_43000_8017AA40[0xE] = {
    &D_43000_8017A844,
    &D_43000_8017A84C,
    D_43000_8017A9AC,
    D_43000_8017A9B8,
    D_43000_8017A9A0,
    D_43000_8017A91C,
    D_43000_8017A8FC,
    D_43000_8017A8C4,
    D_43000_8017A880,
    D_43000_8017A854,
    D_43000_8017A960,
    D_43000_8017A970,
    D_43000_8017A980,
    D_43000_8017A990,
};
s32 D_43000_8017AA78[0x10] = {
    0,
    0x200,
    0x400,
    0x600,
    0x800,
    0xA00,
    0xC00,
    0xE00,
    0x1000,
    0x1200,
    0x1400,
    0x1600,
    0x1800,
    0x1A00,
    0x1C00,
    0x1E00,
};
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[2] = { D_43000_8017A804, D_43000_8017A804 };
// end pc_import_data.py
// clang-format on
#endif
