// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#include "../step_towards.h"

extern u16 GetAngleBetweenEntities(Entity* a, Entity* b);
extern EInit g_EInitSpectralSwordRDAI;
extern u8 D_us_80181914[];
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/e_spectral_sword", func_us_801C0C44);
#else
// The Spectral Sword: keeps a line of spears between itself and Alucard and
// a wall of shields in front, circles him, and sends the spears in
void func_us_801C0C44(Entity* self) {
    Entity* entity;
    Primitive* prim;
    s32 primIndex;
    s32 dx;
    s32 dy;
    s32 dist;
    s32 offset;
    s32 radius;
    s32 x;
    s32 y;
    s32 cx;
    s32 cy;
    s16 angle;
    s16 perp;
    s32 count;
    s32 i;

    if ((self->flags & FLAG_DEAD) && self->step < 8) {
        SetStep(8);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitSpectralSwordRDAI);
        self->animCurFrame = 1;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        self->hitboxOffX = 0;
        self->hitboxOffY = 4;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 1);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.rdaiSword.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        prim->u0 = prim->v0 = 2;
        PGREY(prim, 0) = 0xC0;
        prim->priority = 0xC0;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
        entity = self + 1;
        for (i = 0; i < 6; i++, entity++) {
            CreateEntityFromCurrentEntity(E_UNK_24, entity);
            entity->params = i | (D_us_80181914[i] << 8);
        }
        for (i = 0; i < 4; i++, entity++) {
            CreateEntityFromCurrentEntity(E_UNK_25, entity);
            entity->params = i;
        }
        SetStep(2);
        self->ext.rdaiSword.closeTimer = 0x40;
        self->ext.rdaiSword.attackTimer = 0x40;
        self->ext.rdaiSword.spears = 6;
        self->ext.rdaiSword.shields = 4;
        break;
    case 2:
        if (GetDistanceToPlayerX() < 0x60) {
            SetStep(3);
        }
        break;
    case 3:
        SetStep(6);
        self->ext.rdaiSword.side = GetSideToPlayer() & 1;
        break;
    case 6:
        if (!self->step_s) {
            angle = Random() * 2;
            if (self->ext.rdaiSword.side) {
                angle += 0x100;
            } else {
                angle += 0x500;
            }
            dx = (rcos(angle) * 0x60) >> 12;
            dy = -(rsin(angle) * 0x60) >> 12;
            prim = self->ext.rdaiSword.prim;
            prim->x0 = PLAYER.posX.i.hi + dx;
            prim->y0 = PLAYER.posY.i.hi + dy;
            angle = ratan2(prim->y0 - self->posY.i.hi,
                           prim->x0 - self->posX.i.hi);
            self->velocityX = rcos(angle) * 12;
            self->velocityY = rsin(angle) * 12;
            self->ext.rdaiSword.timer = 0x80;
            self->step_s++;
        }
        MoveEntity();
        prim = self->ext.rdaiSword.prim;
        dx = abs(prim->x0 - self->posX.i.hi);
        dy = abs(prim->y0 - self->posY.i.hi);
        if ((dx < 8 && dy < 8) || !--self->ext.rdaiSword.timer) {
            self->step_s = 0;
        }
        dx = (s16)PLAYER.posX.i.hi - self->posX.i.hi;
        if (self->ext.rdaiSword.side) {
            dx = -dx;
        }
        if (dx > 0x80) {
            self->velocityY = 0;
            if (self->ext.rdaiSword.side) {
                self->velocityX = FIX(-0.75);
            } else {
                self->velocityX = FIX(0.75);
            }
        }
        if (dx < 0x40) {
            self->velocityY = 0;
            if (self->ext.rdaiSword.side) {
                self->velocityX = FIX(0.75);
            } else {
                self->velocityX = FIX(-0.75);
            }
            if (!--self->ext.rdaiSword.closeTimer) {
                SetStep(5);
            }
        } else {
            self->ext.rdaiSword.closeTimer = 0x40;
        }
        if (self->ext.rdaiSword.spears &&
            !--self->ext.rdaiSword.attackTimer) {
            switch (Random() & 3) {
            default: // one spear charges
                self->ext.rdaiSword.attackTimer = 0x100;
                entity = self + 1;
                i = 0;
                count = (Random() % self->ext.rdaiSword.spears) + 1;
                while (true) {
                    if (!(entity->flags & FLAG_DEAD)) {
                        count--;
                    }
                    i++;
                    if (!count) {
                        break;
                    }
                    entity++;
                    if (i >= 6) {
                        break;
                    }
                }
                entity->ext.rdaiSword.attack = 1;
                break;
            case 2: // all at once, toward Alucard
                self->ext.rdaiSword.attackTimer = 0x180;
                angle = GetAngleBetweenEntities(self, &PLAYER);
                entity = self + 1;
                for (i = 0; i < 6; i++, entity++) {
                    entity->ext.rdaiSword.attack = 3;
                    entity->ext.rdaiSword.angle = angle;
                }
                break;
            case 3: // one after the other
                self->ext.rdaiSword.attackTimer = 0x280;
                offset = 0x40;
                entity = self + 1;
                for (i = 0; i < 6; i++, entity++) {
                    if (!(entity->flags & FLAG_DEAD)) {
                        entity->ext.rdaiSword.attackTimer = offset;
                        offset += 0x40;
                        entity->ext.rdaiSword.attack = 5;
                    }
                }
                break;
            }
        }
        break;
    case 5:
        switch (self->step_s) {
        case 0:
            angle = ratan2(0x40 - self->posY.i.hi, 0x80 - self->posX.i.hi);
            self->velocityX = rcos(angle) * 0x30;
            self->velocityY = rsin(angle) * 0x30;
            self->ext.rdaiSword.side ^= 1;
            self->step_s++;
            break;
        case 1:
            MoveEntity();
            if (self->ext.rdaiSword.side) {
                if (self->posX.i.hi > 0x80) {
                    self->velocityX = FIX(3);
                    self->step_s++;
                }
            } else if (self->posX.i.hi < 0x80) {
                self->velocityX = FIX(-3);
                self->step_s++;
            }
            break;
        case 2:
            MoveEntity();
            self->velocityY += FIX(1.0 / 8);
            dx = (s16)PLAYER.posX.i.hi - self->posX.i.hi;
            if (self->ext.rdaiSword.side) {
                dx = -dx;
            }
            if (dx > 0x70) {
                SetStep(6);
            }
            break;
        }
        break;
    case 8:
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            entity = self + 1;
            for (i = 0; i < 10; i++, entity++) {
                entity->flags |= FLAG_DEAD;
            }
            self->rotate = 0;
            self->drawFlags |= ENTITY_ROTATE;
            PlaySfxPositional(SFX_STUTTER_EXPLODE_A);
            self->step_s++;
            break;
        case 1:
            MoveEntity();
            self->velocityY += FIX(0.09375);
            self->rotate -= 4;
            if (!(g_Timer & 7)) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_EXPLOSION, self, entity);
                    entity->params = 3;
                }
            }
            if (self->posY.i.hi > 0x160) {
                self->animCurFrame = 0;
                self->step_s++;
            }
            break;
        case 2:
            count = 0;
            entity = self + 1;
            for (i = 0; i < 10; i++, entity++) {
                if (!entity->animCurFrame) {
                    count++;
                }
            }
            if (count == 10) {
                entity = self;
                for (i = 0; i < 11; i++, entity++) {
                    DestroyEntity(entity);
                }
                return;
            }
            break;
        }
        break;
    }

    if (self->flags & FLAG_DEAD) {
        return;
    }
    // the spears line up across the line to Alucard, the shields on it
    dx = (s16)PLAYER.posX.i.hi - self->posX.i.hi;
    dy = (s16)PLAYER.posY.i.hi - self->posY.i.hi;
    angle = ratan2(dy, dx);
    perp = angle + 0x400;
    dist = SquareRoot0(dx * dx + dy * dy);
    x = self->posX.val;
    y = self->posY.val;
    entity = self + 1;
    offset = -0x30;
    for (i = 0; i < 6; i++, entity++) {
        entity->ext.rdaiSword.targetX.val = x + offset * rcos(perp) * 16;
        entity->ext.rdaiSword.targetY.val = y + offset * rsin(perp) * 16;
        offset += 0x10;
        if (!offset) {
            offset = 0x10;
        }
    }
    radius = dist / 2;
    if (radius < 0x20) {
        radius = 0x20;
    }
    if (radius > 0x48) {
        radius = 0x48;
    }
    cx = x + radius * rcos(angle) * 16;
    perp = angle + 0x400;
    offset = (self->ext.rdaiSword.shields >> 1) * -0x18;
    cy = y + radius * rsin(angle) * 16;
    for (i = 0; i < 4; i++, entity++) {
        if (!(entity->flags & FLAG_DEAD)) {
            entity->ext.rdaiSword.angle = angle;
            entity->ext.rdaiSword.targetX.val = cx + offset * rcos(perp) * 16;
            entity->ext.rdaiSword.targetY.val = cy + offset * rsin(perp) * 16;
            offset += 0x18;
        }
    }
    if (!(self->flags & 0xF)) {
        self->palette = (g_Timer & 1) ? 0x215 : 0x216;
    }
    if (!(g_Timer & 0xF)) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_UNK_23, self, entity);
            entity->ext.et_801CEB08.unk80 = self;
            entity->zPriority = self->zPriority + 1;
        }
    }
}
#endif

#include "../e_spectral_sword_aura.h"

extern EInit g_EInitSpear;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/e_spectral_sword", func_us_801C17E8);
#else
// A spear of the Spectral Sword, params & 0xFF slots after it (params &
// 0x100: a long one). Holds its place in the line until the sword sends it.
void func_us_801C17E8(Entity* self) {
    Entity* other;
    s32 dx;
    s32 dy;
    s32 dist;
    s32 side;
    s32 hit;
    s32 radius;
    s16 angle;
    u8 index;

    if ((self->flags & FLAG_DEAD) && self->step < 8) {
        SetStep(8);
    }
    radius = 0x16;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitSpear);
        self->drawFlags = ENTITY_ROTATE;
        if (self->params & 0x100) {
            self->animCurFrame = 4;
        } else {
            self->animCurFrame = 5;
        }
        SetStep(2);
        break;
    case 2:
        MoveEntity();
        dx = (s16)self->ext.rdaiSword.targetX.i.hi - self->posX.i.hi;
        dy = (s16)self->ext.rdaiSword.targetY.i.hi - self->posY.i.hi;
        dist = SquareRoot0(dx * dx + dy * dy);
        if (dist > 0x38) {
            dist = 0x38;
        }
        angle = ratan2(dy, dx);
        self->velocityX = dist * rcos(angle);
        self->velocityY = dist * rsin(angle);
        StepTowards(&self->rotate,
                    (s16)(GetAngleBetweenEntities(self, &PLAYER) - 0x400), 0x10);
        if (self->ext.rdaiSword.attack &&
            (!(self->ext.rdaiSword.attack & 4) ||
             !--self->ext.rdaiSword.attackTimer)) {
            SetStep(4);
        }
        break;
    case 4:
        switch (self->step_s) {
        case 0:
            if (!(self->ext.rdaiSword.attack & 2)) {
                self->ext.rdaiSword.angle =
                    GetAngleBetweenEntities(self, &PLAYER);
            }
            self->velocityX = rcos(self->ext.rdaiSword.angle - 0x800) << 5;
            index = self->params;
            self->velocityY = rsin(self->ext.rdaiSword.angle - 0x800) << 5;
            // the shield in front of this spear joins in
            other = self - index + 6 + (index * 4) / 6;
            other->ext.rdaiSword.attack =
                (self->ext.rdaiSword.attack & 2) ? 2 : 1;
            self->step_s++;
            break;
        case 1:
            MoveEntity();
            self->velocityX -= self->velocityX / 16;
            self->velocityY -= self->velocityY / 16;
            if (abs(self->velocityX) < 0x2000 &&
                abs(self->velocityY) < 0x2000) {
                self->step_s++;
            }
            StepTowards(&self->rotate,
                        (s16)(ratan2(self->velocityY, self->velocityX) - 0x400),
                        8);
            break;
        case 2:
            angle = self->ext.rdaiSword.angle;
            if (StepTowards(&self->rotate, angle - 0x400, 0x20)) {
                self->velocityX = rcos(angle) << 7;
                self->velocityY = rsin(angle) << 7;
                self->ext.rdaiSword.timer = 0x30;
                PlaySfxPositional(SFX_WEAPON_SWISH_A);
                self->step_s++;
            }
            break;
        case 3:
            MoveEntity();
            side = GetSideToPlayer();
            if (abs(self->velocityY) < abs(self->velocityX)) {
                hit = side & 1;
                if (self->velocityX < 0) {
                    hit ^= 1;
                }
            } else {
                hit = side & 2;
                if (self->velocityY < 0) {
                    hit ^= 2;
                }
            }
            if (hit || !--self->ext.rdaiSword.timer) {
                self->ext.rdaiSword.attack = 0;
                SetStep(2);
            }
            break;
        }
        break;
    case 8:
        switch (self->step_s) {
        case 0:
            other = self - (u8)self->params - 1;
            other->ext.rdaiSword.spears--;
            self->hitboxState = 0;
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
    angle = self->rotate + 0x400;
    if (self->params & 0x100) {
        radius = 0x18;
    }
    self->hitboxOffX = (radius * rcos(angle)) >> 12;
    self->hitboxOffY = (radius * rsin(angle)) >> 12;
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u8 D_us_80181914[8] = { 0, 1, 0, 0, 1, 0, 0, 0 };
// end pc_import_data.py
// clang-format on
#endif
