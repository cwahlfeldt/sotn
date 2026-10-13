// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo0.h"

void EntityExplosion(Entity* self);

extern EInit g_EInitOlroxAfterImage;
extern EInit D_us_801806D8;
extern EInit D_us_801806E4;
extern EInit D_us_801806F0;
extern EInit D_us_801806FC;
extern s32 D_us_80180BA8; // set when Olrox falls: what he summoned vanishes
extern u8 D_us_80180BAC[16];
extern s16 D_us_80180BBC[8];
extern s16 D_us_80180BCC[4];
extern s16 D_us_80180BD4[9][2];
extern s16 D_us_80180BF8[4];
extern u8 D_us_80180C00[];
extern u8 D_us_80180C0C[];
extern u8 D_us_80180C24[];
extern u8 D_us_80180C40[];
extern u8 D_us_80180C4C[];
extern u8 D_us_80180C58[];
extern u8 D_us_80180C6C[];
extern u8 D_us_80180C80[];
extern u8 D_us_80180CAC[];
extern u8 D_us_80180CBC[];
extern u8 D_us_80180CC4[];
extern u8 D_us_80180CD0[];
extern u8 D_us_80180CDC[];
extern u8 D_us_80180CEC[];
extern u8 D_us_80180D00[];
extern u8 D_us_80180D10[];
extern u8 D_us_80180D20[];
extern u8 D_us_80180D2C[];
extern u8 D_us_80180D38[];
extern u8 D_us_80180D48[];
extern u8 D_us_80180D50[];
extern u8 D_us_80180D68[];
extern u8 D_us_80180D74[];
extern u8 D_us_80180D7C[];
extern s16 D_us_80180D94[][6];

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801AD26C);
#else
// turns an angle toward another by at most maxStep
s32 func_us_801AD26C(s16 maxStep, s32 angle, s32 target) {
    s16 diff;

    angle &= 0xFFF;
    diff = (target & 0xFFF) - angle;
    if (diff > 0x800) {
        diff -= 0x1000;
    }
    if (diff < -0x800) {
        diff += 0x1000;
    }
    if (diff < -maxStep) {
        diff = -maxStep;
    }
    if (diff > maxStep) {
        diff = maxStep;
    }
    return (angle + diff) & 0xFFF;
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801AD2F0);
#else
s32 func_us_801AD2F0(s16 x, s16 y) {
    Collider col;

    g_api.CheckCollision(x, y, &col, 0);
    return col.effects & EFFECT_SOLID;
}
#endif

INCLUDE_RODATA("boss/bo0/nonmatchings/2D26C", D_us_801A9344);

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801AD338);
#else
static void OlroxSpawnAfterImage(Entity* self) {
    Entity* entity;

    entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
    if (entity != NULL) {
        CreateEntityFromEntity(E_OLROX_AFTER_IMAGE, self, entity);
        entity->params = self->animCurFrame;
        entity->facingLeft = self->facingLeft;
    }
}

static void OlroxStartMusic(void) {
    stopMusicFlag = 0;
    currentMusicId = 0x334;
    g_api.PlaySfx(0x334);
}

// Olrox's first form
void func_us_801AD338(Entity* self) {
    Entity* entity;
    s32 i;
    s32 count;
    s16 x;
    s16 dx;
    s16 absX;
    u8 attack;
    u8 side;

    g_api.func_8010DFF0(1, 2);
    if ((self->flags & FLAG_DEAD) && self->step < 14) {
        self->hitboxState = 0;
        SetStep(14);
    }

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitOlroxAfterImage);
        self->animCurFrame = 0xE;
        self->hitboxState = 0;
        break;

    case 1:
        if (GetDistanceToPlayerX() < 0x60 && g_Player.vram_flag == 1) {
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_OLROX_DEFEAT, TIMEATTACK_SET_VISITED);
            SetStep(2);
            self->ext.olroxHuman.timer = 0xC0;
        }
        if ((self - 1)->flags & FLAG_DEAD) {
            OlroxStartMusic();
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_OLROX_DEFEAT, TIMEATTACK_SET_VISITED);
            SetStep(13);
            self->hitboxState = 3;
        }
        break;

    case 2:
        if ((self - 1)->flags & FLAG_DEAD) {
            OlroxStartMusic();
            SetStep(13);
            self->hitboxState = 3;
            break;
        }
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180C00, self)) {
                SetSubStep(1);
            }
            break;
        case 1:
            if (!AnimateEntity(D_us_80180C0C, self)) {
                OlroxStartMusic();
                self->hitboxState = 3;
                SetStep(3);
            }
            break;
        }
        break;

    case 3:
        // pick the next move from where Olrox and the player are
        absX = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
        dx = PLAYER.posX.i.hi - self->posX.i.hi;
        if ((absX < 0x50 && (u16)dx < 0x40) ||
            (absX > 0x1B0 && (u16)dx > 0xFFBF)) {
            SetStep(13);
            break;
        }
        if (abs(dx) < 0x24) {
            self->facingLeft = GetSideToPlayer() & 1;
            SetStep(5);
            break;
        }
        if ((absX < 0x80 && (u16)dx < 0x70) ||
            (absX > 0x180 && (u16)dx > 0xFF8F)) {
            SetStep(4);
            break;
        }
        if (abs(dx) < 0x40) {
            self->facingLeft = GetSideToPlayer() & 1;
            SetStep(6);
            self->step_s = 1;
        } else if (abs(dx) > 0xB0) {
            self->facingLeft = GetSideToPlayer() & 1;
            SetStep(6);
        } else {
            self->facingLeft = GetSideToPlayer() & 1;
            if (self->hitPoints < g_api.enemyDefs[50].hitPoints * 3 / 4) {
                self->ext.olroxHuman.attack |= 8;
            }
            SetStep(D_us_80180BAC[self->ext.olroxHuman.attack]);
            self->ext.olroxHuman.attack =
                (self->ext.olroxHuman.attack + 1) & 7;
        }
        break;

    case 4:
        // leap over the player, dropping after-images
        switch (self->step_s) {
        case 0:
            absX = g_Tilemap.scrollX.i.hi + self->posX.i.hi;
            if ((absX < 0x60 && self->facingLeft) ||
                (absX > 0x1B0 && !self->facingLeft)) {
                SetStep(3);
                break;
            }
            if (!AnimateEntity(D_us_80180D00, self)) {
                self->velocityY = FIX(-4.5);
                self->velocityX = 0;
                self->animCurFrame = 0x19;
                if (PLAYER.posX.i.hi < self->posX.i.hi) {
                    self->ext.olroxHuman.leapLeft = 1;
                } else {
                    self->ext.olroxHuman.leapLeft = 0;
                }
                SetSubStep(1);
            }
            break;
        case 1:
            MoveEntity();
            if (self->velocityY > FIX(-1.5)) {
                self->velocityY += FIX(1.0 / 16);
            } else {
                self->velocityY += FIX(1.0 / 8);
            }
            if (self->velocityY >= 0) {
                if (self->ext.olroxHuman.leapLeft) {
                    self->velocityX = FIX(-3);
                } else {
                    self->velocityX = FIX(3);
                }
                if (self->facingLeft == self->ext.olroxHuman.leapLeft) {
                    self->animCurFrame = 0x19;
                } else {
                    self->animCurFrame = 0x2D;
                }
                self->ext.olroxHuman.timer = 0x18;
                self->step_s++;
            }
            break;
        case 2:
            if (!--self->ext.olroxHuman.timer) {
                self->ext.olroxHuman.timer = 8;
                self->step_s++;
            }
            break;
        case 3:
            if (!--self->ext.olroxHuman.timer) {
                self->ext.olroxHuman.timer = 8;
                OlroxSpawnAfterImage(self);
            }
            MoveEntity();
            self->facingLeft = GetSideToPlayer() & 1;
            if (self->ext.olroxHuman.leapLeft == self->facingLeft) {
                self->animCurFrame = 0x19;
            } else {
                self->animCurFrame = 0x2D;
            }
            absX = g_Tilemap.scrollX.i.hi + self->posX.i.hi;
            if (absX < 0x38 && self->velocityX < 0) {
                self->step_s++;
            }
            if (absX > 0x1C8 && self->velocityX > 0) {
                self->step_s++;
            }
            if (GetDistanceToPlayerX() > 0x50) {
                side = GetSideToPlayer() & 1;
                if (self->ext.olroxHuman.leapLeft != side) {
                    self->step_s++;
                }
            }
            break;
        case 4:
            absX = g_Tilemap.scrollX.i.hi + self->posX.i.hi;
            if (absX < 0x38) {
                self->velocityX = 0;
                self->posX.i.hi = 0x38 - g_Tilemap.scrollX.i.hi;
            }
            if (absX > 0x1C8) {
                self->velocityX = 0;
                self->posX.i.hi = 0x1C8 - g_Tilemap.scrollX.i.hi;
            }
            self->velocityY -= FIX(1.0 / 8);
            if (UnkCollisionFunc3(D_us_80180BBC) & 1) {
                self->step_s++;
                PlaySfxPositional(0x647);
            }
            break;
        case 5:
            if (!AnimateEntity(D_us_80180D10, self)) {
                SetStep(3);
            }
            break;
        }
        break;

    case 5:
        // hop back
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180CDC, self)) {
                self->velocityX = FIX(-4);
                if (self->facingLeft) {
                    self->velocityX = FIX(4);
                }
                self->velocityY = FIX(-3);
                self->animCurFrame = 0x2D;
                self->ext.olroxHuman.timer = 8;
                SetSubStep(1);
            }
            break;
        case 1:
            if (!--self->ext.olroxHuman.timer) {
                self->ext.olroxHuman.timer = 8;
                OlroxSpawnAfterImage(self);
            }
            absX = g_Tilemap.scrollX.i.hi + self->posX.i.hi;
            if (absX < 0x38) {
                self->posX.i.hi = 0x38 - g_Tilemap.scrollX.i.hi;
            }
            if (absX > 0x1C8) {
                self->posX.i.hi = 0x1C8 - g_Tilemap.scrollX.i.hi;
            }
            if (UnkCollisionFunc3(D_us_80180BBC) & 1) {
                self->step_s++;
                PlaySfxPositional(0x647);
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80180CEC, self)) {
                SetStep(3);
            }
            break;
        }
        break;

    case 6:
        // dash toward (step_s 0) or away from (step_s 1) the player
        switch (self->step_s) {
        case 0:
            self->velocityX = FIX(2.25);
            if (self->facingLeft) {
                self->velocityX = FIX(-2.25);
            }
            self->animCurFrame = 0x19;
            self->ext.olroxHuman.timer = 8;
            self->step_s = 2;
            break;
        case 1:
            self->velocityX = FIX(-2.25);
            if (self->facingLeft) {
                self->velocityX = FIX(2.25);
            }
            self->animCurFrame = 0x2D;
            self->ext.olroxHuman.timer = 8;
            self->step_s = 2;
            break;
        case 2:
            UnkCollisionFunc2(D_us_80180BCC);
            if (!--self->ext.olroxHuman.timer) {
                self->ext.olroxHuman.timer = 8;
                OlroxSpawnAfterImage(self);
            }
            if (self->animCurFrame == 0x2D) {
                if (self->velocityX == 0 || GetDistanceToPlayerX() > 0x60) {
                    self->facingLeft = GetSideToPlayer() & 1;
                    SetStep(3);
                }
            }
            if (self->animCurFrame == 0x19) {
                if (self->velocityX == 0 || GetDistanceToPlayerX() < 0x70) {
                    self->facingLeft = GetSideToPlayer() & 1;
                    SetStep(3);
                }
            }
            break;
        }
        break;

    case 7:
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180C40, self)) {
                SetSubStep(1);
            }
            break;
        case 1:
            self->step_s++;
            break;
        case 2:
            if (!AnimateEntity(D_us_80180C4C, self)) {
                SetStep(3);
            }
            break;
        }
        break;

    case 8:
        // summon bats in the nine slots after Olrox's
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180C58, self)) {
                SetSubStep(1);
            }
            break;
        case 1:
            entity = self + 1;
            for (i = 0; i < 9; i++, entity++) {
                if (!entity->entityId) {
                    CreateEntityFromCurrentEntity(E_UNK_2D, entity);
                    entity->posX.i.hi = D_us_80180BD4[i][0];
                    entity->ext.olroxChild.parent = self;
                    entity->params = i;
                    entity->posY.i.hi = D_us_80180BD4[i][1];
                }
            }
            self->ext.olroxHuman.releaseBats = 0;
            self->step_s++;
            PlaySfxPositional(0x816);
            break;
        case 2:
            // wait until every bat reached Olrox
            count = 1;
            entity = self + 1;
            for (i = 0; i < 9; i++, entity++) {
                if (entity->entityId && !entity->hitboxState) {
                    count = 0;
                }
            }
            if (count) {
                self->step_s++;
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180C6C, self)) {
                self->step_s++;
            }
            if (self->pose == 5) {
                if (!self->poseTimer) {
                    PlaySfxPositional(0x818);
                }
                self->ext.olroxHuman.releaseBats = 1;
            }
            break;
        case 4:
            SetStep(3);
            break;
        }
        break;

    case 9:
        // open a portal spitting skulls
        switch (self->step_s) {
        case 0:
            count = 0;
            entity = &g_Entities[160];
            for (i = 7; i >= 0; i--, entity++) {
                if (entity->entityId) {
                    count++;
                }
            }
            if ((count & 0xFF) < 5) {
                if (!AnimateEntity(D_us_80180CAC, self)) {
                    SetSubStep(1);
                }
            } else {
                SetStep(3);
            }
            break;
        case 1:
            entity = self + 10;
            CreateEntityFromEntity(E_UNK_2E, self, entity);
            self->ext.olroxHuman.timer = 0x60;
            entity->ext.olroxChild.parent = self;
            self->ext.olroxHuman.closePortal = 0;
            entity->posY.i.hi -= 0x46;
            self->step_s++;
            PlaySfxPositional(0x636);
            break;
        case 2:
            if (!--self->ext.olroxHuman.timer) {
                self->ext.olroxHuman.closePortal = 1;
                self->step_s++;
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180CBC, self)) {
                SetStep(3);
            }
            break;
        }
        break;

    case 10:
        // fire a beam
        if (!self->step_s) {
            if (self->posX.i.hi & 0x100) {
                SetStep(3);
                break;
            }
            self->step_s++;
        }
        if (!AnimateEntity(D_us_80180C24, self)) {
            SetStep(3);
        }
        if (self->pose == 4 && !self->poseTimer) {
            PlaySfxPositional(0x7D1);
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_30, self, entity);
                entity->facingLeft = self->facingLeft;
            }
        }
        break;

    case 11:
        // raise three pillars across the room
        switch (self->step_s) {
        case 0:
            entity = self + 11;
            if (entity->entityId) {
                SetStep(3);
                break;
            }
            x = 0x20;
            if ((s16)(g_Tilemap.scrollX.i.hi + 0x20) < 0x50) {
                x = 0x50;
            }
            if ((s16)(g_Tilemap.scrollX.i.hi + 0x20) > 0x110) {
                x = 0x110 - g_Tilemap.scrollX.i.hi;
            }
            count = Random() & 3;
            for (i = 0; i < 3; i++, entity++) {
                CreateEntityFromCurrentEntity(E_UNK_31, entity);
                entity->params = count & 0xFF;
                count = (count + 1) & 3;
                entity->posX.i.hi = x;
                x += 0x50;
                entity->ext.olroxChild.parent = self;
                entity->posY.i.hi = self->posY.i.hi + 0x1E;
            }
            self->ext.olroxHuman.raisePillars = 0;
            self->step_s++;
            g_api.PlaySfx(0x684);
            break;
        case 1:
            if (!AnimateEntity(D_us_80180C80, self)) {
                SetStep(3);
            }
            if (self->pose == 0x10 && !self->poseTimer) {
                self->ext.olroxHuman.raisePillars = 1;
            }
            break;
        }
        break;

    case 12:
        if (!self->step_s) {
            // both directions are the same in the original
            self->velocityX = FIX(-0.5);
            if (self->facingLeft) {
                self->velocityX = FIX(-0.5);
            }
        }
        UnkCollisionFunc2(D_us_80180BCC);
        if (!AnimateEntity(D_us_80180D20, self)) {
            SetStep(3);
        }
        break;

    case 13:
        // vanish and reappear next to the player
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180CC4, self)) {
                SetSubStep(1);
                self->scaleY = 0x100;
                self->drawFlags |= ENTITY_SCALEY;
                PlaySfxPositional(0x6D9);
            }
            if (self->pose >= 2) {
                self->hitboxState = 0;
            }
            break;
        case 1:
            self->scaleY -= 0x20;
            if (!self->scaleY) {
                self->drawFlags = ENTITY_DEFAULT;
                self->animCurFrame = 0;
                SetSubStep(2);
            }
            break;
        case 2:
            if ((s16)(self->posX.i.hi + g_Tilemap.scrollX.i.hi) < 0x100) {
                self->posX.i.hi =
                    PLAYER.posX.i.hi + ((Random() & 0x3F) + 0x20);
            } else {
                self->posX.i.hi =
                    PLAYER.posX.i.hi - ((Random() & 0x3F) + 0x20);
            }
            absX = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
            if (absX < 0x30) {
                self->posX.i.hi = 0x30;
            }
            if (absX > 0x1D0) {
                self->posX.i.hi = 0xD0;
            }
            self->facingLeft = GetSideToPlayer() & 1;
            SetSubStep(3);
            break;
        case 3:
            if (!AnimateEntity(D_us_80180CD0, self)) {
                SetStep(3);
            }
            if (self->pose >= 5) {
                self->hitboxState = 3;
            }
            break;
        }
        break;

    case 14:
        // defeated: turns into the second form
        switch (self->step_s) {
        case 0:
            self->velocityX = 0;
            if (UnkCollisionFunc3(D_us_80180BBC) & 1) {
                self->step_s++;
            }
            break;
        case 1:
            entity = self - 16;
            for (i = 0; i < 16; i++, entity++) {
                entity->flags |= FLAG_DEAD;
            }
            SetSubStep(4);
            absX = g_Tilemap.scrollX.i.hi + self->posX.i.hi;
            if ((u16)(absX - 0x60) > 0x140) {
                self->scaleY = 0x100;
                self->drawFlags |= ENTITY_SCALEY;
                SetSubStep(2);
            }
            break;
        case 2:
            if (!AnimateEntity(D_us_80180CC4, self)) {
                self->scaleY -= 0x20;
                if (!self->scaleY) {
                    self->drawFlags = ENTITY_DEFAULT;
                    self->animCurFrame = 0;
                    if ((s16)(self->posX.i.hi + g_Tilemap.scrollX.i.hi) <
                        0x60) {
                        self->posX.i.hi += 0x60;
                    } else {
                        self->posX.i.hi -= 0x60;
                    }
                    SetSubStep(3);
                }
            }
            break;
        case 3:
            if (!AnimateEntity(D_us_80180CD0, self)) {
                SetSubStep(4);
            }
            break;
        case 4:
            D_us_80180BA8 |= 1;
            if (!AnimateEntity(D_us_80180D2C, self)) {
                self->step_s++;
            }
            break;
        case 5:
            entity = self + 1;
            DestroyEntity(entity);
            CreateEntityFromEntity(E_UNK_32, self, entity);
            self->ext.olroxHuman.timer = 0xA0;
            entity->posY.i.hi = 0x1D0 - g_Tilemap.scrollY.i.hi;
            entity->facingLeft = self->facingLeft ^ 1;
            self->step_s++;
            break;
        case 6:
            if (!--self->ext.olroxHuman.timer) {
                self->animCurFrame = 0;
                self->step = 0x40;
            }
            break;
        }
        break;

    case 32:
        // debug: browse the frames with the second pad
        FntPrint("charal %x\n", self->animCurFrame);
        if (g_pads[1].pressed & PAD_LEFT) {
            if (self->params) {
                break;
            }
            self->animCurFrame++;
            self->params |= 1;
        } else {
            self->params = 0;
        }
        if (g_pads[1].pressed & PAD_RIGHT) {
            if (self->step_s) {
                break;
            }
            self->animCurFrame--;
            self->step_s |= 1;
        } else {
            self->step_s = 0;
        }
        break;
    }

    self->hitboxWidth = 5;
    self->hitboxOffX = 1;
    self->hitboxOffY = 1;
    self->hitboxHeight = 0x1C;
    if (self->animCurFrame == 0x2D || self->animCurFrame == 0x2E) {
        self->hitboxWidth = 4;
        self->hitboxOffX = 0;
        self->hitboxOffY = 1;
        self->hitboxHeight = 0x12;
    }
    if (self->animCurFrame >= 0x30 && self->animCurFrame < 0x33) {
        self->hitboxOffY = 0xA;
        self->hitboxWidth = 4;
        self->hitboxOffX = 0;
        self->hitboxHeight = 0xD;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801AE858);
#else
// A pillar that rises from the floor, bending the picture behind it
void func_us_801AE858(Entity* self) {
    DRAWENV drawEnv;
    Primitive* prim;
    Primitive* fbPrim;
    Entity* olrox;
    s16* heights;
    s32 primIndex;
    s32 i;
    s32 j;
    u8 u;
    u8 v;
    u8 tmp;
    s16 x;
    s16 y;
    s16 yTop;
    s16 yBottom;
    s16 left;
    s16 h;
    u16 tpage;

    olrox = self->ext.olroxChild.parent;
    heights = self->ext.olroxChild.heights;
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801806F0);
        self->hitboxWidth = 0x10;
        self->scaleX = 0x100;
        self->hitboxHeight = 0;
        self->scaleY = 0;
        self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY;
        self->zPriority += 0xC;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x18);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.olroxChild.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        // the top
        for (i = 0, u = 0; i < 2; i++, u += 0x18) {
            prim->tpage = 0x13;
            prim->clut = 0x214;
            prim->u0 = prim->u2 = u;
            prim->u1 = prim->u3 = u + 0x17;
            prim->v0 = prim->v1 = 0x40;
            prim->v2 = prim->v3 = 0x5E;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            prim->priority = self->zPriority + 0xA;
            prim = prim->next;
        }
        // the body
        for (i = 0; i < 8; i++) {
            u = (i % 2) * 0x18;
            v = (i / 2) * 0x1F;
            prim->v0 = prim->v1 = v - 0x80;
            prim->tpage = 0x12;
            prim->clut = 0x214;
            prim->u0 = prim->u2 = u - 0x30;
            prim->u1 = prim->u3 = u - 0x19;
            if (i < 2) {
                prim->v0 = v - 0x7F;
                prim->v1++;
            }
            prim->v2 = prim->v3 = prim->v0 + 0x1F;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            prim->priority = self->zPriority + 0xA;
            prim = prim->next;
        }
        // the bottom, the top flipped
        for (i = 0, u = 0; i < 2; i++, u += 0x18) {
            prim->tpage = 0x13;
            prim->clut = 0x214;
            prim->u0 = prim->u2 = u;
            prim->u1 = prim->u3 = u + 0x17;
            prim->v0 = prim->v1 = 0x5E;
            prim->v2 = prim->v3 = 0x40;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            prim->priority = self->zPriority + 0xA;
            prim = prim->next;
        }
        // mirror the right column
        prim = self->ext.olroxChild.prim;
        for (i = 0; i < 6; i++) {
            prim = prim->next;
            tmp = prim->u1;
            prim->u1 = prim->u0;
            prim->u0 = tmp;
            tmp = prim->u2;
            prim->u2 = prim->u3;
            prim->u3 = tmp;
            prim = prim->next;
        }
        self->ext.olroxChild.fbPrim = prim;
        while (prim != NULL) {
            prim->drawMode = DRAW_HIDE;
            prim->priority = self->zPriority + 8;
            prim = prim->next;
        }
        self->ext.olroxChild.halfWidth = 0x18;
        self->ext.olroxChild.height = 0;
        self->ext.olroxChild.timer = D_us_80180BF8[self->params];
        break;

    case 1:
        if (!AnimateEntity(D_us_80180D48, self)) {
            SetStep(2);
            self->pose = Random() & 7;
        }
        if (self->scaleY < 0x100) {
            self->scaleY += 0x10;
        }
        break;

    case 2:
        AnimateEntity(D_us_80180D50, self);
        if (olrox->ext.olroxHuman.raisePillars) {
            if (self->ext.olroxChild.timer) {
                self->ext.olroxChild.timer--;
                return;
            }
            self->step++;
        }
        if (self->scaleY < 0x100) {
            self->scaleY += 0x10;
        }
        if (D_us_80180BA8) {
            DestroyEntity(self);
        }
        break;

    case 3:
        // the ground opens
        drawEnv = g_CurrentBuffer->draw;
        tpage = 0x100;
        if (drawEnv.ofs[0]) {
            tpage = 0x104;
        }
        x = self->posX.i.hi;
        y = self->posY.i.hi;
        if (y > 0xF0) {
            y = 0xF0;
        }
        if (self->ext.olroxChild.halfWidth) {
            self->ext.olroxChild.halfWidth--;
            self->ext.olroxChild.height += 2;
        }
        fbPrim = self->ext.olroxChild.fbPrim;
        fbPrim->tpage = tpage;
        fbPrim->u0 = fbPrim->u2 = x - 0x18;
        fbPrim->u1 = fbPrim->u3 = x;
        fbPrim->v0 = fbPrim->v1 = 0;
        fbPrim->v2 = fbPrim->v3 = y;
        fbPrim->x1 = fbPrim->x3 = x;
        fbPrim->x0 = fbPrim->x2 = x - self->ext.olroxChild.halfWidth;
        fbPrim->y2 = fbPrim->y3 = y;
        fbPrim->drawMode = DRAW_UNK02;
        fbPrim->y0 = fbPrim->y1 = -self->ext.olroxChild.height;
        fbPrim = fbPrim->next;
        fbPrim->tpage = tpage;
        fbPrim->u0 = fbPrim->u2 = x;
        fbPrim->u1 = fbPrim->u3 = x + 0x18;
        fbPrim->v0 = fbPrim->v1 = 0;
        fbPrim->v2 = fbPrim->v3 = y;
        fbPrim->x0 = fbPrim->x2 = x;
        fbPrim->x1 = fbPrim->x3 = x + self->ext.olroxChild.halfWidth;
        fbPrim->y2 = fbPrim->y3 = y;
        fbPrim->drawMode = DRAW_UNK02;
        fbPrim->y0 = fbPrim->y1 = -self->ext.olroxChild.height;
        if (!self->ext.olroxChild.halfWidth) {
            g_api.PlaySfx(0x7CA);
            heights[1] = -0x20;
            heights[2] = -0x40;
            heights[3] = -0x60;
            heights[4] = -0x80;
            heights[5] = -0xA0;
            heights[0] = 0;
            heights[6] = -0xC0;
            self->step++;
        }
        break;

    case 4:
        // the pillar shoots up
        AnimateEntity(D_us_80180D50, self);
        heights[0] += 8;
        heights[2] += 6;
        heights[1] += 7;
        heights[4] += 4;
        heights[3] += 5;
        heights[6] += 2;
        heights[5] += 3;
        for (i = 0; i < 7; i++) {
            if (heights[i] > 0x280) {
                heights[i] = 0x280;
            }
        }
        prim = self->ext.olroxChild.prim;
        y = self->posY.i.hi;
        drawEnv = g_CurrentBuffer->draw;
        tpage = 0x100;
        if (drawEnv.ofs[0]) {
            tpage = 0x104;
        }
        fbPrim = self->ext.olroxChild.fbPrim;
        for (i = 0; i < 6; i++) {
            yTop = y - heights[i];
            if (self->posY.i.hi < yTop) {
                yTop = self->posY.i.hi;
            }
            yBottom = y - heights[i + 1];
            if (self->posY.i.hi < yBottom) {
                yBottom = self->posY.i.hi;
            }
            x = self->posX.i.hi;
            left = x - 0x18;
            for (j = 0; j < 2; j++) {
                prim->y0 = prim->y1 = yTop;
                prim->y2 = prim->y3 = yBottom;
                prim->x1 = prim->x3 = x;
                prim->x0 =
                    left + (rcos(heights[i] * 0x18 + (j << 0xB)) / 1024);
                prim->x2 =
                    left + (rcos(heights[i + 1] * 0x18 + (j << 0xB)) / 1024);
                fbPrim->u0 = fbPrim->u2 = left;
                fbPrim->u1 = fbPrim->u3 = x;
                fbPrim->v0 = fbPrim->v1 = yTop;
                fbPrim->tpage = tpage;
                fbPrim->v2 = fbPrim->v3 = yBottom;
                fbPrim->x0 = prim->x0;
                fbPrim->y0 = prim->y0;
                fbPrim->x1 = prim->x1;
                fbPrim->y1 = prim->y1;
                fbPrim->x2 = prim->x2;
                fbPrim->y2 = prim->y2;
                fbPrim->x3 = prim->x3;
                fbPrim->y3 = prim->y3;
                if (yTop & 0x8000) {
                    fbPrim->v0 = fbPrim->v1 = 0;
                    fbPrim->y0 = fbPrim->y1 = 0;
                }
                if (yBottom > 0xF0) {
                    fbPrim->v2 = fbPrim->v3 = 0xF0;
                    fbPrim->y2 = fbPrim->y3 = 0xF0;
                }
                if (fbPrim->x0 > 0x100) {
                    fbPrim->x0 = fbPrim->x2 = 0x100;
                    fbPrim->u0 = fbPrim->u2 = 0xFF;
                }
                if (fbPrim->x1 > 0x100) {
                    fbPrim->x1 = fbPrim->x3 = 0x100;
                    fbPrim->u1 = fbPrim->u3 = 0xFF;
                }
                if (fbPrim->x0 < 0) {
                    fbPrim->x0 = fbPrim->x2 = 0;
                    fbPrim->u0 = fbPrim->u2 = 0;
                }
                if (fbPrim->x1 < 0) {
                    fbPrim->x1 = fbPrim->x3 = 0;
                    fbPrim->u1 = fbPrim->u3 = 0;
                }
                fbPrim->drawMode = DRAW_DEFAULT;
                if (i == 5) {
                    fbPrim->drawMode = DRAW_HIDE;
                }
                fbPrim = fbPrim->next;
                prim = prim->next;
                left += 0x30;
            }
        }
        if (heights[1] > 0) {
            if (heights[5] > 0) {
                h = heights[1] - heights[5];
                if (heights[6] > 0) {
                    h += (heights[5] - heights[6]) / 2;
                }
            } else {
                h = heights[1];
            }
            h /= 2;
            if (h > 0x100) {
                h = 0xFF;
            }
            self->hitboxHeight = h;
            self->hitboxOffY = self->hitboxHeight - heights[1];
        }
        if (heights[6] > 0 && self->scaleX) {
            self->scaleX -= 2;
        }
        if (heights[6] > 0x200) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801AF31C);
#else
// A beam that stretches as it flies
void func_us_801AF31C(Entity* self) {
    Primitive* prim;
    s32 primIndex;
    s16 length;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801806FC);
        if (self->facingLeft) {
            self->posX.i.hi += 0x1C;
        } else {
            self->posX.i.hi -= 0x1C;
        }
        self->posY.i.hi -= 0xC;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        self->ext.olroxChild.prim = prim;
        self->flags |= FLAG_HAS_PRIMS;
        prim->tpage = 0x12;
        prim->clut = 0x212;
        prim->u0 = prim->u2 = 0xA0;
        prim->u1 = prim->u3 = 0xD0;
        prim->v0 = prim->v1 = 0;
        prim->v2 = prim->v3 = 0x10;
        prim->x0 = prim->x1 = prim->x2 = prim->x3 = self->posX.i.hi;
        prim->y0 = prim->y1 = prim->y2 = prim->y3 = self->posY.i.hi;
        prim->r0 = prim->g0 = prim->b0 = 0;
        prim->r1 = prim->g1 = prim->b1 = 0xA0;
        LOW(prim->r2) = LOW(prim->r0);
        LOW(prim->r3) = LOW(prim->r1);
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 |
                         DRAW_TRANSP;
        prim->priority = self->zPriority + 4;
        self->velocityY = 0;
        if (self->facingLeft) {
            self->velocityX = FIX(-0.5);
            self->ext.olroxChild.unkA8.beamLength.i.hi = -0x30;
        } else {
            self->velocityX = FIX(0.5);
            self->ext.olroxChild.unkA8.beamLength.i.hi = 0x30;
        }
        self->hitboxWidth = 0;
        self->hitboxHeight = 5;
        PlaySfxPositional(0x7D1);
        break;

    case 1:
        MoveEntity();
        if (self->facingLeft) {
            self->velocityX -= FIX(1.0 / 8);
            self->ext.olroxChild.unkA8.beamLength.val -= FIX(5);
        } else {
            self->velocityX += FIX(1.0 / 8);
            self->ext.olroxChild.unkA8.beamLength.val += FIX(5);
        }
        prim = self->ext.olroxChild.prim;
        length = self->ext.olroxChild.unkA8.beamLength.i.hi;
        prim->x1 = prim->x3 = self->posX.i.hi + length;
        prim->x0 = prim->x2 = self->posX.i.hi;
        prim->y0 = prim->y1 = self->posY.i.hi - 8;
        prim->y2 = prim->y3 = self->posY.i.hi + 8;
        self->hitboxWidth = abs(length) / 4;
        self->hitboxOffX = abs(length) - self->hitboxWidth - 4;
        break;
    }
    if (abs(self->posX.i.hi) > 0x280) {
        DestroyEntity(self);
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801AF604);
#else
// A skull from the portal, homing on the player (params 1: its ghost)
void func_us_801AF604(Entity* self) {
    Entity* entity;
    s16 angle;

    if (D_us_80180BA8) {
        self->flags |= FLAG_DEAD;
    }
    if (self->flags & FLAG_DEAD) {
        PlaySfxPositional(0x655);
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_LASER_EXPLOSION, self, entity);
            entity->params = 2;
        }
        DestroyEntity(self);
        return;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801806E4);
        self->hitboxState = 0;
        self->hitboxOffY = -1;
        self->drawFlags |= ENTITY_OPACITY;
        self->ext.olroxChild.unkA8.skull.angle =
            ((Random() & 0x3F) << 4) + 0x200;
        if (self->params) {
            self->step = 2;
            self->hitboxState = 0;
            self->zPriority--;
            self->blendMode |= BLEND_ADD | BLEND_TRANSP;
            return;
        }
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_UNK_2F, self, entity);
            entity->params = 1;
            entity->ext.olroxChild.parent = self;
        }
        break;

    case 1:
        if (self->opacity < 0x80) {
            self->hitboxState = 0;
            self->opacity += 4;
        } else {
            self->hitboxState = 3;
        }
        AnimateEntity(D_us_80180D74, self);
        MoveEntity();
        if (self->velocityX > 0) {
            self->facingLeft = 1;
        } else {
            self->facingLeft = 0;
        }
        angle = func_us_801AD26C(
            8, self->ext.olroxChild.unkA8.skull.angle,
            ratan2(-(s16)(PLAYER.posY.i.hi - self->posY.i.hi - 0x20),
                   (s16)(PLAYER.posX.i.hi - self->posX.i.hi)));
        self->velocityX = rcos(angle) * 16;
        self->velocityY = -(rsin(angle) * 16);
        self->ext.olroxChild.unkA8.skull.angle = angle;
        break;

    case 2:
        AnimateEntity(D_us_80180D68, self);
        entity = self->ext.olroxChild.parent;
        self->posX.i.hi = entity->posX.i.hi;
        self->posY.i.hi = entity->posY.i.hi;
        self->opacity = entity->opacity;
        if (!entity->entityId) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801AF8C0);
#else
// The portal skulls come out of
void func_us_801AF8C0(Entity* self) {
    Entity* entity;
    Entity* olrox;

    olrox = self->ext.olroxChild.parent;
    if (olrox->flags & FLAG_DEAD) {
        self->step = 3;
        self->flags |= FLAG_DEAD;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitOlroxAfterImage);
        self->animCurFrame = 0x54;
        self->scaleX = self->scaleY = 0;
        self->hitboxState = 0;
        self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY;
        self->zPriority--;
        break;

    case 1:
        AnimateEntity(D_us_80180D7C, self);
        if (self->scaleX < 0x100) {
            self->scaleX = self->scaleY = self->scaleY + 8;
            break;
        }
        self->scaleX = self->scaleY = 0x100;
        self->step++;
        break;

    case 2:
        AnimateEntity(D_us_80180D7C, self);
        if (!(g_Timer % 12)) {
            entity = AllocEntity(&g_Entities[160], &g_Entities[168]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_2F, self, entity);
                entity->ext.olroxChild.parent = self;
                PlaySfxPositional(0x691);
            }
        }
        if (g_Timer % 5) {
            self->palette = PAL_FLAG(0x213);
        } else {
            self->palette = PAL_FLAG(0x215);
        }
        if (olrox->ext.olroxHuman.closePortal) {
            self->step++;
        }
        break;

    case 3:
        AnimateEntity(D_us_80180D7C, self);
        self->scaleX = self->scaleY = self->scaleY - 8;
        if (self->scaleY < 0) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801AFAF4);
#else
// A bat that spirals into Olrox, circles him, then dives at the player
void func_us_801AFAF4(Entity* self) {
    Entity* entity;
    Entity* olrox;
    s16 x;
    s16 y;
    s16 prevX;
    s32 dx;
    s32 dy;
    s32 dist;
    s16 angle;

    olrox = self->ext.olroxChild.parent;
    if (olrox->flags & FLAG_DEAD) {
        self->flags |= FLAG_DEAD;
    }
    if (self->flags & FLAG_DEAD) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, entity);
            entity->params = 2;
        }
        DestroyEntity(self);
        PlaySfxPositional(0x64E);
        return;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801806D8);
        self->hitboxState = 0;
        self->hitboxOffX = 1;
        self->hitboxOffY = 1;
        self->palette = 0x219;
        self->zPriority = olrox->zPriority + 2;
        self->pose = (Random() & 3) * 2;
        self->ext.olroxChild.timer = (self->params + 1) * 14;
        self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY;
        self->params = Random() & 3;
        break;

    case 1:
        AnimateEntity(D_us_80180D38, self);
        prevX = self->posX.i.hi;
        x = olrox->posX.i.hi;
        y = olrox->posY.i.hi - 0x20;
        dy = self->posY.i.hi - y;
        dx = prevX - olrox->posX.i.hi;
        angle = ratan2(-dy, dx);
        if (olrox->facingLeft) {
            angle += 0x10;
        } else {
            angle -= 0x10;
        }
        dist = SquareRoot0(dx * dx + dy * dy) - 2;
        self->posX.i.hi = x + dist * rcos(angle) / 4096;
        self->posY.i.hi = y - dist * rsin(angle) / 4096;
        self->scaleX = dist * self->params + 0x100;
        if (self->scaleX > 0x300) {
            self->scaleX = 0x300;
        }
        self->scaleY = self->scaleX;
        if (self->posX.i.hi - prevX > 0) {
            self->facingLeft = 0;
        } else {
            self->facingLeft = 1;
        }
        if (dist < 8) {
            self->ext.olroxChild.unkA8.batAngle = Random();
            self->palette = 0x209;
            self->hitboxState = 3;
            self->drawFlags = ENTITY_DEFAULT;
            self->step++;
        }
        break;

    case 2:
        AnimateEntity(D_us_80180D38, self);
        MoveEntity();
        if (self->velocityX > 0) {
            self->facingLeft = 1;
            self->zPriority = olrox->zPriority + 2;
        } else {
            self->facingLeft = 0;
            self->zPriority = olrox->zPriority - 2;
        }
        self->ext.olroxChild.unkA8.batAngle = AdjustValueWithinThreshold(
            2, self->ext.olroxChild.unkA8.batAngle,
            Ratan2Shifted(olrox->posX.i.hi - self->posX.i.hi,
                          olrox->posY.i.hi - self->posY.i.hi));
        SetEntityVelocityFromAngle(self->ext.olroxChild.unkA8.batAngle, 0x10);
        if (olrox->ext.olroxHuman.releaseBats) {
            if (!--self->ext.olroxChild.timer) {
                SetEntityVelocityFromAngle(
                    GetAngleBetweenEntitiesShifted(self, &PLAYER), 0x20);
                self->step++;
            }
        }
        break;

    case 3:
        AnimateEntity(D_us_80180D38, self);
        MoveEntity();
        if (self->velocityX > 0) {
            self->facingLeft = 0;
        } else {
            self->facingLeft = 1;
        }
        if (abs(self->posX.i.hi) > 0x200 || abs(self->posY.i.hi) > 0x200) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", EntityOlroxAfterImage);
#else
void EntityOlroxAfterImage(Entity* self) {
    if (!self->step) {
        InitializeEntity(g_EInitOlroxAfterImage);
        self->palette = 0x217;
        self->drawFlags = ENTITY_OPACITY;
        self->opacity = 0x80;
        self->hitboxState = 0;
        self->blendMode = BLEND_ADD | BLEND_TRANSP;
        self->animCurFrame = self->params;
        self->zPriority -= 2;
    }
    self->opacity -= 2;
    if (!self->opacity) {
        DestroyEntity(self);
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo0/nonmatchings/2D26C", func_us_801B001C);
#else
static void func_us_801B001C_Explode(Entity* self) {
    self->entityId = E_EXPLOSION;
    self->pfnUpdate = EntityExplosion;
    self->params = 0;
    self->step = 0;
}

// Furniture and the pieces it breaks into
void func_us_801B001C(Entity* self) {
    Entity* entity;
    s16* hitbox;
    s32 i;
    s16 offset;
    u8 kind;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = ANIMSET_OVL(6);
        self->unk5A = 0x48;
        self->palette = 0x209;
        self->hitboxState = 2;
        if (self->params & 0x100) {
            self->facingLeft = 1;
        }
        self->zPriority -= 4;
        if (self->params == 1) {
            D_us_80180BA8 = 0;
        }
        kind = self->params;
        self->animCurFrame = kind;
        if (kind < 5) {
            hitbox = D_us_80180D94[kind - 1];
            self->hitboxWidth = hitbox[0];
            self->hitboxHeight = hitbox[1];
            self->hitboxOffX = hitbox[2];
            self->hitboxOffY = hitbox[3];
            self->hitPoints = hitbox[4];
            self->zPriority = hitbox[5];
        } else {
            self->step += kind;
        }
        break;

    case 1:
        if (self->flags & FLAG_DEAD) {
            self->hitboxState = 0;
            self->step += (u8)self->params;
        }
        break;

    case 2:
        self->animCurFrame = 0xA;
        self->velocityX = FIX(1);
        if (!(GetSideToPlayer() & 1)) {
            self->velocityX = FIX(-1);
        }
        self->step = 11;
        break;

    case 3:
        entity = NULL;
        for (i = 0, offset = 0; i < 4; i++, offset -= 30) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_2A, self, entity);
                entity->params = i + 5;
                entity->posX.i.hi += 0x30 + offset;
            }
        }
        if (entity != NULL) {
            entity->params = 0x105;
        }
        for (i = 0, offset = 0; i < 4; i++, offset -= 30) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_EXPLOSION, self, entity);
                entity->params = 0x13;
                entity->posX.i.hi += 0x30 + offset;
            }
        }
        entity = self + 1;
        for (i = 0; i < 3; i++, entity++) {
            entity->flags |= FLAG_DEAD;
        }
        self->animCurFrame = 0;
        self->step = 0x20;
        break;

    case 4:
        for (i = 0; i < 2; i++) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_2A, self, entity);
                entity->params = i + 8;
                entity->posY.i.hi += i * 8;
                if (i) {
                    entity->velocityX = FIX(0.5);
                    if (!(GetSideToPlayer() & 1)) {
                        entity->velocityX = FIX(-0.5);
                    }
                    entity->velocityY = FIX(-1);
                }
            }
        }
        PlaySfxPositional(0x691);
        self->animCurFrame = 0;
        self->step = 0x20;
        break;

    case 5:
        for (i = 0, offset = 0; i < 3; i++, offset += 0x1C) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_2A, self, entity);
                entity->params = i + 0xB;
                entity->posY.i.hi += offset - 0x1C;
            }
        }
        PlaySfxPositional(0x67F);
        self->animCurFrame = 0;
        self->step = 0x20;
        break;

    case 7:
    case 8:
        self->zPriority = 0x6A;
        MoveEntity();
        self->velocityY += FIX(1.0 / 16);
        if (func_us_801AD2F0(self->posX.i.hi, self->posY.i.hi + 8)) {
            self->step = 0x20;
        }
        break;

    case 9:
        self->zPriority = 0x6C;
        self->drawFlags |= ENTITY_ROTATE;
        self->rotate -= 0x10;
        MoveEntity();
        self->velocityY += FIX(3.0 / 32);
        if (func_us_801AD2F0(self->posX.i.hi, self->posY.i.hi + 4)) {
            func_us_801B001C_Explode(self);
        }
        break;

    case 10:
        self->zPriority = 0x6C;
        self->drawFlags |= ENTITY_ROTATE;
        self->rotate += 0x80;
        MoveEntity();
        self->velocityY += FIX(1.0 / 8);
        if (func_us_801AD2F0(self->posX.i.hi, self->posY.i.hi)) {
            func_us_801B001C_Explode(self);
        }
        break;

    case 11:
        self->zPriority = 0x6A;
        MoveEntity();
        self->velocityY += FIX(1.0 / 16);
        if (func_us_801AD2F0(self->posX.i.hi, self->posY.i.hi + 0xA)) {
            self->step = 0x20;
        }
        break;

    case 12:
        self->rotate -= 0x30;
        // fallthrough
    case 13:
        self->zPriority = 0x6B;
        self->drawFlags |= ENTITY_ROTATE;
        self->rotate += 0x28;
        MoveEntity();
        self->velocityY += FIX(3.0 / 32);
        if (func_us_801AD2F0(self->posX.i.hi, self->posY.i.hi + 6)) {
            func_us_801B001C_Explode(self);
        }
        break;

    case 6:
    case 14:
        self->zPriority = 0x6A;
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
EInit D_us_801806D8 = { 0x8006, 0x3A, 0x48, 0x209, 0x33 };
EInit D_us_801806E4 = { 0x8006, 0x50, 0x48, 0x209, 0x34 };
EInit D_us_801806F0 = { 0x8006, 0x3F, 0x48, 0x209, 0x35 };
EInit D_us_801806FC = { 0x8000, 0, 0, 0, 0x36 };
s32 D_us_80180BA8 = 0;
u8 D_us_80180BAC[16] = { 8, 9, 8, 8, 0xA, 8, 0xB, 8, 0xA, 9, 0xA, 0xB, 0xA, 0xA, 0xA, 0xB };
s16 D_us_80180BBC[8] = { 0, 0x1F, 0, 4, 8, -4, -0x10, 0 };
s16 D_us_80180BCC[4] = { 0, 0x1F, 8, 0 };
s16 D_us_80180BD4[9][2] = {
    { -0x20, 0x80 },
    { -0x20, 0x40 },
    { -0x20, 0 },
    { 0x40, -0x20 },
    { 0x80, -0x20 },
    { 0xC0, -0x20 },
    { 0x120, 0 },
    { 0x120, 0x40 },
    { 0x120, 0x80 },
};
s16 D_us_80180BF8[4] = { 0, 0x10, 0x20, 0x30 };
u8 D_us_80180C00[0xC] = { 8, 0xE, 8, 0xF, 0x48, 0x10, 8, 0xF, 0x10, 0xE, 0xFF, 0 };
u8 D_us_80180C0C[0x18] = {
    0x10,
    0xE,
    8,
    0x11,
    8,
    0x12,
    0xC,
    0x13,
    0x10,
    0x14,
    8,
    0x15,
    8,
    0x16,
    0x14,
    0x17,
    0x14,
    0x18,
    8,
    0x19,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180C24[0x1C] = {
    8,
    0x19,
    6,
    0x1A,
    0x18,
    0x1B,
    0xC,
    0x1C,
    1,
    0x1D,
    6,
    0x1E,
    6,
    0x1F,
    0x10,
    0x20,
    6,
    0x21,
    0x10,
    0x1B,
    6,
    0x1A,
    4,
    0x19,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180C40[0xC] = { 4, 0x19, 8, 0x21, 6, 0x30, 6, 0x31, 4, 0x32, 0xFF, 0 };
u8 D_us_80180C4C[0xC] = { 4, 0x32, 6, 0x31, 6, 0x30, 8, 0x21, 4, 0x19, 0xFF, 0 };
u8 D_us_80180C58[0x14] = {
    4,
    0x19,
    4,
    0x22,
    4,
    0x23,
    4,
    0x24,
    4,
    0x27,
    4,
    0x28,
    4,
    0x2A,
    4,
    0x2B,
    4,
    0x2C,
    0xFF,
    0,
};
u8 D_us_80180C6C[0x14] = {
    8,
    0x19,
    8,
    0x22,
    0x10,
    0x23,
    6,
    0x24,
    8,
    0x25,
    0x18,
    0x26,
    6,
    0x27,
    6,
    0x22,
    6,
    0x19,
    0xFF,
    0,
};
u8 D_us_80180C80[0x2C] = {
    8,
    0x19,
    8,
    0x22,
    0x18,
    0x27,
    1,
    0x28,
    1,
    0x27,
    1,
    0x28,
    1,
    0x27,
    1,
    0x28,
    1,
    0x27,
    1,
    0x28,
    0xC,
    0x27,
    8,
    0x29,
    1,
    0x27,
    1,
    0x2A,
    1,
    0x2B,
    8,
    0x2C,
    0x28,
    0x2B,
    8,
    0x2A,
    8,
    0x27,
    8,
    0x22,
    4,
    0x19,
    0xFF,
    0,
};
u8 D_us_80180CAC[0x10] = { 6, 0x19, 1, 0x27, 1, 0x2A, 1, 0x60, 1, 0x61, 8, 0x60, 0xFF, 0, 0, 0 };
u8 D_us_80180CBC[8] = { 6, 0x2A, 6, 0x27, 0x10, 0x19, 0xFF, 0 };
u8 D_us_80180CC4[0xC] = { 0x10, 0x19, 4, 0x36, 1, 0x37, 2, 0x38, 0xFF, 0, 0, 0 };
u8 D_us_80180CD0[0xC] = { 4, 0x39, 2, 0x38, 1, 0x37, 4, 0x36, 0x10, 0x19, 0xFF, 0 };
u8 D_us_80180CDC[0x10] = { 2, 0x19, 6, 0x21, 4, 0x30, 8, 0x31, 2, 0x30, 2, 0x2F, 2, 0x2E, 0xFF, 0 };
u8 D_us_80180CEC[0x14] = {
    2,
    0x2D,
    4,
    0x2E,
    8,
    0x2F,
    8,
    0x30,
    0x10,
    0x31,
    3,
    0x30,
    4,
    0x21,
    4,
    0x19,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_80180D00[0x10] = { 2, 0x19, 0xA, 0x21, 0xA, 0x30, 0x10, 0x31, 3, 0x30, 6, 0x21, 0xFF, 0, 0, 0 };
u8 D_us_80180D10[0x10] = { 4, 0x21, 3, 0x30, 0x10, 0x31, 0xA, 0x30, 0xA, 0x21, 4, 0x19, 0xFF, 0, 0, 0 };
u8 D_us_80180D20[0xC] = { 1, 0x19, 8, 0x33, 0xC, 0x34, 8, 0x35, 2, 0x19, 0xFF, 0 };
u8 D_us_80180D2C[0xC] = { 4, 0x19, 0xA, 0x21, 8, 0x30, 8, 0x31, 0x20, 0x32, 0xFF, 0 };
u8 D_us_80180D38[0x10] = { 4, 0x3A, 4, 0x3B, 4, 0x3C, 4, 0x3D, 4, 0x3E, 4, 0x3C, 0, 0, 0, 0 };
u8 D_us_80180D48[8] = { 6, 0x3F, 6, 0x40, 6, 0x41, 0xFF, 0 };
u8 D_us_80180D50[0x18] = {
    6,
    0x42,
    6,
    0x43,
    6,
    0x44,
    6,
    0x45,
    6,
    0x46,
    6,
    0x47,
    6,
    0x48,
    6,
    0x49,
    6,
    0x4A,
    6,
    0x4B,
    0,
    0,
    0,
    0,
};
u8 D_us_80180D68[0xC] = { 0xA, 0x4C, 0xA, 0x4D, 0xA, 0x4E, 0xA, 0x4F, 0, 0, 0, 0 };
u8 D_us_80180D74[8] = { 0x16, 0x50, 0x36, 0x51, 0, 0, 0, 0 };
u8 D_us_80180D7C[0x18] = {
    3,
    0x54,
    3,
    0x55,
    3,
    0x56,
    3,
    0x57,
    3,
    0x58,
    3,
    0x59,
    3,
    0x5A,
    3,
    0x5B,
    3,
    0x5C,
    3,
    0x5D,
    3,
    0x5E,
    0,
    0,
};
s16 D_us_80180D94[4][6] = {
    { 8, 0x10, 0, 0, 4, 0x6A },
    { 0x40, 6, 0, -8, 0x10, 0x6A },
    { 4, 4, 0, 0, 4, 0x6C },
    { 4, 0x20, 0, 0, 4, 0x6A },
};
// end pc_import_data.py
// clang-format on
#endif
