// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #15. Used by weapons:
// Shuriken, Cross shuriken, Buffalo star, Flame star, TNT
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "shared.h"
#include "gen/w_015_1.h"
#include "gen/w_015_2.h"
#define g_Animset w_015_1
#define g_Animset2 w_015_2
#include "sfx.h"

extern s16 D_6D000_8017A6B8[];
extern AnimationFrame D_6D000_8017A6C0[];
extern s16 D_6D000_8017A6FC[];
extern s16 D_6D000_8017A71C[];
extern AnimationFrame D_6D000_8017A770[];
extern s16 D_6D000_8017A78C[];
extern s32 D_6D000_8017BFC8;

void EntityWeaponAttack(Entity* self) {
    switch (self->step) {
    case 0:
        SetSpriteBank1(g_Animset);
        self->animSet = ANIMSET_OVL(0x10);
        self->palette = PAL_UNK_110;
        self->unk5A = 0x64;
        if (g_HandId) {
            self->animSet += 2;
            self->palette = PAL_UNK_128;
            self->unk5A = 0x66;
        }
        self->animCurFrame = D_6D000_8017A6B8[((self->params >> 8) & 0x7F)];
        self->facingLeft = PLAYER.facingLeft;
        self->zPriority = PLAYER.zPriority - 2;
        self->flags = FLAG_POS_CAMERA_LOCKED;

        SetSpeedX(FIX(8));
        if (((self->params >> 8) & 0x7F) == 3) {
            SetSpeedX(FIX(5.33333));
        }
        self->posY.i.hi = PLAYER.posY.i.hi + PLAYER.hitboxOffY - 3;
        if (PLAYER.step != 2) {
            self->posY.i.hi -= 6;
        }

        if (self->facingLeft != 0) {
            self->posX.i.hi = PLAYER.posX.i.hi - PLAYER.hitboxOffX;
        } else {
            self->posX.i.hi = PLAYER.posX.i.hi + PLAYER.hitboxOffX;
        }

        SetWeaponProperties(self, 0);
        DestroyEntityWeapon(true);

        self->hitboxWidth = 4;
        self->hitboxHeight = 4;
        if (((self->params >> 8) & 0x7F) == 1) {
            self->drawFlags |= ENTITY_ROTATE;
        }
        if (((self->params >> 8) & 0x7F) == 3) {
            self->palette = PAL_FLAG(PAL_CC_FIRE_EFFECT);
            self->drawFlags |= ENTITY_ROTATE;
        }
        if (!((self->params >> 8) & 0x7F)) {
            g_api.PlaySfx(SFX_WEAPON_SWISH_A);
        } else {
            g_api.PlaySfx(SFX_ARROW_SHOT_A);
            if (((self->params >> 8) & 0x7F) == 3) {
                g_api.PlaySfx(SFX_FIREBALL_SHOT_B);
            }
        }
        g_Player.timers[10] = 4;
        self->step++;
        break;

    case 1:
        self->rotate = self->rotate + 256;
        self->posX.val += self->velocityX;

        if ((((self->params >> 8) & 0x7F) == 2) &&
            !((u16)self->ext.weapon.lifetime & 1)) {
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(58, 0), 0);
        }

        if (((self->params >> 8) & 0x7F) == 3) {
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(80, 0), 0);
        }

        self->ext.weapon.lifetime++;

        if (self->hitFlags != 0) {
            self->ext.weapon.lifetime = 24;
            self->hitboxState = 0;
            self->drawFlags |= ENTITY_BLINK;
            self->step++;
        }
        break;

    case 2:
        self->ext.weapon.lifetime--;
        if (self->ext.weapon.lifetime == 0) {
            DestroyEntity(self);
        }
        break;
    }
}

s32 func_ptr_80170004(Entity* self) {
    if (self->step == 0) {
        self->animSet = self->ext.weapon.parent->animSet;
        self->animCurFrame = self->ext.weapon.parent->animCurFrame;
        self->facingLeft = self->ext.weapon.parent->facingLeft;
        self->zPriority = self->ext.weapon.parent->zPriority - 2;
        self->flags = FLAG_POS_CAMERA_LOCKED;
        self->palette = self->ext.weapon.parent->palette;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->ext.timer.t = 10;
        self->blendMode = BLEND_TRANSP;
        self->drawFlags = ENTITY_OPACITY;
        self->opacity = 0x80;
        self->step++;
    }
    if (self->opacity >= 0x30) {
        self->opacity += 0xF8;
    }

    if (--self->ext.timer.t == 0) {
        DestroyEntity(self);
    }
}

static void func_ptr_80170008(Entity* self) {
    Entity* factory;

    if (self->step == 0) {
        self->animSet = 2;
        self->anim = D_6D000_8017A6C0;
        self->zPriority = PLAYER.zPriority - 4;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
        self->velocityY = FIX(-1);

        if (!(D_6D000_8017BFC8 & 1)) {
            factory = g_api.CreateEntFactoryFromEntity(self, FACTORY(4, 11), 0);
            if (factory != NULL) {
                if (g_HandId == 0) {
                    factory->entityId = 0xEF;
                } else {
                    factory->entityId = 0xFF;
                }
            }
        }

        self->posY.i.hi += (rand() % 12) - 6;
        self->posX.i.hi += (rand() & 7) - 3;
        D_6D000_8017BFC8++;

        if (!(rand() & 1)) {
            self->blendMode = ENTITY_MASK_B | ENTITY_MASK_G | ENTITY_MASK_R;
        }
        self->step++;
    }
    self->posY.val += self->velocityY;
    if (self->poseTimer < 0) {
        DestroyEntity(self);
    }
}

void func_ptr_8017000C(Entity* self) {
    Collider collider;
    Primitive* prim;
    s16 xOffset, yOffset;
    s16 x, y;
    Entity* e;

    switch (self->step) {
    case 0:
        SetSpriteBank1(g_Animset);

        self->animSet = ANIMSET_OVL(0x10);
        self->unk5A = 0x64;
        self->palette = PAL_UNK_110;

        if (g_HandId) {
            self->palette = PAL_UNK_128;
            self->unk5A = 0x66;
            self->animSet += 2;
        }

        self->facingLeft = PLAYER.facingLeft;
        self->zPriority = PLAYER.zPriority - 2;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS;

        self->primIndex = g_api.AllocPrimitives(PRIM_TILE, 32);

        if (self->primIndex == -1) {
            self->flags &= ~FLAG_HAS_PRIMS;
        }

        if (self->flags & FLAG_HAS_PRIMS) {
            prim = &g_PrimBuf[self->primIndex];
            while (prim != NULL) {
                prim->r0 = 128;
                prim->g0 = 64;
                prim->b0 = 0;
                prim->u0 = prim->v0 = 1;
                prim->x1 = rand() & 0xF;
                prim->x2 = 0;
                prim->x3 = rand() & 0xF;
                prim->priority = self->zPriority - 2;
                prim->drawMode = DRAW_HIDE | DRAW_UNK02;
                prim = prim->next;
            }
        }
        self->ext.weapon.lifetime = 10;
        self->velocityY = FIX(-5.0 / 2.0);
        SetSpeedX(FIX(3.0 / 2.0));

        self->posY.i.hi = PLAYER.posY.i.hi + PLAYER.hitboxOffY - 8;
        if (PLAYER.step != 2) {
            self->posY.i.hi -= 6;
        }

        self->attack = 1;
        self->attackElement = ELEMENT_HIT;
        self->hitboxState = 2;
        self->nFramesInvincibility = 20;
        self->stunFrames = 4;
        self->hitEffect = 1;
        self->entityRoomIndex = 0;
        g_api.func_80118894(self);
        DestroyEntityWeapon(true);

        self->hitboxWidth = 4;
        self->hitboxHeight = 4;
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        g_Player.timers[10] = 4;
        self->step++;
        break;

    case 1:
        if (!(g_GameTimer & 7)) {
            g_api.PlaySfx(SFX_RAPID_SCRAPE_3X);
        }
        self->animCurFrame = 13;
        self->posX.val += self->velocityX;

        self->posY.val += self->velocityY;
        if (self->velocityY <= FIX(458751.0 / 65536.0)) {
            self->velocityY += FIX(1.0 / 8.0);
        }

        if (self->hitFlags) {
            self->velocityX = self->velocityX * 2 / 3;
            self->hitFlags = 0;
            return;
        }

        xOffset = 4;
        if (self->velocityX < 0) {
            xOffset = -xOffset;
        }
        yOffset = 0;
        x = self->posX.i.hi + xOffset;
        y = self->posY.i.hi + yOffset;
        g_api.CheckCollision(x, y, &collider, 0);
        if (collider.effects & EFFECT_SIDE) {
            if (xOffset < 0) {
                self->posX.i.hi += collider.unkC;
            } else {
                self->posX.i.hi += collider.unk4;
            }
            self->velocityX = -(self->velocityX / 2);
        }

        xOffset = 0;
        yOffset = -4;
        x = self->posX.i.hi + xOffset;
        y = self->posY.i.hi + yOffset;
        g_api.CheckCollision(x, y, &collider, 0);

        if (collider.effects & EFFECT_SOLID) {
            self->posY.i.hi += 1 + collider.unk20;
            self->velocityY = FIX(1);
            self->velocityX = self->velocityX / 2;
        }

        xOffset = 0;
        yOffset = 6;
        x = self->posX.i.hi + xOffset;
        y = self->posY.i.hi + yOffset;
        g_api.CheckCollision(x, y, &collider, 0);

        if (collider.effects & EFFECT_SOLID) {
            e = g_api.CreateEntFactoryFromEntity(self, WFACTORY(76, 0), 0);
            e = g_api.CreateEntFactoryFromEntity(self, WFACTORY(78, 16), 0);
            g_api.ShakeCamera(SHAKE_Y_SMALL2);
            self->step++;
            self->velocityY = FIX(-6);
            self->ext.weapon.lifetime = 16;

            if (self->flags & FLAG_HAS_PRIMS) {
                prim = &g_PrimBuf[self->primIndex];
                while (prim != NULL) {
                    prim->drawMode |= DRAW_HIDE;
                    prim = prim->next;
                }
            }
        }

        x = self->posX.i.hi;
        y = self->posY.i.hi;

        if (self->flags & FLAG_HAS_PRIMS) {
            prim = &g_PrimBuf[self->primIndex];
            while (prim) {
                if (prim->x3 == 0) {
                    if (self->facingLeft) {
                        prim->x0 = x - 8 -
                                   (D_6D000_8017A6FC[prim->x1] * prim->x2 / 15);
                    } else {
                        prim->x0 = x + 8 +
                                   (D_6D000_8017A6FC[prim->x1] * prim->x2 / 15);
                    }

                    prim->y0 = y + (D_6D000_8017A71C[prim->x1] * prim->x2 / 15);

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
        break;

    case 2:
        self->hitboxState = 0;
        if ((self->ext.weapon.unk7E & 7) == 0) {
            g_api.PlaySfx(SFX_FM_EXPLODE_B);
        }
        self->animCurFrame = 0;
        self->posY.val += self->velocityY;

        if (--self->ext.weapon.lifetime == 0) {
            DestroyEntity(self);
            return;
        }

        if (self->ext.weapon.lifetime & 1) {
            e = g_api.CreateEntFactoryFromEntity(
                self, WFACTORY(0x4E, 0) + ((self->ext.weapon.unk7E + 16) << 16),
                0);
            self->ext.weapon.unk7E++;
        }

        break;

    case 3:
        self->hitboxState = 0;
        // the last three bits of weapon lifetime need to be zero.
        if ((self->ext.weapon.lifetime & 7) == 0) {
            g_api.PlaySfx(SFX_FM_EXPLODE_B);
        }
        // no, seriously. the last three bits of weapon lifetime really need
        // to be zero. and don't trust that last comparison. i'm not joking.
        if ((self->ext.weapon.lifetime & 7) == 0) {
            e = g_api.CreateEntFactoryFromEntity(self, WFACTORY(104, 0), 0);
        }

        self->animCurFrame = 0;
        if (--self->ext.weapon.lifetime == 0) {
            DestroyEntity(self);
            return;
        }

        break;
    }
}

s32 func_ptr_80170010(Entity* self) {
    s16 angle;
    s32 accelerationX;
    s32 accelerationY;
    u16 rotation;
    s16 params;

    params = (u16)self->params >> 8;
    if (self->step == 0) {
        if (self->ext.factory.parent->entityId == 0) {
            DestroyEntity(self);
            return;
        }
        SetSpriteBank2(g_Animset2);
        self->animSet = ANIMSET_OVL(0x11);
        self->palette = PAL_UNK_111;
        self->unk5A = 0x65;
        if (g_HandId != 0) {
            self->animSet += 2;
            self->palette += 0x18;
            self->unk5A = 0x67;
        }
        self->anim = D_6D000_8017A770;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
        self->zPriority = PLAYER.zPriority - 2;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        if (params >= 0x10) {
            rotation = D_6D000_8017A78C[params];
            self->scaleY = rotation;
            self->scaleX = rotation;
        } else {
            self->zPriority = PLAYER.zPriority + 2;
            if (g_Player.padPressed & PAD_DOWN) {
                angle = (rand() % 1535) + 256;
                self->velocityX = rcos(angle) << 6;
                self->velocityY = -(rsin(angle) << 6);
                self->posX.val += self->velocityX;
                self->posY.val += self->velocityY;

                accelerationX = -self->velocityX;
                if (accelerationX < 0) {
                    accelerationX += 15;
                }
                self->ext.weapon.accelerationX = (accelerationX >> 4);

                accelerationY = -self->velocityY;
                if (accelerationY < 0) {
                    accelerationY += 15;
                }
                self->ext.weapon.accelerationY = (u32)(accelerationY >> 4);
            } else {
                self->velocityY = FIX(-1);
                self->posY.i.hi -= 12 - (rand() % 24);
                self->posX.i.hi -= 20 - (rand() % 40);
            }
        }
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        self->enemyId = self->ext.factory.parent->enemyId;
        self->hitboxWidth = 12;
        self->hitboxHeight = 12;

        self->step++;
        return;
    }
    if (self->poseTimer < 0) {
        DestroyEntity(self);
        return;
    }

    self->posY.val += self->velocityY;
    self->posX.val += self->velocityX;

    if (params == 0) {
        self->velocityX += self->ext.weapon.accelerationX;
        self->velocityY += self->ext.weapon.accelerationY;
    }
}

static s32 func_ptr_80170014(Entity* self) {
    s16 angle;

    if (self->step == 0) {
        if (self->ext.weapon.parent->entityId == 0) {
            DestroyEntity(self);
            return;
        }
        SetSpriteBank2(g_Animset2);
        self->animSet = ANIMSET_OVL(17);
        self->palette = PAL_UNK_111;
        self->unk5A = 101;
        if (g_HandId != 0) {
            self->animSet += 2;
            self->palette += 24;
            self->unk5A = 103;
        }
        self->anim = D_6D000_8017A770;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
        self->zPriority = PLAYER.zPriority - 2;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        angle = ratan2(-self->ext.weapon.parent->velocityY,
                       self->ext.weapon.parent->velocityX);
        angle += (0x500 + (rand() % 0x600));
        self->velocityX = rcos(angle) * 64;
        self->velocityY = rsin(angle) * -64;
        self->ext.weapon.accelerationX = -self->velocityX / 13;
        self->ext.weapon.accelerationY = -self->velocityY / 13;
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        self->enemyId = self->ext.weapon.parent->enemyId;
        self->hitboxHeight = self->hitboxWidth = 12;
        self->step++;
        return;
    }
    if (self->poseTimer < 0) {
        DestroyEntity(self);
        return;
    }
    self->posY.val += self->velocityY;
    self->posX.val += self->velocityX;
    self->velocityX += self->ext.weapon.accelerationX;
    self->velocityY += self->ext.weapon.accelerationY;
}

static int GetWeaponId(void) { return 15; }

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
extern u8 D_6D000_8017A478[];
u8 D_6D000_8017A478[] ALIGNED4 = {0x00, 0x00, 0x25, 0x8C, 0x68, 0x9C, 0xCD, 0xA0, 0x30, 0xAD, 0x93, 0xAD, 0xF6, 0xAD, 0x99, 0xC6, 0x00, 0xE8, 0x00, 0x80, 0x07, 0xA5, 0x49, 0xB1, 0xAC, 0xBD, 0x0F, 0xCA, 0x72, 0xD6, 0xD5, 0xE2, 0x00, 0x00, 0xAB, 0x84, 0xD0, 0x84, 0x15, 0x85, 0x5A, 0x85, 0x9D, 0x89, 0xDF, 0x8D, 0x3F, 0x96, 0x9F, 0x9E, 0xFF, 0xA6, 0x5F, 0xAF, 0xBF, 0xB7, 0xFF, 0xC3, 0xFF, 0xD7, 0xFF, 0xEB, 0xFF, 0xFF, 0x00, 0x00, 0xEF, 0x8C, 0x30, 0x8D, 0x71, 0x91, 0xB3, 0x95, 0xF4, 0x99, 0x35, 0x9E, 0x77, 0xA2, 0xB8, 0xA6, 0xFA, 0xAA, 0x1B, 0xB7, 0x5C, 0xC3, 0x7C, 0xD3, 0x9D, 0xDF, 0xDE, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0xEF, 0x8C, 0x10, 0x8D, 0x51, 0x91, 0x92, 0x95, 0xB3, 0x99, 0xF4, 0x9D, 0x35, 0xA2, 0x77, 0xA6, 0x97, 0xAA, 0xB8, 0xAE, 0xD9, 0xB2, 0xF9, 0xB6, 0x1A, 0xBB, 0x3B, 0xBF, 0x5C, 0xC3, 0x00, 0x00, 0xB0, 0x88, 0xF1, 0x88, 0x33, 0x8D, 0x75, 0x91, 0xB7, 0x95, 0xF8, 0x95, 0x3A, 0x9A, 0x7C, 0x9E, 0xBE, 0xA2, 0xDE, 0xB2, 0x1E, 0xC3, 0x5E, 0xD3, 0x9E, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0xB0, 0x88, 0xD1, 0x88, 0x13, 0x8D, 0x35, 0x8D, 0x77, 0x91, 0x99, 0x91, 0xDB, 0x95, 0x1D, 0x9A, 0x9D, 0xA2, 0x1E, 0xAF, 0x7E, 0xBB, 0x9E, 0xCB, 0xBE, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0xB0, 0x88, 0xD1, 0x88, 0xF2, 0x88, 0x14, 0x8D, 0x35, 0x8D, 0x77, 0x91, 0x98, 0x91, 0xBA, 0x95, 0xDB, 0x95, 0x1D, 0x9A, 0x5D, 0xA2, 0x9D, 0xAA, 0xFD, 0xB6, 0x3D, 0xBF, 0x9E, 0xCB, 0x00, 0x00, 0xEF, 0x8C, 0x30, 0x8D, 0x71, 0x91, 0xB2, 0x95, 0xF3, 0x99, 0x35, 0x9E, 0x58, 0x9E, 0x7B, 0x9E, 0xBE, 0xA2, 0xDE, 0xAE, 0x1E, 0xBF, 0x5E, 0xCF, 0x9E, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0x30, 0x81, 0x75, 0x89, 0xBA, 0x91, 0xFF, 0x9D, 0x1F, 0xAA, 0x1F, 0xB2, 0x3F, 0xBE, 0x5F, 0xCA, 0x9F, 0xD2, 0xDF, 0xDA, 0x1F, 0xE3, 0x3F, 0xE7, 0x7F, 0xEF, 0xBF, 0xF7, 0xFF, 0xFF, 0x00, 0x00, 0x1F, 0x80, 0x9F, 0x80, 0x3F, 0x81, 0xBF, 0x81, 0x5F, 0x82, 0xDF, 0x82, 0x7F, 0x83, 0xFF, 0x83, 0xFF, 0x93, 0xFF, 0xA7, 0xFF, 0xB7, 0xFF, 0xCB, 0xFF, 0xDB, 0xFF, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0x08, 0xB9, 0x09, 0xBD, 0x2B, 0xC1, 0x4D, 0xC5, 0x4E, 0xC9, 0x70, 0xCD, 0x92, 0xD1, 0xB4, 0xD5, 0xD5, 0xD9, 0x16, 0xE2, 0x58, 0xE6, 0x99, 0xEE, 0xDB, 0xF2, 0x1C, 0xFB, 0x5E, 0xFF, 0x00, 0x00, 0x08, 0xC9, 0x29, 0xD1, 0x6A, 0xD9, 0x8C, 0xE1, 0xCD, 0xE5, 0xEF, 0xED, 0x10, 0xF2, 0x51, 0xFA, 0x93, 0xFE, 0xB4, 0xFE, 0xF5, 0xFE, 0x16, 0xFF, 0x57, 0xFF, 0x78, 0xFF, 0xBA, 0xFF, 0x00, 0x00, 0xA0, 0x91, 0xC0, 0x95, 0xE0, 0x99, 0x00, 0xA2, 0x20, 0xA6, 0x40, 0xAE, 0x60, 0xB2, 0x80, 0xBA, 0xA0, 0xBE, 0xC0, 0xC6, 0x05, 0xCF, 0x4B, 0xDB, 0x70, 0xE3, 0xB6, 0xEF, 0xFC, 0xFB, 0x00, 0x00, 0x8B, 0xB5, 0x8D, 0xB1, 0xB0, 0xB1, 0xD2, 0xB1, 0xF5, 0xB1, 0x17, 0xB2, 0x3A, 0xB2, 0x5D, 0xB2, 0x9D, 0xBA, 0xDD, 0xC6, 0x1E, 0xD3, 0x3E, 0xDF, 0x7E, 0xEB, 0xBE, 0xF7, 0xFF, 0xFF, 0x00, 0x00, 0x8B, 0xB5, 0x8D, 0xB1, 0xAF, 0xB1, 0xD2, 0xB1, 0xF3, 0xAD, 0x34, 0xAA, 0x76, 0xA6, 0xB8, 0xA6, 0xD9, 0xB2, 0x1A, 0xBF, 0x3B, 0xCB, 0x7C, 0xDB, 0x9D, 0xE7, 0xDE, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x12, 0xB1, 0x30, 0xB5, 0x4E, 0xBD, 0x8D, 0xC5, 0xAB, 0xC9, 0xEA, 0xD1, 0x08, 0xDA, 0x47, 0xE2, 0x8A, 0xDE, 0xCE, 0xDA, 0x11, 0xD7, 0x35, 0xD7, 0x78, 0xD3, 0xBC, 0xCF, 0xFF, 0xCF, 0x00, 0x00, 0x08, 0xC9, 0x26, 0xC5, 0x65, 0xC1, 0xA4, 0xC1, 0xC3, 0xBD, 0x02, 0xBE, 0x41, 0xBA, 0x80, 0xBA, 0xA4, 0xBA, 0xE9, 0xBE, 0x2D, 0xC3, 0x52, 0xC3, 0x96, 0xC7, 0xDB, 0xCB, 0xFF, 0xCF, 0x00, 0x00, 0x29, 0xAD, 0x4A, 0xB1, 0x6B, 0xB5, 0x8C, 0xB9, 0xAD, 0xBD, 0xCE, 0xC5, 0xEF, 0xC9, 0x10, 0xCE, 0x31, 0xD2, 0x52, 0xD6, 0x73, 0xDE, 0x94, 0xE2, 0xB5, 0xE6, 0xD6, 0xEA, 0x18, 0xF3};
s16 D_6D000_8017A6B8[4] = { 0xA, 0xB, 0xC, 0xB };
AnimationFrame D_6D000_8017A6C0[0xF] = {
    { 1, 1 },
    { 1, 2 },
    { 1, 3 },
    { 1, 4 },
    { 1, 5 },
    { 1, 4 },
    { 1, 3 },
    { 1, 4 },
    { 1, 3 },
    { 1, 4 },
    { 1, 5 },
    { 1, 6 },
    { 1, 7 },
    { 1, 8 },
    { 0xFFFF, 0 },
};
s16 D_6D000_8017A6FC[0x10] = { -9, -8, -8, -7, -7, -6, -6, -5, -5, -4, -4, -3, -3, -2, -2, -1 };
s16 D_6D000_8017A71C[0x2A] = {
    -0x14,
    -0x14,
    -0x15,
    -0x15,
    -0x16,
    -0x17,
    -0x17,
    -0x18,
    -0x18,
    -0x19,
    -0x19,
    -0x1A,
    -0x1A,
    -0x1B,
    -0x1B,
    -0x1C,
    1,
    9,
    2,
    0xA,
    2,
    0xB,
    2,
    0xC,
    2,
    0xD,
    2,
    0xE,
    2,
    0xF,
    2,
    0x10,
    2,
    0x11,
    2,
    0x12,
    3,
    0x13,
    4,
    0x14,
    -1,
    0,
};
AnimationFrame D_6D000_8017A770[7] = { { 2, 1 }, { 2, 2 }, { 2, 3 }, { 2, 4 }, { 2, 5 }, { 2, 6 }, { 2, 7 } };
s16 D_6D000_8017A78C[0x28] = {
    2,
    8,
    2,
    9,
    2,
    0xA,
    2,
    0xB,
    2,
    0xC,
    2,
    0xD,
    2,
    0xE,
    -1,
    0,
    0xE0,
    0xD0,
    0xC0,
    0xB0,
    0xA0,
    0x90,
    0x80,
    0x70,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
    0x60,
};
s32 D_6D000_8017BFC8;
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[1] = { D_6D000_8017A478 };
// end pc_import_data.py
// clang-format on
#endif
