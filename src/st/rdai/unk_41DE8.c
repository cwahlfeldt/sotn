// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

extern EInit g_EInitShield;
extern u8 D_us_8018191C[];
extern u8 D_us_80181924[];
extern u8 D_us_8018192C[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_41DE8", func_us_801C1DE8);
#else
// A shield of the Spectral Sword, params + 7 slots after it: holds its place
// in the wall facing Alucard and rams him with the spear behind it
void func_us_801C1DE8(Entity* self) {
    Entity* other;
    u8* anim;
    s32 dx;
    s32 dy;
    s32 dist;
    s16 angle;

    if ((self->flags & FLAG_DEAD) && self->step < 8) {
        SetStep(8);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitShield);
        self->animCurFrame = 7;
        self->zPriority += 2;
        SetStep(3);
        break;
    case 2:
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        dist = GetDistanceToPlayerX();
        self->animCurFrame = 8;
        if (dist < 0x40) {
            self->animCurFrame = 7;
        }
        if (dist < 0x20) {
            self->animCurFrame = 6;
        }
        break;
    case 3:
        MoveEntity();
        if (self->animCurFrame != 8) {
            AnimateEntity(D_us_8018192C, self);
        }
        dx = (s16)self->ext.rdaiSword.targetX.i.hi - self->posX.i.hi;
        dy = (s16)self->ext.rdaiSword.targetY.i.hi - self->posY.i.hi;
        dist = SquareRoot0(dx * dx + dy * dy);
        if (dist > 0x38) {
            dist = 0x38;
        }
        angle = ratan2(dy, dx);
        self->velocityX = dist * rcos(angle);
        self->velocityY = dist * rsin(angle);
        if (self->animCurFrame != 6) {
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        }
        if (self->ext.rdaiSword.attack) {
            SetStep(4);
        }
        break;
    case 4:
        switch (self->step_s) {
        case 0:
            self->velocityX = rcos(self->ext.rdaiSword.angle - 0x800) << 5;
            self->velocityY = rsin(self->ext.rdaiSword.angle - 0x800) << 5;
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            anim = D_us_8018191C;
            if (self->ext.rdaiSword.attack & 2) {
                anim = D_us_80181924;
            }
            if (!AnimateEntity(anim, self)) {
                self->ext.rdaiSword.timer = 0x20;
                self->step_s++;
            }
            break;
        case 2:
            if (!--self->ext.rdaiSword.timer) {
                self->ext.rdaiSword.attack = 0;
                SetStep(3);
            }
            break;
        }
        break;
    case 8:
        switch (self->step_s) {
        case 0:
            other = self - self->params - 7;
            other->ext.rdaiSword.shields--;
            self->hitboxState = 0;
            PlaySfxPositional(SFX_METAL_CLANG_C);
            PlaySfxPositional(SFX_QUICK_STUTTER_EXPLODE_B);
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY += FIX(1.0 / 8);
            if (!(g_Timer & 7)) {
                other = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (other != NULL) {
                    CreateEntityFromEntity(E_EXPLOSION, self, other);
                    other->params = 1;
                }
            }
            if (self->posY.i.hi > 0x140) {
                self->animCurFrame = 0;
                self->step_s++;
            }
            break;
        case 2:
            self->animCurFrame = 0;
            break;
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_41DE8", func_us_801C21E4);
#else
// The Sniper of Goth's hair: eleven quads trailing from her head, each pulled
// toward the end of the one before it, swaying unless she is aiming
void func_us_801C21E4(s32 sway) {
    Primitive* prim;
    s32 x, y;
    s32 dx, dy;
    s32 angle;
    s32 squash;
    s32 i;
    s16 px, py;
    u16 phase;

    phase = g_CurrentEntity->ext.sniperOfGoth.hairPhase - 0x60;
    g_CurrentEntity->ext.sniperOfGoth.hairPhase = phase;
    prim = g_CurrentEntity->ext.sniperOfGoth.prim->next;
    if (sway) {
        px = g_CurrentEntity->posX.i.hi - 4;
        py = g_CurrentEntity->posY.i.hi + 4;
    } else {
        px = g_CurrentEntity->posX.i.hi - 6;
        py = g_CurrentEntity->posY.i.hi + 6;
    }
    x = px << 8;
    y = py << 8;
    for (i = 0; i < 11; i++) {
        prim->x0 = px;
        prim->x1 = sway ? px + 0xC : px + 0xE;
        prim->y0 = prim->y1 = py;
        dx = (prim->x2 << 8) + prim->p2 - x;
        dy = (prim->y2 << 8) + prim->p3 - y + 0x180;
        if (g_CurrentEntity->flags & FLAG_DEAD) {
            dy -= 0x400;
        }
        angle = ratan2(-dx, dy);
        if (dx * dx + dy * dy > 0x400000) {
            dx = -((rsin(angle) * 8) >> 4);
            dy = (rcos(angle) * 8) >> 4;
        }
        squash = rcos((s16)phase);
        if (squash < 0) {
            squash = -squash;
        }
        dy = ((0x1000 - squash / 4) * dy) >> 12;
        if (sway) {
            dx += rcos((s16)phase) >> 6;
        }
        x += dx;
        y += dy;
        px = x >> 8;
        py = y >> 8;
        prim->x2 = x >> 8;
        prim->y2 = y >> 8;
        prim->p2 = x;
        prim->p3 = y;
        prim->x3 = sway ? px + 0xC : px + 0xE;
        prim->y3 = py;
        prim = prim->next;
        phase += 0x200;
    }
}
#endif

extern EInit D_us_801808CC;
extern u8 D_us_80181934[];
extern u8 D_us_8018195C[];
extern u8 D_us_8018198C[];
extern u8 D_us_801819A4[];
extern u8 D_us_801819DC[];
extern u8 D_us_80181A14[];
extern u8 D_us_80181A1C[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_41DE8", func_us_801C2418);
#else

// The Sniper of Goth (params 0). She drops in from above when Alucard comes
// near, hovers beside him and shoots arrows (0x27), every fourth attack a
// ring of 32. Params 1 and 2 are her afterimages in the next two slots,
// shown while she charges the ring; params 3 is a fading afterimage.
void func_us_801C2418(Entity* self) {
    Entity* other;
    Primitive* prim;
    s32 dx, dy;
    s32 done;
    s32 i;
    s16 angle;
    u8 v0, v2;
    u16 clut;

    if (g_Timer & 1) {
        self->palette = D_us_801808CC[3];
    } else {
        self->palette = D_us_801808CC[3] + 3;
    }
    if ((self->flags & FLAG_DEAD) && self->step < 9) {
        PlaySfxPositional(SFX_UNK_RDAI_717);
        SetStep(9);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801808CC);
        self->flags |= FLAG_UNK_2000;
        if (self->params == 0) {
            CreateEntityFromCurrentEntity(E_UNK_26, self + 1);
            (self + 1)->params = 1;
            (self + 1)->zPriority = self->zPriority - 1;
            CreateEntityFromCurrentEntity(E_UNK_26, self + 2);
            (self + 2)->params = 2;
            (self + 2)->zPriority = self->zPriority + 2;
            i = g_api.AllocPrimitives(PRIM_GT4, 12);
            if (i == -1) {
                DestroyEntity(self);
                return;
            }
            self->flags |= FLAG_HAS_PRIMS;
            self->primIndex = i;
            prim = &g_PrimBuf[i];
            self->ext.sniperOfGoth.prim = prim;
            prim->tpage = self->unk5A >> 2;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x38;
            prim->v0 = prim->v1 = 0xC8;
            prim->v2 = prim->v3 = 0xFF;
            prim->clut = self->palette + 1;
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority + 1;
            prim = prim->next;
            v0 = 0x28;
            v2 = 0x30;
            for (i = 0; i < 11; i++) {
                prim->tpage = self->unk5A >> 2;
                prim->v0 = prim->v1 = v0;
                prim->u0 = prim->u2 = 0xF0;
                prim->u1 = prim->u3 = 0xFF;
                prim->v2 = prim->v3 = v2;
                prim->clut = self->palette + 2;
                prim->x0 = prim->x2 = prim->x1 = prim->x3 = self->posX.i.hi;
                prim->y0 = prim->y1 = prim->y2 = prim->y3 = self->posY.i.hi;
                prim->drawMode = DRAW_HIDE;
                prim->priority = self->zPriority - 2;
                prim = prim->next;
                v0 += 8;
                v2 += 8;
            }
            SetStep(3);
            break;
        }
        self->hitboxState = 0;
        self->flags |= FLAG_UNK_00200000 | FLAG_UNK_2000;
        if (self->params & 0xFF00) {
            self->animCurFrame = self->params >> 8;
            self->params &= 0xFF;
        } else {
            self->flags &= ~FLAG_DESTROY_IF_OUT_OF_CAMERA;
        }
        SetStep(self->params + 0x10);
        break;

    case 3:
        // waits off-screen, then drops in from above
        AnimateEntity(D_us_801819A4, self);
        switch (self->step_s) {
        case 0:
            self->scaleX = 0x100;
            self->scaleY = 0x100;
            self->opacity = 0xFF;
            self->hitboxState = 0;
            self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY;
            self->rotate = 0x800;
            self->drawFlags |= ENTITY_OPACITY;
            self->step_s++;
            self->drawFlags |= ENTITY_ROTATE;
            // fallthrough
        case 1:
            self->animCurFrame = 0;
            if (GetDistanceToPlayerX() < 0x40) {
                PlaySfxPositional(SFX_ARROW_SHOT_C);
                self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
                self->posY.i.hi = -0x20;
                self->velocityY = FIX(10);
                self->pose = 0;
                self->poseTimer = 0;
                self->animCurFrame = 0x3A;
                self->step_s++;
            }
            break;
        case 2:
            MoveEntity();
            self->animCurFrame = 0x3A;
            self->pose = 0;
            self->poseTimer = 0;
            self->velocityY -= FIX(0.25);
            if (self->velocityY < FIX(-3)) {
                self->velocityY = FIX(-1);
                self->scaleX = 8;
                self->scaleY = 0x180;
                self->step_s++;
            }
            break;
        case 3:
            MoveEntity();
            self->rotate = 0;
            done = 0;
            self->scaleX += 0xC;
            if (self->scaleX > 0x100) {
                self->scaleX = 0x100;
                done = 1;
            }
            self->scaleY -= 4;
            if (self->scaleY < 0x100) {
                self->scaleY = 0x100;
                done++;
            }
            self->opacity -= 2;
            if (self->opacity < 0x80) {
                self->opacity = 0x80;
                done++;
            }
            if (done == 3) {
                self->drawFlags = 0;
                prim = self->ext.sniperOfGoth.prim->next;
                for (i = 0; i < 11; i++) {
                    prim->x0 = prim->x2 = prim->x1 = prim->x3 =
                        self->posX.i.hi;
                    prim->drawMode = DRAW_UNK02;
                    prim->y0 = prim->y1 = prim->y2 = prim->y3 =
                        self->posY.i.hi;
                    prim = prim->next;
                }
                self->hitboxState = 3;
                self->flags &= ~FLAG_UNK_2000;
                SetStep(4);
            }
            break;
        }
        if (g_Timer & 1) {
            self->animCurFrame = 0;
        }
        break;

    case 4:
        // hovers beside Alucard until the next attack
        if (!self->step_s) {
            self->ext.sniperOfGoth.hoverTimer = 0x40;
            self->step_s++;
        }
        MoveEntity();
        AnimateEntity(D_us_80181934, self);
        if (!self->poseTimer && self->pose == 5) {
            PlaySfxPositional(SFX_WING_FLAP_B);
        }
        if (self->facingLeft) {
            dx = PLAYER.posX.i.hi - 0x50;
        } else {
            dx = PLAYER.posX.i.hi + 0x50;
        }
        dy = PLAYER.posY.i.hi - 0x50 - self->posY.i.hi;
        dx -= self->posX.i.hi;
        angle = ratan2(dy, dx);
        self->velocityX = (rcos(angle) * FIX(1.25)) >> 12;
        self->velocityY = (rsin(angle) * FIX(1.25)) >> 12;
        if (abs(dx) < 8 && abs(dy) < 8) {
            self->velocityX = 0;
            self->velocityY = 0;
            self->ext.sniperOfGoth.hoverTimer -= 2;
        } else {
            self->ext.sniperOfGoth.hoverTimer--;
        }
        if (GetDistanceToPlayerX() < 0x50) {
            self->ext.sniperOfGoth.timer--;
        } else {
            self->ext.sniperOfGoth.timer = 0x30;
        }
        if (!self->ext.sniperOfGoth.timer) {
            // Alucard stayed too close: fly past him and turn around
            self->step_s = 0;
            self->step = 7;
        }
        if (self->ext.sniperOfGoth.hoverTimer <= 0) {
            self->ext.sniperOfGoth.attacks =
                (self->ext.sniperOfGoth.attacks + 1) & 3;
            SetStep(5);
            if (!self->ext.sniperOfGoth.attacks) {
                SetStep(6);
            }
        }
        break;

    case 7:
        AnimateEntity(D_us_80181934, self);
        if (!self->poseTimer && self->pose == 5) {
            PlaySfxPositional(SFX_WING_FLAP_B);
        }
        MoveEntity();
        switch (self->step_s) {
        case 0:
            self->velocityX = self->facingLeft ? FIX(3) : FIX(-3);
            self->velocityY = 0;
            self->drawFlags = ENTITY_ROTATE;
            self->rotate = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (self->facingLeft) {
                dx = PLAYER.posX.i.hi - (self->posX.i.hi - 0x40);
            } else {
                dx = (self->posX.i.hi + 0x40) - PLAYER.posX.i.hi;
            }
            if (dx < 0x20) {
                self->step_s++;
            }
            break;
        case 2:
            self->rotate -= 0x40;
            if (self->rotate == -0x200) {
                self->facingLeft ^= 1;
                self->step_s++;
            }
            break;
        case 3:
            self->rotate += 0x40;
            if (!self->rotate) {
                self->drawFlags = 0;
                self->step_s = 0;
                self->step = 8;
            }
            break;
        }
        break;

    case 8:
        switch (self->step_s) {
        case 0:
            self->ext.sniperOfGoth.loops = 0;
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_8018195C, self)) {
                self->ext.sniperOfGoth.loops++;
            }
            if (self->ext.sniperOfGoth.loops == 2) {
                SetStep(4);
            }
            break;
        }
        break;

    case 5:
        // a single arrow
        if (!AnimateEntity(D_us_8018198C, self)) {
            SetStep(8);
        }
        if (!self->poseTimer && self->pose == 1) {
            PlaySfxPositional(SFX_BONE_CREAK);
        }
        if (!self->poseTimer && self->pose == 2) {
            PlaySfxPositional(SFX_ARROW_SHOT_B);
            other = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (other != NULL) {
                CreateEntityFromEntity(E_UNK_27, self, other);
                other->facingLeft = self->facingLeft;
                if (self->facingLeft) {
                    other->posX.i.hi += 0x28;
                } else {
                    other->posX.i.hi -= 0x28;
                }
                other->posY.i.hi += 0x10;
            }
        }
        break;

    case 6:
        // a ring of 32 arrows, with her afterimages shown
        AnimateEntity(D_us_80181A14, self);
        switch (self->step_s) {
        case 0:
            prim = self->ext.sniperOfGoth.prim;
            (self + 1)->ext.sniperOfGoth.shadow = 1;
            prim->x0 = prim->x2 = prim->x1 = prim->x3 = self->posX.i.hi;
            prim->drawMode = DRAW_UNK02;
            prim->y0 = prim->y1 = prim->y2 = prim->y3 = self->posY.i.hi;
            self->ext.sniperOfGoth.timer = 0x28;
            PlaySfxPositional(SFX_MAGIC_NOISE_SWEEP);
            self->step_s++;
            // fallthrough
        case 1:
            prim = self->ext.sniperOfGoth.prim;
            prim->x0 = prim->x2 = prim->x2 - 1;
            prim->x1 = prim->x3 = prim->x3 + 1;
            prim->y0 = prim->y1 = prim->y1 - 1;
            prim->y2 = prim->y3 = prim->y3 + 1;
            if (!--self->ext.sniperOfGoth.timer) {
                SetSubStep(2);
            }
            break;
        case 2:
            (self + 2)->ext.sniperOfGoth.shadow = 1;
            self->ext.sniperOfGoth.timer = 0x20;
            self->step_s++;
            break;
        case 3:
            self->ext.sniperOfGoth.shots = 0x20;
            if (!--self->ext.sniperOfGoth.timer) {
                self->step_s++;
            }
            break;
        case 4:
            other = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (other != NULL) {
                CreateEntityFromEntity(E_UNK_27, self, other);
                other->params = self->ext.sniperOfGoth.shots;
                other->facingLeft = self->facingLeft;
            }
            if (!(self->ext.sniperOfGoth.shots & 3)) {
                PlaySfxPositional(SFX_ARROW_SHOT_C);
            }
            if (!--self->ext.sniperOfGoth.shots) {
                self->ext.sniperOfGoth.timer = 0x20;
                self->step_s++;
            }
            break;
        case 5:
            prim = self->ext.sniperOfGoth.prim;
            prim->x0 = prim->x2 = prim->x2 + 1;
            prim->x1 = prim->x3 = prim->x3 - 1;
            prim->y0 = prim->y1 = prim->y1 + 1;
            prim->y2 = prim->y3 = prim->y3 - 1;
            if (!--self->ext.sniperOfGoth.timer) {
                prim = self->ext.sniperOfGoth.prim;
                prim->drawMode = DRAW_HIDE;
                (self + 1)->ext.sniperOfGoth.shadow = 0;
                (self + 2)->ext.sniperOfGoth.shadow = 0;
                SetStep(8);
            }
            break;
        }
        break;

    case 9:
        // death: falls spinning in a trail of explosions
        AnimateEntity(D_us_801819A4, self);
        switch (self->step_s) {
        case 0:
            self->velocityY = FIX(1.5);
            self->drawFlags = ENTITY_ROTATE;
            self->velocityX = 0;
            self->rotate = 0;
            self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
            self->ext.sniperOfGoth.prim->drawMode = DRAW_HIDE;
            PlaySfxPositional(SFX_FM_STUTTER_EXPLODE);
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY += FIX(1.0 / 16);
            self->rotate += 0x20;
            if (!(g_Timer & 0x1F)) {
                PlaySfxPositional(SFX_BOSS_WING_FLAP);
            }
            if (!(g_Timer & 3)) {
                other = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (other != NULL) {
                    CreateEntityFromEntity(E_EXPLOSION, self, other);
                    other->params = 1;
                    other->zPriority = self->zPriority + 1;
                    other->posX.i.hi += -0x1F + (Random() & 0x3F);
                    other->posY.i.hi += -0x1F + (Random() & 0x3F);
                }
            }
            break;
        }
        break;

    case 0x11:
        // afterimage that follows her
        if (!self->ext.sniperOfGoth.shadow) {
            self->pose = 0;
            self->poseTimer = 0;
            self->animCurFrame = 0;
            break;
        }
        AnimateEntity(D_us_801819DC, self);
        self->facingLeft = (self - 1)->facingLeft;
        self->posX.i.hi = (self - 1)->posX.i.hi;
        self->posY.i.hi = (self - 1)->posY.i.hi;
        other = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (other != NULL) {
            CreateEntityFromEntity(E_UNK_26, self, other);
            other->facingLeft = self->facingLeft;
            other->zPriority = self->zPriority - 1;
            other->params = (self->animCurFrame << 8) + 3;
        }
        if ((self - 1)->entityId != E_UNK_26 ||
            ((self - 1)->flags & FLAG_DEAD)) {
            DestroyEntity(self);
        }
        return;

    case 0x12:
        // afterimage that grows out of her
        if (!self->ext.sniperOfGoth.shadow) {
            self->pose = 0;
            self->poseTimer = 0;
            self->animCurFrame = 0;
            self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
            self->scaleY = 0;
            self->scaleX = 0;
            break;
        }
        AnimateEntity(D_us_80181A1C, self);
        self->scaleY += 0x20;
        if (self->scaleY > 0x100) {
            self->drawFlags = 0;
        }
        self->facingLeft = (self - 2)->facingLeft;
        self->scaleX = self->scaleY;
        self->posX.i.hi = (self - 2)->posX.i.hi;
        self->posY.i.hi = (self - 2)->posY.i.hi;
        if ((self - 2)->entityId != E_UNK_26 ||
            ((self - 2)->flags & FLAG_DEAD)) {
            DestroyEntity(self);
            return;
        }
        break;

    case 0x13:
        // fading afterimage left behind by params 1
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0x60;
            self->step_s++;
        }
        self->opacity -= 8;
        if (g_Timer & 1) {
            self->animSet = 0;
        } else {
            self->animSet = D_us_801808CC[0];
        }
        if (!self->opacity) {
            DestroyEntity(self);
        }
        return;

    case 0xFF:
#include "../pad2_anim_debug.h"
    }

    if (self->animCurFrame < 13) {
        self->hitboxOffX = -6;
        self->hitboxOffY = 9;
        self->hitboxWidth = 0x14;
        self->hitboxHeight = 0x17;
    } else if (self->animCurFrame < 17) {
        self->hitboxOffX = -0xB;
        self->hitboxOffY = -2;
        self->hitboxWidth = 0x14;
        self->hitboxHeight = 0x17;
    } else {
        self->hitboxOffY = -5;
        self->hitboxWidth = 0xE;
        self->hitboxOffX = 0;
        self->hitboxHeight = 0x16;
    }
    if (!self->params) {
        clut = self->palette + 1;
        for (prim = self->ext.sniperOfGoth.prim; prim != NULL;
             prim = prim->next) {
            prim->clut = clut;
            clut = self->palette + 2;
        }
        func_us_801C21E4(self->step != 3 && self->step != 6);
    }
}
#endif

extern EInit D_us_801808D8;
extern EInit D_us_801808E4;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/unk_41DE8", func_us_801C3580);
#else

// The Sniper of Goth's arrows. Params 0 is a single arrow shot down at an
// angle; params 1-32 are the ring, params * 0x80 giving the direction. An
// arrow that hits Alucard sticks to him for a moment.
void func_us_801C3580(Entity* self) {
    s16 angle;

    if (g_Timer & 1) {
        self->palette = D_us_801808CC[3];
    } else {
        self->palette = D_us_801808CC[3] + 3;
    }
    switch (self->step) {
    case 0:
        if (!self->params) {
            InitializeEntity(D_us_801808D8);
            self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
            self->velocityX = self->facingLeft ? FIX(4) : FIX(-4);
            self->velocityY = FIX(4);
            self->hitboxOffX = -4;
            self->hitboxOffY = 5;
        } else {
            InitializeEntity(D_us_801808E4);
            angle = self->params << 7;
            self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
            if (self->facingLeft) {
                self->facingLeft = 0;
                angle = -angle;
            }
            self->rotate = angle;
            self->drawFlags = ENTITY_ROTATE;
            self->posX.i.hi += (rsin(angle) * 0x10) >> 0xC;
            self->posY.i.hi -= (rcos(angle) * 0x10) >> 0xC;
            self->velocityX = rsin(angle) * 0x70;
            self->velocityY = -(rcos(angle) * 0x70);
            self->hitboxOffX = (u32)(rsin(angle) * 5) >> 0xB;
            self->hitboxOffY = -(rcos(angle) * 0xA) >> 0xC;
        }
        // fallthrough
    case 1:
        MoveEntity();
        if (self->hitFlags & 0x80) {
            self->ext.rdaiArrow.offsetX = PLAYER.posX.i.hi - self->posX.i.hi;
            self->ext.rdaiArrow.timer = 0x20;
            self->hitboxState = 0;
            self->step++;
            self->ext.rdaiArrow.offsetY = PLAYER.posY.i.hi - self->posY.i.hi;
            break;
        }
        if (self->hitFlags || (self->flags & FLAG_DEAD)) {
            self->hitboxState = 0;
            self->step_s = 0;
            self->step = 3;
        }
        break;
    case 2:
        self->posX.i.hi = PLAYER.posX.i.hi - self->ext.rdaiArrow.offsetX;
        self->posY.i.hi = PLAYER.posY.i.hi - self->ext.rdaiArrow.offsetY;
        if (!--self->ext.rdaiArrow.timer) {
            DestroyEntity(self);
        }
        break;
    case 3:
        // blocked: bounces away spinning and flickering
        if (!self->step_s) {
            self->drawFlags |= ENTITY_ROTATE;
            self->velocityX = -self->velocityX / 2;
            self->velocityY = -self->velocityY / 2;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += FIX(1.0 / 8);
        self->rotate += 0x40;
        if (g_Timer & 1) {
            self->animCurFrame = 0;
        } else {
            self->animCurFrame = self->params ? 0x3A : 0x11;
        }
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u8 D_us_8018191C[8] = { 4, 8, 4, 7, 0x10, 6, 0xFF, 0 };
u8 D_us_80181924[8] = { 0xC, 8, 0xC, 7, 0x20, 6, 0xFF, 0 };
u8 D_us_8018192C[8] = { 0x20, 6, 0x10, 7, 0x10, 8, 0xFF, 0 };
u8 D_us_80181934[0x28] = {
    2,
    1,
    4,
    2,
    2,
    3,
    1,
    4,
    2,
    5,
    1,
    6,
    1,
    7,
    2,
    8,
    2,
    9,
    4,
    8,
    2,
    0xA,
    6,
    0xB,
    2,
    0xC,
    1,
    6,
    1,
    5,
    1,
    4,
    1,
    3,
    1,
    2,
    0,
    0,
    0,
    0,
};
u8 D_us_8018195C[0x30] = {
    1,
    2,
    1,
    5,
    1,
    8,
    1,
    0xB,
    1,
    5,
    1,
    1,
    1,
    2,
    1,
    3,
    1,
    4,
    1,
    5,
    1,
    6,
    1,
    7,
    1,
    8,
    1,
    9,
    2,
    8,
    1,
    0xA,
    2,
    0xB,
    1,
    0xC,
    1,
    6,
    1,
    5,
    1,
    4,
    1,
    3,
    1,
    2,
    0,
    0,
};
u8 D_us_8018198C[0x18] = {
    0x21,
    0xD,
    0x11,
    0xE,
    1,
    0xF,
    1,
    0x10,
    1,
    0xF,
    1,
    0x10,
    2,
    0xF,
    2,
    0x10,
    3,
    0xF,
    0x14,
    0x10,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801819A4[0x38] = {
    2,
    0x12,
    4,
    0x13,
    1,
    0x14,
    1,
    0x15,
    2,
    0x16,
    1,
    0x17,
    1,
    0x18,
    1,
    0x19,
    1,
    0x1A,
    1,
    0x1B,
    1,
    0x1C,
    1,
    0x1D,
    1,
    0x1E,
    1,
    0x1F,
    1,
    0x20,
    4,
    0x21,
    1,
    0x22,
    1,
    0x23,
    1,
    0x1D,
    1,
    0x1C,
    1,
    0x1B,
    1,
    0x19,
    1,
    0x17,
    1,
    0x16,
    1,
    0x15,
    1,
    0x14,
    1,
    0x13,
    0,
    0,
};
u8 D_us_801819DC[0x38] = {
    2,
    0x24,
    4,
    0x25,
    1,
    0x26,
    1,
    0x27,
    2,
    0x28,
    1,
    0x29,
    1,
    0x2A,
    1,
    0x2B,
    1,
    0x2C,
    1,
    0x2D,
    1,
    0x2E,
    1,
    0x2F,
    1,
    0x30,
    1,
    0x31,
    1,
    0x32,
    4,
    0x33,
    1,
    0x34,
    1,
    0x35,
    1,
    0x2F,
    1,
    0x2E,
    1,
    0x2D,
    1,
    0x2B,
    1,
    0x29,
    1,
    0x28,
    1,
    0x27,
    1,
    0x26,
    1,
    0x25,
    0,
    0,
};
u8 D_us_80181A14[8] = { 8, 0x36, 8, 0x37, 0, 0, 0, 0 };
u8 D_us_80181A1C[0x1C] = {
    1,
    0x3B,
    1,
    0x3F,
    1,
    0x40,
    1,
    0x3C,
    1,
    0x3F,
    1,
    0x40,
    1,
    0x3D,
    1,
    0x3F,
    1,
    0x40,
    1,
    0x3E,
    1,
    0x3F,
    1,
    0x40,
    0,
    0,
    0,
    0,
};
EInit D_us_801808CC = { 0x8005, 0, 0x4C, 0x224, 0xC3 };
EInit D_us_801808D8 = { 0x8005, 0x11, 0x4C, 0x224, 0xC4 };
EInit D_us_801808E4 = { 0x8005, 0x3A, 0x4C, 0x224, 0xC5 };
// end pc_import_data.py
// clang-format on
#endif
