// SPDX-License-Identifier: AGPL-3.0-or-later
// Weapon ID #11. Used by weapons:
// AxeLord shield, Unknown#206
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "shared.h"
#include "gen/w_011_1.h"
#include "gen/w_011_2.h"
#define g_Animset w_011_1
#define g_Animset2 w_011_2
#include "sfx.h"

#ifdef VERSION_PC
extern s8 D_51000_8017AB4C[];
#else
extern s8 D_51000_8017AB4C;
#endif
#ifdef VERSION_PC
extern AnimationFrame* D_51000_8017AB94[];
#else
extern AnimationFrame* D_51000_8017AB94;
#endif
extern AnimationFrame D_51000_8017ABCC[];
extern AnimationFrame D_51000_8017ABE8[];

extern s32 D_51000_8017ABF4[];

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
        g_api.PlayAnimation(&D_51000_8017AB4C, &D_51000_8017AB94);
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

static int GetWeaponId(void) { return 11; }

void EntityWeaponShieldSpell(Entity* self) {
    Primitive* prim;
    Primitive* prevPrim;
    Primitive* nextPrim;
    Entity* unusedEnt;
    s32 i;
    s16 baseCoord;
    s16 selfY;
    s16 angle;
    s16 back;
    s16 front;
    s16 top;
    s16 bottom;

    switch (self->step) {
    case 0:
        for (i = 0; i < 16; i++) {
            D_51000_8017ABF4[i] = (i << 9) * 2;
        }
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 10);
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
        prim->u1 = prim->u3 = 0x1F;
        prim->v0 = prim->v1 = self->ext.shield.unk7D + 0x30;
        prim->v2 = prim->v3 = self->ext.shield.unk7D + 0x6F;
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3 =
                0x80;
        prim->priority = 0x1BC;
        prim->drawMode = DRAW_HIDE;
        for (i = 0; i < 8; i++) {
            prevPrim = prim;
            prim = prim->next;
            nextPrim = prim->next;
            *prim = *prevPrim;
            prim->next = nextPrim;
            prim->v0 = prim->v1 = self->ext.shield.unk7D + 0x30 + i * 64 / 8;
            prim->v2 = prim->v3 =
                self->ext.shield.unk7D + 0x30 + (i + 1) * 64 / 8 - 1;
            prim->clut = PAL_FILL_WHITE;
            prim->priority = 0x1BE;
            prim->drawMode |=
                (DRAW_TPAGE2 + DRAW_TPAGE + DRAW_COLORS + DRAW_TRANSP);
        }
        self->ext.shield.unk92 = 0x120;
        self->ext.shield.unk94 = 0;
        self->ext.shield.unk84 = 0x10;
        self->ext.shield.unk86 = 0;
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
        self->velocityY -= FIX(0.15625);
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
            prim = prim->next;
            for (i = 0; i < 8; i++) {
                prim->drawMode &= ~DRAW_HIDE;
                prim = prim->next;
            }
            self->ext.shield.unk80 = 0x20;
            g_api.PlaySfx(SFX_MAGIC_WEAPON_APPEAR_A);
            self->step++;
        }
        break;
    case 4:
        self->ext.shield.unk82 += 8;
        prim = &g_PrimBuf[self->primIndex];
        if (prim->b3 > 8) {
            prim->b3 -= 4;
        }
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3;
        self->scaleX += 0x10;
        if (self->scaleX >= 0x100) {
            self->scaleX = 0x100;
        }
        self->ext.shield.unk86 = self->scaleX * 32 / 256;
        self->ext.shield.unk92 -= 12;
        if (self->ext.shield.unk92 <= 0) {
            self->ext.shield.unk92 = 0;
            prim = &g_PrimBuf[self->primIndex];
            prim = prim->next;
            prim->drawMode &= ~DRAW_HIDE;
            self->step++;
        }
        break;
    case 5:
        self->ext.shield.unk90 -= 0x10;
        if (self->ext.shield.unk90 < 0) {
            prim = &g_PrimBuf[self->primIndex];
            prim->drawMode |= DRAW_HIDE;
            self->ext.shield.unk90 = 0;
            prim = &g_PrimBuf[self->primIndex];
            prim = prim->next;
            prim->drawMode |= DRAW_HIDE;
            // Blueprint 88 is weapon function 9, func_ptr_80170024
            unusedEnt = g_api.CreateEntFactoryFromEntity(
                self, ((g_HandId + 1) << 0xE) + 88, 0);
            g_unkGraphicsStruct.unk28 = 0;
            self->step++;
        }
        break;
    case 6:
        self->ext.shield.unk90 += 8;
        if (self->ext.shield.unk90 >= 0x80) {
            self->ext.shield.unk80 = 4;
            self->step++;
        }
        break;
    case 7:
        if (--self->ext.shield.unk80 == 0) {
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
    for (i = 0; i < 8; i++) {
        angle = D_51000_8017ABF4[i];
        baseCoord = (rsin(angle) >> 5) * self->ext.shield.unk92 / 256;
        prim->x0 = back + baseCoord;
        prim->x1 = (front + baseCoord) - 1;
        angle = D_51000_8017ABF4[(i + 1) % 8];
        baseCoord = (rsin(angle) >> 5) * self->ext.shield.unk92 / 256;
        prim->x2 = back + baseCoord;
        prim->x3 = (front + baseCoord) - 1;
        prim->y0 = prim->y1 = top + (((bottom - top) * i) / 8);
        prim->y2 = prim->y3 = top + (((bottom - top) * (i + 1)) / 8);
        angle = D_51000_8017ABF4[(i + 6) % 8];
        baseCoord = (rsin(angle) >> 5) * self->ext.shield.unk94 / 256;
        prim->y0 += baseCoord;
        prim->y2 += baseCoord;
        angle = D_51000_8017ABF4[(i + 7) % 8];
        baseCoord = (rsin(angle) >> 5) * self->ext.shield.unk94 / 256;
        prim->y1 += baseCoord;
        prim->y3 += baseCoord;
        angle = D_51000_8017ABF4[i];
        prim->g0 = prim->b0 = prim->g1 = prim->b1 = prim->b3 =
            ((rsin(angle) + 0x1000) >> 6) * self->ext.shield.unk90 / 256;
        prim->r0 = prim->r1 = 0;
        angle = D_51000_8017ABF4[(i + 1) % 8];
        // @bug: Writing to prim->b3 twice
        prim->g2 = prim->b2 = prim->g3 = prim->b3 = prim->b3 =
            ((rsin(angle) + 0x1000) >> 6) * self->ext.shield.unk90 / 256;
        prim->r2 = prim->r3 = 0;
        D_51000_8017ABF4[i] += self->ext.shield.unk8A;
        prim = prim->next;
    }
}

static void func_ptr_80170024(Entity* self) {
    Collider collider;
    Entity* unusedEnt;

    s16 xOffset;
    s16 yOffset;
    s16 xVar;
    s16 yVar;

    switch (self->step) {
    case 0:
        if (self->ext.shield.parent->entityId == 0) {
            DestroyEntity(self);
            return;
        }
        self->animSet = self->ext.shield.parent->animSet;
        self->unk5A = self->ext.shield.parent->unk5A;
        self->zPriority = PLAYER.zPriority + 2;
        self->palette = self->ext.shield.parent->palette;
        self->facingLeft = self->ext.shield.parent->facingLeft;
        if (self->params & 0x7F00) {
            self->flags = FLAG_POS_CAMERA_LOCKED;
            self->animCurFrame = self->ext.shield.parent->animCurFrame;
            self->drawFlags = ENTITY_OPACITY;
            self->blendMode = BLEND_TRANSP;
            self->opacity = 0x80;
            self->poseTimer = 0x18;
            self->zPriority -= 2;
            self->step = 4;
        } else {
            self->anim = D_51000_8017ABCC;
            self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_100000;
            self->ext.shield.equipId =
                self->ext.shield.parent->ext.shield.equipId;
            SetWeaponProperties(self, 0);
            self->hitboxWidth = 8;
            self->hitboxHeight = 20;
            SetSpeedX(FIX(1));
            self->step++;
        }
        break;
    case 1:
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityY += FIX(1.0 / 8);
        if (self->velocityY > FIX(6)) {
            self->velocityY = FIX(6);
        }
        xOffset = 0;
        yOffset = 25;
        xVar = self->posX.i.hi + xOffset;
        yVar = self->posY.i.hi + yOffset;
        g_api.CheckCollision(xVar, yVar, &collider, 0);
        if (collider.effects & EFFECT_SOLID) {
            self->posY.i.hi += collider.unk18;
            g_api.PlaySfx(SFX_STOMP_SOFT_A);
            self->step = 2;
            break;
        }
        if (!(collider.effects & EFFECT_UNK_8000)) {
            xOffset = 8;
            if (self->velocityX < 0) {
                xOffset = -xOffset;
            }
            yOffset = 22;
            xVar = self->posX.i.hi + xOffset;
            yVar = self->posY.i.hi + yOffset;
            g_api.CheckCollision(xVar, yVar, &collider, 0);
            if ((collider.effects & EFFECT_SOLID) &&
                !(collider.effects & EFFECT_UNK_8000)) {
                goto make_other_entity;
            }
            xOffset = 8;
            if (self->velocityX < 0) {
                xOffset = -xOffset;
            }
            yOffset = 8;
            xVar = self->posX.i.hi + xOffset;
            yVar = self->posY.i.hi + yOffset;
            g_api.CheckCollision(xVar, yVar, &collider, 0);
            if ((collider.effects & EFFECT_SOLID) &&
                !(collider.effects & EFFECT_UNK_8000)) {
                goto make_other_entity;
            }
        }
        xOffset = 8;
        if (self->velocityX < 0) {
            xOffset = -xOffset;
        }
        yOffset = -8;
        xVar = self->posX.i.hi + xOffset;
        yVar = self->posY.i.hi + yOffset;
        g_api.CheckCollision(xVar, yVar, &collider, 0);
        if ((collider.effects & EFFECT_SOLID) || (self->hitFlags)) {
            goto make_other_entity;
        }
        if (g_Timer % 4 == 0) {
            // Blueprint 90 is weapon entity 9, func_ptr_80170024, so ourself.
            unusedEnt = g_api.CreateEntFactoryFromEntity(
                self, ((g_HandId + 1) << 0xE) + FACTORY(90, 1), 0);
        }
        break;
    case 2:
        SetSpeedX(FIX(2.5));
        if ((self->ext.timer.t % 10) == 0) {
            g_api.PlaySfx(SFX_STOMP_SOFT_A);
        }
        xOffset = 0;
        yOffset = 25;
        xVar = self->posX.i.hi + xOffset;
        yVar = self->posY.i.hi + yOffset;
        g_api.CheckCollision(xVar, yVar, &collider, 0);
        if (collider.effects & EFFECT_SOLID) {
            self->posY.i.hi += collider.unk18;
        } else {
            self->step = 1;
            self->velocityY = 0;
        }
        if (!(collider.effects & EFFECT_UNK_8000)) {
            xOffset = 8;
            if (self->velocityX < 0) {
                xOffset = -xOffset;
            }
            yOffset = 22;
            xVar = self->posX.i.hi + xOffset;
            yVar = self->posY.i.hi + yOffset;
            g_api.CheckCollision(xVar, yVar, &collider, 0);
            if ((collider.effects & EFFECT_SOLID) &&
                !(collider.effects & EFFECT_UNK_8000)) {
                goto make_other_entity;
            }
            xOffset = 8;
            if (self->velocityX < 0) {
                xOffset = -xOffset;
            }
            yOffset = 8;
            xVar = self->posX.i.hi + xOffset;
            yVar = self->posY.i.hi + yOffset;
            g_api.CheckCollision(xVar, yVar, &collider, 0);
            if ((collider.effects & EFFECT_SOLID) &&
                !(collider.effects & EFFECT_UNK_8000)) {
                goto make_other_entity;
            }
        }
        if ((((collider.effects & EFFECT_UNK_C000) == EFFECT_UNK_C000) &&
             (self->facingLeft)) ||
            (((collider.effects & EFFECT_UNK_C000) == EFFECT_UNK_8000) &&
             (self->facingLeft == 0))) {
            if (!(collider.effects &
                  (EFFECT_SOLID_FROM_ABOVE | EFFECT_SOLID_FROM_BELOW))) {
                SetSpeedX(FIX(1.25));
            }
            // @bug: These next two if-statements are never true because the bit
            // flags don't match up. We're testing against 0x40 and 0x80 flags,
            // and seeing if they match 0x100 or 0x200.
            if ((collider.effects &
                 (EFFECT_SOLID_FROM_ABOVE | EFFECT_SOLID_FROM_BELOW)) ==
                EFFECT_UNK_0100) {
                SetSpeedX(FIX(2));
            }
            if ((collider.effects &
                 (EFFECT_SOLID_FROM_ABOVE | EFFECT_SOLID_FROM_BELOW)) ==
                EFFECT_UNK_0200) {
                SetSpeedX(FIX(2.25));
            }
        }
        xOffset = 8;
        if (self->velocityX < 0) {
            xOffset = -xOffset;
        }
        yOffset = -8;
        xVar = self->posX.i.hi + xOffset;
        yVar = self->posY.i.hi + yOffset;
        g_api.CheckCollision(xVar, yVar, &collider, 0);
        if (!(collider.effects & EFFECT_SOLID) && (!self->hitFlags)) {
            if (self->ext.timer.t % 4 == 0) {
                // Blueprint 90 is weapon entity 9, func_ptr_80170024, so
                // ourself.
                unusedEnt = g_api.CreateEntFactoryFromEntity(
                    self, ((g_HandId + 1) << 0xE) + FACTORY(90, 1), 0);
            }
            self->posX.val += self->velocityX;
        } else {
            goto make_other_entity;
        }
        break;
    case 3:
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    case 4:
        self->opacity -= 2;
        if (--self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    // Not a switch case, but `break` skips this, so we put it in the switch.
    make_other_entity:
        g_api.PlaySfx(SFX_CANDLE_HIT);
        self->step = 3;
        self->anim = D_51000_8017ABE8;
        self->poseTimer = self->pose = 0;
        // Blueprint 90 is weapon entity 9, func_ptr_80170024, so ourself.
        unusedEnt = g_api.CreateEntFactoryFromEntity(
            self, ((g_HandId + 1) << 0xE) + FACTORY(90, 1), 0);
    }
    self->ext.timer.t++;
}

static void func_ptr_80170028(Entity* self) {}

static void WeaponUnused2C(void) {}

static void WeaponUnused30(void) {}

static void WeaponUnused34(void) {}

static void WeaponUnused38(void) {}

static void WeaponUnused3C(void) {}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_51000_8017A958[];
extern u8 D_51000_8017A998[];
extern u8 D_51000_8017A9A0[];
extern u8 D_51000_8017A9A8[];
extern u8 D_51000_8017A9D4[];
extern u8 D_51000_8017AA18[];
extern u8 D_51000_8017AA50[];
extern u8 D_51000_8017AA70[];
extern u8 D_51000_8017AAB4[];
extern u8 D_51000_8017AAC4[];
extern u8 D_51000_8017AAD4[];
extern u8 D_51000_8017AAE4[];
extern u8 D_51000_8017AAF4[];
extern u8 D_51000_8017AB00[];
extern u8 D_51000_8017AB0C[];
u8 D_51000_8017A958[] ALIGNED4 = {0x00, 0x00, 0x22, 0x88, 0xE0, 0x83, 0xE5, 0xEC, 0x8A, 0xF6, 0x48, 0x80, 0x4C, 0x88, 0xB5, 0x88, 0x39, 0x8D, 0x1C, 0x82, 0xA4, 0x9C, 0x29, 0xAD, 0xAD, 0xBD, 0x31, 0xD2, 0xB5, 0xE6, 0x7A, 0xEB, 0x00, 0x00, 0x5A, 0xEB, 0xB1, 0xEA, 0x29, 0xDE, 0xE4, 0xE4, 0x84, 0xA8, 0x00, 0x8C, 0xE0, 0x83, 0x2D, 0x80, 0xDD, 0x80, 0x5F, 0xAA, 0xD1, 0x84, 0xC6, 0x98, 0xAD, 0xB5, 0x31, 0xC6, 0x21, 0x84};
u8 D_51000_8017A998[] ALIGNED4 = {0x02, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017A9A0[] ALIGNED4 = {0x02, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017A9A8[] ALIGNED4 = {0x02, 0x00, 0x03, 0x06, 0x02, 0x00, 0x04, 0x06, 0x02, 0x00, 0x05, 0x06, 0x02, 0x00, 0x06, 0x06, 0x02, 0x00, 0x07, 0x06, 0x02, 0x00, 0x08, 0x06, 0x02, 0x00, 0x09, 0x08, 0x02, 0x00, 0x0A, 0x02, 0x02, 0x00, 0x0B, 0x02, 0x02, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017A9D4[] ALIGNED4 = {0x03, 0x00, 0x0D, 0x02, 0x03, 0x00, 0x0E, 0x0E, 0x03, 0x00, 0x0F, 0x10, 0x03, 0x00, 0x10, 0x12, 0x03, 0x00, 0x11, 0x14, 0x03, 0x00, 0x12, 0x16, 0x03, 0x00, 0x13, 0x18, 0x03, 0x00, 0x14, 0x1A, 0x03, 0x00, 0x15, 0x1C, 0x03, 0x00, 0x16, 0x1A, 0x03, 0x00, 0x17, 0x18, 0x03, 0x00, 0x18, 0x16, 0x03, 0x00, 0x19, 0x14, 0x03, 0x00, 0x1A, 0x12, 0x03, 0x00, 0x1B, 0x10, 0x03, 0x00, 0x1C, 0x0E, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017AA18[] ALIGNED4 = {0x01, 0x00, 0x24, 0x02, 0x01, 0x00, 0x25, 0x02, 0x01, 0x00, 0x26, 0x02, 0x01, 0x00, 0x27, 0x02, 0x01, 0x00, 0x28, 0x02, 0x01, 0x00, 0x29, 0x02, 0x01, 0x00, 0x2A, 0x02, 0x01, 0x00, 0x2B, 0x02, 0x01, 0x00, 0x2C, 0x02, 0x01, 0x00, 0x2D, 0x02, 0x01, 0x00, 0x2E, 0x02, 0x01, 0x00, 0x2F, 0x02, 0x01, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017AA50[] ALIGNED4 = {0x01, 0x00, 0x2A, 0x02, 0x01, 0x00, 0x2B, 0x02, 0x01, 0x00, 0x2C, 0x02, 0x01, 0x00, 0x2D, 0x02, 0x01, 0x00, 0x2E, 0x02, 0x01, 0x00, 0x2F, 0x02, 0x01, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017AA70[] ALIGNED4 = {0x02, 0x00, 0x21, 0x02, 0x02, 0x00, 0x22, 0x02, 0x02, 0x00, 0x23, 0x02, 0x02, 0x00, 0x24, 0x02, 0x02, 0x00, 0x25, 0x02, 0x02, 0x00, 0x26, 0x02, 0x02, 0x00, 0x27, 0x02, 0x02, 0x00, 0x28, 0x02, 0x02, 0x00, 0x29, 0x02, 0x02, 0x00, 0x2A, 0x02, 0x02, 0x00, 0x2B, 0x02, 0x02, 0x00, 0x2C, 0x02, 0x02, 0x00, 0x2D, 0x02, 0x02, 0x00, 0x2E, 0x02, 0x02, 0x00, 0x2F, 0x02, 0x02, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017AAB4[] ALIGNED4 = {0x02, 0x00, 0x31, 0x02, 0x02, 0x00, 0x32, 0x02, 0x02, 0x00, 0x33, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_51000_8017AAC4[] ALIGNED4 = {0x02, 0x00, 0x34, 0x04, 0x02, 0x00, 0x35, 0x04, 0x02, 0x00, 0x36, 0x04, 0xFF, 0xFF, 0x00, 0x00};
u8 D_51000_8017AAD4[] ALIGNED4 = {0x02, 0x00, 0x33, 0x02, 0x02, 0x00, 0x32, 0x02, 0x02, 0x00, 0x31, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_51000_8017AAE4[] ALIGNED4 = {0x02, 0x00, 0x36, 0x04, 0x02, 0x00, 0x35, 0x04, 0x02, 0x00, 0x34, 0x04, 0xFF, 0xFF, 0x00, 0x00};
u8 D_51000_8017AAF4[] ALIGNED4 = {0x04, 0x00, 0x1D, 0x02, 0x04, 0x00, 0x1E, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017AB00[] ALIGNED4 = {0x02, 0x00, 0x1F, 0x0C, 0x02, 0x00, 0x20, 0x0C, 0x00, 0x00, 0x00, 0x00};
u8 D_51000_8017AB0C[] ALIGNED4 = {0x03, 0x00, 0x37, 0x22, 0x03, 0x00, 0x38, 0x22, 0x03, 0x00, 0x38, 0x22, 0x03, 0x00, 0x38, 0x20, 0x03, 0x00, 0x38, 0x1E, 0x03, 0x00, 0x39, 0x02, 0x03, 0x00, 0x3A, 0x02, 0x03, 0x00, 0x3B, 0x02, 0x03, 0x00, 0x3C, 0x02, 0x03, 0x00, 0x3D, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
s8 D_51000_8017AB4C[0x48] = {
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
AnimationFrame* D_51000_8017AB94[0xE] = {
    &D_51000_8017A998,
    &D_51000_8017A9A0,
    D_51000_8017AB00,
    D_51000_8017AB0C,
    D_51000_8017AAF4,
    D_51000_8017AA70,
    D_51000_8017AA50,
    D_51000_8017AA18,
    D_51000_8017A9D4,
    D_51000_8017A9A8,
    D_51000_8017AAB4,
    D_51000_8017AAC4,
    D_51000_8017AAD4,
    D_51000_8017AAE4,
};
AnimationFrame D_51000_8017ABCC[7] = {
    { 3, 0xC3F },
    { 2, 0xC40 },
    { 2, 0xC41 },
    { 3, 0xC42 },
    { 2, 0xC41 },
    { 2, 0xC40 },
    { 0, 0 },
};
AnimationFrame D_51000_8017ABE8[3] = { { 1, 0xE43 }, { 0xC, 0x1044 }, { 0xFFFF, 0 } };
s32 D_51000_8017ABF4[0x10] = {
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
u16* g_WeaponCluts[2] = { D_51000_8017A958, D_51000_8017A958 };
// end pc_import_data.py
// clang-format on
#endif
