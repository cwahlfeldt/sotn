// SPDX-License-Identifier: AGPL-3.0-or-later
//! O=1
// Weapon ID #29. Used by weapons:
// Fire shield, Unknown#213
#include "weapon_private.h"
extern u16* g_WeaponCluts[];
extern s32 g_HandId;
#include "shared.h"
#include "gen/w_029_1.h"
#include "gen/w_029_2.h"
#define g_Animset w_029_1
#define g_Animset2 w_029_2
#include "items.h"
#include "sfx.h"

#ifdef VERSION_PC
extern s32 D_CF000_8017AC78[];
#else
extern s32 D_CF000_8017AC78;
#endif
#ifdef VERSION_PC
extern AnimationFrame* D_CF000_8017ACC0[];
#else
extern s32 D_CF000_8017ACC0;
#endif
extern u16 D_CF000_8017ACF8[];
extern u16 D_CF000_8017AD04[];
extern u16 D_CF000_8017AD10[];
extern AnimationFrame D_CF000_8017AD24[];
extern FireShieldDragon D_CF000_8017C9A0[][20];

static void EntityWeaponAttack(Entity* self) {
    s32 anim;
    s32 handButton;
    s32 unkAC_offset;

    anim = 0;
    unkAC_offset = 0;
    if (g_HandId != 0) {
        handButton = PAD_CIRCLE;
    } else {
        handButton = PAD_SQUARE;
    }
    if (!(handButton & g_Player.padPressed) && (self->step < 3)) {
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
        self->ext.weapon.lifetime = 128;
        self->flags = FLAG_POS_CAMERA_LOCKED;
        self->animCurFrame = 0x3E;
    }
    if ((PLAYER.step == 2) && (PLAYER.step_s != 2)) {
        unkAC_offset++;
    }
    switch (self->step) {
    case 0:
        SetSpriteBank1(g_Animset);
        if (self->params & ANIMSET_OVL_FLAG) {
            self->animSet = ANIMSET_OVL(18);
            self->ext.weapon.unk80 = 0x128;
            self->unk5A = 0x66;
        } else {
            self->animSet = ANIMSET_OVL(16);
            self->ext.weapon.unk80 = 0x110;
            self->unk5A = 0x64;
        }
        self->flags = FLAG_POS_PLAYER_LOCKED + FLAG_UNK_20000;
        self->zPriority = PLAYER.zPriority - 2;
        g_Player.unk48 = 1;
        SetWeaponProperties(self, 0);
        self->step++;
    case 1:
        self->ext.weapon.anim = unkAC_offset + 10;
        if (self->poseTimer < 0) {
            self->step++;
        }
        break;
    case 2:
        switch (PLAYER.ext.player.anim) {
        case 9:
        case 10:
        case 11:
            anim++;
        case 7:
            anim++;
        case 8:
            anim++;
        case 12:
            anim++;
        case 13:
            anim++;
        case 24:
        case 25:
            anim++;
        case 14:
        case 15:
            anim++;
        case 26:
            anim += 2;
            self->pose = PLAYER.pose;
            break;
        default:
            self->pose = 0;
            anim = anim + unkAC_offset;
            break;
        }
        self->ext.weapon.anim = anim;
        self->poseTimer = 2;
        break;
    case 3:
        g_Player.unk48 = 0;
        self->ext.weapon.anim = unkAC_offset + 12;
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
        self->rotate = self->rotate + 0x80;
        if (--self->ext.weapon.lifetime < 16) {
            self->drawFlags |= ENTITY_BLINK;
        }
        if (--self->ext.weapon.lifetime == 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    if (self->step != 4) {
        g_api.PlayAnimation(&D_CF000_8017AC78, &D_CF000_8017ACC0);
        if (g_GameTimer % 5 == 0) {
            g_api.CreateEntFactoryFromEntity(self, WFACTORY(0x38, 0), 0);
        }
    }
    self->palette =
        self->ext.weapon.unk80 + D_CF000_8017ACF8[g_GameTimer / 2 % 5];
    self->drawFlags = PLAYER.drawFlags;
    self->scaleY = PLAYER.scaleY;
    self->rotPivotY = PLAYER.rotPivotY;
    return;
}

s32 func_ptr_80170004(Entity* self) {
    if (self->ext.weapon.parent->ext.weapon.equipId != ITEM_FIRE_SHIELD) {
        DestroyEntity(self);
        return;
    }
    if (self->step == 0) {
        self->animSet = self->ext.weapon.parent->animSet;
        self->zPriority = self->ext.weapon.parent->zPriority + 4;
        self->unk5A = self->ext.weapon.parent->unk5A;
        self->ext.weapon.unk80 = self->ext.weapon.parent->ext.weapon.unk80;
        self->animCurFrame = self->ext.weapon.parent->animCurFrame;
        self->flags = FLAG_POS_CAMERA_LOCKED;
        self->drawFlags = ENTITY_OPACITY | ENTITY_MASK_R;
        self->opacity = 0x80;
        self->ext.weapon.unk7E = 0x14;
        self->step++;
    } else {
        if (self->opacity >= 7) {
            self->opacity -= 7;
        }

        if (--self->ext.weapon.unk7E == 0) {
            DestroyEntity(self);
            return;
        }
    }
    self->palette =
        self->ext.weapon.unk80 + D_CF000_8017AD04[g_GameTimer / 2 % 5];
}

static void func_ptr_80170008(Entity* self) {}

static void func_ptr_8017000C(Entity* self) {}

static s32 func_ptr_80170010(Entity* self) {}

static s32 func_ptr_80170014(Entity* self) {}

static int GetWeaponId(void) { return 29; }

static void EntityWeaponShieldSpell(Entity* self) {
    Primitive* prim;
    s32 var_v0;
    u16 selfX;
    u16 selfY;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        SetSpriteBank1(g_Animset);
        if (g_HandId != 0) {
            self->animSet = ANIMSET_OVL(18);
            self->ext.weapon.childPalette = PAL_UNK_128;
            self->unk5A = 0x66;
            self->ext.shield.unk7C = 0;
            self->ext.shield.unk7D = 0x80;
        } else {
            self->animSet = ANIMSET_OVL(16);
            self->ext.weapon.childPalette = PAL_UNK_110;
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
        self->scaleY = 0;
        self->scaleX = 0;
        prim = &g_PrimBuf[self->primIndex];
        prim->clut = PAL_UNK_19F;
        prim->tpage = 0x19;
        prim->u0 = prim->u2 = 0x80;
        prim->u1 = prim->u3 = 0xCF;
        prim->v0 = prim->v1 = self->ext.shield.unk7C;
        prim->v2 = prim->v3 = self->ext.shield.unk7C + 0x4F;
        self->ext.weapon.unk82 = 0;
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3 =
                0x80;
        prim->priority = self->zPriority - 4;
        prim->drawMode =
            DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
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
        self->ext.weapon.unk82 = self->scaleX * 40 / 256;

        if (self->scaleX >= 0x100) {
            self->ext.weapon.unk82 = 0x28;
            self->scaleX = 0x100;
            self->scaleY = 0x100;
            self->ext.weapon.unk80 = 8;
            self->step++;
        }
        self->palette =
            self->ext.weapon.childPalette + D_CF000_8017ACF8[g_Timer / 2 % 5];
        break;
    case 2:
        if (--self->ext.weapon.unk80 == 0) {
            self->step++;
        }
        self->palette =
            self->ext.weapon.childPalette + D_CF000_8017ACF8[g_Timer / 2 % 5];
        break;
    case 3:
        self->palette = self->ext.weapon.childPalette +
                        D_CF000_8017ACF8[g_GameTimer / 2 % 5];
        self->scaleX -= 0x10;
        self->scaleY = self->scaleX;
        if (self->scaleX <= 0) {
            self->scaleX = 0;
            self->anim = D_CF000_8017AD24;
            self->pose = 0;
            self->poseTimer = 0;
            self->flags |= FLAG_UNK_100000;
            self->ext.weapon.unk80 = 0x20;
            self->palette = self->ext.weapon.childPalette;
            self->zPriority = 0x1B6;
            g_api.PlaySfx(SFX_TRANSFORM);
            self->step++;
        }
        break;
    case 4:
        self->ext.weapon.unk82 += 8;
        self->scaleX += 0x10;
        if (self->scaleX >= 0x100) {
            self->scaleX = 0x100;
        }
        self->scaleY = self->scaleX;
        prim = &g_PrimBuf[self->primIndex];
        if (prim->b3 >= 9) {
            prim->b3 -= 4;
        }
        prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
            prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3;
        if (--self->ext.weapon.unk80 == 0) {
            prim->drawMode |= DRAW_HIDE;
            g_api.PlaySfx(SFX_FIREBALL_SHOT_A);
            // TODO: FACTORY()
            g_api.CreateEntFactoryFromEntity(
                self, ((g_HandId + 1) << 14) + 102, 0);
            self->ext.weapon.unk80 = 0x18;
            g_unkGraphicsStruct.unk28 = 0;
            self->step++;
        }
        break;
    case 5:
        if (--self->ext.weapon.unk80 == 0) {
            self->step++;
        }
        break;
    case 6:
        self->scaleX -= 0x10;
        self->scaleY = self->scaleX;
        if (self->scaleX <= 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    selfX = self->posX.i.hi;
    selfY = self->posY.i.hi;
    prim = &g_PrimBuf[self->primIndex];
    prim->x0 = prim->x2 = selfX - self->ext.weapon.unk82;
    prim->x1 = prim->x3 = (selfX + self->ext.weapon.unk82) - 1;
    prim->y0 = prim->y1 = selfY - self->ext.weapon.unk82;
    prim->y2 = prim->y3 = (selfY + self->ext.weapon.unk82) - 1;
    return;
}

static void func_ptr_80170024(Entity* self) {
    // All these variables are kind of guessing names, behavior of this
    // function is a little unclear.
    Primitive* firstPrim;
    s16 prev_x2;
    s16 prev_y2;
    s16 dragonNumber;
    Primitive* prim;
    s16 angle1;
    s16 temp_s1;
    s16 angle2;
    s32 centerX;
    s32 nextX;
    s32 centerY;
    s32 nextY;
    s16 prev_x3;
    s16 prev_x4;
    s32 unkCosine;
    s32 unkSine;
    s16 size;
    s32 i;
    s16 facingLeft;
    s16 nextX_copy;
    s16 nextY_copy;

    FireShieldDragon* var_s7;

    dragonNumber = (u8)self->params;
    facingLeft = self->facingLeft;
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x13);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        if (g_HandId != 0) {
            self->ext.weapon.childPalette = PAL_UNK_128;
            self->ext.shield.unk7D = 0x80;
        } else {
            self->ext.weapon.childPalette = PAL_UNK_110;
            self->ext.shield.unk7D = 0;
        }
        self->flags |= FLAG_KEEP_ALIVE_OFFCAMERA | FLAG_HAS_PRIMS;
        self->ext.shield.unk9C = 0x40;

        for (i = 0, prim = &g_PrimBuf[self->primIndex]; prim != NULL; i++,
            prim = prim->next) {
            prim->tpage = 0x19;
            prim->clut = self->ext.weapon.childPalette;
            prim->priority = 0x1BC;
            prim->drawMode = DRAW_DEFAULT;
            prim->u0 = prim->u2 = 0x50;
            prim->v0 = prim->v1 = self->ext.shield.unk7D + 8;
            prim->u1 = prim->u3 = 0x7F;
            prim->v2 = prim->v3 = self->ext.shield.unk7D + 0x2F;
            if (i == 0) { // for the first prim. Maybe this is the dragon head
                          // or something?
                prim->u0 = prim->u1 = 0;
                prim->u2 = prim->u3 = 0x57;
                prim->v0 = prim->v2 = self->ext.shield.unk7D + 0x30;
                prim->v1 = prim->v3 = self->ext.shield.unk7D + 0x6F;
            }
            prim->priority = 0x1BA;
        }
        var_s7 = &D_CF000_8017C9A0[dragonNumber][0];
        for (i = 0; i < 20; i++) {
            var_s7->unk8 = 0;
            if (facingLeft) {
                var_s7->unk8 = 0x800;
            }
            var_s7->unk8 += (dragonNumber * 0x540);
            var_s7->unkA = 0xC00;
            var_s7->posX.val = self->posX.val;
            var_s7->posY.val = self->posY.val;
            var_s7->unkC = 0x70;
            var_s7->unk10 = 20;
            if (i == 19) {
                var_s7->unk10 = 0;
            }
            if (i == 18) {
                var_s7->unk10 = 10;
            }
            var_s7->spawnDelay = i * 2 + 1;
            var_s7->unkE = 0xC00;
            var_s7->unk14 = 256;
            var_s7->state = 0;
            var_s7++;
        }
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        self->hitboxHeight = 20;
        self->hitboxWidth = 20;
        self->step += 1;
        break;
    case 1:
        var_s7 = &D_CF000_8017C9A0[dragonNumber][0];
        for (i = 0; i < 20; var_s7++, i++) {
            if (var_s7->state != 0) {
                var_s7->unkE += 0x80;
                var_s7->unk14 -= 4;
                temp_s1 = rsin(var_s7->unkE) / 2 + 0x1000;
                if (var_s7->unk14 < 0) {
                    var_s7->unk14 = 0;
                }
                unkCosine =
                    ((rcos(var_s7->unk8) * var_s7->unkC) >> 0xC) * temp_s1;
                unkSine =
                    -((rsin(var_s7->unk8) * var_s7->unkC) >> 0xC) * temp_s1;
                var_s7->posX.val += unkCosine;
                var_s7->posY.val += unkSine;
                if (!facingLeft) {
                    var_s7->unk8 += ((s16)var_s7->unkA >> 4);
                } else {
                    var_s7->unk8 -= ((s16)var_s7->unkA >> 4);
                }
            }
            switch (var_s7->state) {
            case 0:
                if (--var_s7->spawnDelay == 0) {
                    var_s7->state++;
                }
                break;
            case 1:
                var_s7->unkA -= 40;
                if (var_s7->unkE > 0x1400) {
                    var_s7->state++;
                }
                break;
            case 2:
                var_s7->unkA -= 80;
                var_s7->unkC -= 4;
                if (var_s7->unkC < 0x40) {
                    if (!(self->params & 0x7F)) {
                        g_api.PlaySfx(SFX_FIREBALL_SHOT_A);
                    }
                    var_s7->state++;
                }
                break;
            case 3:
                var_s7->unkE -= 0x60;
                var_s7->unkA -= 48;

                if ((var_s7->posX.val < FIX(-288) ||
                     var_s7->posX.val > FIX(288)) ||
                    (var_s7->posY.val < FIX(0) ||
                     var_s7->posY.val > FIX(256))) {
                    var_s7->unkA = 0;
                }
                var_s7->unkC += 2;
                if (var_s7->unkC > 0x100) {
                    DestroyEntity(self);
                    return;
                }
                break;
            }
        }
        self->ext.shield.unk9C += 2;
        if (self->ext.shield.unk9C > 0x100) {
            self->ext.shield.unk9C = 0x100;
        }
    }
    firstPrim = prim = &g_PrimBuf[self->primIndex];
    var_s7 = &D_CF000_8017C9A0[dragonNumber][0];
    for (i = 0; i < 19; i++, prim = prim->next) {
        prim->clut =
            self->ext.weapon.childPalette + D_CF000_8017AD10[g_Timer / 2 % 10];
        prim->x0 = prev_x2;
        prim->y0 = prev_y2;
        prim->x1 = prev_x3;
        prim->y1 = prev_x4;
        var_s7++;
        size = var_s7->unk10 *
               (rsin(var_s7->unkE) * var_s7->unk14 / 256 + 4096) / 4096;
        centerX = var_s7->posX.i.hi;
        centerY = var_s7->posY.i.hi;
        angle1 = var_s7->unk8 - 0x400;
        angle2 = var_s7->unk8 + 0x400;
        prev_x2 = prim->x2 = centerX + (((rcos(angle1) >> 4) * size) >> 8);
        prev_y2 = prim->y2 = centerY - (((rsin(angle1) >> 4) * size) >> 8);
        prim->x3 = prev_x3 = centerX + (((rcos(angle2) >> 4) * size) >> 8);
        prim->y3 = prev_x4 = centerY - (((rsin(angle2) >> 4) * size) >> 8);
    }

    var_s7 = &D_CF000_8017C9A0[dragonNumber][0];
    size = ((self->ext.shield.unk9C * 54 / 256) *
            (rsin(var_s7->unkE) * var_s7->unk14 / 256 + 4096)) /
           4096;
    nextX = (var_s7 + 1)->posX.i.hi;
    nextX_copy = nextX;
    nextY = (var_s7 + 1)->posY.i.hi;
    nextY_copy = nextY;
    angle1 = var_s7->unk8 - 0x199;
    angle2 = var_s7->unk8 + 0x199;
    firstPrim->x0 = nextX + (((rcos(angle1) >> 4) * size) >> 8);
    firstPrim->y0 = nextY - (((rsin(angle1) >> 4) * size) >> 8);
    firstPrim->x1 = nextX + (((rcos(angle2) >> 4) * size) >> 8);
    firstPrim->y1 = nextY - (((rsin(angle2) >> 4) * size) >> 8);
    angle1 = var_s7->unk8 + 0x999;
    angle2 = var_s7->unk8 + 0x667;
    firstPrim->x2 = nextX + (((rcos(angle1) >> 4) * size) >> 8);
    firstPrim->y2 = nextY - (((rsin(angle1) >> 4) * size) >> 8);
    firstPrim->x3 = nextX + (((rcos(angle2) >> 4) * size) >> 8);
    firstPrim->y3 = nextY - (((rsin(angle2) >> 4) * size) >> 8);
    self->posX.i.hi = nextX_copy;
    self->posY.i.hi = nextY_copy;
    // TODO: FACTORY()
    g_api.CreateEntFactoryFromEntity(self, ((g_HandId + 1) << 14) + 100, 0);
}

static void func_ptr_80170028(Entity* self) {
    if (self->ext.weapon.parent->entityId == 0) {
        DestroyEntity(self);
        return;
    }
    if (self->step == 0) {
        self->ext.weapon.equipId = self->ext.weapon.parent->ext.weapon.equipId;
        SetWeaponProperties(self, 0);
        self->hitboxHeight = 14;
        self->hitboxWidth = 14;
        self->ext.weapon.unk80 = 10;
        self->step++;
        return;
    }
    if (--self->ext.weapon.unk80 == 0) {
        DestroyEntity(self);
    }
}

static void WeaponUnused2C(void) {}

static void WeaponUnused30(void) {}

static void WeaponUnused34(void) {}

static void WeaponUnused38(void) {}

static void WeaponUnused3C(void) {}

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
FireShieldDragon D_CF000_8017C9A0[4][20];
extern u8 D_CF000_8017A884[];
extern u8 D_CF000_8017AAC4[];
extern u8 D_CF000_8017AACC[];
extern u8 D_CF000_8017AAD4[];
extern u8 D_CF000_8017AB00[];
extern u8 D_CF000_8017AB44[];
extern u8 D_CF000_8017AB7C[];
extern u8 D_CF000_8017AB9C[];
extern u8 D_CF000_8017ABE0[];
extern u8 D_CF000_8017ABF0[];
extern u8 D_CF000_8017AC00[];
extern u8 D_CF000_8017AC10[];
extern u8 D_CF000_8017AC20[];
extern u8 D_CF000_8017AC2C[];
extern u8 D_CF000_8017AC38[];
u8 D_CF000_8017A884[] ALIGNED4 = {0x00, 0x00, 0x21, 0x04, 0xA5, 0x10, 0x2A, 0x21, 0xAE, 0x31, 0x33, 0x42, 0xD8, 0x52, 0x8B, 0x10, 0x13, 0x00, 0x1F, 0x00, 0x70, 0x1D, 0x1D, 0x12, 0x5D, 0x16, 0xBE, 0x1E, 0xFE, 0x26, 0x5F, 0x2F, 0x00, 0x00, 0x21, 0x04, 0xA5, 0x10, 0x2A, 0x21, 0xAE, 0x31, 0x33, 0x42, 0xD8, 0x52, 0x8B, 0x10, 0x13, 0x00, 0x1F, 0x00, 0x70, 0x1D, 0x5F, 0x2F, 0x1D, 0x12, 0x5D, 0x16, 0xBE, 0x1E, 0xFE, 0x26, 0x00, 0x00, 0x21, 0x04, 0xA5, 0x10, 0x2A, 0x21, 0xAE, 0x31, 0x33, 0x42, 0xD8, 0x52, 0x8B, 0x10, 0x13, 0x00, 0x1F, 0x00, 0x70, 0x1D, 0xFE, 0x26, 0x5F, 0x2F, 0x1D, 0x12, 0x5D, 0x16, 0xBE, 0x1E, 0x00, 0x00, 0x21, 0x04, 0xA5, 0x10, 0x2A, 0x21, 0xAE, 0x31, 0x33, 0x42, 0xD8, 0x52, 0x8B, 0x10, 0x13, 0x00, 0x1F, 0x00, 0x70, 0x1D, 0xBE, 0x1E, 0xFE, 0x26, 0x5F, 0x2F, 0x1D, 0x12, 0x5D, 0x16, 0x00, 0x00, 0x21, 0x04, 0xA5, 0x10, 0x2A, 0x21, 0xAE, 0x31, 0x33, 0x42, 0xD8, 0x52, 0x8B, 0x10, 0x13, 0x00, 0x1F, 0x00, 0x70, 0x1D, 0x5D, 0x16, 0xBE, 0x1E, 0xFE, 0x26, 0x5F, 0x2F, 0x1D, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5D, 0x16, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x2F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x2F, 0x00, 0x00, 0x5D, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x2F, 0x00, 0x00, 0x5D, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x2F, 0x00, 0x00, 0x5D, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5D, 0x16, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x2F, 0x00, 0x00, 0x00, 0x00, 0x6A, 0x04, 0xF1, 0x00, 0x7A, 0x01, 0x3F, 0x12, 0x3F, 0x2B, 0x63, 0x0C, 0xCE, 0x39, 0xB5, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x97, 0x32, 0xF3, 0x4F, 0xC0, 0x03, 0x00, 0x00, 0x6A, 0x04, 0xF1, 0x00, 0x7A, 0x01, 0x3F, 0x12, 0x3F, 0x2B, 0x63, 0x0C, 0xCE, 0x39, 0xB5, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x2E, 0x00, 0x00, 0xE0, 0x03, 0xC0, 0x02, 0x00, 0x00, 0x6A, 0x04, 0xF1, 0x00, 0x7A, 0x01, 0x3F, 0x12, 0x3F, 0x2B, 0x63, 0x0C, 0xCE, 0x39, 0xB5, 0x56, 0x00, 0x00, 0x00, 0x00, 0x14, 0x2E, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 0x40, 0x02, 0x00, 0x00, 0x6A, 0x04, 0xF1, 0x00, 0x7A, 0x01, 0x3F, 0x12, 0x3F, 0x2B, 0x63, 0x0C, 0xCE, 0x39, 0xB5, 0x56, 0x00, 0x00, 0x14, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0x80, 0x01, 0x00, 0x00, 0x6A, 0x04, 0xF1, 0x00, 0x7A, 0x01, 0x3F, 0x12, 0x3F, 0x2B, 0x63, 0x0C, 0xCE, 0x39, 0xB5, 0x56, 0x55, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x00, 0x00, 0x17, 0x00, 0x1B, 0x01, 0x3F, 0x02, 0xFF, 0x37, 0xFF, 0x67, 0x63, 0x0C, 0xCE, 0x39, 0xB5, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 0x40, 0x02, 0x00, 0x00, 0x1F, 0x00, 0x1A, 0x00, 0x15, 0x00, 0x11, 0x00, 0x0F, 0x00, 0x0D, 0x00, 0x0B, 0x00, 0x09, 0x00, 0xBF, 0x00, 0x5F, 0x01, 0xFF, 0x01, 0x1F, 0x13, 0xFF, 0x2F, 0xFF, 0x4F, 0xFF, 0x7F, 0x00, 0x00, 0x1A, 0x00, 0x18, 0x00, 0x16, 0x00, 0x15, 0x00, 0x13, 0x00, 0x12, 0x00, 0x10, 0x00, 0x0F, 0x00, 0x16, 0x00, 0x1F, 0x01, 0x7F, 0x05, 0xDF, 0x09, 0x3F, 0x0A, 0x9F, 0x0E, 0x1F, 0x13};
u8 D_CF000_8017AAC4[] ALIGNED4 = {0x02, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017AACC[] ALIGNED4 = {0x02, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017AAD4[] ALIGNED4 = {0x02, 0x00, 0x03, 0x06, 0x02, 0x00, 0x04, 0x06, 0x02, 0x00, 0x05, 0x06, 0x02, 0x00, 0x06, 0x06, 0x02, 0x00, 0x07, 0x06, 0x02, 0x00, 0x08, 0x06, 0x02, 0x00, 0x09, 0x08, 0x02, 0x00, 0x0A, 0x02, 0x02, 0x00, 0x0B, 0x02, 0x02, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017AB00[] ALIGNED4 = {0x03, 0x00, 0x0D, 0x02, 0x03, 0x00, 0x0E, 0x0E, 0x03, 0x00, 0x0F, 0x10, 0x03, 0x00, 0x10, 0x12, 0x03, 0x00, 0x11, 0x14, 0x03, 0x00, 0x12, 0x16, 0x03, 0x00, 0x13, 0x18, 0x03, 0x00, 0x14, 0x1A, 0x03, 0x00, 0x15, 0x1C, 0x03, 0x00, 0x16, 0x1A, 0x03, 0x00, 0x17, 0x18, 0x03, 0x00, 0x18, 0x16, 0x03, 0x00, 0x19, 0x14, 0x03, 0x00, 0x1A, 0x12, 0x03, 0x00, 0x1B, 0x10, 0x03, 0x00, 0x1C, 0x0E, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017AB44[] ALIGNED4 = {0x01, 0x00, 0x24, 0x02, 0x01, 0x00, 0x25, 0x02, 0x01, 0x00, 0x26, 0x02, 0x01, 0x00, 0x27, 0x02, 0x01, 0x00, 0x28, 0x02, 0x01, 0x00, 0x29, 0x02, 0x01, 0x00, 0x2A, 0x02, 0x01, 0x00, 0x2B, 0x02, 0x01, 0x00, 0x2C, 0x02, 0x01, 0x00, 0x2D, 0x02, 0x01, 0x00, 0x2E, 0x02, 0x01, 0x00, 0x2F, 0x02, 0x01, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017AB7C[] ALIGNED4 = {0x01, 0x00, 0x2A, 0x02, 0x01, 0x00, 0x2B, 0x02, 0x01, 0x00, 0x2C, 0x02, 0x01, 0x00, 0x2D, 0x02, 0x01, 0x00, 0x2E, 0x02, 0x01, 0x00, 0x2F, 0x02, 0x01, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017AB9C[] ALIGNED4 = {0x02, 0x00, 0x21, 0x02, 0x02, 0x00, 0x22, 0x02, 0x02, 0x00, 0x23, 0x02, 0x02, 0x00, 0x24, 0x02, 0x02, 0x00, 0x25, 0x02, 0x02, 0x00, 0x26, 0x02, 0x02, 0x00, 0x27, 0x02, 0x02, 0x00, 0x28, 0x02, 0x02, 0x00, 0x29, 0x02, 0x02, 0x00, 0x2A, 0x02, 0x02, 0x00, 0x2B, 0x02, 0x02, 0x00, 0x2C, 0x02, 0x02, 0x00, 0x2D, 0x02, 0x02, 0x00, 0x2E, 0x02, 0x02, 0x00, 0x2F, 0x02, 0x02, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017ABE0[] ALIGNED4 = {0x02, 0x00, 0x31, 0x02, 0x02, 0x00, 0x32, 0x02, 0x02, 0x00, 0x33, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_CF000_8017ABF0[] ALIGNED4 = {0x02, 0x00, 0x34, 0x04, 0x02, 0x00, 0x35, 0x04, 0x02, 0x00, 0x36, 0x04, 0xFF, 0xFF, 0x00, 0x00};
u8 D_CF000_8017AC00[] ALIGNED4 = {0x02, 0x00, 0x33, 0x02, 0x02, 0x00, 0x32, 0x02, 0x02, 0x00, 0x31, 0x02, 0xFF, 0xFF, 0x00, 0x00};
u8 D_CF000_8017AC10[] ALIGNED4 = {0x02, 0x00, 0x36, 0x04, 0x02, 0x00, 0x35, 0x04, 0x02, 0x00, 0x34, 0x04, 0xFF, 0xFF, 0x00, 0x00};
u8 D_CF000_8017AC20[] ALIGNED4 = {0x04, 0x00, 0x1D, 0x02, 0x04, 0x00, 0x1E, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017AC2C[] ALIGNED4 = {0x02, 0x00, 0x1F, 0x0C, 0x02, 0x00, 0x20, 0x0C, 0x00, 0x00, 0x00, 0x00};
u8 D_CF000_8017AC38[] ALIGNED4 = {0x03, 0x00, 0x37, 0x22, 0x03, 0x00, 0x38, 0x22, 0x03, 0x00, 0x38, 0x22, 0x03, 0x00, 0x38, 0x20, 0x03, 0x00, 0x38, 0x1E, 0x03, 0x00, 0x39, 0x02, 0x03, 0x00, 0x3A, 0x02, 0x03, 0x00, 0x3B, 0x02, 0x03, 0x00, 0x3C, 0x02, 0x03, 0x00, 0x3D, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
s32 D_CF000_8017AC78[0x12] = {
    0,
    0x0C04F90E,
    0x0C040D0E,
    0x0C04F9F5,
    0x0C04FB01,
    0xE1E0,
    0x0C04FA16,
    0x0C04FA0F,
    0x0C04FB10,
    0x0C04FC11,
    0x0C04FD12,
    0x0C04FE13,
    0x0C04FF14,
    0x0C040015,
    0x0C040116,
    0x0C04FB0C,
    0x0C04FD0A,
    0x0C04FF08,
};
AnimationFrame* D_CF000_8017ACC0[0xE] = {
    (AnimationFrame*)&D_CF000_8017AAC4,
    (AnimationFrame*)&D_CF000_8017AACC,
    (AnimationFrame*)&D_CF000_8017AC2C,
    (AnimationFrame*)&D_CF000_8017AC38,
    (AnimationFrame*)&D_CF000_8017AC20,
    (AnimationFrame*)&D_CF000_8017AB9C,
    (AnimationFrame*)&D_CF000_8017AB7C,
    (AnimationFrame*)&D_CF000_8017AB44,
    (AnimationFrame*)&D_CF000_8017AB00,
    (AnimationFrame*)&D_CF000_8017AAD4,
    (AnimationFrame*)&D_CF000_8017ABE0,
    (AnimationFrame*)&D_CF000_8017ABF0,
    (AnimationFrame*)&D_CF000_8017AC00,
    (AnimationFrame*)&D_CF000_8017AC10,
};
u16 D_CF000_8017ACF8[6] = { 0, 1, 2, 3, 4, 0 };
u16 D_CF000_8017AD04[6] = { 5, 6, 7, 8, 9, 0 };
u16 D_CF000_8017AD10[0xA] = { 0xA, 0xF, 0xB, 0xF, 0xC, 0xF, 0xD, 0xF, 0xE, 0xF };
AnimationFrame D_CF000_8017AD24[9] = {
    { 3, 0xC3F },
    { 3, 0xC40 },
    { 3, 0xC3F },
    { 3, 0xC40 },
    { 3, 0xC41 },
    { 3, 0xC42 },
    { 3, 0xC41 },
    { 3, 0xC42 },
    { 0, 0 },
};
s32 g_HandId = HAND_ID;
u16* g_WeaponCluts[2] = { D_CF000_8017A884, D_CF000_8017A884 };
// end pc_import_data.py
// clang-format on
#endif
