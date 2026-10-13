// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #20. Used by weapons:
// Karma Coin
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "shared.h"
#include "gen/w_020_1.h"
#include "gen/w_020_2.h"
#define g_Animset w_020_1
#define g_Animset2 w_020_2
#include "sfx.h"

extern AnimationFrame D_90000_8017A850[];
extern AnimationFrame D_90000_8017A864[];
extern AnimationFrame D_90000_8017A8C8[];
extern AnimationFrame D_90000_8017A8D8[];
extern AnimationFrame D_90000_8017A8E8[];
extern AnimationFrame D_90000_8017A954[];
extern AnimationFrame D_90000_8017A9D8[];
#ifdef VERSION_PC
extern FrameProperty D_90000_8017A9E0[];
#else
extern FrameProperty D_90000_8017A9E0;
#endif
extern u16 D_90000_8017AA10[];
#ifdef VERSION_PC
extern u8 D_90000_8017AA1C[];
#else
extern u8 D_90000_8017AA1C;
#endif
extern s32 D_90000_8017AADC[];
extern s32 D_90000_8017AB44[];
extern s32 D_90000_8017C238;

static void EntityWeaponAttack(Entity* self) {
    Collider sp10;
    Entity* child;
    Primitive* prim;

    u16 var_s4;
    s16 angle_s3;
    s32 var_s2;
    u8* var_s1;

    s16 xVar;
    s16 yVar;

    s16 xOffset;
    s16 yOffset;
    s16 sp6a;
    s16 sp68;
    s16 sp66;
    s16 sp64;
    s16 sp62;
    u16 sp60;
    s16 var_s8;
    s16 var_s7;

    sp60 = D_90000_8017AA10[g_GameTimer % 3] % 0x8000;
    var_s4 = 0;
    if (g_HandId != 0) {
        sp60 += 0x18;
        var_s4 += 0x80;
    }
    switch (self->step) {
    default:
        return;
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_G4, 0x1A);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[self->primIndex];
        prim->drawMode = DRAW_HIDE;
        SetSpriteBank1(g_Animset);
        self->animSet = ANIMSET_OVL(0x10);
        self->palette = PAL_UNK_110;
        self->unk5A = 0x64;
        if (g_HandId != 0) {
            self->animSet += 2;
            self->palette += 0x18;
            self->unk5A += 2;
        }
        self->zPriority = PLAYER.zPriority - 2;
        self->facingLeft = PLAYER.facingLeft;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS | FLAG_UNK_100000;
        self->anim = D_90000_8017A850;
        SetSpeedX((rand() & 0x3FFF) + 0xE000);
        self->velocityY = FIX(-4.0);
        self->facingLeft = 0;
        DestroyEntityWeapon(1);
        self->step += 1;
        return;
    case 1:
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityY += FIX(0.15625);
        xOffset = 0;
        yOffset = 1;
        xVar = self->posX.i.hi + xOffset;
        yVar = self->posY.i.hi + yOffset;
        g_api.CheckCollision(xVar, yVar, &sp10, 0);
        if (sp10.effects & EFFECT_SOLID) {
            self->posY.i.hi += sp10.unk18;
            self->poseTimer = self->pose = 0;
            self->anim = D_90000_8017A864;
            g_api.PlaySfx(SFX_GOLD_PICKUP);
            self->step += 1;
            return;
        }
        xOffset = 4;
        if (self->velocityX < 0) {
            xOffset = -xOffset;
        }
        yOffset = 0;
        xVar = self->posX.i.hi + xOffset;
        yVar = self->posY.i.hi + yOffset;
        g_api.CheckCollision(xVar, yVar, &sp10, 0);
        if (sp10.effects & EFFECT_SIDE) {
            if (xOffset < 0) {
                self->posX.i.hi += sp10.unkC;
            } else {
                self->posX.i.hi += sp10.unk4;
            }
            self->velocityX = -(self->velocityX / 2);
        }
        xOffset = 0;
        yOffset = -8;
        xVar = self->posX.i.hi + xOffset;
        yVar = self->posY.i.hi + yOffset;
        g_api.CheckCollision(xVar, yVar, &sp10, 0);
        if (sp10.effects & EFFECT_SOLID) {
            self->posY.i.hi += sp10.unk20 + 1;
            self->velocityY = FIX(1.0);
            self->velocityX = self->velocityX / 2;
        }
        return;
    case 2:
        if (((self->pose % 8) == 5) && (self->poseTimer == 1)) {
            g_api.PlaySfxVolPan(SFX_GOLD_PICKUP,
                                D_90000_8017AB44[self->ext.karmacoin.unk84], 0);
            self->ext.karmacoin.unk84++;
        }
        if (self->poseTimer < 0) {
            self->poseTimer = self->pose = 0;
            // This appears to be the actual Karma Coin coin-flip, deciding
            // heads/tails
            if (rand() & 1) {
                // Heads
                self->anim = D_90000_8017A8C8;
            } else {
                // Tails
                self->anim = D_90000_8017A8D8;
            }
            self->step += 1;
        }
        return;
    case 3:
        if ((self->pose == 1) && (self->poseTimer == 0x38)) {
            // Useless if-statement
            if (self->anim == D_90000_8017A8C8) {
                g_api.PlaySfx(SFX_KARMA_COIN_JINGLE);
            } else {
                g_api.PlaySfx(SFX_KARMA_COIN_JINGLE);
            }
        }
        if (self->poseTimer < 0) {
            // Tails: A small head pops up, and lightning strikes the coin
            if (self->anim == D_90000_8017A8D8) {
                g_Player.timers[10] = 4;
                self->anim = D_90000_8017A8E8;
                self->poseTimer = self->pose = 0;
                self->zPriority = 0x1B6;
                self->flags &= ~FLAG_UNK_100000;
                g_api.func_80118C28(7);
                g_api.PlaySfx(SFX_THUNDER_B);
                SetWeaponProperties(self, 0);
                self->step = 4;
            } else {
                // Heads: A giant angelic figure appears, with a soul-steal
                // effect. Not clear what part of the function triggers
                // soul-steal.
                g_Player.timers[12] = 4;
                self->drawFlags = ENTITY_BLINK;
                self->blendMode = BLEND_TRANSP | BLEND_ADD;
                self->posY.i.hi = 0;
                self->posX.i.hi = 0x80;
                self->zPriority = 0x1BA;
                self->animCurFrame = 0x1D;
                self->flags &= ~(FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000);
                self->ext.karmacoin.timer = 0xE0;
                self->opacity = 0x80;
                g_api.func_80118C28(8);
                g_api.PlaySfx(SFX_TRANSFORM_3X);
                prim = &g_PrimBuf[self->primIndex];
                prim->r0 = prim->g0 = prim->b0 = 0x5F;
                prim->r1 = prim->g1 = prim->b1 = 0x5F;
                prim->r2 = prim->g2 = prim->b2 = 0x1F;
                prim->r3 = prim->g3 = prim->b3 = 0x1F;
                prim->priority = 0x1BC;
                prim = prim->next;

                prim->x0 = prim->x2 = 0x54;
                prim->x1 = prim->x3 = 0xAC;
                prim->y0 = prim->y1 = 0x30;
                prim->y2 = prim->y3 = 0x98;
                prim->v0 = prim->v1 = var_s4;
                prim->v2 = prim->v3 = var_s4 + 0x67;
                prim->u0 = prim->u2 = 0x80;
                prim->u1 = prim->u3 = 0x80 + 0x57;
                prim->type = PRIM_GT4;
                prim->priority = 0x1B4;

                prim->drawMode = DRAW_HIDE;
                prim->tpage = 0x19;
                prim->clut = sp60;
                prim = prim->next;
                var_s1 = &D_90000_8017AA1C;
                for (var_s2 = 0; var_s2 < 24; var_s2++) {
                    prim->x0 = *var_s1++;
                    prim->u0 = prim->x0 + 0x80;
                    prim->x0 += 0x54;
                    prim->y0 = *var_s1++;
                    prim->v0 = var_s4 + prim->y0;
                    prim->y0 += 0x30;
                    prim->x2 = *var_s1++;
                    prim->u2 = prim->x2 + 0x80;
                    prim->x2 += 0x54;
                    prim->y2 = *var_s1++;
                    prim->v2 = var_s4 + prim->y2;
                    prim->y2 += 0x30;
                    prim->x1 = *var_s1++;
                    prim->u1 = prim->x1 + 0x80;
                    prim->x1 += 0x54;
                    prim->y1 = *var_s1++;
                    prim->v1 = var_s4 + prim->y1;
                    prim->y1 += 0x30;
                    prim->x3 = *var_s1++;
                    prim->u3 = prim->x3 + 0x80;
                    prim->x3 += 0x54;
                    prim->y3 = *var_s1++;
                    prim->v3 = var_s4 + prim->y3;
                    prim->y3 += 0x30;
                    prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 =
                        prim->b1 = prim->r2 = prim->g2 = prim->b2 = prim->r3 =
                            prim->g3 = prim->b3 = 0x80;
                    prim->type = PRIM_GT4;
                    prim->priority = 0x1B6;
                    prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_HIDE |
                                     DRAW_COLORS | DRAW_TRANSP;
                    prim->tpage = 0x19;
                    prim->clut = sp60;
                    prim = prim->next;
                }
                self->ext.karmacoin.unk8A = 0x40;
                self->ext.karmacoin.unk90 = 0;
                self->ext.karmacoin.unk92 = self->ext.karmacoin.unk94 = 0x100;
                self->ext.karmacoin.unk7E = 0x20;
                self->attack = 4;
                self->attackElement = ELEMENT_HIT;
                self->hitboxState = 0x82;
                self->nFramesInvincibility = 0x50;
                self->stunFrames = 4;
                self->hitEffect = 2;
                self->entityRoomIndex = 0;
                g_api.func_80118894(self);
                self->step = 5;
            }
        }
        return;
    case 4:
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        g_api.UpdateAnim(&D_90000_8017A9E0, NULL);
        return;
    case 5:
        if (!(g_GameTimer & 3)) {
            self->ext.karmacoin.unk7E++;
        }
        if (self->ext.karmacoin.unk7E >= 0x61) {
            self->ext.karmacoin.unk7E = 0x60;
        }
        self->ext.karmacoin.unk90 += 8;
        if (self->ext.karmacoin.unk90 > 0x120) {
            self->ext.karmacoin.unk90 = 0x120;
        }
        self->ext.karmacoin.unk92 -= 4;
        if (self->ext.karmacoin.unk92 < 0x28) {
            self->ext.karmacoin.unk92 = 0x28;
            child =
                g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x54, 0), 0);
            self->step++;
        }
        self->ext.karmacoin.unk94 = self->ext.karmacoin.unk92;
        break;

    case 6:
        if (!(g_GameTimer & 3)) {
            self->ext.karmacoin.unk7E++;
        }
        if (self->ext.karmacoin.unk7E >= 0x61) {
            self->ext.karmacoin.unk7E = 0x60;
        }
        --self->ext.karmacoin.unk92;
        if (self->ext.karmacoin.unk92 < 0) {
            prim = &g_PrimBuf[self->primIndex];
            prim = prim->next;
            prim->drawMode &= ~DRAW_HIDE;
            self->ext.karmacoin.unk92 = 0;
            self->step++;
        }
        self->ext.karmacoin.unk94 = self->ext.karmacoin.unk92;
        break;
    case 7:
        self->ext.karmacoin.unk90 -= 0x10;
        if (self->ext.karmacoin.unk90 < 0) {
            self->ext.karmacoin.unk90 = 0;
        }
        if (--self->ext.karmacoin.timer < 0) {
            self->step += 1;
        }
        if (!(g_GameTimer & 3)) {
            self->ext.karmacoin.unk7E++;
        }
        if (self->ext.karmacoin.unk7E >= 0x61) {
            self->ext.karmacoin.unk7E = 0x60;
        }
        prim = &g_PrimBuf[self->primIndex];
        if (!(self->ext.karmacoin.timer & 1) && (prim->b3 > 1)) {
            prim->b3--;
        }
        prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3;
        break;
    case 8:
        self->drawFlags |= ENTITY_OPACITY;
        self->opacity--;
        if (self->opacity < 4) {
            self->opacity = 4;
        }
        prim = &g_PrimBuf[self->primIndex];
        if (prim->b1 > 8) {
            prim->b1 -= 2;
        }
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1;
        self->ext.karmacoin.unk90 += 0x10;
        if (self->ext.karmacoin.unk90 > 0x120) {
            prim = &g_PrimBuf[self->primIndex];
            prim = prim->next;
            prim->drawMode |= DRAW_HIDE;
            g_api.PlaySfx(SFX_TELEPORT_BANG_B);
            self->step += 1;
        }
        break;
    case 9:
        self->drawFlags |= ENTITY_OPACITY;
        self->opacity -= 2;
        if (self->opacity < 4) {
            self->opacity = 4;
        }
        prim = &g_PrimBuf[self->primIndex];
        if (prim->b1 >= 9) {
            prim->b1 -= 2;
        }
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1;
        self->ext.karmacoin.unk90 -= 8;
        if (self->ext.karmacoin.unk90 < 0) {
            self->ext.karmacoin.unk90 = 0;
        }
        self->ext.karmacoin.unk92 += 12;
        if (self->ext.karmacoin.unk92 > 0x200) {
            DestroyEntity(self);
            return;
        }
        self->ext.karmacoin.unk94 = self->ext.karmacoin.unk92;
        break;
    }
    prim = &g_PrimBuf[self->primIndex];
    prim->x0 = 0x74;
    prim->x1 = 0x8C;
    prim->x2 = 0x80 - self->ext.karmacoin.unk7E;
    prim->x3 = self->ext.karmacoin.unk7E + 0x80;
    prim->y0 = prim->y1 = 0;
    prim->y2 = prim->y3 = 0xF0;

    prim->drawMode =
        DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
    prim = prim->next;
    prim->clut = sp60;
    prim = prim->next;
    var_s1 = &D_90000_8017AA1C;
    for (var_s2 = 0; var_s2 < 24; var_s2++) {
        prim->drawMode &= ~DRAW_HIDE;
        if (var_s2 == 0) {
            angle_s3 = D_90000_8017AADC[var_s2];
            yVar = -(rsin(angle_s3) >> 5) * self->ext.karmacoin.unk94 / 256;
            xVar = (rcos(angle_s3) >> 5) * self->ext.karmacoin.unk92 / 256;
            prim->x0 = *var_s1++;
            prim->x0 += xVar + 0x54;
            prim->y0 = *var_s1++;
            prim->y0 += yVar + 0x30;
            prim->x2 = *var_s1++;
            prim->x2 += xVar + 0x54;
            prim->y2 = *var_s1++;
            prim->y2 += yVar + 0x30;
        } else {
            prim->x0 = sp68;
            prim->y0 = sp66;
            prim->x2 = sp64;
            prim->y2 = sp62;
            var_s1 += 4;
        }
        var_s7 = D_90000_8017AADC[(var_s2 + 1) % 24];
        var_s8 = -(rsin(var_s7) >> 5) * self->ext.karmacoin.unk94 / 256;
        sp6a = (rcos(var_s7) >> 5) * self->ext.karmacoin.unk92 / 256;
        prim->x1 = *var_s1++;
        sp68 = prim->x1 += sp6a + 0x54;
        prim->y1 = *var_s1++;
        sp66 = prim->y1 += var_s8 + 0x30;
        prim->x3 = *var_s1++;
        sp64 = prim->x3 += sp6a + 0x54;
        prim->y3 = *var_s1++;
        sp62 = prim->y3 += var_s8 + 0x30;
        angle_s3 = D_90000_8017AADC[var_s2];
        prim->b0 = prim->b2 = prim->g0 = prim->g2 = prim->r0 = prim->r2 =
            ((rsin(angle_s3) + 0x1000) >> 6) * self->ext.karmacoin.unk90 / 256;
        angle_s3 = D_90000_8017AADC[(var_s2 + 1) % 24];
        prim->b1 = prim->b3 = prim->g1 = prim->g3 = prim->r1 = prim->r3 =
            ((rsin(angle_s3) + 0x1000) >> 6) * self->ext.karmacoin.unk90 / 256;
        D_90000_8017AADC[var_s2] += self->ext.karmacoin.unk8A;
        prim = prim->next;
    }
}

s32 func_ptr_80170004(Entity* self) {}

static void func_ptr_80170008(Entity* self) {
    if (self->ext.weapon.parent->entityId == 0) {
        DestroyEntity(self);
        return;
    }
    if (self->step == 0) {
        self->animSet = self->ext.weapon.parent->animSet;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
        self->zPriority = PLAYER.zPriority;

        if (D_90000_8017C238 % 4 == 1) {
            self->velocityX = FIX(8.0 / 128);
            self->velocityY = FIX(34.0 / 128);
            self->anim = D_90000_8017A9D8;
            self->posX.i.hi = 140;
            self->posY.i.hi = 100;
            self->zPriority = PLAYER.zPriority - 4;
        } else {
            self->zPriority = PLAYER.zPriority;
            self->anim = D_90000_8017A954;
            self->pose = rand() & 0x1F;
            self->posX.i.hi = (D_90000_8017C238 % 3) * 10 + 148;
            self->posY.i.hi = (D_90000_8017C238 % 3) * 10 + 76;
            self->velocityX = FIX(19.0 / 256); // strange, FIX is usually /128
            self->velocityY = (rand() & 0x1FFF) + FIX(0.25);
        }
        D_90000_8017C238++;
        self->step++;
    }
    self->palette = D_90000_8017AA10[g_GameTimer % 3];
    if (g_HandId != 0) {
        self->palette += 0x18;
    }
    self->posX.val += self->velocityX;
    self->posY.val += self->velocityY;
}

static void func_ptr_8017000C(Entity* self) {}

static s32 func_ptr_80170010(Entity* self) {}

static s32 func_ptr_80170014(Entity* self) {}

static int GetWeaponId(void) { return 20; }

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
extern u8 D_90000_8017A630[];
u8 D_90000_8017A630[] ALIGNED4 = {0x00, 0x00, 0xE1, 0x86, 0x2A, 0x9D, 0x71, 0x99, 0xF5, 0xA5, 0x77, 0xB2, 0xF8, 0xC2, 0x59, 0xDB, 0x38, 0xB2, 0xC4, 0x90, 0x28, 0xA1, 0xAD, 0xB1, 0x50, 0xCA, 0x6F, 0xE9, 0xD5, 0xDA, 0x7A, 0xE7, 0x00, 0x00, 0x20, 0x80, 0x21, 0x84, 0x41, 0x84, 0x42, 0x88, 0x63, 0x8C, 0x84, 0x90, 0xC6, 0x98, 0x29, 0xA5, 0x8C, 0xB1, 0xEF, 0xBD, 0x52, 0xCA, 0xD6, 0xDA, 0x39, 0xE7, 0x9C, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0xA5, 0x9C, 0xE7, 0xA4, 0x29, 0xAD, 0x6B, 0xB5, 0xAD, 0xBD, 0xCF, 0xC1, 0x11, 0xCA, 0x53, 0xD2, 0x95, 0xDA, 0xD7, 0xE2, 0x00, 0xFC, 0x38, 0xA9, 0xD8, 0xBD, 0x63, 0x8C, 0x84, 0x98, 0x00, 0x00, 0x00, 0x84, 0x20, 0x88, 0x21, 0x8C, 0x41, 0x90, 0x42, 0x94, 0x63, 0x98, 0x84, 0x9C, 0x60, 0xD4, 0xC0, 0xEC, 0x80, 0xF1, 0x04, 0xF6, 0x85, 0xFA, 0x4B, 0xFF, 0xD5, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x26, 0x84, 0x0E, 0x91, 0x73, 0x99, 0xF9, 0x9D, 0x7E, 0xAA, 0xDE, 0xBE, 0x3D, 0xD7, 0x21, 0x84, 0x83, 0x90, 0x28, 0xA5, 0xAC, 0xB5, 0x51, 0xCA, 0xD5, 0xDA, 0x7A, 0xEF, 0xFE, 0xFF, 0x00, 0x00, 0xA5, 0x9C, 0xE7, 0xA4, 0x29, 0xAD, 0x6B, 0xB5, 0xAD, 0xBD, 0xCF, 0xC1, 0x11, 0xCA, 0x53, 0xD2, 0x95, 0xDA, 0xD7, 0xE2, 0x00, 0xFC, 0x35, 0x82, 0xBB, 0x82, 0x63, 0x8C, 0x84, 0x98, 0x00, 0x00, 0x84, 0xCC, 0xA5, 0xB4, 0xE7, 0xBC, 0x29, 0xC5, 0x4B, 0xCD, 0x8D, 0xD5, 0xF0, 0xD1, 0x74, 0xCE, 0xD7, 0xCA, 0x5B, 0xCB, 0x00, 0xFC, 0x7F, 0x8C, 0x7F, 0x8C, 0x63, 0xFC, 0x63, 0xE4, 0x00, 0x00, 0xE5, 0x90, 0x88, 0x8C, 0xA8, 0x90, 0xC8, 0x98, 0xE8, 0x9C, 0x08, 0xA5, 0x2A, 0xA5, 0xE9, 0x98, 0x21, 0x84, 0x42, 0x88, 0x63, 0x8C, 0xA5, 0x94, 0xC6, 0x98, 0xE7, 0x9C, 0x29, 0xA5, 0x00, 0x00, 0xA9, 0xA2, 0xE7, 0xA0, 0x50, 0xB1, 0xB3, 0xC5, 0x36, 0xCA, 0xB9, 0xCE, 0x3B, 0xD3, 0x18, 0xC2, 0x83, 0x90, 0x28, 0xA5, 0xAC, 0xAD, 0x51, 0xBA, 0x6B, 0xF1, 0xD5, 0xDA, 0x7A, 0xEF, 0x00, 0x00, 0x02, 0xC8, 0x04, 0xC8, 0x06, 0xC8, 0x08, 0xC8, 0x0A, 0xC8, 0x0C, 0xC8, 0x0E, 0xC8, 0x10, 0xC8, 0x12, 0xC8, 0x14, 0xC8, 0x16, 0xC8, 0x18, 0xC8, 0x1A, 0xC8, 0x1C, 0xC8, 0x1E, 0xC8, 0x00, 0x00, 0x02, 0xD0, 0x04, 0xD0, 0x06, 0xD0, 0x08, 0xD0, 0x0A, 0xD0, 0x0C, 0xD0, 0x0E, 0xD0, 0x10, 0xD0, 0x12, 0xD0, 0x14, 0xD0, 0x16, 0xD0, 0x18, 0xD0, 0x1A, 0xD0, 0x1C, 0xD0, 0x1E, 0xD0, 0x00, 0x00, 0x02, 0xD8, 0x04, 0xD8, 0x06, 0xD8, 0x08, 0xD8, 0x0A, 0xD8, 0x0C, 0xD8, 0x0E, 0xD8, 0x10, 0xD8, 0x12, 0xD8, 0x14, 0xD8, 0x16, 0xD8, 0x18, 0xD8, 0x1A, 0xD8, 0x1C, 0xD8, 0x1E, 0xD8, 0x00, 0x00, 0x02, 0xE0, 0x04, 0xE0, 0x06, 0xE0, 0x08, 0xE0, 0x0A, 0xE0, 0x0C, 0xE0, 0x0E, 0xE0, 0x10, 0xE0, 0x12, 0xE0, 0x14, 0xE0, 0x16, 0xE0, 0x18, 0xE0, 0x1A, 0xE0, 0x1C, 0xE0, 0x1E, 0xE0, 0x00, 0x00, 0x02, 0xE8, 0x04, 0xE8, 0x06, 0xE8, 0x08, 0xE8, 0x0A, 0xE8, 0x0C, 0xE8, 0x0E, 0xE8, 0x10, 0xE8, 0x12, 0xE8, 0x14, 0xE8, 0x16, 0xE8, 0x18, 0xE8, 0x1A, 0xE8, 0x1C, 0xE8, 0x1E, 0xE8, 0x00, 0x00, 0x02, 0xF0, 0x04, 0xF0, 0x06, 0xF0, 0x08, 0xF0, 0x0A, 0xF0, 0x0C, 0xF0, 0x0E, 0xF0, 0x10, 0xF0, 0x12, 0xF0, 0x14, 0xF0, 0x16, 0xF0, 0x18, 0xF0, 0x1A, 0xF0, 0x1C, 0xF0, 0x1E, 0xF0, 0x00, 0x00, 0x02, 0xF8, 0x04, 0xF8, 0x06, 0xF8, 0x08, 0xF8, 0x0A, 0xF8, 0x0C, 0xF8, 0x0E, 0xF8, 0x10, 0xF8, 0x12, 0xF8, 0x14, 0xF8, 0x16, 0xF8, 0x18, 0xF8, 0x1A, 0xF8, 0x1C, 0xF8, 0x1E, 0xF8, 0x00, 0x00, 0x42, 0x00, 0x44, 0x00, 0x46, 0x00, 0x48, 0x00, 0x4A, 0x00, 0x4C, 0x00, 0x4E, 0x00, 0x50, 0x00, 0x52, 0x00, 0x54, 0x00, 0x56, 0x00, 0x58, 0x00, 0x5A, 0x00, 0x5C, 0x00, 0x5E, 0x00};
AnimationFrame D_90000_8017A850[5] = { { 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 0, 0 } };
AnimationFrame D_90000_8017A864[0x19] = {
    { 4, 5 },
    { 4, 6 },
    { 4, 7 },
    { 4, 8 },
    { 3, 5 },
    { 3, 6 },
    { 3, 7 },
    { 3, 8 },
    { 2, 5 },
    { 2, 6 },
    { 2, 7 },
    { 2, 8 },
    { 3, 9 },
    { 3, 0xA },
    { 3, 0xB },
    { 2, 9 },
    { 2, 0xA },
    { 2, 0xB },
    { 2, 9 },
    { 2, 0xA },
    { 2, 0xB },
    { 1, 9 },
    { 1, 0xA },
    { 1, 0xB },
    { 0xFFFF, 0 },
};
AnimationFrame D_90000_8017A8C8[4] = { { 0x20, 0xA }, { 0x40, 0xC }, { 0x18, 0xA }, { 0xFFFF, 0 } };
AnimationFrame D_90000_8017A8D8[4] = { { 0x20, 0xA }, { 0x40, 0xD }, { 0x18, 0xA }, { 0xFFFF, 0 } };
AnimationFrame D_90000_8017A8E8[0x1B] = {
    { 1, 0x1019 },
    { 1, 0x101A },
    { 1, 0x101B },
    { 1, 0x40E },
    { 1, 0x60F },
    { 1, 0x810 },
    { 1, 0xA11 },
    { 1, 0xC12 },
    { 1, 0xC13 },
    { 1, 0xE14 },
    { 1, 0xE15 },
    { 1, 0x616 },
    { 0xFFFF, 0 },
    { 1, 0x1E },
    { 1, 0x1F },
    { 1, 0x20 },
    { 0, 0 },
    { 5, 0x17 },
    { 4, 0x18 },
    { 3, 0x19 },
    { 3, 0x1A },
    { 3, 0x1B },
    { 3, 0x1C },
    { 3, 0x19 },
    { 5, 0x18 },
    { 7, 0x17 },
    { 0, 0 },
};
AnimationFrame D_90000_8017A954[0x21] = {
    { 2, 0x22 },
    { 2, 0x23 },
    { 2, 0x24 },
    { 2, 0x25 },
    { 2, 0x26 },
    { 2, 0x27 },
    { 2, 0x28 },
    { 2, 0x29 },
    { 3, 0x22 },
    { 3, 0x23 },
    { 3, 0x24 },
    { 3, 0x25 },
    { 3, 0x26 },
    { 3, 0x27 },
    { 3, 0x28 },
    { 3, 0x29 },
    { 4, 0x22 },
    { 4, 0x23 },
    { 4, 0x24 },
    { 4, 0x25 },
    { 4, 0x26 },
    { 5, 0x27 },
    { 5, 0x28 },
    { 5, 0x29 },
    { 5, 0x22 },
    { 6, 0x23 },
    { 6, 0x24 },
    { 6, 0x25 },
    { 7, 0x26 },
    { 8, 0x27 },
    { 9, 0x28 },
    { 0x19, 0x29 },
    { 0, 0 },
};
AnimationFrame D_90000_8017A9D8[2] = { { 0x80, 0x21 }, { 0, 0 } };
FrameProperty D_90000_8017A9E0[0xC] = {
    { 0, 0, 0, 0 },
    { -0xC, -0xC, 0, 0 },
    { 0, -0xD, 0x10, 0x10 },
    { 0xBC, 0x33, 0, 0 },
    { -1, -0x21, 8, 0x24 },
    { 0, -0x21, 0xC, 0x24 },
    { -1, 0x9B, 7, 0x66 },
    { 1, 0x54, 7, 0x1E },
    { -0x13, -0x13, 0, 0 },
    { 0xC0, -3, 0, 0 },
    { -0x2D, 0xB4, 0, 0 },
    { -0xF, -0xF, 0, 0 },
};
u16 D_90000_8017AA10[6] = { 0x8110, 0x8117, 0x8118, 0x8110, 3, 0 };
u8 D_90000_8017AA1C[0xC0] = {
    0,
    0,
    0,
    0,
    8,
    0,
    0,
    8,
    8,
    0,
    0,
    8,
    0x10,
    0,
    0,
    0x10,
    0x10,
    0,
    0,
    0x10,
    0x18,
    0,
    0,
    0x18,
    0x18,
    0,
    0,
    0x18,
    0x20,
    0,
    0,
    0x20,
    0x20,
    0,
    0,
    0x20,
    0x28,
    0,
    0,
    0x28,
    0x28,
    0,
    0,
    0x28,
    0x30,
    0,
    0,
    0x30,
    0x30,
    0,
    0,
    0x30,
    0x38,
    0,
    0,
    0x38,
    0x38,
    0,
    0,
    0x38,
    0x40,
    0,
    0,
    0x40,
    0x40,
    0,
    0,
    0x40,
    0x48,
    0,
    0,
    0x48,
    0x48,
    0,
    0,
    0x48,
    0x50,
    0,
    0,
    0x50,
    0x50,
    0,
    0,
    0x50,
    0x58,
    0,
    0,
    0x58,
    0x58,
    0,
    0,
    0x58,
    0x58,
    8,
    0,
    0x60,
    0x58,
    8,
    0,
    0x60,
    0x58,
    0x10,
    0,
    0x68,
    0x58,
    0x10,
    0,
    0x68,
    0x58,
    0x18,
    8,
    0x68,
    0x58,
    0x18,
    8,
    0x68,
    0x58,
    0x20,
    0x10,
    0x68,
    0x58,
    0x20,
    0x10,
    0x68,
    0x58,
    0x28,
    0x18,
    0x68,
    0x58,
    0x28,
    0x18,
    0x68,
    0x58,
    0x30,
    0x20,
    0x68,
    0x58,
    0x30,
    0x20,
    0x68,
    0x58,
    0x38,
    0x28,
    0x68,
    0x58,
    0x38,
    0x28,
    0x68,
    0x58,
    0x40,
    0x30,
    0x68,
    0x58,
    0x40,
    0x30,
    0x68,
    0x58,
    0x48,
    0x38,
    0x68,
    0x58,
    0x48,
    0x38,
    0x68,
    0x58,
    0x50,
    0x40,
    0x68,
    0x58,
    0x50,
    0x40,
    0x68,
    0x58,
    0x58,
    0x48,
    0x68,
    0x58,
    0x58,
    0x48,
    0x68,
    0x58,
    0x60,
    0x50,
    0x68,
    0x58,
    0x60,
    0x50,
    0x68,
    0x58,
    0x68,
    0x58,
    0x68,
};
s32 D_90000_8017AADC[0x1A] = {
    0,
    0x100,
    0x200,
    0x300,
    0x400,
    0x500,
    0x600,
    0x700,
    0x800,
    0x900,
    0xA00,
    0xB00,
    0xC00,
    0xD00,
    0xE00,
    0xF00,
    0x1000,
    0x1100,
    0x1200,
    0x1300,
    0x1400,
    0x1500,
    0x1600,
    0x1700,
    0x1800,
    0x1900,
};
s32 D_90000_8017AB44[0x14] = { 0x50, 0x44, 0x3C, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s32 D_90000_8017C238;
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[2] = { D_90000_8017A630, D_90000_8017A630 };
// end pc_import_data.py
// clang-format on
#endif
