// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo8.h"

void func_801CE3FC(s16* offsets) {
    Entity* entity;
    s32 i;

    for (i = 0; i < 4; i++) {
        entity = g_CurrentEntity + offsets[i];
        polarPlacePart(entity);
    }
    offsets += 4;

    while (*offsets) {
        if (*offsets != 0xFF) {
            entity = g_CurrentEntity + *offsets;
            polarPlacePart(entity);
        }
        offsets++;
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_80195938);
#else
// draws his cape between the shoulder (+1), the hip (+12) and the cape's
// tip, which hangs from part +15
void func_us_80195938(void) {
    Primitive* prim;
    s16 angle;

    prim = g_CurrentEntity->ext.prim;
    prim->x0 = (g_CurrentEntity + 1)->posX.i.hi;
    prim->y0 = (g_CurrentEntity + 1)->posY.i.hi;
    prim->x1 = (g_CurrentEntity + 12)->posX.i.hi;
    prim->y1 = (g_CurrentEntity + 12)->posY.i.hi;
    prim->x2 = (g_CurrentEntity + 15)->posX.i.hi;
    prim->y2 = (g_CurrentEntity + 15)->posY.i.hi;
    angle = (g_CurrentEntity + 15)->ext.galamoth.rotate;
    if (g_CurrentEntity->facingLeft) {
        prim->x2 -= (rcos(angle) * 0xB) >> 12;
    } else {
        prim->x2 += (rcos(angle) * 0xB) >> 12;
    }
    prim->y2 += (rsin(angle) * 0xA) >> 12;
    prim->drawMode &= ~DRAW_COLORS;
    prim->clut = g_CurrentEntity->palette & 0xFFF;
    if (g_CurrentEntity->facingLeft) {
        if (prim->x1 < prim->x2) {
            prim->drawMode |= DRAW_COLORS;
        }
    } else if (prim->x2 < prim->x1) {
        prim->drawMode |= DRAW_COLORS;
    }
}
#endif

static void func_801D0B40(void) {
    Entity* ent;
    Entity* parent;

    ent = g_CurrentEntity + 7;
    parent = ent->ext.GH_Props.parent;
    ent->ext.GH_Props.rotate = parent->ext.GH_Props.rotate + 0x100;

    ent = g_CurrentEntity + 11;
    parent = ent->ext.GH_Props.parent;
    ent->ext.GH_Props.rotate = parent->ext.GH_Props.rotate + 0x180;
}

extern s32 abs(s32 value);

#include "func_us_80195ad8.h"

extern EInit g_EInitInteractable;
extern EInit g_EInitGalamothLvl50;
extern EInit g_EInitGalamothLvl0;
extern s32 D_us_80180C0C;
extern s16 D_us_80180C10[];
extern s8 D_us_80180C88[];
extern giantBroBodyPartsInit D_us_80180CD0[];
extern s16 D_us_80180D70[];
extern s16 D_us_80180D90[];
extern s16 D_us_80180DB0[];
extern s16 D_us_80180DD0[];
extern s16 D_us_80180DE8[];
extern s16 D_us_80180E00[];
extern s16 D_us_80180E10[];
extern unkStr_801CDD80 D_us_80180EBC[];
extern unkStr_801CDD80 D_us_80180ECC[];
extern unkStr_801CDD80 D_us_80180F10[];
extern s16 D_us_80180F28[];
extern unkStr_801CDD80 D_us_80180F40[];
extern unkStr_801CDD80 D_us_80181014[];
extern unkStr_801CDD80 D_us_801810E8[];
extern unkStr_801CDD80 D_us_80181120[];
extern unkStr_801CDD80 D_us_801811A8[];
extern s16 D_us_801811D8[];
extern unkStr_801CDD80 D_us_80181208[];
extern unkStr_801CDD80 D_us_80181258[];
extern unkStr_801CDD80 D_us_801812B0[];
extern unkStr_801CDD80 D_us_801812C8[];
extern unkStr_801CDD80 D_us_80181318[];
extern unkStr_801CDD80 D_us_80181378[];
extern unkStr_801CDD80 D_us_801813B0[];
extern unkStr_801CDD80 D_us_801814A8[];
extern unkStr_801CDD80 D_us_801814C8[];
extern unkStr_801CDD80 D_us_80181518[];
extern unkStr_801CDD80 D_us_80181588[];
extern unkStr_801CDD80 D_us_801815DC[];
extern unkStr_801CDD80 D_us_801816D4[];
extern unkStr_801CDD80 D_us_801816EC[];
extern unkStr_801CDD80 D_us_80181714[];
extern unkStr_801CDD80 D_us_80181734[];
extern unkStr_801CDD80 D_us_80181744[];
extern u16 PrizeDrops[];

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_80195D80);
#else
// Galamoth's fight flags share PrizeDrops' four bytes, which hold two
// ITEMDROP_SMALL_HEART (0): bit 0 the fight started, 1 he died, 2 he is gone
#define GALAMOTH_FIGHT (*(s32*)PrizeDrops)

// takes a step with the foot at list[3] and lands it on the floor
#define GALAMOTH_STEP(list, facing, nextStep)                                  \
    if (self->ext.galamoth.unkB0[0] >= 2) {                                    \
        collider.unk18 = 0x1D;                                                 \
        part = self + (list)[3];                                               \
        func_us_801955A0(part, &collider);                                     \
        if (part->ext.et_8019921C.hitFloor) {                                  \
            g_api.PlaySfx(0x647);                                              \
            part->posY.i.hi += collider.unk18;                                 \
            self->ext.galamoth.unk84 ^= 1;                                     \
            func_801CE228();                                                   \
            if (func_us_801955F8(part, (facing))) {                            \
                func_801CE1E8(nextStep);                                       \
            } else {                                                           \
                func_us_80195AD8();                                            \
            }                                                                  \
        }                                                                      \
    }

// Galamoth
void func_us_80195D80(Entity* self) {
    Collider collider;
    Entity* part;
    Entity* prev;
    Primitive* prim;
    giantBroBodyPartsInit* parts;
    s16* list;
    s16* p;
    s32 primIndex;
    s16 angle;

    FntPrint("step %x\n", self->step);
    FntPrint("step_s %x\n", self->step_s);
    if ((self->step & 1) && (self->hitFlags & 3)) {
        func_801CE1E8(0x18);
    }
    if ((self->flags & FLAG_DEAD) && self->step < 0x20) {
        for (p = D_us_80180D70; *p; p++) {
            (self + *p)->hitboxState = 0;
        }
        self->hitboxState = 0;
        D_us_80180C0C = 1;
        if ((self + 16)->ext.et_8019921C.hitFloor ||
            (self + 13)->ext.et_8019921C.hitFloor) {
            func_801CE1E8(0x20);
        }
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitGalamothLvl50);
        self->animCurFrame = 7;
        self->hitboxOffX = D_us_80180C88[7 * 4 + 0];
        self->hitboxOffY = D_us_80180C88[7 * 4 + 1];
        self->hitboxWidth = (u8)D_us_80180C88[7 * 4 + 2];
        self->hitboxHeight = (u8)D_us_80180C88[7 * 4 + 3];
        primIndex = g_api.AllocPrimitives(PRIM_GT3, 1);
        if (primIndex == -1) {
            self->step = 0;
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        prim->tpage = 0x12;
        prim->clut = 0x200;
        prim->v0 = 2;
        prim->u1 = 0xD1;
        prim->v1 = prim->v2 = 0x2B;
        PGREY(prim, 0) = 0x10;
        prim->u0 = prim->u2 = 0xF9;
        PGREY(prim, 1) = 0x40;
        PGREY(prim, 2) = 0x40;
        prim->drawMode = DRAW_UNK02;
        prim->priority = self->zPriority;
        // fallthrough
    case 1:
        if (UnkCollisionFunc3(D_us_80180C10) & 1) {
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            self->step++;
        }
        break;
    case 2:
        for (parts = D_us_80180CD0, prev = self; parts->eArrayOffset;
             parts++) {
            part = self + parts->eArrayOffset;
            CreateEntityFromCurrentEntity(E_ID(UNK_17), part);
            part->ext.galamoth.length = parts->length;
            part->ext.galamoth.parent = self + parts->eArrayParentOffset;
            part->params = parts->params;
            part->nextPart = prev;
            prev = part;
            part->parent = self;
            part->zPriority = self->zPriority + parts->zOffset;
        }
        self->nextPart = prev;
        self->parent = NULL;
        self->step++;
        // fallthrough
    case 3:
        func_801CE228();
        self->ext.galamoth.unk84 = 0;
        func_801CE1E8(4);
        break;
    case 4:
        // wait for the fight to start
        self->ext.galamoth.attacking = 0;
        switch (self->step_s) {
        case 0:
            func_801CDF1C(D_us_80180DB0, D_us_80180EBC, 0);
            func_801CDE10(D_us_80180DB0);
            func_801D0A00(D_us_80180DB0);
            polarPlacePartsList(D_us_80180D70);
            self->step_s++;
            break;
        case 1:
            if (self->hitFlags || (GALAMOTH_FIGHT & 1)) {
                self->step_s++;
            }
            break;
        case 2:
            GALAMOTH_FIGHT |= 1;
            self->step_s++;
            break;
        case 3:
            func_801CDF1C(D_us_80180DB0, D_us_80180ECC, 0);
            func_801CDE10(D_us_80180DB0);
            func_801D0A00(D_us_80180DB0);
            polarPlacePartsList(D_us_80180D70);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                func_us_80195AD8();
            }
            break;
        }
        break;
    case 5:
        // walk forward
        self->ext.galamoth.attacking = 0;
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180DD0;
        } else {
            list = D_us_80180DE8;
        }
        func_801CDF1C(list, D_us_80181378, 0);
        func_801CDE10(list);
        func_801D0A00(list);
        func_801D0B40();
        polarPlacePartsList(D_us_80180D70);
        GALAMOTH_STEP(list, self->facingLeft, 7);
        break;
    case 18:
        self->ext.galamoth.attacking = 0;
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180D90;
        } else {
            list = D_us_80180DB0;
        }
        func_801CDF1C(list, D_us_80181014, 0);
        func_801CDE10(list);
        func_801D0A00(list);
        polarPlacePartsList(D_us_80180D70);
        if (self->ext.galamoth.unkB0[0] == 3 && !self->ext.galamoth.unkB4[0]) {
            self->ext.galamoth.unk84 ^= 1;
        }
        if (self->ext.galamoth.unkB0[0] == 6 && !self->ext.galamoth.unkB4[0]) {
            g_api.ShakeCamera(1);
            g_api.PlaySfx(0x63D);
        }
        if (!self->ext.galamoth.unkB0[0] && !self->ext.galamoth.unkB4[0]) {
            func_us_80195AD8();
        }
        break;
    case 7:
        // walk back
        self->ext.galamoth.attacking = 0;
        if (!self->step_s) {
            self->ext.galamoth.unkB0[0] = 1;
            self->step_s++;
        }
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180E00;
        } else {
            list = D_us_80180E10;
        }
        func_801CDF1C(list, D_us_801813B0, 0);
        func_801CDE10(list);
        func_801D0A00(list);
        func_801D0B40();
        polarPlacePartsList(D_us_80180D70);
        GALAMOTH_STEP(list, self->facingLeft ^ 1, 5);
        break;
    case 10:
        // jump back
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180DD0;
        } else {
            list = D_us_80180DE8;
        }
        switch (self->step_s) {
        case 0:
            self->step_s++;
            break;
        case 1:
            func_801CDF1C(list, D_us_801814A8, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                self->step_s++;
            }
            break;
        case 2:
            if (self->facingLeft) {
                self->velocityX = FIX(3);
            } else {
                self->velocityX = FIX(-3);
            }
            self->velocityY = FIX(-7);
            func_801CE228();
            self->step_s++;
            // fallthrough
        case 3:
            MoveEntity();
            self->velocityY += FIX(0.25);
            func_801CDF1C(list, D_us_801814C8, 0);
            func_801CDE10(list);
            func_801CE3FC(list);
            collider.unk18 = 0x1D;
            part = self + list[3];
            func_us_801955A0(part, &collider);
            if (part->ext.et_8019921C.hitFloor) {
                part->posY.i.hi += collider.unk18;
                self->ext.galamoth.unk84 ^= 1;
                func_801CE228();
                self->step_s = 1;
                func_us_801955F8(part, self->facingLeft);
                func_801CE1E8(7);
            }
            break;
        }
        func_801D0B40();
        polarPlacePartsList(D_us_80180D70);
        break;
    case 26:
        // swing his hand
        self->ext.galamoth.attacking = 1;
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180D90;
        } else {
            list = D_us_80180DB0;
        }
        func_801CDF1C(list, D_us_80181588, 0);
        func_801CDE10(list);
        func_801D0A00(list);
        func_801D0B40();
        polarPlacePartsList(D_us_80180D70);
        (self + list[5])->ext.galamoth.unk88 = 1;
        if (!self->ext.galamoth.unkB0[0] && !self->ext.galamoth.unkB4[0]) {
            self->ext.galamoth.attacking = 0;
            (self + list[5])->ext.galamoth.unk88 = 0;
            func_us_80195AD8();
        }
        if (self->ext.galamoth.unkB0[0] == 3 && !self->ext.galamoth.unkB4[0]) {
            g_api.PlaySfx(0x7C8);
        }
        break;
    case 9:
        // swing his staff at Alucard four times
        switch (self->step_s) {
        case 0:
            if (self->ext.galamoth.unk84 == 1) {
                list = D_us_80180D90;
            } else {
                list = D_us_80180DB0;
            }
            func_801CDF1C(list, D_us_80180F10, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            if (!self->ext.galamoth.unkB0[0] && !self->ext.galamoth.unkB4[0]) {
                self->ext.galamoth.count = 4;
                self->step_s++;
            }
            break;
        case 1:
            angle = ratan2((self + 3)->posX.i.hi - PLAYER.posX.i.hi,
                           PLAYER.posY.i.hi - (self + 3)->posY.i.hi);
            if (self->facingLeft) {
                angle = -angle;
            }
            if (angle > 0x700) {
                angle = 0x700;
            }
            if (angle < 0x200) {
                angle = 0x200;
            }
            func_801CDD00(self + 8, angle, 0xC);
            func_801CDD00(self + 10, angle, 0xC);
            func_801CDD00(self + 11, angle, 0xC);
            angle -= 0x400;
            func_801CDD00(self + 2, angle, 0xC);
            func_801CDD00(self + 3, angle / 2, 0xC);
            self->ext.galamoth.timer = 0xC;
            self->step_s++;
            // fallthrough
        case 2:
            polarPlacePartsWithAngvel(D_us_80180F28);
            if (!--self->ext.galamoth.timer) {
                self->step_s++;
            }
            break;
        case 3:
            func_801CDF1C(D_us_80180F28, D_us_80180F40, 0);
            func_801CDE10(D_us_80180F28);
            if (!self->ext.galamoth.unkB0[0] && !self->ext.galamoth.unkB4[0]) {
                if (--self->ext.galamoth.count) {
                    self->step_s = 1;
                } else {
                    func_us_80195AD8();
                }
            }
            break;
        }
        polarPlacePartsList(D_us_80180D70);
        break;
    case 11:
        // turn around
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180DD0;
        } else {
            list = D_us_80180DE8;
        }
        self->ext.galamoth.attacking = 0;
        func_801CDF1C(list, D_us_80181518, 0);
        func_801CDE10(list);
        func_801D0A00(list);
        func_801D0B40();
        polarPlacePartsList(D_us_80180D70);
        if (!self->ext.galamoth.unkB4[0]) {
            self->facingLeft ^= 1;
            func_us_80195AD8();
        }
        break;
    case 12:
        // fireballs from his staff
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180D90;
        } else {
            list = D_us_80180DB0;
        }
        switch (self->step_s) {
        case 0:
            self->ext.galamoth.attacking = 0;
            func_801CDF1C(list, D_us_801810E8, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            self->ext.galamoth.timer = 0x20;
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                self->step_s++;
            }
            // fallthrough
        case 1:
            if (!(g_Timer % 5)) {
                part = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (part != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_18), part);
                    part->ext.et_801980E4.source = self + 7;
                }
            }
            if (!--self->ext.galamoth.timer) {
                self->ext.galamoth.timer = 0x100;
                self->ext.galamoth.attacking = 1;
                self->step_s++;
            }
            break;
        case 2:
            if (!(self->ext.galamoth.timer & 0x1F)) {
                g_api.PlaySfx(0x655);
                part = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (part != NULL) {
                    CreateEntityFromEntity(E_ID(UNK_19), self + 7, part);
                }
            }
            if (!--self->ext.galamoth.timer) {
                self->ext.galamoth.attacking = 0;
                self->step_s++;
            }
            break;
        case 3:
            func_801CDF1C(list, D_us_80181120, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                func_us_80195AD8();
            }
            break;
        }
        polarPlacePartsList(D_us_80180D70);
        break;
    case 14:
        // lightning from his staff to the floor
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180D90;
        } else {
            list = D_us_80180DB0;
        }
        switch (self->step_s) {
        case 0:
            self->ext.galamoth.attacking = 0;
            func_801CDF1C(list, D_us_801811A8, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            self->ext.galamoth.timer = 0x20;
            if (!self->ext.galamoth.unkB4[0]) {
                if (!self->ext.galamoth.unkB0[0]) {
                    self->step_s++;
                }
                if (!self->ext.galamoth.unkB4[0] &&
                    self->ext.galamoth.unkB0[0] == 3) {
                    g_api.PlaySfx(0x6C6);
                }
            }
            // fallthrough
        case 1:
            if (!(g_Timer % 5)) {
                part = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (part != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_18), part);
                    part->ext.et_801980E4.source = self + 7;
                }
            }
            if (!--self->ext.galamoth.timer) {
                g_api.PlaySfx(0x80F);
                self->ext.galamoth.attacking = 1;
                self->ext.galamoth.timer = 0x100;
                self->step_s++;
            }
            break;
        case 2:
            func_801CDF1C(D_us_801811D8, D_us_80181208, 0);
            polarPlacePartsWithAngvel(D_us_801811D8);
            if (!(self->ext.galamoth.timer & 0xF)) {
                part = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (part != NULL) {
                    CreateEntityFromEntity(E_ID(UNK_1A), self + 7, part);
                    part->facingLeft = self->facingLeft;
                }
            }
            if (!(self->ext.galamoth.timer & 0x1F)) {
                g_api.PlaySfx(0x80F);
            }
            if (!--self->ext.galamoth.timer) {
                func_801CE228();
                self->ext.galamoth.timer = 0x80;
                self->step_s++;
            }
            break;
        case 3:
            func_801CDF1C(D_us_801811D8, D_us_80181208, 0);
            polarPlacePartsWithAngvel(D_us_801811D8);
            if (!(self->ext.galamoth.timer & 0x1F)) {
                g_api.PlaySfx(0x80F);
            }
            if (!--self->ext.galamoth.timer) {
                func_801CE228();
                self->ext.galamoth.attacking = 0;
                self->step_s++;
            }
            break;
        case 4:
            func_801CDF1C(list, D_us_80181258, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                func_us_80195AD8();
            }
            break;
        }
        polarPlacePartsList(D_us_80180D70);
        break;
    case 24:
        // flinch
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180D90;
        } else {
            list = D_us_80180DB0;
        }
        func_801CDF1C(list, D_us_801815DC, 0);
        func_801CDE10(list);
        func_801D0A00(list);
        polarPlacePartsList(D_us_80180D70);
        self->ext.galamoth.attacking = 1;
        if (!self->ext.galamoth.unkB0[0] && !self->ext.galamoth.unkB4[0]) {
            self->ext.galamoth.attacking = 0;
            func_us_80195AD8();
        }
        break;
    case 22:
        // lightning between his hands, then thrown across the room
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180D90;
        } else {
            list = D_us_80180DB0;
        }
        switch (self->step_s) {
        case 0:
            self->ext.galamoth.attacking = 0;
            func_801CDF1C(list, D_us_801812B0, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            if (!(g_Timer % 5)) {
                part = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (part != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_18), part);
                    part->ext.et_801980E4.source = self + 7;
                }
            }
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                self->step_s++;
            }
            break;
        case 1:
            part = self + 20;
            DestroyEntity(part);
            CreateEntityFromCurrentEntity(E_ID(UNK_1C), part);
            part->ext.galamothLightning.source = self + 7;
            part->params = 0;
            part->facingLeft = self->facingLeft;
            part = self + 21;
            DestroyEntity(part);
            CreateEntityFromCurrentEntity(E_ID(UNK_1C), part);
            part->ext.galamothLightning.source = self + 11;
            part->params = 1;
            part->facingLeft = self->facingLeft;
            g_api.PlaySfx(0x80F);
            self->step_s++;
            // fallthrough
        case 2:
            if (!(g_Timer % 5)) {
                g_api.PlaySfx(0x80F);
                part = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (part != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_18), part);
                    part->ext.et_801980E4.source = self + 7;
                }
            }
            func_801CDF1C(list, D_us_801812C8, 0);
            func_801CDE10(list);
            if (++self->ext.galamoth.timer & 1) {
                (self + 1)->ext.galamoth.rotate += 0x18;
            } else {
                (self + 1)->ext.galamoth.rotate -= 0x18;
            }
            func_801D0A00(list);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                (self + 1)->ext.galamoth.rotate = 0;
                self->ext.galamoth.attacking = 1;
                self->step_s++;
            }
            break;
        case 3:
            func_801CDF1C(list, D_us_80181318, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            if (self->ext.galamoth.unkB4[0] && self->ext.galamoth.unkB0[0] == 1) {
                (self + 21)->ext.galamothLightning.release = 1;
                (self + 20)->ext.galamothLightning.release = 1;
            }
            if (!(g_Timer & 0x1F)) {
                g_api.PlaySfx(0x665);
            }
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                g_api.PlaySfx(0x653);
                self->step_s++;
            }
            break;
        case 4:
            self->ext.galamoth.attacking = 0;
            func_us_80195AD8();
            break;
        }
        polarPlacePartsList(D_us_80180D70);
        break;
    case 32:
        // death: the parts burn and fall apart, then he fades out
        if (self->ext.galamoth.unk84 == 1) {
            list = D_us_80180D90;
        } else {
            list = D_us_80180DB0;
        }
        self->ext.galamoth.attacking = 1;
        if (++self->ext.galamoth.timer & 1) {
            (self + 1)->ext.galamoth.rotate += 0x18;
        } else {
            (self + 1)->ext.galamoth.rotate -= 0x18;
        }
        if (!(g_Timer & 0x1F)) {
            g_api.PlaySfx(0x655);
        }
        switch (self->step_s) {
        case 0:
            (self + 2)->step = 0x20;
            (self + 2)->ext.galamoth.unk8D = 1;
            (self + 2)->step_s = 0;
            self->step_s = 2;
            GALAMOTH_FIGHT |= 2;
            break;
        case 1:
            func_801CDF1C(list, D_us_801816D4, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            polarPlacePartsList(D_us_80180D70);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                self->ext.galamoth.unk84 = 1;
                self->step_s++;
            }
            break;
        case 2:
            func_801CDF1C(list, D_us_801816EC, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            polarPlacePartsList(D_us_80180D70);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                g_api.PlaySfx(0x7C5);
                g_api.PlaySfx(0x7C7);
                (self + 2)->ext.galamoth.unk8C = 1;
                (self + 4)->step = 0x20;
                (self + 4)->ext.galamoth.unk8D = 1;
                (self + 6)->step = 0x20;
                (self + 7)->step = 0x20;
                self->step_s++;
            }
            break;
        case 3:
            func_801CDF1C(list, D_us_80181714, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            polarPlacePartsList(D_us_80180D70);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                g_api.PlaySfx(0x7C5);
                g_api.PlaySfx(0x7C7);
                (self + 4)->ext.galamoth.unk8C = 1;
                (self + 6)->ext.galamoth.unk8C = 1;
                (self + 7)->ext.galamoth.unk8C = 1;
                part = self + list[2];
                part->step = 0x20;
                part->ext.galamoth.unk8D = 1;
                part->ext.galamoth.unk8E = 1;
                (self + list[3])->step = 0x20;
                (self + list[5])->step = 0x20;
                self->step_s++;
            }
            break;
        case 4:
            func_801CDF1C(list, D_us_80181734, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            polarPlacePartsList(D_us_80180D70);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                g_api.PlaySfx(0x7C5);
                g_api.PlaySfx(0x7C7);
                (self + list[2])->ext.galamoth.unk8C = 1;
                (self + list[3])->ext.galamoth.unk8C = 1;
                (self + list[5])->ext.galamoth.unk8C = 1;
                (self + 8)->step = 0x20;
                (self + 10)->step = 0x20;
                (self + 11)->step = 0x20;
                (self + 8)->ext.galamoth.unk8D = 1;
                (self + 8)->ext.galamoth.unk8E = 1;
                self->ext.prim->drawMode = DRAW_HIDE;
                self->step_s++;
            }
            break;
        case 5:
            func_801CDF1C(list, D_us_80181744, 0);
            func_801CDE10(list);
            func_801D0A00(list);
            polarPlacePartsList(D_us_80180D70);
            if (!self->ext.galamoth.unkB4[0] && !self->ext.galamoth.unkB0[0]) {
                g_api.PlaySfx(0x7C5);
                (self + 8)->ext.galamoth.unk8C = 1;
                (self + 10)->ext.galamoth.unk8C = 1;
                (self + 11)->ext.galamoth.unk8C = 1;
                self->step_s++;
            }
            break;
        case 6:
            for (p = D_us_80180D70; *p; p++) {
                (self + *p)->drawFlags |= ENTITY_OPACITY;
            }
            (self + 3)->step = 0x20;
            (self + 1)->step = 0x20;
            (self + 12)->step = 0x20;
            self->opacity = 0x80;
            (self + 1)->ext.galamoth.unk8D = 1;
            (self + 1)->ext.galamoth.unk8E = 1;
            self->drawFlags |= ENTITY_OPACITY;
            self->step_s++;
            // fallthrough
        case 7:
            self->opacity -= 2;
            for (p = D_us_80180D70; *p; p++) {
                (self + *p)->opacity = self->opacity;
            }
            if (!self->opacity) {
                self->step_s++;
            }
            break;
        case 8:
            self->palette = PAL_FLAG(0x20A);
            self->step_s++;
            // fallthrough
        case 9:
            if (!(self->ext.galamoth.timer & 7)) {
                self->palette++;
            }
            self->posY.i.hi++;
            for (p = D_us_80180D70; *p; p++) {
                part = self + *p;
                part->palette = self->palette;
                part->posY.val += FIX(0.25);
            }
            if (self->palette > PAL_FLAG(0x216)) {
                self->step_s++;
            }
            break;
        case 10:
            for (p = D_us_80180D70; *p; p++) {
                DestroyEntity(self + *p);
            }
            g_api.PlaySfx(0x654);
            GALAMOTH_FIGHT |= 4;
            DestroyEntity(self);
            return;
        }
        break;
    }
    func_us_80195938();
    for (p = D_us_80180D70; *p; p++) {
        part = self + *p;
        part->ext.galamoth.unkA8 = 0;
        part->facingLeft = self->facingLeft;
    }
    D_8006C384.y = self->ext.galamoth.unkB0[0];
    D_8006C38C.y = self->ext.galamoth.unkB4[0];
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_80197B1C);
#else
// a body part; params is its sprite
void func_us_80197B1C(Entity* self) {
    Collider collider;
    Entity* entity;
    s8* hitbox;
    s16 angle;
    s16 length;
    s16 dist;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitGalamothLvl50);
        if (self->params == 9) {
            InitializeEntity(g_EInitGalamothLvl0);
        }
        if (self->params == 3 || self->params == 0x10) {
            self->hitboxState = 1;
        }
        self->animCurFrame = self->params;
        hitbox = &D_us_80180C88[self->animCurFrame * 4];
        self->drawFlags |= ENTITY_ROTATE;
        self->hitboxWidth = (u8)hitbox[2];
        self->step = 1;
        self->hitboxHeight = (u8)hitbox[3];
        break;
    case 1:
        self->rotate = self->ext.galamoth.rotate;
        dist = D_us_80180C88[self->animCurFrame * 4 + 1];
        self->hitboxOffX = (-dist * rsin(self->ext.galamoth.rotate)) >> 12;
        self->hitboxOffY = (dist * rcos(self->ext.galamoth.rotate)) >> 12;
        if (self->ext.galamoth.flip) {
            self->ext.galamoth.flip = 0;
            self->facingLeft ^= 1;
            self->rotate = -self->rotate;
        }
        break;
    case 32:
        switch (self->step_s) {
        case 0:
            if (!(g_Timer & 7)) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[256]);
                if (entity != NULL) {
                    if (self->ext.galamoth.unk8D) {
                        CreateEntityFromEntity(
                            E_ID(MINOTAUR_SPIT_LIQUID), self, entity);
                    } else {
                        CreateEntityFromEntity(E_ID(UNK_20), self, entity);
                    }
                    entity->zPriority = self->zPriority + 1;
                    entity->rotate = self->rotate;
                    entity->facingLeft = self->facingLeft;
                    angle = self->ext.galamoth.rotate;
                    length = -self->ext.galamoth.length;
                    if (self->facingLeft) {
                        angle = -angle;
                    }
                    entity->posX.val -= length * rsin(angle) * 0x10;
                    entity->posY.val += length * rcos(angle) * 0x10;
                    if (self->ext.galamoth.unk8E) {
                        entity->rotate += 0x800;
                    }
                }
            }
            self->rotate = self->ext.galamoth.rotate;
            if (self->ext.galamoth.unk8C) {
                self->step_s++;
            }
            break;
        case 1:
            // fall off as a piece
            entity = AllocEntity(
                &g_Entities[STAGE_ENTITY_START], &g_Entities[TOTAL_ENTITY_COUNT]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_ID(UNK_1F), self, entity);
                entity->params = self->animCurFrame;
                entity->facingLeft = self->facingLeft;
                entity->rotate = self->rotate;
            }
            self->animCurFrame = 0;
            if (self->ext.galamoth.unk8D) {
                self->step_s += 1;
            } else {
                self->step_s += 2;
            }
            // fallthrough
        case 2:
            if (!(g_Timer & 3)) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(
                        E_ID(MINOTAUR_SPIT_LIQUID), self, entity);
                    entity->zPriority = self->zPriority + 1;
                    entity->rotate = self->ext.galamoth.rotate;
                    entity->facingLeft = self->facingLeft;
                    angle = self->ext.galamoth.rotate;
                    length = -self->ext.galamoth.length;
                    if (self->facingLeft) {
                        angle = -angle;
                    }
                    entity->posX.val -= length * rsin(angle) * 0x10;
                    entity->posY.val += length * rcos(angle) * 0x10;
                    if (self->ext.galamoth.unk8E) {
                        entity->rotate += 0x800;
                    }
                }
            }
            break;
        }
        break;
    }
    switch (self->animCurFrame) {
    case 5:
    case 12:
        // the feet
        collider.unk18 = 0x1D;
        func_us_801955A0(self, &collider);
        return;
    case 6:
    case 13:
        // the hands
        switch (self->ext.galamoth.parent->ext.et_8019921C.hitFloor) {
        case 2:
            self->ext.galamoth.rotate = 0x200;
            break;
        case 4:
            self->ext.galamoth.rotate = -0x200;
            break;
        case 1:
            self->ext.galamoth.rotate = 0;
            break;
        }
        if (self->ext.galamoth.unk88) {
            angle = self->rotate;
            self->hitboxOffX += -(rsin(angle) * 0x10) >> 12;
            self->hitboxOffY += (rcos(angle) * 0x10) >> 12;
            self->attack = g_api.enemyDefs[0xCA].attack;
            self->attackElement = g_api.enemyDefs[0xCA].attackElement;
        } else {
            self->attack = g_api.enemyDefs[0xC6].attack;
            self->attackElement = g_api.enemyDefs[0xC6].attackElement;
        }
        FntPrint("attack %x\n", self->attackElement);
        return;
    case 9:
    case 17:
        // the head looks angry while he attacks
        if ((self - 2)->ext.galamoth.attacking) {
            self->animCurFrame = 0x11;
        } else {
            self->animCurFrame = 9;
        }
        break;
    }
}
#endif

extern EInit g_EInitGalamothLvl50;
extern s32 D_us_80180C0C;

void func_us_801980E4(Entity* self) {
    Entity* source;

    if (D_us_80180C0C != 0) {
        DestroyEntity(self);
        return;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitGalamothLvl50);
        self->hitboxState = 0;
        self->animCurFrame = 0x10;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        self->drawFlags = ENTITY_SCALEY | ENTITY_SCALEX;
        self->scaleX = self->scaleY = 0x100;
        self->drawFlags |= ENTITY_OPACITY;
        self->opacity = 0xC0;
        self->drawFlags |= ENTITY_ROTATE;
        /* fall through */
    case 1:
        if (g_Timer & 1) {
            self->palette = PAL_FLAG(0x206);
        } else {
            self->palette = PAL_FLAG(0x207);
        }
        source = self->ext.et_801980E4.source;
        self->facingLeft = source->facingLeft;
        self->rotate = source->rotate;
        self->posX.i.hi = source->posX.i.hi;
        self->posY.i.hi = source->posY.i.hi;
        self->scaleX = self->scaleY += 0x20;
        self->opacity -= 8;
        if (!self->opacity) {
            DestroyEntity(self);
        }
        break;
    }
}

extern EInit D_us_80180A8C;
extern u8 D_us_80180C20[];

void func_us_80198210(Entity* self) {
    Entity* entities;
    s16 angle;

    if ((self->flags & FLAG_DEAD) || D_us_80180C0C != 0) {
        self->velocityX = 0;
        self->velocityY = 0;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180A8C);
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        entities = g_Entities;
        angle = GetAngleBetweenEntities(self, entities);
        self->velocityX = rcos(angle) * 0x140;
        self->velocityY = rsin(angle) * 0x140;
        /* fall through */

    case 1:
        AnimateEntity(D_us_80180C20, self);
        MoveEntity();
        self->velocityX -= self->velocityX / 0x10;
        self->velocityY -= self->velocityY / 0x10;
        if (abs(self->velocityX) < 0x4000 && abs(self->velocityY) < 0x4000) {
            self->drawFlags = ENTITY_SCALEY | ENTITY_SCALEX;
            self->scaleX = self->scaleY = 0x100;
            self->hitboxState = 0;
            self->step++;
        }
        break;

    case 2:
        AnimateEntity(D_us_80180C20, self);
        self->scaleX = self->scaleY -= 0x10;
        if (!self->scaleX) {
            DestroyEntity(self);
        }
        break;
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_801983EC);
#else
// lightning from his staff, aimed at the floor in front of him and widening
void func_us_801983EC(Entity* self) {
    Entity* entity;
    Primitive* prim;
    Primitive* primTwo;
    s32 primIndex;
    s32 i;
    s32 posX;
    s32 posY;
    s16 boltAngle;
    s16 tempAngle;
    u8 closeToTarget;

    if (D_us_80180C0C) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->ext.galamothLightning.range = -0x80;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x28);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.galamothLightning.prim = prim;
        self->ext.galamothLightning.primTwo = prim;
        self->flags |= FLAG_HAS_PRIMS;
        i = 0;
        while (prim != NULL) {
            PGREY(prim, 0) = 0x50;
            prim->tpage = 0x1A;
            prim->clut = 0x194;
            prim->u0 = prim->u1 = (i * 0x10) - 0x70;
            prim->u2 = prim->u3 = (i * 0x10) - 0x60;
            prim->v0 = prim->v2 = 0xD0;
            prim->v1 = prim->v3 = 0xC0;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority - 1;
            prim = prim->next;
            if (++i >= 6) {
                i = 0;
            }
        }
        // fallthrough
    case 1:
        prim = self->ext.galamothLightning.prim;
        prim->x0 = self->posX.i.hi;
        prim->y0 = self->posY.i.hi;
        prim->x1 = prim->x0;
        prim->y1 = prim->y1 - 0x10;
        prim->x2 = self->posX.i.hi;
        prim->y2 = self->posY.i.hi;
        prim->x3 = prim->x2;
        prim->y3 = prim->y2 - 0x10;
        self->ext.galamothLightning.primTwo = prim;
        self->ext.galamothLightning.boltAngle = 0xC00;
        self->ext.galamothLightning.segmentsBeforeAim = 0;
        self->step++;
        // fallthrough
    case 2:
        for (i = 0; i < 6; i++) {
            prim = self->ext.galamothLightning.primTwo;
            boltAngle = self->ext.galamothLightning.boltAngle;
            if (self->facingLeft) {
                posX = self->posX.i.hi + self->ext.galamothLightning.range;
            } else {
                posX = self->posX.i.hi - self->ext.galamothLightning.range;
            }
            posY = 0x1C0 - g_Tilemap.scrollY.i.hi - prim->y2;
            posX -= prim->x2;
            if (posY < 0) {
                // reached the floor
                prim->y3 = prim->y2 = 0x1C0 - g_Tilemap.scrollY.i.hi;
                self->step = 3;
                return;
            }
            closeToTarget = false;
            if (abs(posX) < 0x40) {
                closeToTarget = abs(posY) < 0x40;
            }
            if (!self->ext.galamothLightning.segmentsBeforeAim) {
                self->ext.galamothLightning.segmentsBeforeAim = 4;
                if (closeToTarget) {
                    self->ext.galamothLightning.segmentsBeforeAim = 2;
                }
                tempAngle = ratan2(-posY, posX) - boltAngle;
                if (tempAngle > 0x800) {
                    tempAngle -= 0x1000;
                }
                if (tempAngle < -0x800) {
                    tempAngle += 0x1000;
                }
                if (!closeToTarget) {
                    tempAngle /= 4;
                } else {
                    tempAngle /= 2;
                }
                self->ext.galamothLightning.angleStep = tempAngle;
            }
            boltAngle += self->ext.galamothLightning.angleStep;
            if (!closeToTarget) {
                boltAngle += 0xC0 - ((Random() & 3) << 7);
            }
            boltAngle &= 0xFFF;
            primTwo = prim->next;
            if (primTwo == NULL) {
                self->step = 3;
                return;
            }
            LOW(primTwo->x0) = LOW(prim->x2);
            LOW(primTwo->x1) = LOW(prim->x3);
            self->ext.galamothLightning.boltAngle = boltAngle;
            self->ext.galamothLightning.primTwo = primTwo;
            primTwo->x2 = primTwo->x0 + ((rcos(boltAngle) * 0xC) >> 12);
            primTwo->y2 = primTwo->y0 - ((rsin(boltAngle) * 0xC) >> 12);
            tempAngle = boltAngle - 0x400;
            primTwo->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                                DRAW_UNK02 | DRAW_TRANSP;
            posX = primTwo->x3 = primTwo->x2 + ((rcos(tempAngle) * 0x10) >> 12);
            posY = primTwo->y3 =
                primTwo->y2 - ((rsin(tempAngle) * 0x10) >> 12);
            posX -= PLAYER.posX.i.hi;
            posY -= PLAYER.posY.i.hi;
            if (abs(posX) < 0xC && abs(posY) < 0xC) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_1D), entity);
                    entity->posX.i.hi = primTwo->x3;
                    entity->posY.i.hi = primTwo->y3;
                }
            }
            self->ext.galamothLightning.segmentsBeforeAim--;
        }
        break;
    case 3:
        prim = self->ext.galamothLightning.prim;
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->ext.galamothLightning.range += 8;
        self->step = 1;
        if (self->ext.galamothLightning.range > 0x80) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_80198964);
#else
// follows one of his hands (source) during the lightning attack; params 0
// casts six bolts at the next entity, the other spawner. Once released,
// both fly off and the bolts follow
void func_us_80198964(Entity* self) {
    Entity* entity;
    s32 i;

    if (self->ext.galamothLightning.source->entityId != E_ID(UNK_17) ||
        D_us_80180C0C) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        if (!self->params) {
            for (i = 0; i < 6; i++) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_1B), entity);
                    entity->ext.galamothLightning.source = self;
                    entity->params = i;
                    entity->facingLeft = self->facingLeft;
                }
            }
        }
        // fallthrough
    case 1:
        self->posX.i.hi = self->ext.galamothLightning.source->posX.i.hi;
        self->posY.i.hi = self->ext.galamothLightning.source->posY.i.hi;
        if (self->ext.galamothLightning.release) {
            self->step++;
        }
        break;
    case 2:
        if (self->facingLeft) {
            self->velocityX = FIX(6);
        } else {
            self->velocityX = FIX(-6);
        }
        self->velocityY = 0;
        self->step++;
        // fallthrough
    case 3:
        MoveEntity();
        self->velocityX += self->velocityX / 128;
        if ((u32)(self->posX.i.hi + g_Tilemap.scrollX.i.hi + 0x40) > 0x280) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_8019C7B8_from_rcen);
#else
// a bolt between the two spawners 0x1C
void func_us_8019C7B8_from_rcen(Entity* self) {
    Entity* entity;
    Entity* source;
    Primitive* prim;
    Primitive* primTwo;
    s32 primIndex;
    s32 i;
    s32 posX;
    s32 posY;
    s16 boltAngle;
    s16 tempAngle;
    u8 closeToTarget;

    if (D_us_80180C0C) {
        DestroyEntity(self);
        return;
    }
    source = self->ext.galamothLightning.source;
    self->posX.i.hi = source->posX.i.hi;
    self->posY.i.hi = source->posY.i.hi;
    self->ext.galamothLightning.release = source->ext.galamothLightning.release;
    if (source->entityId != E_ID(UNK_1C)) {
        DestroyEntity(self);
        return;
    }
    closeToTarget = false;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x10);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.galamothLightning.prim = prim;
        self->ext.galamothLightning.primTwo = prim;
        self->flags |= FLAG_HAS_PRIMS;
        i = 0;
        while (prim != NULL) {
            PGREY(prim, 0) = 0x50;
            prim->tpage = 0x1A;
            prim->clut = 0x194;
            prim->u0 = prim->u1 = (i * 0x10) - 0x70;
            prim->u2 = prim->u3 = (i * 0x10) - 0x60;
            prim->v0 = prim->v2 = 0xD0;
            prim->v1 = prim->v3 = 0xC0;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority - 1;
            prim = prim->next;
            if (++i >= 6) {
                i = 0;
            }
        }
        // fallthrough
    case 1:
        prim = self->ext.galamothLightning.prim;
        prim->x0 = self->posX.i.hi;
        prim->y0 = self->posY.i.hi;
        prim->x1 = prim->x0;
        prim->y1 = prim->y1 - 0x10;
        prim->x2 = self->posX.i.hi;
        prim->y2 = self->posY.i.hi;
        prim->x3 = prim->x2;
        prim->y3 = prim->y2 - 0x10;
        self->ext.galamothLightning.primTwo = prim;
        self->ext.galamothLightning.boltAngle = Random() * 0x10;
        if (self->ext.galamothLightning.release) {
            // fan out behind the thrown spawners
            if (self->facingLeft) {
                self->ext.galamothLightning.boltAngle = 0x100;
                self->ext.galamothLightning.boltAngle =
                    (self->params << 7) + 0x100;
                self->ext.galamothLightning.boltAngle += Random() * 2;
            } else {
                self->ext.galamothLightning.boltAngle = 0x700;
                self->ext.galamothLightning.boltAngle =
                    0x700 - (self->params << 7);
                self->ext.galamothLightning.boltAngle -= Random() * 2;
            }
        }
        while (prim != NULL) {
            PGREY(prim, 0) = 0x50;
            if (self->ext.galamothLightning.release) {
                PGREY(prim, 0) = 0x50 - self->params * 8;
            }
            prim->drawMode = DRAW_HIDE;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim = prim->next;
        }
        self->ext.galamothLightning.segmentsBeforeAim = 0;
        self->step++;
        // fallthrough
    case 2:
        // nb. loop until the bolt reaches its target or runs out of prims
        for (;;) {
            prim = self->ext.galamothLightning.primTwo;
            boltAngle = self->ext.galamothLightning.boltAngle;
            entity = self->ext.galamothLightning.source + 1;
            posX = entity->posX.i.hi - prim->x2;
            posY = entity->posY.i.hi - prim->y2;
            if (abs(posX) < 0x10 && abs(posY) < 0x10) {
                break;
            }
            if (!self->ext.galamothLightning.release) {
                closeToTarget = false;
                if (abs(posX) < 0x20) {
                    closeToTarget = abs(posY) < 0x20;
                }
            }
            if (!self->ext.galamothLightning.segmentsBeforeAim) {
                self->ext.galamothLightning.segmentsBeforeAim = 4;
                if (closeToTarget) {
                    self->ext.galamothLightning.segmentsBeforeAim = 2;
                }
                tempAngle = ratan2(-posY, posX) - boltAngle;
                if (tempAngle > 0x800) {
                    tempAngle -= 0x1000;
                }
                if (tempAngle < -0x800) {
                    tempAngle += 0x1000;
                }
                if (self->ext.galamothLightning.release) {
                    self->ext.galamothLightning.segmentsBeforeAim = 0x10;
                    tempAngle /= 8;
                } else if (!closeToTarget) {
                    tempAngle /= 4;
                } else {
                    tempAngle /= 2;
                }
                self->ext.galamothLightning.angleStep = tempAngle;
            }
            boltAngle += self->ext.galamothLightning.angleStep;
            if (!closeToTarget && !self->ext.galamothLightning.release) {
                boltAngle += 0x120 - ((Random() & 3) * 0xC0);
            }
            boltAngle &= 0xFFF;
            primTwo = prim->next;
            if (primTwo == NULL) {
                break;
            }
            LOW(primTwo->x0) = LOW(prim->x2);
            LOW(primTwo->x1) = LOW(prim->x3);
            self->ext.galamothLightning.boltAngle = boltAngle;
            self->ext.galamothLightning.primTwo = primTwo;
            primTwo->x2 = primTwo->x0 + ((rcos(boltAngle) * 0xC) >> 12);
            primTwo->y2 = primTwo->y0 - ((rsin(boltAngle) * 0xC) >> 12);
            tempAngle = boltAngle - 0x400;
            posX = primTwo->x3 = primTwo->x2 + ((rcos(tempAngle) * 0x10) >> 12);
            posY = primTwo->y3 =
                primTwo->y2 - ((rsin(tempAngle) * 0x10) >> 12);
            posX -= PLAYER.posX.i.hi;
            posY -= PLAYER.posY.i.hi;
            if (abs(posX) < 0xC && abs(posY) < 0xC) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromCurrentEntity(E_ID(UNK_1D), entity);
                    entity->posX.i.hi = primTwo->x3;
                    entity->posY.i.hi = primTwo->y3;
                }
            }
            primTwo->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                                DRAW_UNK02 | DRAW_TRANSP;
            self->ext.galamothLightning.segmentsBeforeAim--;
        }
        self->step = 1;
        break;
    case 3:
        prim = self->ext.galamothLightning.prim;
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->step = 1;
        break;
    }
}
#endif

extern EInit D_us_80180A98;

void func_us_801991D4(Entity* self) {
    if (!self->step) {
        InitializeEntity(D_us_80180A98);
        return;
    }
    DestroyEntity(self);
}

void func_us_8019921C(Entity* self) {
    s32 args[9];

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitGalamothLvl50);
        self->animCurFrame = self->params;
        self->drawFlags = ENTITY_ROTATE;
        /* fall through */
    case 1:
        MoveEntity();
        self->rotate -= ROT(2.8125);
        self->velocityY += FIX(0.125);
        if (self->facingLeft) {
            self->velocityX = FIX(0.5);
        } else {
            self->velocityX = FIX(-0.5);
        }
        args[6] = 0x20;
        func_us_801955A0(self, args);
        if (self->ext.et_8019921C.hitFloor) {
            self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
            self->zPriority = 0x5F;
        }
        break;
    }
}

extern EInit D_us_80180AA4;
extern AnimationFrame D_us_80180C4C[];

void EntityMinotaurSpitLiquid(Entity* self) {
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180AA4);
        if (self->facingLeft) {
            self->rotate = -self->rotate;
        }
        self->facingLeft = 0;
        self->rotate += 0x100 - Random() * 2;

        angle = self->rotate;
        self->velocityX = rsin(angle) * 0x28;
        self->velocityY = rcos(angle) * -0x28;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        self->drawFlags =
            ENTITY_OPACITY | ENTITY_ROTATE | ENTITY_SCALEY | ENTITY_SCALEX;
        self->scaleX = 0x20;
        self->scaleY = 0x100;
        self->opacity = 0x80;
        /* fall through */

    case 1:
        MoveEntity();
        self->scaleX += 0xC;
        self->scaleY += 2;
        self->opacity -= 1;
        if (!AnimateEntity(D_us_80180C4C, self)) {
            DestroyEntity(self);
        }
        break;
    }
}

extern EInit D_us_80180AB0;
extern u8 D_us_80180C68[];

void func_us_8019943C(Entity* self) {
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180AB0);
        if (self->facingLeft) {
            self->rotate = -self->rotate;
        }
        self->facingLeft = 0;
        self->rotate += 0x100 - Random() * 2;

        angle = self->rotate;
        self->velocityX = rsin(angle) * 0x20;
        self->velocityY = rcos(angle) * -0x20;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        /* fall through */
    case 1:
        MoveEntity();
        self->posY.i.hi--;
        if (AnimateEntity(D_us_80180C68, self) == 0) {
            DestroyEntity(self);
        }
        break;
    }
}

extern s32 D_us_801817B8;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_8019953C);
#else
// starts the fight once Alucard is past the middle and places Galamoth
void func_us_8019953C(Entity* self) {
    Entity* entity;
    s16 x;
    s16 y;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        CreateEntityFromCurrentEntity(E_ID(UNK_16), &g_Entities[80]);
        g_Entities[80].posX.i.hi = 0x190 - g_Tilemap.scrollX.i.hi;
        g_Entities[80].posY.i.hi = 0x1A3 - g_Tilemap.scrollY.i.hi;
        // fallthrough
    case 1:
        if (PLAYER.posX.i.hi + g_Tilemap.scrollX.i.hi > 0x100) {
            GALAMOTH_FIGHT |= 1;
        }
        if (GALAMOTH_FIGHT & 1) {
            D_us_801817B8 = 1;
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_GALAMOTH_DEFEAT, TIMEATTACK_SET_VISITED);
            stopMusicFlag = true;
            currentMusicId = MU_DEATH_BALLAD;
            self->step++;
        }
        break;
    case 3:
        if (GALAMOTH_FIGHT & 2) {
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_GALAMOTH_DEFEAT, TIMEATTACK_SET_RECORD);
            g_api.PlaySfx(SET_UNK_90);
            currentMusicId = MU_CURSE_ZONE;
            self->step++;
        }
        break;
    case 4:
        if (GALAMOTH_FIGHT & 4) {
            self->step++;
        }
        break;
    case 5:
        x = 0x100 - g_Tilemap.scrollX.i.hi;
        y = 0x180 - g_Tilemap.scrollY.i.hi;
        entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_ID(LIFE_UP_SPAWN), self, entity);
            entity->params = 5;
            entity->posX.i.hi = x;
            entity->posY.i.hi = y;
            stopMusicFlag = true;
            D_us_801817B8 = 0;
            currentMusicId = MU_CURSE_ZONE;
            self->step++;
        }
        break;
    case 2:
    case 6:
        if (g_api.func_80131F68() == false) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;
    }
}
#endif

INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_801BA164_from_cat);

extern EInit D_us_80180A68;
extern u16 D_us_801817A8[];
extern u16 D_us_801817B0[];
#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo8/nonmatchings/unk_15868", func_us_80199A58);
#else
// BO1's EntityBossDoor: falls shut when the fight starts, opens after it
void func_us_80199A58(Entity* self) {
    Collider collider;
    Entity* newEntity;
    s32 i;
    s32 tilePos;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180A68);
        self->animCurFrame = self->params + 0x10;
        if (self->params) {
            self->posX.i.hi = 7 - g_Tilemap.scrollX.i.hi;
            self->posY.i.hi = 0x141 - g_Tilemap.scrollY.i.hi;
        } else {
            self->posX.i.hi = 0x1F7 - g_Tilemap.scrollX.i.hi;
            self->posY.i.hi = 0x41 - g_Tilemap.scrollY.i.hi;
        }
        tilePos = self->params ? 0x320 : 0x13F;
        for (i = 0; i < 4; i++, tilePos -= 0x20) {
            g_Tilemap.fg[tilePos] = D_us_801817B0[i];
        }
        self->zPriority = 0x5F;
        // fallthrough
    case 1:
        if (D_us_801817B8) {
            self->step++;
        }
        break;
    case 2:
        self->posY.val += self->velocityY;
        self->velocityY += FIX(0.5);
        g_api.CheckCollision(
            self->posX.i.hi, (s16)(self->posY.i.hi + 0x20), &collider, 0);
        if (collider.effects) {
            self->posY.i.hi += collider.unk18;
            self->velocityY = -self->velocityY;
            self->velocityY /= 4;
            if (!(D_us_801817B8 & 0x10)) {
                D_us_801817B8 |= 0x10;
            }
            newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (newEntity != NULL) {
                CreateEntityFromEntity(E_INTENSE_EXPLOSION, self, newEntity);
                newEntity->params = 0x10;
                newEntity->posX.i.hi += ((Random() & 7) * 2) - 8;
                newEntity->posY.i.hi += 0x20;
            }
            if (self->velocityY >= -0x1FFF) {
                self->step++;
            }
        }
        break;
    case 3:
        tilePos = self->params ? 0x320 : 0x13F;
        for (i = 0; i < 4; i++, tilePos -= 0x20) {
            g_Tilemap.fg[tilePos] = D_us_801817A8[i];
        }
        self->step++;
        break;
    case 4:
        if (!D_us_801817B8) {
            self->step++;
        }
        break;
    case 5:
        tilePos = self->params ? 0x320 : 0x13F;
        for (i = 0; i < 4; i++, tilePos -= 0x20) {
            g_Tilemap.fg[tilePos] = D_us_801817B0[i];
        }
        self->step++;
        break;
    case 6:
        self->flags |= FLAG_DESTROY_IF_OUT_OF_CAMERA;
        self->posY.i.hi--;
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180E20[];
extern u8 D_us_80180E4C[];
extern u8 D_us_80180E68[];
extern u8 D_us_80180E84[];
extern u8 D_us_80180EA0[];
extern u8 D_us_80180F34[];
extern u8 D_us_80180F50[];
extern u8 D_us_80180F6C[];
extern u8 D_us_80180F88[];
extern u8 D_us_80180FA4[];
extern u8 D_us_80180FC0[];
extern u8 D_us_80180FDC[];
extern u8 D_us_80180FF8[];
extern u8 D_us_8018105C[];
extern u8 D_us_80181078[];
extern u8 D_us_80181094[];
extern u8 D_us_801810B0[];
extern u8 D_us_801810CC[];
extern u8 D_us_80181138[];
extern u8 D_us_80181154[];
extern u8 D_us_80181170[];
extern u8 D_us_8018118C[];
extern u8 D_us_801811E8[];
extern u8 D_us_801811F8[];
extern u8 D_us_80181220[];
extern u8 D_us_8018123C[];
extern u8 D_us_80181278[];
extern u8 D_us_80181294[];
extern u8 D_us_801812E0[];
extern u8 D_us_801812FC[];
extern u8 D_us_80181330[];
extern u8 D_us_80181348[];
extern u8 D_us_80181360[];
extern u8 D_us_801813E8[];
extern u8 D_us_80181400[];
extern u8 D_us_80181418[];
extern u8 D_us_80181430[];
extern u8 D_us_80181448[];
extern u8 D_us_80181460[];
extern u8 D_us_80181478[];
extern u8 D_us_80181490[];
extern u8 D_us_80181500[];
extern u8 D_us_80181528[];
extern u8 D_us_80181540[];
extern u8 D_us_80181558[];
extern u8 D_us_80181570[];
extern u8 D_us_801815C0[];
extern u8 D_us_801815F4[];
extern u8 D_us_80181610[];
extern u8 D_us_8018162C[];
extern u8 D_us_80181648[];
extern u8 D_us_80181664[];
extern u8 D_us_80181680[];
extern u8 D_us_8018169C[];
extern u8 D_us_801816B8[];
// a pose; the 0x10 bytes after it (an unused table) are left out
u8 D_us_80180E20[] ALIGNED4 = {0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x02};
u8 D_us_80180E4C[] ALIGNED4 = {0x00, 0x05, 0x00, 0xFF, 0x00, 0x01, 0x00, 0xFB, 0x00, 0x00, 0x40, 0xFC, 0x00, 0xFD, 0x00, 0xFB, 0x00, 0xFE, 0x00, 0xFF, 0x80, 0x01, 0x80, 0x01, 0x60, 0x03, 0x00, 0x02};
u8 D_us_80180E68[] ALIGNED4 = {0x00, 0x02, 0x00, 0xFE, 0x00, 0x02, 0x00, 0xFD, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFC, 0x00, 0xFD, 0x00, 0xFF, 0xC0, 0x01, 0x80, 0x01, 0x80, 0x01, 0x00, 0x02, 0x00, 0x03};
u8 D_us_80180E84[] ALIGNED4 = {0x40, 0x02, 0xC0, 0xFD, 0x80, 0x02, 0x00, 0xFD, 0x00, 0x00, 0x00, 0xFF, 0x80, 0xFC, 0x80, 0xFD, 0x80, 0xFF, 0xC0, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x80, 0x03};
u8 D_us_80180EA0[] ALIGNED4 = {0x00, 0x01, 0xC0, 0xFF, 0x80, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x80, 0xFF, 0x00, 0xFF, 0xC0, 0x01, 0x00, 0x01, 0x80, 0x01, 0xC0, 0x01, 0x80, 0x01};
u8 D_us_80180F34[] ALIGNED4 = {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x06, 0x80, 0x06, 0x00, 0x00};
u8 D_us_80180F50[] ALIGNED4 = {0x00, 0x03, 0x00, 0xFF, 0x80, 0x01, 0x70, 0xFD, 0x00, 0x00, 0x00, 0x00, 0x40, 0xFC, 0x80, 0xFC, 0x00, 0x00, 0x00, 0x03, 0x00, 0xFE, 0x80, 0x00, 0x80, 0x05, 0x80, 0x00};
u8 D_us_80180F6C[] ALIGNED4 = {0x80, 0x03, 0x00, 0xFF, 0xC0, 0x01, 0x00, 0xFD, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFD, 0xC0, 0xFD, 0x80, 0x04, 0x00, 0x07, 0x00, 0xFF, 0x00, 0x02, 0x00, 0x08, 0x80, 0x00};
u8 D_us_80180F88[] ALIGNED4 = {0x80, 0x03, 0x00, 0xFF, 0x80, 0x01, 0xE0, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFD, 0xC0, 0xFD, 0x00, 0x05, 0x00, 0x08, 0x20, 0xFF, 0x40, 0x02, 0x00, 0x09, 0x80, 0x00};
u8 D_us_80180FA4[] ALIGNED4 = {0x80, 0x00, 0x00, 0xFD, 0x00, 0x03, 0x80, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0xFD, 0x00, 0x05, 0x80, 0x06, 0x80, 0xFE, 0x80, 0x00, 0x00, 0x09, 0x80, 0x00};
u8 D_us_80180FC0[] ALIGNED4 = {0x00, 0xFF, 0xC0, 0xFC, 0x00, 0x03, 0x40, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x80, 0xFC, 0x00, 0x03, 0x00, 0x06, 0x00, 0xFD, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00};
u8 D_us_80180FDC[] ALIGNED4 = {0x80, 0xFE, 0xE0, 0xFC, 0x00, 0x03, 0x40, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xC0, 0xFC, 0x80, 0x01, 0x00, 0x02, 0x00, 0xFE, 0x80, 0xFF, 0x00, 0x02, 0x80, 0x00};
u8 D_us_80180FF8[] ALIGNED4 = {0x00, 0x00, 0x80, 0xFD, 0x80, 0x03, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFD, 0x00, 0xFD, 0xC0, 0x00, 0x80, 0x02, 0x00, 0xFE, 0x80, 0xFF, 0x80, 0x02, 0x80, 0x00};
u8 D_us_8018105C[] ALIGNED4 = {0x80, 0x01, 0x80, 0x00, 0x80, 0x00, 0x00, 0xFE, 0x00, 0x00, 0xC0, 0xFF, 0x00, 0xFE, 0x80, 0xFF, 0x00, 0x02, 0x80, 0x02, 0x00, 0xFF, 0x00, 0x03, 0x80, 0x02, 0x80, 0x03};
u8 D_us_80181078[] ALIGNED4 = {0x80, 0x02, 0xC0, 0x00, 0x00, 0x01, 0xC0, 0xFD, 0x00, 0x00, 0xC0, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x80, 0xFD, 0x00, 0x01, 0x00, 0x07, 0x80, 0x01};
u8 D_us_80181094[] ALIGNED4 = {0x40, 0x02, 0x00, 0x01, 0x80, 0xFE, 0xA0, 0xFD, 0x00, 0x00, 0xC0, 0xFF, 0xE0, 0xFF, 0x00, 0x00, 0x80, 0x05, 0x00, 0x07, 0x80, 0xFD, 0xA0, 0x01, 0x00, 0x09, 0xA0, 0x01};
u8 D_us_801810B0[] ALIGNED4 = {0x00, 0x02, 0x00, 0x01, 0x80, 0xFE, 0x80, 0xFE, 0x00, 0x00, 0xC0, 0xFF, 0x20, 0x00, 0x40, 0x00, 0x00, 0x06, 0x80, 0x07, 0x80, 0xFD, 0x80, 0x01, 0x80, 0x08, 0x80, 0x01};
u8 D_us_801810CC[] ALIGNED4 = {0x00, 0x01, 0x80, 0x00, 0x80, 0xFE, 0xC0, 0xFD, 0x00, 0x00, 0xC0, 0xFF, 0xE0, 0xFF, 0x00, 0x00, 0xC0, 0x02, 0x00, 0x06, 0x80, 0xFF, 0x00, 0x02, 0x00, 0x07, 0x00, 0x02};
u8 D_us_80181138[] ALIGNED4 = {0x80, 0xFF, 0x00, 0xFF, 0x80, 0x00, 0x80, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0xFF, 0x00, 0xFF, 0x80, 0x01, 0x00, 0x02, 0x80, 0x04, 0x00, 0x03, 0x00, 0x05};
u8 D_us_80181154[] ALIGNED4 = {0x80, 0xFE, 0x00, 0xFE, 0x80, 0x02, 0x40, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0xFF, 0x00, 0x01, 0x80, 0x04, 0x00, 0xFE, 0x00, 0x01, 0x00, 0x06, 0x00, 0x02};
u8 D_us_80181170[] ALIGNED4 = {0x00, 0xFE, 0x80, 0xFD, 0x40, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFD, 0xC0, 0xFF, 0x80, 0x03, 0x00, 0x05, 0x00, 0xFC, 0x80, 0xFE, 0x00, 0x05, 0x80, 0xFE};
u8 D_us_8018118C[] ALIGNED4 = {0x00, 0xFE, 0x80, 0xFD, 0x40, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFD, 0x00, 0xFE, 0x00, 0x03, 0x80, 0x04, 0x80, 0xFC, 0x00, 0xFF, 0x80, 0x04, 0x80, 0xFF};
u8 D_us_801811E8[] ALIGNED4 = {0x00, 0xFD, 0x00, 0x03, 0x80, 0x04, 0x80, 0xFC, 0x00, 0xFF, 0x80, 0x04, 0x80, 0xFF, 0x00, 0x00};
u8 D_us_801811F8[] ALIGNED4 = {0x10, 0xFD, 0xE0, 0x02, 0xA0, 0x04, 0xA0, 0xFC, 0x20, 0xFF, 0x90, 0x04, 0xA0, 0xFF, 0x00, 0x00};
u8 D_us_80181220[] ALIGNED4 = {0x00, 0xFF, 0xC0, 0xFD, 0x80, 0x03, 0x00, 0x01, 0x00, 0x00, 0xC0, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x80, 0x02, 0x00, 0x06, 0x00, 0xFE, 0x00, 0x02, 0x00, 0x07, 0x40, 0x02};
u8 D_us_8018123C[] ALIGNED4 = {0x40, 0xFF, 0x00, 0xFE, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x40, 0x00, 0x40, 0x02, 0x80, 0x04, 0xC0, 0xFD, 0x80, 0x02, 0x80, 0x06, 0xC0, 0x02};
u8 D_us_80181278[] ALIGNED4 = {0x80, 0x01, 0x00, 0x01, 0x80, 0x00, 0x60, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFE, 0x00, 0xFF, 0x00, 0xFE, 0x80, 0xFF, 0x00, 0x02, 0x80, 0x03, 0x80, 0x04, 0x00, 0x04};
u8 D_us_80181294[] ALIGNED4 = {0x80, 0x01, 0x00, 0x01, 0x80, 0x00, 0x60, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x40, 0xFE, 0x80, 0xFE, 0x40, 0xFE, 0x20, 0x00, 0xC0, 0x01, 0xC0, 0x02, 0x60, 0x04, 0x00, 0x04};
u8 D_us_801812E0[] ALIGNED4 = {0x00, 0x02, 0x00, 0x01, 0x00, 0xFF, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0xFD, 0xC0, 0xFF, 0x00, 0x06, 0x40, 0x08, 0xC0, 0x02, 0x80, 0x08};
u8 D_us_801812FC[] ALIGNED4 = {0x80, 0x01, 0x00, 0x01, 0x60, 0xFF, 0x80, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x40, 0x00, 0x60, 0xFD, 0xA0, 0xFF, 0x40, 0x06, 0x80, 0x08, 0x40, 0x02, 0xA0, 0x08};
u8 D_us_80181330[] ALIGNED4 = {0x40, 0x00, 0xE0, 0xFF, 0x20, 0x00, 0xE0, 0xFD, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x40, 0xFF, 0x40, 0xFF, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00};
u8 D_us_80181348[] ALIGNED4 = {0x70, 0xFF, 0x70, 0xFF, 0x58, 0x01, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0x40, 0xFF, 0x40, 0xFF, 0x00, 0x00};
u8 D_us_80181360[] ALIGNED4 = {0x00, 0xFF, 0x80, 0xFE, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0xC0, 0xFE, 0xC0, 0xFE, 0x00, 0x00};
u8 D_us_801813E8[] ALIGNED4 = {0x00, 0x02, 0x00, 0xFF, 0x00, 0x01, 0x00, 0xFD, 0x00, 0x00, 0x00, 0xFC, 0x20, 0xFF, 0xC0, 0xFF, 0x00, 0x01, 0x80, 0xFF, 0x40, 0x02, 0x00, 0x00};
u8 D_us_80181400[] ALIGNED4 = {0xC0, 0xFE, 0x80, 0xFE, 0x40, 0x02, 0x00, 0xFC, 0x00, 0x00, 0x00, 0xFC, 0x00, 0xFF, 0x00, 0xFF, 0xC0, 0x02, 0x00, 0xFF, 0x40, 0x00, 0x00, 0x00};
u8 D_us_80181418[] ALIGNED4 = {0x40, 0xFE, 0x00, 0xFE, 0xC0, 0x02, 0x00, 0xFE, 0x00, 0x00, 0x00, 0xFE, 0x20, 0xFF, 0x00, 0x00, 0x40, 0x04, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80181430[] ALIGNED4 = {0x40, 0xFF, 0x00, 0xFD, 0x40, 0x02, 0x00, 0xFF, 0x80, 0xFE, 0x00, 0x00, 0x40, 0xFF, 0x00, 0x00, 0x40, 0x04, 0x80, 0xFE, 0xC0, 0xFF, 0x00, 0x00};
u8 D_us_80181448[] ALIGNED4 = {0x20, 0x00, 0x80, 0xFC, 0xC0, 0x01, 0x00, 0x00, 0xC0, 0xFD, 0x00, 0x01, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x04, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80181460[] ALIGNED4 = {0x80, 0x00, 0x00, 0xFC, 0x00, 0x01, 0x40, 0x00, 0x80, 0xFC, 0x00, 0x01, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x03, 0x40, 0xFF, 0x80, 0x00, 0x00, 0x00};
u8 D_us_80181478[] ALIGNED4 = {0x00, 0x03, 0x00, 0xFE, 0x80, 0x01, 0x80, 0xFF, 0x80, 0xFE, 0x80, 0x00, 0x00, 0xFD, 0x80, 0x00, 0x80, 0x01, 0xC0, 0x00, 0x80, 0x00, 0x00, 0x00};
u8 D_us_80181490[] ALIGNED4 = {0x80, 0x02, 0x80, 0xFE, 0x80, 0x02, 0x80, 0xFE, 0x00, 0x01, 0x00, 0xFF, 0x00, 0xFE, 0x80, 0x00, 0x80, 0x01, 0xC0, 0x00, 0x80, 0x00, 0x00, 0x00};
u8 D_us_80181500[] ALIGNED4 = {0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 D_us_80181528[] ALIGNED4 = {0x80, 0x00, 0xC0, 0xFF, 0x00, 0x03, 0x00, 0xFE, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x01, 0x80, 0xFF, 0x00, 0xFF, 0x00, 0x03, 0x00, 0x01, 0x80, 0x01};
u8 D_us_80181540[] ALIGNED4 = {0xA0, 0x00, 0xC0, 0xFF, 0x00, 0x04, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x80, 0x01, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x04, 0x40, 0xFE, 0x00, 0x00};
u8 D_us_80181558[] ALIGNED4 = {0x80, 0x00, 0xC0, 0xFF, 0x00, 0x05, 0x80, 0x03, 0x00, 0x00, 0x00, 0x04, 0x80, 0x01, 0x80, 0xFF, 0x00, 0x01, 0x00, 0x05, 0x00, 0xFE, 0x00, 0x00};
u8 D_us_80181570[] ALIGNED4 = {0x80, 0x00, 0xC0, 0xFF, 0x80, 0x04, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x80, 0x01, 0x80, 0xFF, 0x80, 0x00, 0x00, 0x03, 0x00, 0xFF, 0x80, 0x00};
u8 D_us_801815C0[] ALIGNED4 = {0x00, 0x01, 0xC0, 0xFF, 0x80, 0xFE, 0x00, 0xFE, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x01, 0x80, 0x01, 0x00, 0xFF, 0x80, 0x01, 0x80, 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 0x03};
u8 D_us_801815F4[] ALIGNED4 = {0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0xFD, 0x00, 0x00, 0x80, 0xFF, 0x00, 0xFE, 0x80, 0xFE, 0x00, 0xFE, 0x00, 0xFF, 0x80, 0x01, 0x80, 0x01, 0x60, 0x03, 0x00, 0x02};
u8 D_us_80181610[] ALIGNED4 = {0x00, 0x05, 0x00, 0xFF, 0x00, 0x01, 0x00, 0xFB, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFE, 0x80, 0xFE, 0x00, 0xFE, 0x00, 0xFF, 0x80, 0x01, 0x80, 0x01, 0x60, 0x03, 0x00, 0x02};
u8 D_us_8018162C[] ALIGNED4 = {0x00, 0x05, 0x00, 0xFF, 0x00, 0x01, 0x00, 0xFB, 0x00, 0x00, 0x00, 0xFF, 0x80, 0xFE, 0x00, 0xFF, 0x00, 0xFE, 0x00, 0xFF, 0x80, 0x01, 0x80, 0x01, 0x60, 0x03, 0x00, 0x02};
u8 D_us_80181648[] ALIGNED4 = {0x00, 0x05, 0x00, 0xFF, 0x00, 0x01, 0x00, 0xFB, 0x00, 0x00, 0x00, 0xFF, 0x60, 0xFE, 0x80, 0xFE, 0x00, 0xFF, 0x00, 0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x04, 0x00, 0x02};
u8 D_us_80181664[] ALIGNED4 = {0x00, 0x05, 0x00, 0xFF, 0x00, 0x01, 0x00, 0xFB, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFE, 0x00, 0xFD, 0x00, 0x03, 0x00, 0x04, 0x80, 0x01, 0x80, 0x01, 0x00, 0x06, 0x00, 0x02};
u8 D_us_80181680[] ALIGNED4 = {0x00, 0x05, 0x00, 0xFF, 0x00, 0x01, 0x00, 0xFB, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0xFD, 0x00, 0xFD, 0x80, 0x03, 0x80, 0x04, 0x80, 0x01, 0xA0, 0x01, 0x00, 0x07, 0x00, 0x02};
u8 D_us_8018169C[] ALIGNED4 = {0x80, 0x03, 0x00, 0xFF, 0x80, 0xFF, 0x80, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFC, 0x00, 0xFC, 0x80, 0x02, 0x80, 0x04, 0x00, 0x02, 0x80, 0x02, 0x00, 0x07, 0x00, 0x03};
u8 D_us_801816B8[] ALIGNED4 = {0x80, 0x07, 0x80, 0x01, 0x00, 0x01, 0x80, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x01, 0x80, 0x04, 0x00, 0x04, 0x00, 0x06, 0x00, 0x07, 0x00, 0x07};
s32 D_us_80180C0C = 0;
s16 D_us_80180C10[8] = { 0, 0x1D, 0, 4, 4, -4, -8, 0 };
u8 D_us_80180C20[0x2C] = {
    1,
    0x13,
    1,
    0x14,
    1,
    0x15,
    1,
    0x16,
    1,
    0x12,
    1,
    0x17,
    1,
    0x18,
    1,
    0x19,
    1,
    0x1A,
    1,
    0x12,
    1,
    0x1B,
    1,
    0x1C,
    1,
    0x1D,
    1,
    0x1E,
    1,
    0x12,
    1,
    0x1F,
    1,
    0x20,
    1,
    0x21,
    1,
    0x22,
    1,
    0x12,
    0,
    0,
    0,
    0,
};
AnimationFrame D_us_80180C4C[7] = {
    { 0x103, 0x203 },
    { 0x303, 0x403 },
    { 0x503, 0x603 },
    { 0x703, 0x803 },
    { 0x903, 0xA03 },
    { 0xB03, 0xC03 },
    { 0xD03, 0xFF },
};
u8 D_us_80180C68[0x20] = {
    2,
    1,
    2,
    2,
    2,
    3,
    2,
    4,
    2,
    5,
    2,
    6,
    2,
    7,
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
    0xFF,
    0,
    0,
    0,
};
giantBroBodyPartsInit D_us_80180CD0[0x10] = {
    { 0xC, 1, 0x2E, 0xB, 5 },
    { 0xD, 0xC, 0x30, 0xC, 6 },
    { 0xE, 0xD, 0, 0xD, 7 },
    { 0xF, 1, 0x2E, 4, 0xFFFD },
    { 0x10, 0xF, 0x30, 5, 0xFFFC },
    { 0x11, 0x10, 0, 6, 0xFFFB },
    { 1, 0, 0x26, 8, 1 },
    { 3, 0, 0xFFEC, 0xA, 3 },
    { 2, 3, 0xFFEC, 9, 2 },
    { 4, 3, 0x22, 0xE, 8 },
    { 6, 4, 0x2E, 0xF, 9 },
    { 7, 6, 0x4C, 0x10, 0xA },
    { 8, 3, 0x22, 1, 0xFFFA },
    { 0xA, 8, 0x2E, 2, 0xFFF9 },
    { 0xB, 0xA, 0x10, 3, 0xFFF8 },
    { 0, 0, 0, 0, 0 },
};
s16 D_us_80180D70[0x10] = { 1, 3, 2, 4, 6, 7, 8, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0 };
s16 D_us_80180D90[0x10] = { 0xC, 0xD, 0xF, 0x10, 0xE, 0x11, 3, 2, 4, 6, 8, 0xA, 7, 0xB, 0, 0 };
s16 D_us_80180DB0[0x10] = { 0xF, 0x10, 0xC, 0xD, 0x11, 0xE, 3, 2, 4, 6, 8, 0xA, 7, 0xB, 0, 0 };
s16 D_us_80180DD0[0xC] = { 0xC, 0xD, 0xF, 0x10, 0xE, 0x11, 3, 4, 6, 8, 0xA, 0 };
s16 D_us_80180DE8[0xC] = { 0xF, 0x10, 0xC, 0xD, 0x11, 0xE, 3, 8, 0xA, 4, 6, 0 };
s16 D_us_80180E00[8] = { 0xC, 0xD, 0xF, 0x10, 0xE, 0x11, 0, 0 };
s16 D_us_80180E10[8] = { 0xF, 0x10, 0xC, 0xD, 0x11, 0xE, 0, 0 };
unkStr_801CDD80 D_us_80180EBC[2] = { { 1, D_us_80180E4C }, { 0, NULL } };
unkStr_801CDD80 D_us_80180ECC[5] = {
    { 0x24, D_us_80180E68 },
    { 0xC, D_us_80180E84 },
    { 0x1C, D_us_80180E20 },
    { 0x18, D_us_80180EA0 },
    { 0, NULL },
};
extern u8 D_us_80180EF4[];
extern u8 D_us_80181398[];
u8 D_us_80180EF4[] ALIGNED4 = {0x00, 0x01, 0x80, 0x00, 0x40, 0x00, 0x80, 0xFE, 0x00, 0x00, 0xA0, 0xFF, 0x40, 0xFE, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x01, 0x00, 0x01, 0x00, 0x04, 0x20, 0x01, 0x80, 0x01};
u8 D_us_80181398[] ALIGNED4 = {0x00, 0x02, 0xC0, 0x00, 0x00, 0xFF, 0x80, 0xFE, 0x00, 0x00, 0xC0, 0xFF, 0x00, 0x00, 0xC0, 0xFE, 0xC0, 0xFE, 0x00, 0x01, 0x80, 0x02, 0x00, 0x00};
// spimdisasm decoded these two as a string and words; the words after
// each terminator are unreferenced
unkStr_801CDD80 D_us_80180F10[] = { { 0x20, (s16*)D_us_80180EF4 }, { 0, NULL } };
s16 D_us_80180F28[6] = { 3, 2, 8, 0xA, 0xB, 0 };
unkStr_801CDD80 D_us_80180F40[2] = { { 0x12, D_us_80180F34 }, { 0, NULL } };
unkStr_801CDD80 D_us_80181014[9] = {
    { 0x28, D_us_80180F50 },
    { 0x10, D_us_80180F6C },
    { 0xC, D_us_80180F88 },
    { 4, D_us_80180FA4 },
    { 8, D_us_80180FC0 },
    { 6, D_us_80180FDC },
    { 0x12, D_us_80180FF8 },
    { 0x1C, D_us_80180EA0 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_801810E8[7] = {
    { 0x10, D_us_80180E20 },
    { 8, D_us_80180EA0 },
    { 0x12, D_us_8018105C },
    { 8, D_us_80181078 },
    { 6, D_us_80181094 },
    { 0x18, D_us_801810B0 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_80181120[3] = { { 0x10, D_us_801810CC }, { 0xC, D_us_80180EA0 }, { 0, NULL } };
unkStr_801CDD80 D_us_801811A8[6] = {
    { 0x10, D_us_80180EA0 },
    { 0x10, D_us_80181138 },
    { 8, D_us_80181154 },
    { 6, D_us_80181170 },
    { 0x18, D_us_8018118C },
    { 0, NULL },
};
s16 D_us_801811D8[8] = { 3, 4, 6, 8, 0xA, 7, 0xB, 0 };
unkStr_801CDD80 D_us_80181208[3] = { { 2, D_us_801811E8 }, { 2, D_us_801811F8 }, { 0, NULL } };
unkStr_801CDD80 D_us_80181258[4] = {
    { 6, D_us_80181220 },
    { 0x18, D_us_8018123C },
    { 0x30, D_us_80180EA0 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_801812B0[3] = { { 0x10, D_us_80180EA0 }, { 0xA, D_us_80181278 }, { 0, NULL } };
unkStr_801CDD80 D_us_801812C8[3] = { { 0x40, D_us_80181294 }, { 0x10, D_us_80181294 }, { 0, NULL } };
unkStr_801CDD80 D_us_80181318[3] = { { 8, D_us_801812E0 }, { 0x20, D_us_801812FC }, { 0, NULL } };
unkStr_801CDD80 D_us_80181378[4] = {
    { 0x10, D_us_80181330 },
    { 0x10, D_us_80181348 },
    { 0x10, D_us_80181360 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_801813B0[] = {
    { 0x18, (s16*)D_us_80181348 }, { 0x18, (s16*)D_us_80181330 },
    { 0x18, (s16*)D_us_80181398 }, { 0, NULL },
};
unkStr_801CDD80 D_us_801814A8[4] = {
    { 0x18, D_us_801813E8 },
    { 0xA, D_us_80181400 },
    { 8, D_us_80181418 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_801814C8[7] = {
    { 7, D_us_80181430 },
    { 7, D_us_80181448 },
    { 7, D_us_80181460 },
    { 0x10, D_us_80181478 },
    { 0x20, D_us_80181490 },
    { 0x40, D_us_80181490 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_80181518[2] = { { 0x20, D_us_80181500 }, { 0, NULL } };
unkStr_801CDD80 D_us_80181588[7] = {
    { 0xC, D_us_80180E20 },
    { 8, D_us_80181528 },
    { 8, D_us_80181540 },
    { 0x10, D_us_80181558 },
    { 0x10, D_us_80181570 },
    { 0x20, D_us_80180EA0 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_801815DC[3] = { { 0xC, D_us_801815C0 }, { 0x12, D_us_80180E20 }, { 0, NULL } };
unkStr_801CDD80 D_us_801816D4[3] = { { 0x10, D_us_80180E20 }, { 0x10, D_us_80180EA0 }, { 0, NULL } };
unkStr_801CDD80 D_us_801816EC[5] = {
    { 0x28, D_us_801815F4 },
    { 8, D_us_801815F4 },
    { 0xC, D_us_80181610 },
    { 0x20, D_us_8018162C },
    { 0, NULL },
};
unkStr_801CDD80 D_us_80181714[4] = {
    { 0x30, D_us_80181648 },
    { 0x50, D_us_80181664 },
    { 0x40, D_us_80181680 },
    { 0, NULL },
};
unkStr_801CDD80 D_us_80181734[2] = { { 0x50, D_us_8018169C }, { 0, NULL } };
unkStr_801CDD80 D_us_80181744[2] = { { 0x10, D_us_801816B8 }, { 0, NULL } };
u16 D_us_801817A8[4] = { 0x51E, 0x1D, 0x1D, 0x1D };
u16 D_us_801817B0[4] = { 0x25, 0, 0, 0 };
s32 D_us_801817B8 = 0;
s8 D_us_80180C88[0x48] = {
    0,
    0,
    0,
    0,
    0,
    -0x12,
    0xB,
    0xA,
    0,
    -4,
    8,
    8,
    -1,
    1,
    0x10,
    0x10,
    0,
    -0xA,
    0xC,
    0x14,
    0,
    -0x12,
    0xB,
    0x12,
    -3,
    0xF,
    0xC,
    0xD,
    -1,
    0xC,
    0xE,
    0xF,
    1,
    -1,
    0xE,
    0xF,
    1,
    -0x12,
    0x16,
    0x10,
    5,
    -2,
    0x13,
    0x11,
    0,
    -0xA,
    0xE,
    0x14,
    1,
    -0x12,
    0xB,
    0x12,
    -4,
    0xE,
    0xE,
    0xD,
    -1,
    -0xF,
    0xE,
    0xD,
    0,
    -0x10,
    0xB,
    0xC,
    0,
    -1,
    0xB,
    0xA,
    0,
    -0x10,
    0x18,
    0xF,
};
// end pc_import_data.py
// clang-format on
#endif
