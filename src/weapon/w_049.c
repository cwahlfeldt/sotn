// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #49. Used by weapons:
// Firebrand, Marsil, Unknown#179, Unknown#180
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "gen/w_049_1.h"
#include "gen/w_049_2.h"
#define g_Animset w_049_1
#define g_Animset2 w_049_2
#include "shared.h"
#include "sfx.h"

extern WeaponAnimation D_15B000_8017AFFC[];
extern s16 D_15B000_8017B03C[];
extern AnimationFrame D_15B000_8017B074[];
extern AnimationFrame D_15B000_8017B0D4[];
extern AnimationFrame D_15B000_8017B10C[];
extern s32 D_15B000_8017C8CC;

/* near-duplicate of EntityWeaponAttack_w_048 */
void EntityWeaponAttack(Entity* self) {
    s8 animIndex = (self->params & 0x7FFF) >> 8;
    WeaponAnimation* anim = &D_15B000_8017AFFC[animIndex];

    self->posX.val = PLAYER.posX.val;
    self->posY.val = PLAYER.posY.val;
    self->facingLeft = PLAYER.facingLeft;

    if ((PLAYER.ext.player.anim < anim->frameStart) ||
        (PLAYER.ext.player.anim >= anim->frameStart + 7) ||
        g_Player.unk46 == 0) {
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
        self->flags = FLAG_POS_PLAYER_LOCKED | FLAG_UNK_20000;
        self->zPriority = PLAYER.zPriority - 2;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;

        if (animIndex == 1) {
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x5B, 0), 0);
        }
        if (animIndex == 3) {
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x5F, 0), 0);
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x40, 1), 0);
        }
        SetWeaponProperties(self, 0);
        D_15B000_8017C8CC = 0;
        self->step += 1;
    }
    self->ext.weapon.anim = PLAYER.ext.player.anim - anim->frameStart;
    if ((PLAYER.poseTimer == 1) && (PLAYER.pose == anim->soundFrame)) {
        g_api.PlaySfx(anim->soundId);
    }
    if (g_api.UpdateUnarmedAnim(anim->frameProps, anim->frames) < 0) {
        DestroyEntity(self);
        return;
    }
    if ((D_15B000_8017C8CC == 0) && (PLAYER.pose == 1)) {
        if ((animIndex == PLAYER.pose) || (animIndex == 3)) {
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x5D, 0), 0);
            g_api.PlaySfx(SFX_FM_EXPLODE_B);
        }
        if (animIndex == 2) {
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x5D, 1), 0);
            g_api.PlaySfx(SFX_FM_EXPLODE_B);
        }
    }
    D_15B000_8017C8CC = PLAYER.pose;
    self->drawFlags = PLAYER.drawFlags;
    self->scaleY = PLAYER.scaleY;
    self->rotPivotY = PLAYER.rotPivotY;
}

void func_15B000_8017B88C(Entity* ent, Point16* outPoint, bool arg2) {
    s32 idx;

    idx = PLAYER.ext.player.anim - 0x41;
    if (PLAYER.facingLeft) {
        ent->posX.i.hi = PLAYER.posX.i.hi - D_15B000_8017B03C[idx * 4 + 0];
        if (arg2) {
            outPoint->x = -(D_15B000_8017B03C[idx * 4 + 2] * 3 / 4);
        } else {
            outPoint->x = -(D_15B000_8017B03C[idx * 4 + 2] * 2 / 3);
        }
    } else {
        ent->posX.i.hi = PLAYER.posX.i.hi + D_15B000_8017B03C[idx * 4 + 0];
        if (arg2) {
            outPoint->x = D_15B000_8017B03C[idx * 4 + 2] * 3 / 4;
        } else {
            outPoint->x = D_15B000_8017B03C[idx * 4 + 2] * 2 / 3;
        }
    }
    ent->posY.i.hi = PLAYER.posY.i.hi + D_15B000_8017B03C[idx * 4 + 1];
    if (arg2) {
        outPoint->y = D_15B000_8017B03C[idx * 4 + 3] * 3 / 4;
    } else {
        outPoint->y = D_15B000_8017B03C[idx * 4 + 3] * 2 / 3;
    }
}

s32 func_ptr_80170004(Entity* self) {
    s32 angle;
    s32 magnitude;
    s16 baseX, baseY;
    s16 offsetX;
    bool rotate;
    s16 initialX, initialY;
    u8 params;

    if ((PLAYER.ext.player.anim < 0x41 || PLAYER.ext.player.anim > 0x47) ||
        g_Player.unk46 == 0) {
        DestroyEntity(self);
        return;
    }

    params = self->params;
    offsetX = 0xC;
    if (PLAYER.facingLeft) {
        offsetX = -0xC;
    }

    baseX = offsetX + PLAYER.posX.i.hi;
    baseY = PLAYER.posY.i.hi - 0x1A;
    if (PLAYER.drawFlags & ENTITY_SCALEY) {
        baseY = PLAYER.posY.i.hi - 0x1F;
    }

    switch (self->step) {
    case 0:
        rotate = 0;
        self->animSet = 9;
        self->anim = D_15B000_8017B074;
        self->zPriority = PLAYER.zPriority + 2;
        self->facingLeft = 1;
        self->flags = FLAG_UNK_100000 | FLAG_POS_PLAYER_LOCKED | FLAG_UNK_20000;
        self->drawFlags = ENTITY_ROTATE;
        self->ext.weapon.unk80 = params * 512;
        self->rotate = self->ext.weapon.unk80 - FIX(1.0 / 64);
        self->attack = self->ext.weapon.parent->attack;
        self->attackElement = self->ext.weapon.parent->attackElement;
        self->hitboxState = self->ext.weapon.parent->hitboxState;
        self->nFramesInvincibility =
            self->ext.weapon.parent->nFramesInvincibility;
        self->stunFrames = self->ext.weapon.parent->stunFrames;
        self->hitEffect = self->ext.weapon.parent->hitEffect;
        self->entityRoomIndex = self->ext.weapon.parent->entityRoomIndex;
        g_api.func_80118894(self);
        self->hitboxWidth = 6;
        self->hitboxHeight = 6;
        self->ext.weapon.unk7E = 0x40;
        self->hitboxOffX = 0;
        self->hitboxOffY = 0;
        self->ext.weapon.lifetime = 0x28;
        self->step++;
        break;
    case 1:
        rotate = 1;
        initialX = self->ext.weapon.unk7E; // ???
        if (self->ext.weapon.unk7E >= 8) {
            self->ext.weapon.unk7E -= 3;
        }
        if (--self->ext.weapon.lifetime == 0x18 && params == 0) {
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x40, 0), 0);
        }
        self->ext.weapon.unk80 = self->ext.weapon.unk80 - 0x20;
        if (self->ext.weapon.lifetime < 0x11) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    angle = self->ext.weapon.unk80;
    initialX = self->posX.i.hi;
    magnitude = self->ext.weapon.unk7E;
    initialY = self->posY.i.hi;
    self->posX.i.hi = baseX + (((rcos(angle) >> 4) * magnitude) >> 8);
    self->posY.i.hi = baseY - (((rsin(angle) >> 4) * magnitude) >> 8);
    if (rotate != 0) {
        self->rotate = ratan2(-(s16)(self->posY.i.hi - initialY),
                              (s16)(self->posX.i.hi - initialX));
    }
}

void func_ptr_80170008(Entity* self) {
    Primitive* prim;
    s16 primIndex;
    s32 i;
    s16 angle;
    s16 lifetime;
    s32 xOffset;
    u16 posY;
    u16 posX;
    u8 red, green;

    if (!(PLAYER.ext.weapon.anim > 0x40 && PLAYER.ext.weapon.anim < 0x48) ||
        g_Player.unk46 == 0) {
        DestroyEntity(self);
        return;
    }

    xOffset = 12;
    if (PLAYER.facingLeft != 0) {
        xOffset = -12;
    }

    self->posX.i.hi = xOffset + PLAYER.posX.i.hi;
    self->posY.i.hi = PLAYER.posY.i.hi - 26;
    if (PLAYER.drawFlags & ENTITY_SCALEY) {
        self->posY.i.hi -= 5;
    }

    switch (self->step) {
    case 0:
        primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, 16);
        self->primIndex = primIndex;
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }

        self->flags = FLAG_HAS_PRIMS | FLAG_POS_PLAYER_LOCKED | FLAG_UNK_20000;
        prim = &g_PrimBuf[self->primIndex];

        for (i = 0; i < 16; i++) {
            angle = i * 256;
            prim->u0 = ((rcos(angle) * 2) >> 8) + 32;
            prim->v0 = -32 - ((rsin(angle) * 2) >> 8);

            angle = (i + 1) * 256;
            prim->u1 = ((rcos((s32)angle) * 2) >> 8) + 32;
            prim->v1 = -32 - ((rsin((s32)angle) * 2) >> 8);

            if (prim->v0 < 4) {
                prim->v0 = 0xFF;
            }
            if (prim->v1 < 4) {
                prim->v1 = 0xFF;
            }

            prim->u2 = prim->u3 = 0x20;
            prim->v2 = prim->v3 = 0xE0;
            prim->g3 = prim->g2 = 0x7F;
            prim->clut = PAL_FILL_WHITE;
            prim->tpage = 0x1A;
            prim->r1 = prim->b1 = prim->g1 = 0;
            prim->r0 = prim->b0 = prim->g0 = 0;
            prim->r2 = prim->r3 = 0xFF;
            prim->b2 = prim->b3 = 0;
            prim->type = PRIM_GT4;
            prim->priority = PLAYER.zPriority + 2;
            prim->drawMode = DRAW_UNK_200 | DRAW_UNK_100 | DRAW_TPAGE2 |
                             DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            prim = prim->next;
        }
        self->ext.weapon.lifetime = 4;
        self->ext.weapon.unk7E = 0x19;
        if (self->params & 0x7F00) {
            self->ext.weapon.unk7E = 40;
        }
        self->step++;
        break;

    case 1:
        if (self->ext.weapon.lifetime < 40) {
            self->ext.weapon.lifetime += 6;
        }
        self->ext.weapon.unk7E--;
        if (!self->ext.weapon.unk7E) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    red = 0x80;
    if (self->ext.weapon.unk7E < 8) {
        red = (self->ext.weapon.unk7E * 16);
    }
    i = 0; // FAKE? why here?
    green = red >> 1;
    posX = self->posX.i.hi;
    posY = self->posY.i.hi;
    lifetime = self->ext.weapon.lifetime;
    prim = &g_PrimBuf[self->primIndex];

    for (i = 0; i < 16; i++) {
        prim->x2 = prim->x3 = posX;
        prim->y2 = prim->y3 = posY;

        prim->r3 = red;
        prim->r2 = red;

        prim->g3 = green;
        prim->g2 = green;

        angle = i * 256;
        prim->x0 = posX + (((rcos(angle) >> 4) * lifetime) >> 8);
        prim->y0 = posY - (((rsin(angle) >> 4) * lifetime) >> 8);

        angle = (i + 1) * 256;
        prim->x1 = posX + (((rcos(angle) >> 4) * lifetime) >> 8);
        prim->y1 = posY - (((rsin(angle) >> 4) * lifetime) >> 8);
        prim = prim->next;
    }
}

void func_ptr_8017000C(Entity* self) {
    Point16 point;
    s32 palette;
    u32 upperParams;
    s32 lowerParams;
    s16 tempX, tempY;

    if ((PLAYER.ext.player.anim < 0x41 || PLAYER.ext.player.anim > 0x47 ||
         g_Player.unk46 == 0) &&
        (self->step == 0)) {
        DestroyEntity(self);
        return;
    }

    upperParams = (self->params >> 8) & 0x7F;
    lowerParams = (u8)self->params;
    if (self->step == 0) {
        self->facingLeft = PLAYER.facingLeft;
        if (upperParams == 1) {
            func_15B000_8017B88C(self, &point, false);
        } else {
            func_15B000_8017B88C(self, &point, true);
        }

        tempX = point.x;
        tempY = point.y;
        self->posX.i.hi += (tempX * lowerParams / 8);
        self->posY.i.hi += (tempY * lowerParams / 8);
        self->posY.i.hi += rand() % 16;
        self->posX.i.hi += rand() % 4;

        SetSpriteBank2(g_Animset2);
        self->animSet = ANIMSET_OVL(0x11);
        self->unk5A = 0x65;
        palette = rand() & 7;
        self->palette = palette + 0x117;
        if (g_HandId != 0) {
            self->palette = palette + 0x12F;
            self->unk5A += 2;
            self->animSet += 2;
        }
        self->anim = D_15B000_8017B0D4;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleX = 0x80;
        self->scaleY = 0x80;
        self->scaleX = self->scaleY = (rand() & 0x3F) | 0x80;
        self->zPriority = PLAYER.zPriority + (rand() & 1) * 4;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        if (rand() % 3) {
            self->blendMode = BLEND_TRANSP | BLEND_QUARTER;
        }

        self->velocityY = -FIX(0.5);
        if (PLAYER.drawFlags & ENTITY_SCALEY) {
            if (PLAYER.ext.player.anim >= 0x43 &&
                PLAYER.ext.player.anim < 0x45) {
                self->posY.i.hi -= 1;
            } else {
                self->posY.i.hi -= 3;
            }
        }
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        self->enemyId = self->ext.weapon.parent->enemyId;
        self->hitboxOffX = 0;
        self->hitboxOffY = 0;
        self->hitboxWidth = 6;
        self->hitboxHeight = 6;
        self->step++;
    } else {
        self->posY.val += self->velocityY;
        if (self->poseTimer < 0) {
            DestroyEntity(self);
        }
    }
}

s32 func_ptr_80170010(Entity* self) {
    s32 magnitude;
    s16 baseX, baseY;
    s16 offsetX;
    s16 angle;
    s8 params;

    if ((PLAYER.ext.player.anim < 0x41 || PLAYER.ext.player.anim > 0x47 ||
         g_Player.unk46 == 0) &&
        (self->step == 0 || self->step == 1)) {
        DestroyEntity(self);
        return;
    }

    params = self->params;
    offsetX = 0xC;
    if (PLAYER.facingLeft != 0) {
        offsetX = -0xC;
    }

    baseX = offsetX + PLAYER.posX.i.hi;
    baseY = PLAYER.posY.i.hi - 0x1A;
    if (PLAYER.drawFlags & ENTITY_SCALEY) {
        baseY = PLAYER.posY.i.hi - 0x1F;
    }

    switch (self->step) {
    case 0:
        self->facingLeft = 1;
        self->flags = FLAG_POS_PLAYER_LOCKED | FLAG_UNK_20000;
        self->ext.weapon.unk80 = params * 512;
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        self->enemyId = self->ext.weapon.parent->enemyId;
        self->hitboxWidth = 0;
        self->hitboxHeight = 0;
        self->ext.weapon.unk7E = 0xC;
        self->ext.weapon.lifetime = rand() % 5;
        self->step++;
        break;
    case 1:
        self->ext.weapon.unk7E += 4;
        self->ext.weapon.unk80 -= 0x20;
        if (self->ext.weapon.unk7E >= 0x40) {
            self->step += 1;
        }
        break;
    case 2:
        self->ext.weapon.unk7E += 4;
        self->flags = FLAG_POS_CAMERA_LOCKED;
        if (self->ext.weapon.unk7E >= 0x70) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    angle = self->ext.weapon.unk80;
    magnitude = self->ext.weapon.unk7E;
    self->ext.weapon.lifetime++;
    self->posX.i.hi = baseX + (((rcos(angle) >> 4) * magnitude) >> 8);
    self->posY.i.hi = baseY - (((rsin(angle) >> 4) * magnitude) >> 8);
    if (self->ext.weapon.lifetime % 7 == 0) {
        g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x65, 0), 0);
    }
}

static s32 func_ptr_80170014(Entity* self) {
    switch (self->step) {
    case 0:
        SetSpriteBank2(g_Animset2);
        self->animSet = ANIMSET_OVL(0x11);
        self->unk5A = 101;
        self->palette = PAL_UNK_117;
        if (g_HandId != 0) {
            self->palette = PAL_UNK_12F;
            self->unk5A += 2;
            self->animSet += 2;
        }
        self->anim = D_15B000_8017B10C;
        self->zPriority = PLAYER.zPriority + 2;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        self->scaleX = self->scaleY = 0x100;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        self->enemyId = self->ext.weapon.parent->enemyId;
        self->hitboxHeight = self->hitboxWidth = 12;
        self->hitboxOffY = self->hitboxOffX = 0;
        self->velocityY = FIX(-0.375);
        g_api.PlaySfx(SFX_FM_EXPLODE_B);
        self->step++;
        return;
    case 1:
        self->posY.val += self->velocityY;
        if (self->poseTimer < 0) {
            DestroyEntity(self);
        }
    }
}

static int GetWeaponId(void) { return 49; }

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
extern u8 D_15B000_8017AC00[];
extern u8 D_15B000_8017AF7C[];
extern void* D_15B000_8017AFE0[];
extern u8 D_15B000_8017AEE0[];
extern u8 D_15B000_8017AEFC[];
extern u8 D_15B000_8017AF18[];
extern u8 D_15B000_8017AF34[];
extern u8 D_15B000_8017AF4C[];
extern u8 D_15B000_8017AF64[];
u8 D_15B000_8017AC00[] ALIGNED4 = {0x00, 0x00, 0x00, 0x80, 0x7F, 0x81, 0xB6, 0x81, 0xDF, 0x81, 0x7F, 0x82, 0xFE, 0xA2, 0x7F, 0xD7, 0x3F, 0x84, 0x9E, 0x81, 0x1F, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x94, 0xD2, 0x9C, 0xF3, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7D, 0xB7, 0x7D, 0xB7, 0x7D, 0xB7, 0x3F, 0x84, 0x1E, 0x81, 0x1E, 0x81, 0x7D, 0xB7, 0x7D, 0xB7, 0x7D, 0xB7, 0x94, 0xD2, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x1F, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x8E, 0xDF, 0x99, 0x1F, 0x80, 0xB0, 0x80, 0x1A, 0x81, 0xDF, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x94, 0xD2, 0x5C, 0xE3, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xC0, 0x9D, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x80, 0x00, 0x00, 0x3F, 0x84, 0x00, 0x00, 0xB1, 0x80, 0x00, 0x00, 0x57, 0x81, 0x00, 0x00, 0x94, 0xD2, 0x1C, 0xD3, 0x00, 0x00, 0x00, 0x80, 0x58, 0x81, 0xB6, 0x81, 0x9B, 0x81, 0x3D, 0x82, 0xFF, 0xA2, 0x1F, 0xD3, 0x3F, 0x84, 0xC0, 0xB9, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x94, 0xD2, 0x9C, 0xF3, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x7D, 0xB7, 0x7D, 0xB7, 0x7D, 0xB7, 0x7D, 0xB7, 0x7D, 0xB7, 0x3F, 0x84, 0xC0, 0xB9, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x94, 0xD2, 0x9C, 0xF3, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x9E, 0x81, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x81, 0x00, 0x00, 0x3F, 0x84, 0xC0, 0xB9, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x94, 0xD2, 0x9C, 0xF3, 0x00, 0x00, 0xEF, 0x8C, 0x30, 0x8D, 0x71, 0x91, 0xB3, 0x95, 0xF4, 0x99, 0x35, 0x9E, 0x77, 0xA2, 0xB8, 0xA6, 0xFA, 0xAA, 0x1B, 0xB7, 0x5C, 0xC3, 0x7C, 0xD3, 0x9D, 0xDF, 0xDE, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0xEF, 0x8C, 0x10, 0x8D, 0x51, 0x91, 0x92, 0x95, 0xB3, 0x99, 0xF4, 0x9D, 0x35, 0xA2, 0x77, 0xA6, 0x97, 0xAA, 0xB8, 0xAE, 0xD9, 0xB2, 0xF9, 0xB6, 0x1A, 0xBB, 0x3B, 0xBF, 0x5C, 0xC3, 0x00, 0x00, 0xB0, 0x88, 0xF1, 0x88, 0x33, 0x8D, 0x75, 0x91, 0xB7, 0x95, 0xF8, 0x95, 0x3A, 0x9A, 0x7C, 0x9E, 0xBE, 0xA2, 0xDE, 0xB2, 0x1E, 0xC3, 0x5E, 0xD3, 0x9E, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0xB0, 0x88, 0xD1, 0x88, 0x13, 0x8D, 0x35, 0x8D, 0x77, 0x91, 0x99, 0x91, 0xDB, 0x95, 0x1D, 0x9A, 0x9D, 0xA2, 0x1E, 0xAF, 0x7E, 0xBB, 0x9E, 0xCB, 0xBE, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0xB0, 0x88, 0xD1, 0x88, 0xF2, 0x88, 0x14, 0x8D, 0x35, 0x8D, 0x77, 0x91, 0x98, 0x91, 0xBA, 0x95, 0xDB, 0x95, 0x1D, 0x9A, 0x5D, 0xA2, 0x9D, 0xAA, 0xFD, 0xB6, 0x3D, 0xBF, 0x9E, 0xCB, 0x00, 0x00, 0xEF, 0x8C, 0x30, 0x8D, 0x71, 0x91, 0xB2, 0x95, 0xF3, 0x99, 0x35, 0x9E, 0x58, 0x9E, 0x7B, 0x9E, 0xBE, 0xA2, 0xDE, 0xAE, 0x1E, 0xBF, 0x5E, 0xCF, 0x9E, 0xDF, 0xDE, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0x30, 0x81, 0x75, 0x89, 0xBA, 0x91, 0xFF, 0x9D, 0x1F, 0xAA, 0x1F, 0xB2, 0x3F, 0xBE, 0x5F, 0xCA, 0x9F, 0xD2, 0xDF, 0xDA, 0x1F, 0xE3, 0x3F, 0xE7, 0x7F, 0xEF, 0xBF, 0xF7, 0xFF, 0xFF, 0x00, 0x00, 0x1F, 0x80, 0x9F, 0x80, 0x3F, 0x81, 0xBF, 0x81, 0x5F, 0x82, 0xDF, 0x82, 0x7F, 0x83, 0xFF, 0x83, 0xFF, 0x93, 0xFF, 0xA7, 0xFF, 0xB7, 0xFF, 0xCB, 0xFF, 0xDB, 0xFF, 0xEF, 0xFF, 0xFF, 0x00, 0x00, 0x08, 0xB9, 0x09, 0xBD, 0x2B, 0xC1, 0x4D, 0xC5, 0x4E, 0xC9, 0x70, 0xCD, 0x92, 0xD1, 0xB4, 0xD5, 0xD5, 0xD9, 0x16, 0xE2, 0x58, 0xE6, 0x99, 0xEE, 0xDB, 0xF2, 0x1C, 0xFB, 0x5E, 0xFF, 0x00, 0x00, 0x08, 0xC9, 0x29, 0xD1, 0x6A, 0xD9, 0x8C, 0xE1, 0xCD, 0xE5, 0xEF, 0xED, 0x10, 0xF2, 0x51, 0xFA, 0x93, 0xFE, 0xB4, 0xFE, 0xF5, 0xFE, 0x16, 0xFF, 0x57, 0xFF, 0x78, 0xFF, 0xBA, 0xFF, 0x00, 0x00, 0xA0, 0x91, 0xC0, 0x95, 0xE0, 0x99, 0x00, 0xA2, 0x20, 0xA6, 0x40, 0xAE, 0x60, 0xB2, 0x80, 0xBA, 0xA0, 0xBE, 0xC0, 0xC6, 0x05, 0xCF, 0x4B, 0xDB, 0x70, 0xE3, 0xB6, 0xEF, 0xFC, 0xFB, 0x00, 0x00, 0x8B, 0xB5, 0x8D, 0xB1, 0xB0, 0xB1, 0xD2, 0xB1, 0xF5, 0xB1, 0x17, 0xB2, 0x3A, 0xB2, 0x5D, 0xB2, 0x9D, 0xBA, 0xDD, 0xC6, 0x1E, 0xD3, 0x3E, 0xDF, 0x7E, 0xEB, 0xBE, 0xF7, 0xFF, 0xFF, 0x00, 0x00, 0x8B, 0xB5, 0x8D, 0xB1, 0xAF, 0xB1, 0xD2, 0xB1, 0xF3, 0xAD, 0x34, 0xAA, 0x76, 0xA6, 0xB8, 0xA6, 0xD9, 0xB2, 0x1A, 0xBF, 0x3B, 0xCB, 0x7C, 0xDB, 0x9D, 0xE7, 0xDE, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x12, 0xB1, 0x30, 0xB5, 0x4E, 0xBD, 0x8D, 0xC5, 0xAB, 0xC9, 0xEA, 0xD1, 0x08, 0xDA, 0x47, 0xE2, 0x8A, 0xDE, 0xCE, 0xDA, 0x11, 0xD7, 0x35, 0xD7, 0x78, 0xD3, 0xBC, 0xCF, 0xFF, 0xCF, 0x00, 0x00, 0x08, 0xC9, 0x26, 0xC5, 0x65, 0xC1, 0xA4, 0xC1, 0xC3, 0xBD, 0x02, 0xBE, 0x41, 0xBA, 0x80, 0xBA, 0xA4, 0xBA, 0xE9, 0xBE, 0x2D, 0xC3, 0x52, 0xC3, 0x96, 0xC7, 0xDB, 0xCB, 0xFF, 0xCF, 0x00, 0x00, 0x29, 0xAD, 0x4A, 0xB1, 0x6B, 0xB5, 0x8C, 0xB9, 0xAD, 0xBD, 0xCE, 0xC5, 0xEF, 0xC9, 0x10, 0xCE, 0x31, 0xD2, 0x52, 0xD6, 0x73, 0xDE, 0x94, 0xE2, 0xB5, 0xE6, 0xD6, 0xEA, 0x18, 0xF3};
u8 D_15B000_8017AF7C[] ALIGNED4 = {0x00, 0x00, 0x00, 0x00, 0x04, 0xE9, 0x08, 0x05, 0x1E, 0xEF, 0x23, 0x08, 0x1E, 0xF2, 0x12, 0x04, 0xE0, 0xE1, 0x00, 0x00, 0xFC, 0xFA, 0x08, 0x05, 0x16, 0x00, 0x23, 0x08, 0x16, 0x03, 0x12, 0x04, 0x1D, 0x16, 0x0D, 0x0B, 0x0C, 0x07, 0x13, 0x0C, 0x0E, 0x0E, 0x12, 0x04, 0x01, 0xE7, 0x08, 0x05, 0x1B, 0xED, 0x23, 0x08, 0x1B, 0xF0, 0x12, 0x04, 0xF8, 0xE6, 0x08, 0x05, 0x18, 0xEF, 0x23, 0x08, 0x18, 0xF2, 0x12, 0x04, 0x17, 0x00, 0x1B, 0x0E, 0x12, 0x02, 0x15, 0x0B, 0x1D, 0xF2, 0x22, 0x04, 0x15, 0x03, 0x22, 0x04, 0x1C, 0x15, 0x0C, 0x0A, 0x1A, 0xF0, 0x22, 0x04, 0x17, 0xF2, 0x22, 0x04, 0x16, 0x03, 0x1A, 0x0A};
void* D_15B000_8017AFE0[] = {D_15B000_8017AEE0, D_15B000_8017AEE0, D_15B000_8017AEFC, D_15B000_8017AF18, D_15B000_8017AF34, D_15B000_8017AF4C, D_15B000_8017AF64};
u8 D_15B000_8017AEE0[] ALIGNED4 = {0x01, 0x02, 0x02, 0x04, 0x03, 0x04, 0x04, 0x04, 0x05, 0x04, 0x06, 0x06, 0x07, 0x06, 0x08, 0x06, 0x09, 0x06, 0x0A, 0x08, 0x0B, 0x08, 0x0C, 0x08, 0x0D, 0x08, 0xFF, 0xFF};
u8 D_15B000_8017AEFC[] ALIGNED4 = {0x0E, 0x0A, 0x0F, 0x0C, 0x10, 0x0C, 0x11, 0x0C, 0x12, 0x0C, 0x13, 0x0E, 0x14, 0x0E, 0x15, 0x0E, 0x16, 0x0E, 0x1F, 0x08, 0x20, 0x08, 0x21, 0x08, 0x22, 0x08, 0xFF, 0xFF};
u8 D_15B000_8017AF18[] ALIGNED4 = {0x0E, 0x0A, 0x17, 0x10, 0x18, 0x12, 0x19, 0x10, 0x1A, 0x12, 0x1B, 0x14, 0x1C, 0x14, 0x1D, 0x14, 0x1E, 0x14, 0x1F, 0x08, 0x20, 0x08, 0x21, 0x08, 0x22, 0x08, 0xFF, 0xFF};
u8 D_15B000_8017AF34[] ALIGNED4 = {0x23, 0x16, 0x24, 0x18, 0x25, 0x18, 0x26, 0x18, 0x27, 0x18, 0x28, 0x1A, 0x29, 0x1A, 0x2A, 0x1A, 0x2B, 0x1A, 0x2C, 0x08, 0xFF, 0xFF, 0x00, 0x00};
u8 D_15B000_8017AF4C[] ALIGNED4 = {0x2D, 0x1C, 0x2E, 0x1E, 0x2F, 0x1E, 0x30, 0x1E, 0x31, 0x1E, 0x32, 0x20, 0x33, 0x20, 0x34, 0x20, 0x35, 0x20, 0x3E, 0x08, 0xFF, 0xFF, 0x00, 0x00};
u8 D_15B000_8017AF64[] ALIGNED4 = {0x2D, 0x1C, 0x36, 0x22, 0x37, 0x22, 0x38, 0x22, 0x39, 0x22, 0x3A, 0x24, 0x3B, 0x24, 0x3C, 0x24, 0x3D, 0x24, 0x3E, 0x08, 0xFF, 0xFF, 0x00, 0x00};
WeaponAnimation D_15B000_8017AFFC[4] = {
    { D_15B000_8017AFE0, D_15B000_8017AF7C, 0, 0x60C, 0x41, 4, 0 },
    { D_15B000_8017AFE0, D_15B000_8017AF7C, 0, 0x60C, 0x41, 2, 0 },
    { D_15B000_8017AFE0, D_15B000_8017AF7C, 0, 0x62C, 0x41, 4, 0 },
    { D_15B000_8017AFE0, D_15B000_8017AF7C, 0, 0x62C, 0x41, 4, 0 },
};
s16 D_15B000_8017B03C[0x1C] = {
    0x18,
    -0xE,
    0x40,
    0,
    0x18,
    -0xE,
    0x40,
    0,
    0x14,
    2,
    0x40,
    0,
    0x12,
    0xE,
    0x30,
    0x22,
    0x18,
    -0x10,
    0x40,
    0,
    0x18,
    -0xE,
    0x40,
    0,
    0x18,
    4,
    0x30,
    0x22,
};
AnimationFrame D_15B000_8017B074[0x18] = {
    { 1, 1 },
    { 1, 2 },
    { 1, 3 },
    { 1, 4 },
    { 1, 5 },
    { 1, 6 },
    { 1, 7 },
    { 1, 8 },
    { 0, 0 },
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
AnimationFrame D_15B000_8017B0D4[0xE] = {
    { 3, 1 },
    { 3, 2 },
    { 3, 3 },
    { 3, 4 },
    { 3, 5 },
    { 3, 6 },
    { 3, 7 },
    { 3, 8 },
    { 3, 9 },
    { 3, 0xA },
    { 3, 0xB },
    { 3, 0xC },
    { 3, 0xD },
    { 0xFFFF, 0 },
};
AnimationFrame D_15B000_8017B10C[0xE] = {
    { 2, 1 },
    { 2, 2 },
    { 2, 3 },
    { 2, 4 },
    { 2, 5 },
    { 2, 6 },
    { 2, 7 },
    { 2, 8 },
    { 2, 9 },
    { 2, 0xA },
    { 2, 0xB },
    { 2, 0xC },
    { 2, 0xD },
    { 0xFFFF, 0 },
};
s32 D_15B000_8017C8CC;
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[1] = { D_15B000_8017AC00 };
// end pc_import_data.py
// clang-format on
#endif
