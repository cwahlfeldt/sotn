// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo1.h"

extern u8* D_us_801805E0[];
extern u8 D_us_80180600[];
extern u8 D_us_80180608[];
extern u16 D_us_80180610[];
extern u8 D_us_80180620[];

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", EntityBreakable);
#else
// RBO1's breakable tables have no names yet
#define g_eBreakableAnimations D_us_801805E0
#define g_eBreakableHitboxes D_us_80180600
#define g_eBreakableExplosionTypes D_us_80180608
#define g_eBreakableanimSets D_us_80180610
#define blend_modes D_us_80180620
#include "../../st/e_breakable.h"
#undef g_eBreakableAnimations
#undef g_eBreakableHitboxes
#undef g_eBreakableExplosionTypes
#undef g_eBreakableanimSets
#undef blend_modes
#endif

// a part of Beelzebub's body
typedef struct {
    s16 eArrayOffset;
    s16 eArrayParentOffset;
    s16 length;
    s16 animCurFrame;
    s16 zOffset;
    s16 rotOffset;
} BeelzebubPartInit;

void func_us_8019ED80_from_rbo2(Entity* self);
void polarPlacePartsWithAngvel(s16* entOffsets);
void func_801CDD00(Entity* entity, s16 arg1, s16 arg2);
void func_801CDD80(s16* entOffsets, unkStr_801CDD80* arg1);
void func_801CDF1C(s16 entIndices[], unkStr_801CDD80* arg1, s32 arg2);
void func_801CE1E8(s32 step);
void func_801CE228(void);
void polarPlacePartsList(s16* offsets);
void func_us_8019D260_from_rcen(void);

extern EInit g_EInitBeezelbub;
extern s32 bossFlags;
extern s32 D_us_80180630;
extern s32 D_us_80180634;
extern BeelzebubPartInit D_us_8018063C[];
extern s16 D_us_801806F0[];
extern s16 D_us_8018070C[];
extern unkStr_801CDD80 D_us_80180770[];
extern unkStr_801CDD80 D_us_801807D0[];
extern u8 D_us_80180840[];
extern s8 D_us_8018088C[];
extern u8 D_us_801808D0[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_801923A8);
#else
// Beelzebub. His parts share his hit points through D_us_80180630
void func_us_801923A8(Entity* self) {
    Entity* entity;
    Entity* prev;
    BeelzebubPartInit* parts;
    s8* hitbox;
    s16* p;
    s32 i;
    s32 count;

    FntPrint("boss_life %x\n", D_us_80180630);
    if ((self->flags & FLAG_DEAD) && self->step != 4) {
        func_801CE1E8(4);
    }
    if (self->step < 4) {
        switch (self->step) {
        case 0:
            prev = self;
            InitializeEntity(g_EInitBeezelbub);
            self->animCurFrame = 0x14;
            D_us_80180634 = 0;
            self->ext.beelzebub.prevHp = self->hitPoints;
            D_us_80180630 = self->hitPoints;
            hitbox = &D_us_8018088C[D_us_801808D0[self->animCurFrame] * 4];
            self->hitboxOffX = hitbox[0];
            self->hitboxOffY = hitbox[1];
            self->hitboxWidth = (u8)hitbox[2];
            self->hitboxHeight = (u8)hitbox[3];
            for (parts = D_us_8018063C, i = 0; parts->eArrayOffset; parts++) {
                entity = self + parts->eArrayOffset;
                CreateEntityFromCurrentEntity(E_ID(UNK_17), entity);
                entity->ext.beelzebub.length = parts->length;
                entity->ext.beelzebub.parent =
                    self + parts->eArrayParentOffset;
                entity->params = i;
                entity->ext.beelzebub.rotOffset = parts->rotOffset;
                i++;
                entity->nextPart = prev;
                prev = entity;
                entity->zPriority = self->zPriority + parts->zOffset;
            }
            self->nextPart = prev;
            CreateEntityFromCurrentEntity(E_ID(UNK_18), self + 24);
            (self + 24)->ext.beelzebubChains.body = self;
            func_801CE1E8(1);
            // fallthrough
        case 1:
            polarPlacePartsList(D_us_801806F0);
            if (bossFlags & 1) {
                func_801CE1E8(2);
            }
            break;
        default:
            // every 0x180 frames, a swarm: two flies more for each part lost
            if (!self->ext.beelzebub.timer) {
                g_api.PlaySfx(0x7D5);
                count = D_us_80180634 + 2;
                self->ext.beelzebub.timer = 0x180;
                for (i = 0; i < count; i++) {
                    entity = AllocEntity(&g_Entities[128], &g_Entities[152]);
                    if (entity != NULL) {
                        CreateEntityFromCurrentEntity(E_ID(UNK_19), entity);
                        entity->params = i & 1;
                        entity->ext.beelzebub.parent = self;
                        entity->posY.i.hi = (i / 2) << 6;
                        entity->rotate = (0x1000 / count) * i;
                        if (i & 1) {
                            entity->posX.i.hi = self->posX.i.hi + 0xC0;
                        } else {
                            entity->posX.i.hi = self->posX.i.hi - 0xC0;
                        }
                        entity->zPriority = self->zPriority;
                    }
                }
            }
            self->ext.beelzebub.timer--;
            switch (self->step) {
            case 2:
                AnimateEntity(D_us_80180840, self);
                func_801CDF1C(D_us_8018070C, D_us_801807D0, 0);
                polarPlacePartsWithAngvel(D_us_8018070C);
                polarPlacePartsList(D_us_801806F0);
                if (self->hitFlags & 3) {
                    g_api.PlaySfx(0x8C2);
                    func_801CE1E8(3);
                }
                break;
            case 3:
                func_801CDF1C(D_us_8018070C, D_us_80180770, 0);
                polarPlacePartsWithAngvel(D_us_8018070C);
                polarPlacePartsList(D_us_801806F0);
                if (!self->ext.beelzebub.unkB4[0] &&
                    !self->ext.beelzebub.unkB0[0]) {
                    func_801CE1E8(2);
                }
                break;
            }
            break;
        }
    } else {
        switch (self->step) {
        case 4:
            // death: fall to the floor in flames
            switch (self->step_s) {
            case 0:
                D_us_80180630 = -1;
                self->hitboxState = 0;
                bossFlags |= 2;
                self->step_s++;
                self->drawFlags |= ENTITY_ROTATE;
                // fallthrough
            case 1:
                MoveEntity();
                self->velocityY += 0x400;
                self->rotate++;
                if (!(g_Timer & 7)) {
                    PlaySfxPositional(0x654);
                }
                if (!(g_Timer & 3)) {
                    entity = AllocEntity(&g_Entities[80], &g_Entities[248]);
                    if (entity != NULL) {
                        CreateEntityFromEntity(E_ID(UNK_1D), self, entity);
                        entity->posY.i.hi -= Random() & 0xF;
                        entity->posX.i.hi =
                            entity->posX.i.hi - 0x20 + (Random() & 0x3F);
                        entity->zPriority = self->zPriority + 1;
                    }
                    func_us_8019D260_from_rcen();
                }
                if (self->posY.i.hi + g_Tilemap.scrollY.i.hi + 0x18 > 0x1C0) {
                    self->ext.beelzebub.timer = 0x80;
                    PlaySfxPositional(0x7C5);
                    self->step_s++;
                }
                break;
            case 2:
                if (!(g_Timer & 7)) {
                    PlaySfxPositional(0x654);
                    for (i = 0; i < 12; i++) {
                        entity =
                            AllocEntity(&g_Entities[80], &g_Entities[248]);
                        if (entity != NULL) {
                            CreateEntityFromEntity(E_ID(UNK_1D), self, entity);
                            entity->rotate = ((Random() & 0x7F) << 4) - 0x400;
                            entity->params = 3;
                            entity->zPriority = self->zPriority + 1;
                        }
                    }
                }
                if (!--self->ext.beelzebub.timer) {
                    self->ext.beelzebub.timer = 0x40;
                    self->step_s++;
                }
                break;
            case 3:
                if (self->ext.beelzebub.timer < 0x30) {
                    self->animCurFrame = 0;
                }
                entity = AllocEntity(&g_Entities[80], &g_Entities[248]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_ID(UNK_1D), self, entity);
                    entity->params = 4;
                    entity->zPriority = self->zPriority + 1;
                    entity->posY.i.hi += 0x10;
                }
                if (!--self->ext.beelzebub.timer) {
                    self->step_s++;
                }
                break;
            case 4:
                bossFlags |= 4;
                break;
            }
            break;
        case 0xFF:
#include "../../st/pad2_anim_debug.h"
        }
    }
    for (p = D_us_801806F0; *p; p++) {
        (self + *p)->ext.beelzebub.unkA8 = 0;
    }
    if (!(self->flags & FLAG_DEAD)) {
        if (D_us_80180630 < 0) {
            D_us_80180630 = 1;
        }
        if (D_us_80180630 != self->ext.beelzebub.prevHp && self->step != 3) {
            func_801CE1E8(3);
        }
        D_us_80180630 += self->hitPoints - self->ext.beelzebub.prevHp;
        self->hitPoints = D_us_80180630;
        self->ext.beelzebub.prevHp = (s16)D_us_80180630;
    }
}
#endif

extern EInit g_EInitBeezelbub801804A0;
extern s16 D_us_801806D8[];
extern u8 D_us_8018084C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_80192C5C);
#else
// a part of his body: it breaks off once the shared hit points drop below
// its threshold and it is hit, or its parent part broke
void func_us_80192C5C(Entity* self) {
    Entity* entity;
    s8* hitbox;
    s32 i;
    s32 damage;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitBeezelbub801804A0);
        self->ext.beelzebub.prevHp = self->hitPoints;
        self->animCurFrame = D_us_8018063C[self->params].animCurFrame;
        self->drawFlags |= ENTITY_ROTATE;
        hitbox = &D_us_8018088C[D_us_801808D0[self->animCurFrame] * 4];
        self->hitboxOffX = hitbox[0];
        self->hitboxOffY = hitbox[1];
        self->hitboxWidth = (u8)hitbox[2];
        self->hitboxHeight = (u8)hitbox[3];
        for (i = 0; i < 3; i++) {
            entity = AllocEntity(&g_Entities[216], &g_Entities[248]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1A), self, entity);
                entity->ext.beelzebub.parent = self;
                entity->zPriority = self->zPriority;
            }
        }
        break;
    case 1:
        self->rotate = self->ext.beelzebub.rotate;
        if (!(Random() & 0x3F)) {
            entity = AllocEntity(&g_Entities[112], &g_Entities[128]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1B), self, entity);
                entity->ext.beelzebub.parent = self;
                entity->zPriority = self->zPriority + 1;
                entity->posX.i.hi = entity->posX.i.hi - 8 + (Random() & 0xF);
                entity->posY.i.hi = entity->posY.i.hi - 8 + (Random() & 0xF);
            }
        }
        damage = self->hitPoints - self->ext.beelzebub.prevHp;
        self->ext.beelzebub.prevHp = self->hitPoints;
        D_us_80180630 += damage;
        if (D_us_80180630 < D_us_801806D8[self->params] &&
            ((self->ext.beelzebub.parent->flags & FLAG_DEAD) ||
             (self->hitFlags & 3))) {
            D_us_80180634++;
            SetStep(2);
        }
        break;
    case 2:
        entity = AllocEntity(&g_Entities[192], &g_Entities[248]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_ID(UNK_1C), self, entity);
            entity->params = self->animCurFrame;
            entity->zPriority = self->zPriority;
            entity->rotate = self->rotate;
        }
        self->animCurFrame = 0;
        self->hitboxState = 0;
        self->flags |= FLAG_DEAD;
        self->step++;
        return;
    case 3:
        return;
    }
    if (!self->params) {
        AnimateEntity(D_us_8018084C, self);
    }
}
#endif

extern EInit g_EInitInteractable;
extern s32 D_us_80180638;
extern s16 D_us_801807F8[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_80192F84);
#else
// the chains, from anchors on the ceiling to points on seven parts; each
// entry of D_us_801807F8 is {part, x, y on the part, anchor x, y}. A broken
// part's chain swings free
void func_us_80192F84(Entity* self) {
    Entity* part;
    Primitive* prim;
    s16* chain;
    s32 primIndex;
    s32 i;
    s32 j;
    s32 used;
    s32 fx;
    s32 fy;
    s32 dx;
    s32 dy;
    s32 sinA;
    s32 cosA;
    s32 anchorX;
    s32 anchorY;
    s32 targetY;
    s32 accX;
    s32 accY;
    s32 x;
    s32 y;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.func_800EDB58(PRIM_SPRT | 0x10, 0x100);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.beelzebubChains.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->tpage = 0x12;
            prim->clut = 0x200;
            prim->u0 = 0xD0;
            prim->v0 = 0x70;
            prim->u1 = prim->v1 = 8;
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority;
            prim = prim->next;
        }
        return;
    case 1:
        // one link per 6 pixels, plus two
        for (i = 0, chain = D_us_801807F8; i < 7; i++, chain += 5) {
            part = self->ext.beelzebubChains.body + chain[0];
            angle = part->rotate;
            sinA = rsin(angle);
            cosA = rcos(angle);
            dx = (chain[3] - g_Tilemap.scrollX.i.hi) -
                 (((chain[1] * cosA - chain[2] * sinA) >> 12) +
                  part->posX.i.hi);
            dy = (chain[4] - g_Tilemap.scrollY.i.hi) -
                 (((chain[1] * sinA + chain[2] * cosA) >> 12) +
                  part->posY.i.hi);
            self->ext.beelzebubChains.links[i] =
                SquareRoot0(dx * dx + dy * dy) / 6 + 2;
        }
        self->step++;
        // fallthrough
    case 2:
        prim = self->ext.beelzebubChains.prim;
        used = 0;
        for (i = 0, chain = D_us_801807F8; i < 7; i++, chain += 5) {
            part = self->ext.beelzebubChains.body + chain[0];
            if (part->flags & FLAG_DEAD) {
                if (!((D_us_80180638 >> i) & 1)) {
                    g_api.PlaySfx(0x602);
                    D_us_80180638 |= 1 << i;
                }
                // each link hangs at most 6 pixels below the one before;
                // x1 and y1 keep the fractions
                fx = (chain[3] - g_Tilemap.scrollX.i.hi) << 8;
                fy = (chain[4] - g_Tilemap.scrollY.i.hi) << 8;
                for (j = 0; j < self->ext.beelzebubChains.links[i]; j++) {
                    dx = ((prim->x0 << 8) | prim->x1) - fx;
                    dy = ((prim->y0 << 8) | prim->y1) - fy + 0x800;
                    angle = ratan2(dy, dx);
                    if (dx * dx + dy * dy > 0x240000) {
                        dx = (rcos(angle) * 6) >> 4;
                        dy = (rsin(angle) * 6) >> 4;
                    }
                    fx += dx;
                    fy += dy;
                    prim->x1 = fx & 0xFF;
                    prim->y1 = fy & 0xFF;
                    prim->drawMode = DRAW_UNK02;
                    prim->x0 = fx >> 8;
                    prim->y0 = fy >> 8;
                    if ((u32)prim->x0 > 0x100 || (u32)(prim->y0 - 0x10) > 0xE0) {
                        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
                    }
                    prim = prim->next;
                }
            } else {
                angle = part->rotate;
                sinA = rsin(angle);
                cosA = rcos(angle);
                anchorX = chain[3] - g_Tilemap.scrollX.i.hi;
                anchorY = chain[4] - g_Tilemap.scrollY.i.hi;
                dx = (((chain[1] * cosA - chain[2] * sinA) >> 12) +
                      part->posX.i.hi) -
                     anchorX;
                targetY = ((chain[1] * sinA + chain[2] * cosA) >> 12) +
                          part->posY.i.hi;
                angle = ratan2(targetY - anchorY, dx);
                sinA = rsin(angle);
                cosA = rcos(angle);
                accX = 0;
                accY = sinA * 6;
                for (j = 0; j < self->ext.beelzebubChains.links[i]; j++) {
                    accX += cosA * 6;
                    x = anchorX + (accX >> 12);
                    y = (s16)(anchorY + (accY >> 12));
                    prim->drawMode = DRAW_UNK02;
                    prim->x0 = x;
                    prim->y0 = y;
                    if ((u32)x > 0x100 || (u32)(y - 0x10) > 0xE0) {
                        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
                    }
                    if (targetY < y) {
                        prim->drawMode |= DRAW_HIDE;
                    }
                    used++;
                    prim = prim->next;
                    accY += sinA * 6;
                }
            }
        }
        FntPrint("used_work %x\n", used);
        while (prim->next != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        prim->tpage = 0x12;
        prim->clut = 0x200;
        prim->x0 = prim->y0 = 0;
        prim->drawMode = DRAW_UNK02;
        break;
    }
}
#endif

extern EInit D_us_801804AC;
extern u8 D_us_80180854[];
extern u8 D_us_80180860[];
extern u8 D_us_80180868[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_801936FC);
#else
// a fly of the swarm: closes in on its place on a circle around him
// (params 0 from behind, 1 from in front), circles, then dives at Alucard
void func_us_801936FC(Entity* self) {
    Entity* entity;
    Entity* body;
    s32 targetX;
    s32 targetY;
    s32 dx;
    s32 dy;
    s32 dist;
    s16 angle;

    self->rotate += 0x10;
    if (bossFlags & 2) {
        self->flags |= FLAG_DEAD;
    }
    if (self->flags & FLAG_DEAD) {
        PlaySfxPositional(0x680);
        entity = AllocEntity(&g_Entities[192], &g_Entities[248]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, entity);
            entity->params = 3;
            entity->zPriority = self->zPriority;
        }
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804AC);
        self->animCurFrame = 0;
        self->hitboxState = 0;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
        if (self->params) {
            self->step = 3;
            self->zPriority += 6;
        } else {
            self->step = 2;
            self->zPriority -= 6;
        }
        return;
    case 2:
    case 3:
        if (self->step == 2) {
            AnimateEntity(D_us_80180860, self);
        } else {
            AnimateEntity(D_us_80180868, self);
        }
        angle = self->rotate;
        body = self->ext.beelzebub.parent;
        targetX = body->posX.i.hi + ((rcos(angle) * 0x60) >> 12);
        targetY = body->posY.i.hi + ((rsin(angle) * 0x60) >> 12);
        dx = targetX - self->posX.i.hi;
        dy = targetY - self->posY.i.hi;
        angle = ratan2(dy, dx);
        dist = SquareRoot0(dx * dx + dy * dy) - 3;
        self->posX.i.hi = targetX - ((dist * rcos(angle)) >> 12);
        self->posY.i.hi = targetY - ((dist * rsin(angle)) >> 12);
        if (self->step == 2) {
            // from behind: grows from small
            self->scaleX = 0x100 - dist;
            if ((s16)self->scaleX < 0x40) {
                self->scaleX = 0x40;
            }
        } else {
            // from in front: shrinks from large
            self->scaleX = dist + 0x100;
            if ((s16)self->scaleX > 0x1C0) {
                self->scaleX = 0x1C0;
            }
        }
        self->scaleY = self->scaleX;
        if (dist < 2) {
            self->drawFlags = 0;
            if (self->step == 2) {
                self->zPriority += 0xC;
            }
            SetStep(4);
        }
        break;
    case 4:
        if (!self->step_s) {
            self->ext.beelzebub.timer = 0x80;
            self->hitboxState = 3;
            self->step_s++;
        }
        if (!AnimateEntity(D_us_80180854, self)) {
            self->facingLeft = GetSideToPlayer() & 1;
        }
        angle = self->rotate;
        body = self->ext.beelzebub.parent;
        self->posX.i.hi = body->posX.i.hi + ((rcos(angle) * 0x60) >> 12);
        self->posY.i.hi = body->posY.i.hi + ((rsin(angle) * 0x60) >> 12);
        if (!--self->ext.beelzebub.timer) {
            SetStep(5);
        }
        break;
    case 5:
        angle = GetAngleBetweenEntities(self, &PLAYER);
        self->velocityX = (rcos(angle) * FIX(2.5)) >> 12;
        self->velocityY = (rsin(angle) * FIX(2.5)) >> 12;
        if (self->velocityX > 0) {
            self->facingLeft = 0;
        } else {
            self->facingLeft = 1;
        }
        self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
        self->step++;
        // fallthrough
    case 6:
        MoveEntity();
        AnimateEntity(D_us_80180854, self);
        break;
    }
}
#endif

extern EInit D_us_801804C4;
extern u8 D_us_80180870[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_80193C2C);
#else
// a fly buzzing around a part; when the part breaks it follows the part's
// parent, and when he dies it flies away
void func_us_80193C2C(Entity* self) {
    Entity* target;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804C4);
        self->ext.beelzebub.angle = Random() * 0x10;
        self->pose = Random() & 1;
        self->poseTimer = Random() & 1;
        Random();
        self->zPriority = 0x92;
        // fallthrough
    case 1:
        target = self->ext.beelzebub.parent;
        if (target->flags & FLAG_DEAD) {
            target = target->ext.beelzebub.parent;
            self->ext.beelzebub.parent = target;
        }
        self->rotate = ratan2(target->posY.i.hi - self->posY.i.hi,
                              target->posX.i.hi - self->posX.i.hi);
        self->ext.beelzebub.timer = (Random() & 0x1F) + 0x10;
        self->step++;
        // fallthrough
    case 2:
        AnimateEntity(D_us_80180870, self);
        MoveEntity();
        angle =
            LimitAngleChange(0x18, self->ext.beelzebub.angle, self->rotate);
        self->velocityX = rcos(angle) * 0x18;
        self->velocityY = rsin(angle) * 0x18;
        self->ext.beelzebub.angle = angle;
        if (!--self->ext.beelzebub.timer) {
            self->step--;
        }
        if (bossFlags & 2) {
            self->step = 3;
        }
        return;
    case 3:
        MoveEntity();
        self->velocityY = FIX(-1.5);
        if (self->posY.i.hi < 0) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern EInit D_us_801804B8;
extern u8 D_us_80180878[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_80193E24);
#else
// a maggot crawling on a part; it drops to the floor and crawls away
void func_us_80193E24(Entity* self) {
    Entity* parent;
    s32 x;
    s16 angle;

    if (bossFlags & 2) {
        self->flags |= FLAG_DEAD;
    }
    if (self->flags & FLAG_DEAD) {
        self->step = 0;
        self->drawFlags = 0;
        self->pfnUpdate = EntityExplosion;
        self->params = 0;
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804B8);
        self->animCurFrame = 0xA;
        self->drawFlags = ENTITY_ROTATE;
        self->zPriority = 0x90;
        // fallthrough
    case 1:
        parent = self->ext.beelzebub.parent;
        if (parent->flags & FLAG_DEAD) {
            self->step = 2;
        }
        if (!self->step_s) {
            self->ext.beelzebub.angle =
                ratan2(parent->posY.i.hi - self->posY.i.hi,
                       parent->posX.i.hi - self->posX.i.hi);
            self->ext.beelzebub.timer = (Random() & 0xF) + 4;
            self->step_s++;
        }
        AnimateEntity(D_us_80180878, self);
        MoveEntity();
        angle =
            LimitAngleChange(0x20, self->rotate, self->ext.beelzebub.angle);
        self->velocityX = rcos(angle) * 4;
        self->velocityY = rsin(angle) * 4;
        self->rotate = angle;
        if (!--self->ext.beelzebub.timer) {
            self->step_s = 0;
            if (!(Random() & 0x7F)) {
                self->step = 2;
            }
        }
        return;
    case 2:
        self->flags |= FLAG_DESTROY_IF_BARELY_OUT_OF_CAMERA;
        MoveEntity();
        self->velocityY += 0x800;
        if (self->posY.i.hi + g_Tilemap.scrollY.i.hi + 1 > 0x1C0) {
            self->drawFlags = 0;
            self->velocityX = 0;
            self->velocityY = 0;
            self->posY.i.hi = 0x1BF - g_Tilemap.scrollY.i.hi;
            self->facingLeft = GetSideToPlayer() & 1;
            self->step++;
        }
        break;
    case 3:
        AnimateEntity(D_us_80180878, self);
        MoveEntity();
        if (self->facingLeft) {
            self->velocityX = FIX(-0.25);
        } else {
            self->velocityX = FIX(0.25);
        }
        x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (self->velocityX > 0) {
            if (x > 0x3D0) {
                x = 0;
            }
        } else if (x < 0xC0) {
            x = 0;
        }
        if (!x) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern EInit D_us_801804D0;
extern s16* sprites_rbo1_2[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_80194108);
#else
// a broken part falling and burning; params is its sprite
void func_us_80194108(Entity* self) {
    Entity* entity;
    s16* sprite;
    s32 bottom;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804D0);
        self->drawFlags = ENTITY_ROTATE;
        self->animCurFrame = self->params;
        self->ext.beelzebub.rotVel = (Random() & 3) + 2;
        PlaySfxPositional(0x7C7);
        // fallthrough
    case 1:
        MoveEntity();
        self->velocityY += 0x1000;
        self->rotate += self->ext.beelzebub.rotVel;
        // the sprite's top and height (its first part)
        sprite = sprites_rbo1_2[self->animCurFrame];
#ifdef VERSION_PC
        // two parts are invisible joints (frame 0, no sprite); PSX read
        // their sprite's fields from low memory
        if (sprite == NULL) {
            static s16 noSprite[6];
            sprite = noSprite;
        }
#endif
        bottom = sprite[3] + sprite[5];
        if (!(g_Timer & 7)) {
            entity = AllocEntity(&g_Entities[192], &g_Entities[248]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1D), self, entity);
                entity->posY.i.hi += sprite[3] + sprite[5] / 2;
                entity->params = 1;
                entity->posX.i.hi = entity->posX.i.hi - 8 + (Random() & 0xF);
                entity->zPriority = self->zPriority + 1;
            }
        }
        if (bottom + self->posY.i.hi + g_Tilemap.scrollY.i.hi > 0x1C0) {
            entity = AllocEntity(&g_Entities[192], &g_Entities[248]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_EXPLOSION, self, entity);
                entity->params = 3;
                entity->posY.i.hi += sprite[3] + sprite[5] / 2;
            }
            func_us_8019D260_from_rcen();
            DestroyEntity(self);
        }
        return;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_8019ED80_from_rbo2);
#else
// RBO1's polarPlacePart adds each part's rotOffset to its angle
void func_us_8019ED80_from_rbo2(Entity* self) {
    s16 angle;
    Entity* src;

    angle = self->ext.beelzebub.rotate + self->ext.beelzebub.rotOffset;
    if (g_CurrentEntity->facingLeft) {
        angle = -angle;
    }
    src = self->ext.beelzebub.parent;
    self->posX.val = src->posX.val;
    self->posY.val = src->posY.val;
    self->posX.val -= self->ext.beelzebub.length * rsin(angle) * 16;
    self->ext.beelzebub.unkA8 |= 1;
    self->posY.val += self->ext.beelzebub.length * rcos(angle) * 16;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", polarPlacePartsWithAngvel);
#else
// the giant brothers' helpers, as in giantbro_helpers.h (and RBO8), but
// with RBO1's polarPlacePart
void polarPlacePartsWithAngvel(s16* entOffsets) {
    Entity* ent;

    while (*entOffsets) {
        if (*entOffsets != 0xFF) {
            ent = g_CurrentEntity + *entOffsets;
            ent->ext.GH_Props.rotate += ent->ext.GH_Props.rotVel;
            func_us_8019ED80_from_rbo2(ent);
        }
        entOffsets++;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_801CDD00);
#else
void func_801CDD00(Entity* entity, s16 arg1, s16 arg2) {
    s16 temp_t0 = arg1 - entity->ext.GH_Props.rotate;

    if (temp_t0 > 0x800) {
        temp_t0 = temp_t0 - 0x1000;
    }

    if (temp_t0 < -0x800) {
        temp_t0 = temp_t0 + 0x1000;
    }

    temp_t0 = temp_t0 / arg2;
    entity->ext.GH_Props.rotVel = temp_t0;
    entity->ext.GH_Props.unkA4 = arg1;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_801CDD80);
#else
void func_801CDD80(s16* entOffsets, unkStr_801CDD80* arg1) {
    Entity* var_s1;
    s16* ptr = arg1->unk4;

    while (*entOffsets) {
        if (*entOffsets != 0xFF) {
            var_s1 = g_CurrentEntity + *entOffsets;
            func_801CDD00(var_s1, *ptr, arg1->unk0);
        }
        ptr++;
        entOffsets++;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_801CDF1C);
#else
void func_801CDF1C(s16 entIndices[], unkStr_801CDD80* arg1, s32 arg2) {

    arg1 += (u16)g_CurrentEntity->ext.GH_Props.unkB0[arg2];

    if (!g_CurrentEntity->ext.GH_Props.unkB4[arg2]) {
        func_801CDD80(entIndices, arg1);
        g_CurrentEntity->ext.GH_Props.unkB4[arg2] = arg1->unk0;
    }
    if (!--g_CurrentEntity->ext.GH_Props.unkB4[arg2]) {
        arg1++;
        if (!arg1->unk0) {
            g_CurrentEntity->ext.GH_Props.unkB0[arg2] = 0;
        } else {
            ++g_CurrentEntity->ext.GH_Props.unkB0[arg2];
        }
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_801CE1E8);
#else
void func_801CE1E8(s32 step) {
    s32 i;

    g_CurrentEntity->step = (u16)step;
    g_CurrentEntity->step_s = 0;
    g_CurrentEntity->pose = 0;
    g_CurrentEntity->poseTimer = 0;
    for (i = 0; i < 4; i++) {
        g_CurrentEntity->ext.GH_Props.unkB0[i] = 0;
        g_CurrentEntity->ext.GH_Props.unkB4[i] = 0;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_801CE228);
#else
void func_801CE228(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        g_CurrentEntity->ext.GH_Props.unkB0[i] = 0;
        g_CurrentEntity->ext.GH_Props.unkB4[i] = 0;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", polarPlacePartsList);
#else
void polarPlacePartsList(s16* offsets) {
    Entity* entity;

    while (*offsets) {
        entity = g_CurrentEntity + *offsets;
        if (!entity->ext.GH_Props.unkA8) {
            func_us_8019ED80_from_rbo2(entity);
        }
        offsets++;
    }
}
#endif

// decompiled in src/boss/bo1/e_explosion_flame.c
extern EInit g_EInitParticle;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_8019D260_from_rcen);
#else
// BO1's func_us_8019D260_from_rcen with RBO1's flame id
void func_us_8019D260_from_rcen(void) {
    s16 angle;
    s32 i;
    s8 type;
    Entity* entity;

    type = Random() & 3;
    angle = ((Random() & 0xF) << 8) - 0x800;
    for (i = 0; i < 6; i++) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_ID(UNK_1D), g_CurrentEntity, entity);
            entity->ext.et_801A518C.unk89 = 6 - i;
            entity->ext.et_801A518C.unk88 = type;
            entity->params = 2;
            entity->ext.et_801A518C.unk84 = angle;
            entity->zPriority = g_CurrentEntity->zPriority + 1;
        }
    }
}
#endif

extern EntityConfig D_us_8018094C[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo1/nonmatchings/unk_12274", func_us_801947E4);
#else
// flames and smoke: params 0 rises fast, 1 stays, 2 a burst from
// func_us_8019D260_from_rcen, 3 sprays and curls, 4 flies up and fades
void func_us_801947E4(Entity* self) {
    EntityConfig* config;
    s32 params;
    s32 speed;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitParticle);
        params = self->params & 0xF;
        config = &D_us_8018094C[params];
        self->palette = config->palette + 0x2E0;
        self->blendMode = config->blendMode;
        self->animSet = config->animSet;
        self->unk5A = config->unk5A;
        self->step = params + 1;
        self->ext.et_801A518C.anim = config->animData;
        if (self->params & 0xFF00) {
            self->zPriority = (self->params & 0xFF00) >> 8;
        }
        if (self->params & 0xF0) {
            self->palette = PAL_FLAG(0x19F);
            self->blendMode = BLEND_TRANSP;
            self->facingLeft = 1;
        }
        return;
    case 1:
        MoveEntity();
        self->velocityY = FIX(-1);
        break;
    case 2:
        break;
    case 3:
        if (!self->step_s) {
            self->drawFlags |= ENTITY_ROTATE;
            switch (self->ext.et_801A518C.unk88) {
            case 1:
                if ((u8)self->ext.et_801A518C.unk89 >= 4) {
                    self->ext.et_801A518C.unk89 -= 3;
                    self->ext.et_801A518C.unk84 -= 0x800;
                }
                break;
            case 2:
                self->ext.et_801A518C.unk84 +=
                    (u8)self->ext.et_801A518C.unk89 * 0xC0;
                break;
            }
            self->ext.et_801A518C.unk84 &= 0xFFF;
            self->rotate = self->ext.et_801A518C.unk84;
            speed = ((u8)self->ext.et_801A518C.unk89 * 0x140) / 28;
            self->velocityX = speed * rsin(self->ext.et_801A518C.unk84);
            self->velocityY = -(speed * rcos(self->ext.et_801A518C.unk84));
            self->step_s++;
        }
        if (self->pose >= 13) {
            self->velocityX -= self->velocityX / 4;
            self->velocityY -= self->velocityY / 4;
        }
        MoveEntity();
        break;
    case 4:
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY | ENTITY_ROTATE;
            self->opacity = 0x80;
            self->facingLeft = Random() & 1;
            angle = self->rotate;
            if (self->facingLeft) {
                angle = -angle;
            }
            self->velocityX = rsin(angle) * 0x28;
            self->velocityY = -(rcos(angle) * 0x28);
            self->ext.et_801A518C.acceleration = (Random() << 4) + 0x1000;
            self->step_s++;
        }
        MoveEntity();
        angle = self->rotate;
        self->opacity--;
        if (self->facingLeft) {
            angle = -angle;
        }
        self->velocityX +=
            (rsin(angle) * self->ext.et_801A518C.acceleration) >> 12;
        self->velocityY +=
            (-self->ext.et_801A518C.acceleration * rcos(angle)) >> 12;
        break;
    case 5:
        if (!self->step_s) {
            self->drawFlags = ENTITY_OPACITY;
            self->opacity = 0x80;
            self->facingLeft = Random() & 1;
            self->velocityX = (Random() << 9) - FIX(1);
            self->velocityY = FIX(-2.5);
            self->ext.et_801A518C.accelerationY = -(Random() << 4) - 0x1000;
            self->step_s++;
        }
        MoveEntity();
        self->velocityY += self->ext.et_801A518C.accelerationY;
        self->opacity--;
        break;
    default:
        return;
    }
    if (!AnimateEntity(self->ext.et_801A518C.anim, self)) {
        DestroyEntity(self);
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_801805D0[];
extern u8 D_us_801805D8[];
extern u8 D_us_80180728[];
extern u8 D_us_80180740[];
extern u8 D_us_80180758[];
extern u8 D_us_801807A0[];
extern u8 D_us_801807B8[];
extern u8 D_us_801808F4[];
extern u8 D_us_80180910[];
extern u8 D_us_80180930[];
u8 D_us_801805D0[] ALIGNED4 = {0x04, 0x01, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00};
u8 D_us_801805D8[] ALIGNED4 = {0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180728[] ALIGNED4 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80180740[] ALIGNED4 = {0xC0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0x00, 0xFF, 0x00, 0x01, 0xC0, 0xFF, 0xC0, 0xFF, 0x80, 0x00, 0x00, 0x00, 0xE0, 0xFF};
u8 D_us_80180758[] ALIGNED4 = {0xA0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x30, 0x01, 0xC0, 0xFF, 0x80, 0x01, 0xB8, 0xFF, 0xE0, 0xFF, 0xA0, 0x00, 0xE0, 0xFF, 0xC0, 0xFF};
u8 D_us_801807A0[] ALIGNED4 = {0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xE0, 0xFF, 0xF0, 0xFF, 0x20, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xF0, 0xFF, 0x10, 0x00, 0xF0, 0xFF};
u8 D_us_801807B8[] ALIGNED4 = {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0xD8, 0xFF, 0x08, 0x00, 0x10, 0x00, 0x00, 0x00, 0x08, 0x00, 0xF8, 0xFF, 0x00, 0x00, 0x08, 0x00};
u8 D_us_801808F4[] ALIGNED4 = {0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0xFF, 0x00};
u8 D_us_80180910[] ALIGNED4 = {0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x04, 0x02, 0x05, 0x02, 0x06, 0x02, 0x07, 0x02, 0x08, 0x02, 0x09, 0x02, 0x0A, 0x02, 0x0B, 0x02, 0x0C, 0x02, 0x0D, 0x02, 0x0E, 0xFF, 0x00, 0x00, 0x00};
u8 D_us_80180930[] ALIGNED4 = {0x03, 0x01, 0x03, 0x02, 0x03, 0x03, 0x03, 0x04, 0x03, 0x05, 0x03, 0x06, 0x03, 0x07, 0x03, 0x08, 0x03, 0x09, 0x03, 0x0A, 0x03, 0x0B, 0x03, 0x0C, 0x03, 0x0D, 0xFF, 0x00};
u8* D_us_801805E0[8] = { D_us_801805D0, D_us_801805D8, NULL, NULL, NULL, NULL, NULL, NULL };
u8 D_us_80180600[8] = { 8, 8, 0, 0, 0, 0, 0, 0 };
u8 D_us_80180608[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u16 D_us_80180610[8] = { 3, 3, 0, 0, 0, 0, 0, 0 };
u8 D_us_80180620[0x10] = { 0x70, 0x30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s32 D_us_80180630 = 0;
s32 D_us_80180634 = 0;
s32 D_us_80180638 = 0;
// splat cut this table after its first word
BeelzebubPartInit D_us_8018063C[] = {
    {2, 0, -48, 18, -2, 512},
    {1, 0, 24, 27, 2, 256},
    {4, 0, 28, 0, 2, 1536},
    {5, 4, 36, 23, -1, 992},
    {6, 5, 44, 24, 1, 32},
    {7, 0, 48, 0, 2, -1216},
    {8, 7, 56, 25, -1, 0},
    {9, 8, 40, 26, 1, 0},
    {10, 1, 64, 28, 1, 512},
    {11, 10, 64, 29, 2, -256},
    {12, 1, 64, 30, 1, 0},
    {13, 12, 64, 31, 2, 0},
    {0, 0, 0, 0, 0, 0},
};
s16 D_us_801806D8[0xC] = { 0, 0x12C, 0, 0x64, 0x1F4, 0, 0xC8, 0x258, 0x190, 0x320, 0x2BC, 0x384 };
s16 D_us_801806F0[0xE] = { 2, 4, 5, 6, 7, 8, 9, 1, 0xA, 0xB, 0xC, 0xD, 0, 0 };
s16 D_us_8018070C[0xE] = { 2, 4, 7, 5, 6, 8, 9, 1, 0xA, 0xB, 0xC, 0xD, 0, 0 };
unkStr_801CDD80 D_us_80180770[6] = {
    { 0x10, D_us_80180728 },
    { 0x40, D_us_80180740 },
    { 0x30, D_us_80180758 },
    { 0x20, D_us_80180740 },
    { 0x80, D_us_80180728 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_801807D0[5] = {
    { 0x10, D_us_80180728 },
    { 0x10, D_us_801807A0 },
    { 0x10, D_us_80180728 },
    { 8, D_us_801807B8 },
    { 0, NULL },
};
s16 D_us_801807F8[0x24] = {
    2,
    -0x16,
    -0x2E,
    0x250,
    0x3C,
    0,
    -0x12,
    -0x32,
    0x220,
    0x3C,
    0,
    0x3E,
    -0x20,
    0x2C0,
    0x3C,
    5,
    -0x12,
    -0x24,
    0x1C0,
    0x3C,
    1,
    -0x28,
    -0x1C,
    0x170,
    0x3C,
    0xA,
    -0x22,
    -0x18,
    0x150,
    0x3C,
    0xC,
    0x1A,
    -0x34,
    0x320,
    0x3C,
    0,
};
u8 D_us_80180840[0xC] = { 0x10, 0x14, 0x10, 0x15, 0x10, 0x16, 0x10, 0x15, 0, 0, 0, 0 };
u8 D_us_8018084C[8] = { 0x10, 0x12, 0x10, 0x13, 0, 0, 0, 0 };
u8 D_us_80180854[0xC] = { 2, 1, 2, 2, 2, 3, 2, 2, 0, 0, 0, 0 };
u8 D_us_80180860[8] = { 4, 4, 4, 5, 0, 0, 0, 0 };
u8 D_us_80180868[8] = { 4, 6, 4, 7, 0, 0, 0, 0 };
u8 D_us_80180870[8] = { 2, 8, 2, 9, 0, 0, 0, 0 };
u8 D_us_80180878[0x14] = {
    4,
    0xA,
    4,
    0xB,
    4,
    0xC,
    4,
    0xD,
    4,
    0xE,
    4,
    0xF,
    4,
    0x10,
    4,
    0x11,
    0,
    0,
    0,
    0,
};
s8 D_us_8018088C[0x44] = {
    0,
    0,
    0,
    0,
    0,
    -4,
    0x14,
    0xF,
    -0x2C,
    -0x20,
    0,
    0,
    -4,
    -4,
    0,
    0,
    0,
    -1,
    4,
    4,
    -5,
    -3,
    0x10,
    0x15,
    9,
    -0xA,
    0x24,
    0x14,
    0x10,
    -5,
    0x18,
    0xC,
    -3,
    0,
    5,
    0x1E,
    0,
    -0x15,
    5,
    0x17,
    0,
    -2,
    5,
    0x21,
    -1,
    -1,
    0x17,
    0x12,
    6,
    -0xA,
    0xF,
    0x13,
    -5,
    3,
    6,
    0x32,
    2,
    -0x19,
    8,
    0x1E,
    0,
    -4,
    8,
    0x30,
    -4,
    -8,
    0,
    0,
};
u8 D_us_801808D0[0x24] = {
    0,
    1,
    1,
    1,
    2,
    2,
    2,
    2,
    3,
    3,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    5,
    5,
    6,
    6,
    6,
    7,
    8,
    9,
    0xA,
    0xB,
    0xC,
    0xD,
    0xE,
    0xF,
    0x10,
    0,
    0,
    0,
};
EntityConfig D_us_8018094C[5] = {
    { 0xE, 0x79, 0, 0x30, D_us_801808F4 },
    { 0x8004, 0x54, 0, 0, D_us_80180910 },
    { 0x8004, 0x54, 3, 0x30, D_us_80180910 },
    { 0x8005, 0x55, 3, 0x30, D_us_80180930 },
    { 0x8005, 0x55, 3, 0x30, D_us_80180930 },
};
// end pc_import_data.py
// clang-format on
#endif
