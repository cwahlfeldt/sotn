// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo5.h"

extern EInit g_EInitEnvironment;
extern s32 D_us_801806DC;

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_2159C", EntityRbo3Door);
#else
// The arena doors. params 0/1 are the doors (each spawns its lower half
// with params 2), which close when the fight starts and open after it
void EntityRbo3Door(Entity* self) {
    Entity* lower;
    s32 tile;
    s32 i;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitEnvironment);
        self->zPriority = 0x5C;
        if (self->params & 2) {
            self->animCurFrame = 0xE;
            return;
        }
        if (self->params & 1) {
            self->animCurFrame = 0xD;
            self->posX.i.hi = 0x1D0 - g_Tilemap.scrollX.i.hi;
        } else {
            self->animCurFrame = 0xC;
            self->posX.i.hi = 0x10 - g_Tilemap.scrollX.i.hi;
        }
        self->posY.i.hi = 0x20 - g_Tilemap.scrollY.i.hi;
        lower = self + 1;
        CreateEntityFromEntity(E_RBO3DOOR, self, lower);
        lower->params = 2;
        lower->posY.i.hi = 0xA0;
        if (self->params) {
            lower->posX.i.hi += 0x10;
        }
        return;
    case 1:
        if (D_us_801806DC) {
            if (self->params & 2) {
                self->step = 8;
                return;
            }
            tile = self->params ? 0xDE : 0xC1;
            for (i = 3; i >= 0; i--, tile += 0x20) {
                g_Tilemap.fg[tile] = 0x4B3;
            }
            self->step = 2;
        }
        break;
    case 2:
        self->posY.val += FIX(1.125);
        if (self->posY.val > FIX(70)) {
            self->posY.val = FIX(70);
            self->step++;
        }
        break;
    case 3:
        if (!D_us_801806DC) {
            tile = self->params ? 0xDE : 0xC1;
            for (i = 3; i >= 0; i--, tile += 0x20) {
                g_Tilemap.fg[tile] = 0;
            }
            self->step++;
        }
        break;
    case 4:
        self->posY.val -= FIX(0.75);
        if (self->posY.i.hi + g_Tilemap.scrollY.i.hi < 0x20) {
            self->posY.i.hi = 0x20 - g_Tilemap.scrollY.i.hi;
            self->step++;
        }
        break;
    case 8:
        self->posY.val -= FIX(1.125);
        if (self->posY.val < FIX(129)) {
            self->posY.val = FIX(129);
            self->step++;
        }
        break;
    case 9:
        if (!D_us_801806DC) {
            self->step++;
        }
        break;
    case 10:
        self->posY.val += FIX(0.75);
        if (self->posY.i.hi + g_Tilemap.scrollY.i.hi > 0xA0) {
            self->posY.i.hi = 0xA0 - g_Tilemap.scrollY.i.hi;
            self->step++;
        }
        break;
    }
}
#endif

#include "../../st/e_background_sky_land.h"

void func_us_801A19CC(s32 arg0) {
    g_CurrentEntity->poseTimer = 0;
    g_CurrentEntity->pose = 0;
    g_CurrentEntity->ext.et_801A19CC.unk7D = 0;
    g_CurrentEntity->step = arg0 & 0xFF;
    g_CurrentEntity->step_s = 0;
}

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_2159C", func_us_801A19FC);
#else
void MakeEntityFromId(u16 entityId, Entity* src, Entity* dst);

// Sheds count feathers: copies of the Hippogryph entity in steps 0x30/0x32
void func_us_801A19FC(s32 count, s32 offsetY) {
    Entity* entity;
    s32 i;

    for (i = 0; i < (count & 0xFF); i++) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity == NULL) {
            continue;
        }
        DestroyEntity(entity);
        MakeEntityFromId(E_UNK_22, g_CurrentEntity, entity);
        entity->flags = FLAG_DESTROY_IF_OUT_OF_CAMERA |
                        FLAG_DESTROY_IF_BARELY_OUT_OF_CAMERA |
                        FLAG_POS_CAMERA_LOCKED | FLAG_UNK_00200000 |
                        FLAG_UNK_2000;
        entity->posX.i.hi = entity->posX.i.hi - 0x20 + (Random() & 0x3F);
        entity->velocityY = FIX(-0.25);
        entity->posY.i.hi += (Random() & 0x3F) + offsetY - 0x20;
        if (Random() & 1) {
            entity->zPriority++;
        }
        entity->step = 0x30;
        entity->params = Random() & 1;
        if (entity->params) {
            entity->step = 0x32;
            if (Random() & 1) {
                entity->ext.et_801A19CC.accelX = 0xD00;
                entity->velocityX = -0xD000;
            } else {
                entity->ext.et_801A19CC.accelX = -0xD00;
                entity->velocityX = 0xD000;
            }
        }
        if (g_CurrentEntity->step == 0x16) {
            if (g_CurrentEntity->facingLeft) {
                g_CurrentEntity->velocityX += FIX(0.25);
            } else {
                g_CurrentEntity->velocityX -= FIX(0.25);
            }
        }
    }
}
#endif

bool CheckHippogryphFloorCollision(s16* sensors) {
    if (UnkCollisionFunc3(sensors) & 1) {
        return true;
    } else {
        if (g_CurrentEntity->posY.i.hi + g_Tilemap.scrollY.i.hi >= 184) {
            g_CurrentEntity->posY.i.hi = 183 - g_Tilemap.scrollY.i.hi;
            g_CurrentEntity->velocityX = 0;
            g_CurrentEntity->velocityY = 0;
            return true;
        }
        return false;
    }
}

extern u8 D_us_80180804[];
extern u8 D_us_80180824[];
extern u8 D_us_80180830[];
extern u8 D_us_80180844[];
extern u8 D_us_80180858[];
extern u8 D_us_80180884[];
extern u8 D_us_8018088C[];
extern u8 D_us_80180894[];
extern u8 D_us_8018089C[];
extern u8 D_us_801808AC[];
extern u8 D_us_801808BC[];
extern u8 D_us_801808D4[];
extern u8 D_us_801808FC[];
extern u8 D_us_8018091C[];
extern u8 D_us_80180928[];
extern u8 D_us_80180938[];
extern u8 D_us_80180944[];
extern u8 D_us_80180950[];
extern u8 D_us_8018096C[];
extern u8 D_us_8018098C[];
extern u8 D_us_801809D8[];
extern u8 D_us_80180A2C[];
extern u8 D_us_80180A48[];
extern s16 D_us_80180AAC[];
extern s16 D_us_80180ACC[];
extern s16 D_us_80180ADC[];
extern s16 D_us_80180AE4[];
extern u8 D_us_80180AEC[];
extern s32 D_us_801806E0;
extern s32 D_us_801806E4[];
extern s32 D_us_801807E4[];
extern s32 D_us_80180B74;
extern EInit g_EInitHippogryph;
void func_us_801A3B88(Entity* self);

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_2159C", func_us_801A1C14);
#else
u8 CheckColliderOffsets(s16* arg0, u8 facing);
void func_us_801A19FC(s32 count, s32 offsetY);

// fixed point values kept in two s16 prim fields, integer part first
#define PRIM_FIX(hi, lo) (((s32)(hi) << 16) | (u16)(lo))

// The Hippogryph. Steps 0x30 and 0x32 are its feathers (func_us_801A19FC)
void func_us_801A1C14(Entity* self) {
    Primitive* prim;
    Entity* entity;
    Entity* start;
    s32 primIndex;
    s32 x;
    s32 dx;
    s32 dy;
    s32 i;
    s32 n;
    s32 fix;
    s32 len;
    s32 speed;
    s32 angle;
    s32 radius;
    s32 velA;
    s32 velB;
    u8 frame;

    if (self->hitParams && (self->step & 1)) {
        if (self->facingLeft) {
            self->velocityX = FIX(-1.5);
        } else {
            self->velocityX = FIX(1.5);
        }
        self->velocityY = FIX(-4);
        func_us_801A19CC(0x20);
    }
    if ((self->flags & FLAG_DEAD) && self->step < 0x22) {
        entity = self->ext.et_801A19CC.unk94;
        self->hitboxState = 0;
        // the original clears only the integer halves
        self->velocityX &= 0xFFFF;
        self->velocityY &= 0xFFFF;
        self->ext.et_801A19CC.timer = 0;
        self->animCurFrame = 0x6E;
        if (entity != NULL) {
            DestroyEntity(entity);
        }
        if (self->flags & FLAG_HAS_PRIMS) {
            g_api.FreePrimitives(self->primIndex);
            self->flags &= ~FLAG_HAS_PRIMS;
        }
        func_us_801A19CC(0x24);
    }

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitHippogryph);
        self->ext.et_801A19CC.zPriority = self->zPriority;
        self->zPriority = PLAYER.zPriority + 1;
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        self->ext.et_801A19CC.unk7D = 1;
        break;

    case 1:
        self->hitboxState = 0;
        AnimateEntity(D_us_8018091C, self);
        if (self->ext.et_801A19CC.unk7D && GetDistanceToPlayerX() < 0x80) {
            func_us_801A19CC(3);
            if (!D_us_801806DC) {
                g_api.TimeAttackController(
                    TIMEATTACK_EVENT_HYPPOGRYPH_DEFEAT,
                    TIMEATTACK_SET_VISITED);
            }
            self->hitboxState = 3;
            self->nextPart = self + 1;
            D_us_801806DC |= 1;
            CreateEntityFromEntity(E_UNK_23, self, self + 1);
        }
        break;

    case 3:
        if (!AnimateEntity(D_us_80180804, self)) {
            func_us_801A19CC(4);
        }
        break;

    case 4:
        // take off
        n = false;
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_801808BC, self)) {
                self->animCurFrame = 0x31;
                self->velocityY = FIX(-5);
                self->ext.et_801A19CC.timer = 4;
                self->ext.et_801A19CC.unk7D = 0;
                self->step_s++;
            }
            if (self->animCurFrame == 0x29 && !self->ext.et_801A19CC.unk7D) {
                func_us_801A19FC(4, 0);
                self->ext.et_801A19CC.unk7D = 1;
                if (self->facingLeft) {
                    self->velocityX = FIX(0.375);
                } else {
                    self->velocityX = FIX(-0.375);
                }
                self->posY.i.hi -= 4;
            }
            if (self->ext.et_801A19CC.unk7D) {
                MoveEntity();
                if (self->animCurFrame == 0x2A || self->animCurFrame == 0x2B) {
                    self->velocityY -= FIX(0.375);
                }
            }
            break;
        case 1:
            MoveEntity();
            self->velocityY += 0x1E00;
            if (!--self->ext.et_801A19CC.timer) {
                self->pose = 0;
                self->ext.et_801A19CC.timer = 0;
                self->step_s++;
            }
            break;
        case 2:
            MoveEntity();
            self->velocityY += FIX(0.125);
            if (!AnimateEntity(D_us_801808D4, self)) {
                func_us_801A19CC(6);
            }
            n = true; // skips the height check below
            break;
        }
        if (!n && self->posY.i.hi + g_Tilemap.scrollY.i.hi < 0x72) {
            self->velocityY = FIX(-4.5);
            self->animCurFrame = 0x31;
            self->ext.et_801A19CC.unk7D = 0;
            self->step_s = 2;
        }
        break;

    case 6:
        // fly across the room
        if (!self->step_s) {
            self->ext.et_801A19CC.timer = 0x50;
            if (self->facingLeft) {
                self->velocityX = FIX(1.75);
            } else {
                self->velocityX = FIX(-1.75);
            }
            self->velocityY = 0;
            self->step_s++;
        }
        MoveEntity();
        AnimateEntity(D_us_80180830, self);
        if (self->pose == 1) {
            PlaySfxPositional(SFX_HIPPOGRYPH_WING_FLAP);
        }
        if (self->animCurFrame == 6 || self->animCurFrame == 7) {
            self->velocityY -= 0x2800;
        } else {
            self->velocityY += 0xE00;
        }
        if (!--self->ext.et_801A19CC.timer) {
            // x is the distance travelled from the wall it flew from
            x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
            if (self->velocityX < 0) {
                x = 0x200 - x;
            }
            if (GetDistanceToPlayerX() > 0x50 && x < 0x90) {
                self->velocityX = 0;
                self->velocityY = 0;
                func_us_801A19CC(0x16);
            } else if (x < 0x180) {
                func_us_801A19CC(8);
            } else {
                self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
                func_us_801A19CC(6);
            }
        }
        break;

    case 8:
        // land
        switch (self->step_s) {
        case 0:
            if (self->facingLeft) {
                self->velocityX = FIX(0.5);
            } else {
                self->velocityX = FIX(-0.5);
            }
            self->velocityY = FIX(3.5);
            self->step_s++;
            // fallthrough
        case 1:
            if (!AnimateEntity(D_us_80180938, self)) {
                self->pose = 0;
                self->poseTimer = 0;
                self->step_s++;
            }
            MoveEntity();
            if (self->posY.i.hi + g_Tilemap.scrollY.i.hi > 0x88) {
                self->pose = 0;
                self->poseTimer = 0;
                self->step_s = 2;
            }
            break;
        case 2:
            AnimateEntity(D_us_80180944, self);
            MoveEntity();
            if (CheckColliderOffsets(D_us_80180ADC, 0)) {
                self->pose = 0;
                self->poseTimer = 0;
                self->ext.et_801A19CC.unk7D = 0;
                self->step_s++;
            }
            break;
        case 3:
            if ((CheckHippogryphFloorCollision(D_us_80180AAC) & 1) &&
                !self->ext.et_801A19CC.unk7D) {
                self->ext.et_801A19CC.unk7D = 1;
            }
            self->velocityY -= 0x7C00;
            if (self->velocityY <= FIX(0.5)) {
                self->velocityY = FIX(0.5);
            }
            if (!AnimateEntity(D_us_80180950, self) &&
                self->ext.et_801A19CC.unk7D) {
                func_us_801A19CC(0xA);
                self->ext.et_801A19CC.unk7D = 0;
            }
            break;
        }
        break;

    case 0xA:
        // choose the next attack, further from the player picks later entries
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        n = Random() & 7;
        dx = GetDistanceToPlayerX();
        if (dx > 0x50) {
            n += 4;
        }
        if (dx > 0x80) {
            n += 4;
        }
        func_us_801A19CC(D_us_80180AEC[n & 0xFF]);
        break;

    case 0xC:
        // breathe fire
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_80180858, self)) {
                self->ext.et_801A19CC.timer = 0x40;
                self->step_s++;
                PlaySfxPositional(SFX_HIPPOGRYPH_FIRE_ATTACK);
            }
            if (self->pose == 4) {
                PlaySfxPositional(SFX_ELECTRICITY);
            }
            break;
        case 1:
            if (self->ext.et_801A19CC.timer & 1) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_UNK_27, self, entity);
                    entity->pfnUpdate = func_us_801A3B88;
                    entity->facingLeft = self->facingLeft;
                    if (self->facingLeft) {
                        entity->posX.i.hi += 0x1D;
                    } else {
                        entity->posX.i.hi -= 0x1D;
                    }
                    entity->posY.i.hi -= 2;
                    entity->params = self->ext.et_801A19CC.unk7D;
                }
                start = self->ext.et_801A19CC.unk7D ? &g_Entities[160]
                                                    : &g_Entities[224];
                entity = AllocEntity(start, start + 32);
                if (entity != NULL) {
                    DestroyEntity(entity);
                    CreateEntityFromEntity(E_UNK_27, self, entity);
                    entity->pfnUpdate = func_us_801A3B88;
                    entity->facingLeft = self->facingLeft;
                    if (self->facingLeft) {
                        entity->posX.i.hi += 0x1D;
                    } else {
                        entity->posX.i.hi -= 0x1D;
                    }
                    entity->params = 2;
                    entity->posY.i.hi -= 2;
                    entity->params = self->ext.et_801A19CC.unk7D + 2;
                }
                self->ext.et_801A19CC.unk7D ^= 1;
            }
            if (!--self->ext.et_801A19CC.timer) {
                self->ext.et_801A19CC.unk7D = 0;
                func_us_801A19CC(0xA);
            }
            break;
        }
        break;

    case 0xD:
        // run along the floor
        if (!self->ext.et_801A19CC.unk7D) {
            self->ext.et_801A19CC.unk7D = 1;
            self->ext.et_801A19CC.timer = 0x80;
            if (self->facingLeft) {
                self->velocityX = FIX(1.25);
            } else {
                self->velocityX = FIX(-1.25);
            }
            self->velocityY = 0;
        }
        MoveEntity();
        AnimateEntity(D_us_80180824, self);
        if (self->animCurFrame == 5) {
            if (!self->poseTimer) {
                PlaySfxPositional(SFX_STOMP_HARD_A);
            }
            if (self->facingLeft) {
                self->velocityX -= 0x1C00;
            } else {
                self->velocityX += 0x1C00;
            }
        }
        if (self->animCurFrame == 1) {
            if (self->facingLeft) {
                self->velocityX += 0x1C00;
            } else {
                self->velocityX -= 0x1C00;
            }
        }
        x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
        if (self->facingLeft) {
            if (x > 0x180) {
                self->facingLeft = 0;
                self->velocityX = FIX(-1.25);
                self->pose = 0;
                self->poseTimer = 0;
            }
        } else if (x < 0x80) {
            self->facingLeft = 1;
            self->velocityX = FIX(1.25);
            self->pose = 0;
            self->poseTimer = 0;
        }
        if (!CheckColliderOffsets(D_us_80180AE4, self->facingLeft)) {
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            func_us_801A19CC(4);
        }
        if (!--self->ext.et_801A19CC.timer) {
            func_us_801A19CC(0xA);
        }
        break;

    case 0x10:
        // dash up and away
        switch (self->step_s) {
        case 0:
            if (!AnimateEntity(D_us_801808FC, self)) {
                func_us_801A19FC(4, 0);
                self->ext.et_801A19CC.timer = 0x40;
                if (self->facingLeft) {
                    self->velocityX = FIX(4.5);
                } else {
                    self->velocityX = FIX(-4.5);
                }
                self->velocityY = 0;
                self->pose = 0;
                self->poseTimer = 0;
                self->animCurFrame = 0x21;
                self->step_s++;
            }
            break;
        case 1:
            if (self->ext.et_801A19CC.timer < 0x20) {
                AnimateEntity(D_us_80180884, self);
            }
            MoveEntity();
            self->velocityY -= FIX(0.0625);
            if (self->posY.i.hi + g_Tilemap.scrollY.i.hi < 0x48) {
                self->step_s++;
            }
            break;
        case 2:
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            func_us_801A19CC(6);
            break;
        }
        break;

    case 0x14:
        // lay eggs
        if (!AnimateEntity(D_us_8018098C, self)) {
            self->ext.et_801A19CC.unk7D = 0;
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            func_us_801A19CC(4);
        }
        if (self->animCurFrame == 0x4D && !self->ext.et_801A19CC.unk7D) {
            self->ext.et_801A19CC.unk7D = 1;
            n = (Random() & 1) | 2;
            velA = FIX(-0.5);
            velB = 0;
            for (i = 0; i < (n & 0xFF); i++) {
                entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_UNK_25, self, entity);
                    entity->ext.et_801A3FD4.timer = (Random() & 0x1F) - 0x40;
                    if (self->facingLeft) {
                        entity->velocityX = velB + FIX(0.5);
                        entity->posX.i.hi -= 0x10;
                    } else {
                        entity->velocityX = velA;
                        entity->posX.i.hi += 0x10;
                    }
                }
                velA += FIX(0.5);
                velB -= FIX(0.5);
            }
        }
        break;

    case 0x12:
        // swoop down, grab the player and drop them
        switch (self->step_s) {
        case 0:
            AnimateEntity(D_us_8018088C, self);
            MoveEntity();
            if (GetDistanceToPlayerY() < 0x50) {
                self->step_s++;
            }
            break;
        case 1:
            MoveEntity();
            if (!AnimateEntity(D_us_80180894, self)) {
                self->step_s++;
            }
            self->velocityY -= FIX(0.03125);
            if (g_Player.unk60) {
                break;
            }
            x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
            if (self->facingLeft) {
                x = 0x200 - x;
            }
            if (x >= 0x80) {
                dy = PLAYER.posY.i.hi - self->posY.i.hi;
                dx = PLAYER.posX.i.hi - self->posX.i.hi;
                if (dy < 0) {
                    dy = 0x80;
                }
                if (abs(dx) < 0xC && dy < 0x38 &&
                    !(g_Player.status & 0x40C401D2)) {
                    PlaySfxPositional(SFX_HIPPOGRYPH_SQUAWK);
                    g_Player.unk60 = 1;
                    g_Player.unk64 = 0x10;
                    g_Player.unk62 = 0;
                    self->posX.i.hi = PLAYER.posX.i.hi;
                    self->posY.i.hi = PLAYER.posY.i.hi - 0x20;
                }
            }
            break;
        case 2:
            MoveEntity();
            self->velocityY -= FIX(0.375);
            if (!AnimateEntity(D_us_8018089C, self)) {
                self->ext.et_801A19CC.timer = 0x40;
                self->step_s++;
                if (!g_Player.unk60) {
                    self->velocityY -= FIX(0.375);
                }
            }
            break;
        case 3:
            if (g_Player.unk60) {
                AnimateEntity(D_us_801808AC, self);
                if (!(--self->ext.et_801A19CC.timer % 47)) {
                    func_us_801A19FC(8, -8);
                    g_Player.unk60 = 3;
                    g_Player.unk64 = 0x10;
                }
            } else {
                AnimateEntity(D_us_80180830, self);
                if (self->pose == 1) {
                    PlaySfxPositional(SFX_HIPPOGRYPH_WING_FLAP);
                }
            }
            MoveEntity();
            if (self->posY.i.hi + g_Tilemap.scrollY.i.hi < 0x48) {
                g_Player.unk60 = 0;
                self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
                func_us_801A19CC(6);
            }
            break;
        }
        // carry the player
        if (g_Player.unk60) {
            PLAYER.posX.i.hi = self->posX.i.hi;
            PLAYER.posY.i.hi = self->posY.i.hi + 0x20;
        }
        break;

    case 0x16:
        // dive at the player
        switch (self->step_s) {
        case 0:
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            if (!AnimateEntity(D_us_80180844, self)) {
                self->ext.et_801A19CC.unk7D++;
            }
            if (self->pose == 1) {
                PlaySfxPositional(SFX_HIPPOGRYPH_WING_FLAP);
            }
            MoveEntity();
            if (self->animCurFrame == 6 || self->animCurFrame == 7) {
                self->velocityY -= 0x2800;
            } else {
                self->velocityY += FIX(0.0625);
            }
            if (self->ext.et_801A19CC.unk7D >= 2) {
                self->ext.et_801A19CC.unk7D = 0;
                self->pose = 0;
                self->poseTimer = 0;
                self->step_s++;
                if (self->facingLeft) {
                    self->velocityX = FIX(4);
                } else {
                    self->velocityX = FIX(-4);
                }
                self->velocityY = FIX(4.5);
            }
            break;
        case 1:
            MoveEntity();
            AnimateEntity(D_us_80180884, self);
            self->velocityY -= 0x1800;
            if (self->velocityY <= FIX(0.5)) {
                self->velocityY = FIX(0.5);
            }
            if (CheckColliderOffsets(D_us_80180ADC, 0)) {
                self->pose = 0;
                self->poseTimer = 0;
                self->ext.et_801A19CC.unk7D = 0;
                self->step_s++;
            }
            break;
        case 2:
            self->velocityY -= 0x3800;
            if (CheckHippogryphFloorCollision(D_us_80180AAC) & 1) {
                self->ext.et_801A19CC.unk7D = 1;
            }
            if (self->facingLeft) {
                self->velocityX -= FIX(0.0625);
            } else {
                self->velocityX += FIX(0.0625);
            }
            if (self->animCurFrame == 0x5E) {
                func_us_801A19FC(2, -2);
            }
            if (!AnimateEntity(D_us_8018096C, self) &&
                self->ext.et_801A19CC.unk7D) {
                func_us_801A19CC(0xA);
            }
            break;
        }
        break;

    case 0x20:
        // hit
        AnimateEntity(D_us_80180928, self);
        if (CheckHippogryphFloorCollision(D_us_80180AAC) & 1) {
            self->animCurFrame = 0x50;
            func_us_801A19CC(0xA);
        }
        break;

    case 0x24:
        // death
        switch (self->step_s) {
        case 0:
            if (g_Player.unk60) {
                g_Player.unk60 = 0;
            }
            g_api.TimeAttackController(
                TIMEATTACK_EVENT_HYPPOGRYPH_DEFEAT, TIMEATTACK_SET_RECORD);
            self->velocityY = 0;
            x = self->posX.i.hi + g_Tilemap.scrollX.i.hi;
            D_us_801806E0 |= 2;
            if (x < 0x38) {
                self->velocityX = FIX(0.5);
            }
            // BUG: likely meant x > 0x1D8, this almost always wins
            if (x < 0x1D8) {
                self->velocityX = FIX(-0.5);
            }
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY += FIX(0.25);
            if (self->posY.i.hi + g_Tilemap.scrollY.i.hi > 0x80) {
                self->step_s++;
            }
            break;
        case 2:
            if (CheckHippogryphFloorCollision(D_us_80180AAC) & 1) {
                func_us_801A19FC(0x16, -0x20);
                self->step_s = 3;
            } else if (self->posY.i.hi + g_Tilemap.scrollY.i.hi >= 0xB8) {
                func_us_801A19FC(0x16, -0x20);
                self->step_s = 3;
                self->velocityX = 0;
                self->velocityY = 0;
                self->posY.i.hi = 0xB7 - g_Tilemap.scrollY.i.hi;
            }
            break;
        case 3:
            // a cross of light
            primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, 2);
            if (primIndex != -1) {
                prim = &g_PrimBuf[primIndex];
                self->primIndex = primIndex;
                self->ext.et_801A19CC.prim = prim;
                self->flags |= FLAG_HAS_PRIMS;
                while (prim != NULL) {
                    prim->r0 = 0x80;
                    prim->r1 = 0x40;
                    prim->g0 = 0x60;
                    prim->g1 = 0x20;
                    prim->b0 = 0x60;
                    prim->b1 = 0x20;
                    prim->x0 = prim->x1 = self->posX.i.hi;
                    prim->y0 = prim->y1 = self->posY.i.hi;
                    prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                                     DRAW_TRANSP;
                    prim->priority = self->zPriority + 2;
                    prim = prim->next;
                }
                self->step_s++;
            } else {
                self->step_s = 5;
                self->ext.et_801A19CC.prim = NULL;
            }
            self->zPriority = self->ext.et_801A19CC.zPriority - 8;
            break;
        case 4:
            self->ext.et_801A19CC.timer += 6;
            speed = 6;
            if (self->ext.et_801A19CC.timer > 0x80) {
                speed = -8;
            }
            prim = self->ext.et_801A19CC.prim;
            prim->x1 += speed;
            prim = prim->next;
            prim->x1 -= speed;
            if (self->ext.et_801A19CC.timer > 0xF0) {
                g_api.FreePrimitives(self->primIndex);
                self->ext.et_801A19CC.timer = 0;
                self->flags &= ~FLAG_HAS_PRIMS;
                self->step_s++;
            }
            break;
        case 5:
            // rays spinning out of the body, then sparks
            primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, 0x6F);
            if (primIndex == -1) {
                self->step_s = 8;
                break;
            }
            prim = &g_PrimBuf[primIndex];
            self->primIndex = primIndex;
            self->ext.et_801A19CC.prim = prim;
            self->flags |= FLAG_HAS_PRIMS;
            for (i = 0; i < 0x1F; i++) {
                prim->type = PRIM_LINE_G2;
                prim->r0 = 0x60;
                prim->g0 = 0x40;
                prim->b0 = 0x40;
                prim->r1 = 0x10;
                prim->g1 = 8;
                prim->b1 = 8;
                prim->x0 = prim->x1 = self->posX.i.hi;
                prim->y0 = prim->y1 = self->posY.i.hi;
                // growth speed in x2:x3, length in y2:y3
                prim->x2 = D_us_801806E4[i] >> 16;
                prim->y2 = 0;
                prim->y3 = 0;
                prim->x3 = D_us_801806E4[i];
                prim->drawMode =
                    DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
                prim->priority = self->zPriority + 2;
                prim = prim->next;
            }
            self->ext.et_801A19CC.sparks = prim;
            for (i = 0; i < 0x50; i++) {
                prim->type = PRIM_TILE;
                prim->r0 = prim->g0 = prim->b0 = 0x70;
                prim->u0 = prim->v0 = 2;
                prim->drawMode =
                    DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
                prim->priority = self->zPriority + 2;
                // position in x0:x1 and y0:y1, velocity in x2:x3, y2:y3
                prim->x0 = self->posX.i.hi;
                prim->x1 = 0;
                prim->y1 = 0;
                prim->y0 = self->posY.i.hi;
                n = Random() & 7;
                angle = (Random() & 0xF) << 8;
                fix = (rsin(angle) * D_us_801807E4[n]) >> 0xC;
                prim->x2 = fix >> 16;
                prim->x3 = fix;
                fix = (rcos(angle) * D_us_801807E4[n]) >> 0xC;
                prim->y2 = fix >> 16;
                prim->y3 = fix;
                prim->r1 = Random() & 0x17; // start delay
                prim->r2 = 0;
                prim = prim->next;
            }
            self->step_s++;
            break;
        case 6:
        case 7:
            if (self->step_s == 6) {
                AnimateEntity(D_us_801809D8, self);
            }
            prim = self->ext.et_801A19CC.sparks;
            for (i = 0; i < 0x50; i++) {
                if (prim->r1) {
                    prim->r1--;
                } else {
                    fix = PRIM_FIX(prim->x0, prim->x1) +
                          PRIM_FIX(prim->x2, prim->x3);
                    prim->x0 = fix >> 16;
                    prim->x1 = fix;
                    fix = PRIM_FIX(prim->y0, prim->y1) +
                          PRIM_FIX(prim->y2, prim->y3);
                    prim->y0 = fix >> 16;
                    prim->y1 = fix;
                    prim->r2++;
                    if (prim->r2 > 0x20) {
                        prim->u0 = prim->v0 = 1;
                    }
                    if (prim->r2 > 0x30) {
                        prim->u0 = prim->v0 = 0;
                    }
                }
                prim = prim->next;
            }
            prim = self->ext.et_801A19CC.prim;
            for (i = 0; i < 0x1F; i++) {
                if (self->step_s == 6) {
                    speed = PRIM_FIX(prim->x2, prim->x3);
                } else {
                    prim->r0 -= 2;
                    prim->g0--;
                    prim->b0--;
                    speed = PRIM_FIX(-prim->x2, -prim->x3);
                }
                len = PRIM_FIX(prim->y2, prim->y3);
                if (self->ext.et_801A19CC.timer <
                    (self->step_s == 6 ? 0xC : 0x14)) {
                    len += speed;
                }
                radius = (s16)(len >> 16);
                angle = (i << 7) + self->ext.et_801A19CC.timer * 0x10;
                prim->x1 = prim->x0 + ((radius * rcos(angle)) >> 0xC);
                prim->y1 = prim->y0 + ((radius * rsin(angle)) >> 0xC);
                prim->y2 = len >> 16;
                prim->y3 = len;
                prim = prim->next;
            }
            if (self->step_s == 6) {
                if (self->ext.et_801A19CC.timer++ > 0x30) {
                    self->blendMode = BLEND_TRANSP | BLEND_ADD;
                    self->opacity = 0x80;
                    self->drawFlags |= ENTITY_OPACITY;
                    self->step_s++;
                }
                if (!(g_Timer & 7)) {
                    PlaySfxPositional(SFX_GUARD_TINK);
                }
            } else {
                self->opacity -= 4;
                if (self->opacity > 0xE0) {
                    self->opacity = 0;
                }
                if (!(g_Timer & 7)) {
                    PlaySfxPositional(SFX_GUARD_TINK);
                }
                if (self->ext.et_801A19CC.timer++ > 0x4E) {
                    self->ext.et_801A19CC.timer = 0;
                    g_api.FreePrimitives(self->primIndex);
                    self->flags &= ~FLAG_HAS_PRIMS;
                    self->step_s++;
                }
            }
            break;
        case 8:
            x = 0x100 - g_Tilemap.scrollX.i.hi;
            dy = 0x80 - g_Tilemap.scrollY.i.hi;
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_LIFE_UP_SPAWN, entity);
                entity->posX.i.hi = x;
                entity->posY.i.hi = dy;
                entity->params = 6;
                D_us_80180B74 = 0;
                CreateEntityFromCurrentEntity(E_CUTSCENE, &g_Entities[200]);
                self->step_s++;
            }
            break;
        }
        break;

    case 0x30:
    case 0x32:
        // feather
        if (self->step == 0x30) {
            AnimateEntity(D_us_80180A2C, self);
        } else {
            AnimateEntity(D_us_80180A48, self);
            self->velocityX += self->ext.et_801A19CC.accelX;
            if ((u32)(self->velocityX + 0xD000) > 0x1A000) {
                self->ext.et_801A19CC.accelX = -self->ext.et_801A19CC.accelX;
            }
        }
        self->velocityY -= 0x3800;
        if (UnkCollisionFunc3(D_us_80180ACC) & 1) {
            DestroyEntity(self);
        }
        break;

    case 0x82:
        FntPrint("charal %x\n", self->animCurFrame);
        if (g_pads[0].pressed & PAD_SQUARE) {
            if (!self->params) {
                self->animCurFrame++;
                self->params |= 1;
            }
        } else {
            self->params = 0;
        }
        break;
    }

    frame = self->animCurFrame;
    if (frame >= 0xE && frame < 0xE + 0xB) {
        self->hitboxWidth = 6;
        self->hitboxHeight = 0x14;
        self->hitboxOffX = 8;
        self->hitboxOffY = -8;
    } else if ((u8)(frame - 0x3A) < 6) {
        self->hitboxWidth = 6;
        self->hitboxHeight = 0x14;
        self->hitboxOffX = 0;
        self->hitboxOffY = -8;
    } else {
        self->hitboxWidth = 0x14;
        self->hitboxHeight = 8;
        self->hitboxOffX = 0;
        self->hitboxOffY = -2;
    }
}
#endif

extern EInit D_us_801804F0;
extern u8 D_us_80180A00[];
extern u8 D_us_80180A0C[];

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_2159C", func_us_801A3B88);
#else
// A puff of the Hippogryph's fire breath. params 0/1 go up/down, 2/3 go
// straight; 3 is the one with a hitbox. 0 and 3 leave trails (E_UNK_26)
void func_us_801A3B88(Entity* self) {
    Entity* entity;

    if (D_us_801806E0 & 2) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804F0);
        self->animSet = 2;
        self->animCurFrame = 1;
        self->opacity = 0x80;
        self->rotate = 0xE00;
        self->scaleX = self->scaleY = 0x20;
        self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY | ENTITY_ROTATE |
                           ENTITY_OPACITY;
        self->hitboxState = 0;
        self->blendMode |= BLEND_TRANSP | BLEND_ADD;
        if (self->facingLeft) {
            self->velocityX = 0x46800;
        } else {
            self->velocityX = -0x46800;
        }
        if (self->params) {
            self->velocityY = FIX(0.1875);
        } else {
            self->velocityY = FIX(-0.375);
        }
        if (self->params >= 2) {
            self->opacity = 0x7C;
            self->velocityY = 0;
            if (self->params == 3) {
                self->hitboxState = 1;
            }
        }
        break;
    case 1:
        AnimateEntity(D_us_80180A00, self);
        MoveEntity();
        self->velocityY -= FIX(0.078125);
        self->scaleX = self->scaleY = self->scaleX + 0x18;
        self->opacity -= 5;
        if (self->scaleX > 0xF0) {
            self->velocityX /= 4;
            self->pose = 0;
            self->poseTimer = 0;
            self->step++;
        }
        if (self->params == 3 && (Random() & 1)) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                DestroyEntity(entity);
                CreateEntityFromEntity(E_UNK_26, self, entity);
                entity->facingLeft = self->facingLeft;
                entity->params = self->params;
                entity->scaleX = self->scaleX;
                entity->scaleY = self->scaleY;
            }
        }
        if (self->params == 0 && (Random() & 7)) {
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                DestroyEntity(entity);
                CreateEntityFromEntity(E_UNK_26, self, entity);
                entity->facingLeft = self->facingLeft;
                entity->params = self->params;
                entity->scaleX = self->scaleX;
                entity->scaleY = self->scaleY;
            }
        }
        break;
    case 2:
        MoveEntity();
        if (!AnimateEntity(D_us_80180A0C, self)) {
            DestroyEntity(self);
        }
        break;
    }
}
#endif

extern EInit g_EInitParticle;
extern AnimateEntityFrame D_us_80180A18[];

void func_us_801A3E78(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitParticle);
        self->animSet = 2;
        self->animCurFrame = 1;
        self->drawFlags |= ENTITY_SCALEX | ENTITY_SCALEY | ENTITY_OPACITY;
        self->drawFlags |= ENTITY_ROTATE;
        self->blendMode |= BLEND_TRANSP | BLEND_ADD;
        self->opacity = 0x80;
        if (self->facingLeft) {
            self->velocityX = FIX(1);
        } else {
            self->velocityX = -FIX(1);
        }
        if (self->params) {
            self->velocityY = FIX(0.5);
            return;
        }
        self->velocityY = -FIX(1);
        return;

    case 1:
        MoveEntity();
        if (self->facingLeft) {
            self->velocityX -= FIX(0.125);
        } else {
            self->velocityX += FIX(0.125);
        }
        self->velocityY -= FIX(0.0625);
        if (self->params) {
            self->rotate -= ROT(5.625);
        } else {
            self->rotate += ROT(5.625);
        }
        self->scaleX -= 0xA;
        self->opacity -= 6;
        if (!AnimateEntity(D_us_80180A18, self)) {
            DestroyEntity(self);
        }
        return;
    }
}

extern s32 D_us_801806E0;
extern EInit D_us_801804E4;
extern s16 D_us_80180ABC[];
extern AnimateEntityFrame D_us_801809F0[];
extern AnimateEntityFrame D_us_801809F8[];

void func_us_801A3FD4(Entity* self) {
    Entity* entity;

    if (D_us_801806E0 & 2) {
        self->flags |= FLAG_DEAD;
    }

    if (self->flags & FLAG_DEAD) {
        entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
        if (entity != NULL) {
            DestroyEntity(entity);
            CreateEntityFromEntity(E_EXPLOSION, self, entity);
            entity->params = 0;
        }
        DestroyEntity(self);
        return;
    }

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804E4);
        self->hitboxState = 2;
        self->drawFlags |= ENTITY_ROTATE;
        self->rotate = ((Random() & 3) << 9) - 0x300;
        self->ext.et_801A3FD4.rotateDir = 1;
        return;

    case 1:
        if (UnkCollisionFunc3(D_us_80180ABC) & 1) {
            self->step++;
        }

    case 2:
        if (!self->ext.et_801A3FD4.timer) {
            if (AnimateEntity(D_us_801809F0, self)) {
                return;
            }
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                DestroyEntity(entity);
                CreateEntityFromEntity(E_EXPLOSION, self, entity);
                entity->params = 0;
            }
            self->facingLeft = GetSideToPlayer() & 1;
            self->drawFlags = ENTITY_DEFAULT;
            if (self->facingLeft) {
                self->velocityX = -FIX(2.5);
            } else {
                self->velocityX = FIX(2.5);
            }
            self->velocityY = -FIX(0.25);
            self->pose = 0;
            self->poseTimer = 0;
            self->hitboxState = 3;
            PlaySfxPositional(SFX_NO1_BIRD_CYCLE);
            self->step++;
            return;
        } else if (self->ext.et_801A3FD4.timer-- < 8) {
            self->rotate += self->ext.et_801A3FD4.rotateDir << 9;
            self->ext.et_801A3FD4.rotateDir = -self->ext.et_801A3FD4.rotateDir;
            return;
        }
        break;

    case 3:
        AnimateEntity(D_us_801809F8, self);
        MoveEntity();
        self->velocityY -= FIX(0.0390625);
        break;
    }
}

extern u8 D_us_80180B0C[];

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_2159C", func_us_801A425C);
#else
// The Hippogryph's head: a second hitbox that follows its animation
void func_us_801A425C(Entity* self) {
    Entity* parent = self - 1;
    u8 frame;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->hitPoints = 0x7FFE;
        self->hitboxState = 2;
        self->parent = parent;
        self->nextPart = parent;
        self->attack = 0;
        self->attackElement = 0;
        self->flags |= FLAG_SUPPRESS_STUN;
    }
    frame = parent->animCurFrame;
    if (frame >= 0x68) {
        frame = 0;
    }
    switch (D_us_80180B0C[frame]) {
    case 0:
        self->hitboxOffX = 0;
        self->hitboxOffY = 0;
        self->hitboxWidth = 0;
        self->hitboxHeight = 0;
        break;
    case 1:
        self->hitboxOffX = 8;
        self->hitboxOffY = -0x18;
        self->hitboxWidth = 0x14;
        self->hitboxHeight = 0x12;
        break;
    case 2:
        self->hitboxOffX = 0xC;
        self->hitboxOffY = -8;
        self->hitboxWidth = 0x12;
        self->hitboxHeight = 8;
        break;
    case 3:
        self->hitboxOffX = -2;
        self->hitboxOffY = 0x18;
        self->hitboxWidth = 8;
        self->hitboxHeight = 0x12;
        break;
    case 4:
        self->hitboxOffX = 6;
        self->hitboxOffY = 0xA;
        self->hitboxWidth = 0x10;
        self->hitboxHeight = 0xA;
        break;
    case 5:
        self->hitboxOffX = 0x10;
        self->hitboxOffY = -0x20;
        self->hitboxWidth = 0x16;
        self->hitboxHeight = 0x16;
        break;
    case 6:
        self->hitboxOffX = 2;
        self->hitboxOffY = -0x20;
        self->hitboxWidth = 0xC;
        self->hitboxHeight = 0x18;
        break;
    }
    self->facingLeft = parent->facingLeft;
    self->posX.i.hi = parent->posX.i.hi;
    self->posY.i.hi = parent->posY.i.hi;
    if (parent->entityId != E_UNK_22) {
        DestroyEntity(self);
    }
}
#endif

extern EInit D_us_801804F0;

void EntityFireDamageHitbox(Entity* self) {
    if (!self->step) {
        InitializeEntity(D_us_801804F0);
        self->hitboxWidth = 24;
        self->hitboxHeight = 4;
        self->hitboxOffX = -48;
        self->hitboxOffY = -2;
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/unk_2159C", func_us_801A4494);
#else
// Boss music while the fight is on
void func_us_801A4494(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        // fallthrough
    case 1:
        if (D_us_801806DC) {
            stopMusicFlag = 1;
            currentMusicId = MU_DEATH_BALLAD;
            self->step++;
        }
        break;
    case 2:
        if (!g_api.func_80131F68()) {
            stopMusicFlag = 0;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;
    case 3:
        if (!D_us_80180B74) {
            g_api.PlaySfx(0x91);
            self->step++;
        }
        break;
    case 4:
        break;
    }
}
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
s32 D_us_801806E0 = 0;
s32 D_us_801806E4[0x40] = {
    0x40000,
    0x18000,
    0x2D000,
    0x42000,
    0x34000,
    0x24000,
    0x12000,
    0x40000,
    0x48000,
    0x31000,
    0x32000,
    0x2F000,
    0x18000,
    0x4E000,
    0x34000,
    0x31000,
    0x40000,
    0x48000,
    0x2D000,
    0x30000,
    0x34000,
    0x44000,
    0x18000,
    0x30000,
    0x48000,
    0x1F000,
    0x32000,
    0x4F000,
    0x40000,
    0x24000,
    0x44000,
    0x31000,
    0x40000,
    0x18000,
    0x2D000,
    0x42000,
    0x34000,
    0x24000,
    0x12000,
    0x40000,
    0x48000,
    0x31000,
    0x32000,
    0x2F000,
    0x18000,
    0x4E000,
    0x34000,
    0x31000,
    0x40000,
    0x48000,
    0x2D000,
    0x30000,
    0x34000,
    0x44000,
    0x18000,
    0x30000,
    0x48000,
    0x1F000,
    0x32000,
    0x4F000,
    0x40000,
    0x24000,
    0x44000,
    0x31000,
};
s32 D_us_801807E4[8] = { 0x30000, 0x18000, 0x2D000, 0x32000, 0x24000, 0x24000, 0x12000, 0x38000 };
u8 D_us_80180804[0x20] = {
    5,
    0x36,
    5,
    0x37,
    5,
    0x38,
    5,
    0x39,
    5,
    0x3A,
    3,
    0x3B,
    3,
    0x3C,
    3,
    0x3D,
    3,
    0x3C,
    3,
    0x3B,
    5,
    0x3A,
    5,
    0x39,
    5,
    0x38,
    5,
    0x37,
    5,
    0x36,
    0,
    0,
};
u8 D_us_80180824[0xC] = { 8, 1, 8, 2, 8, 3, 8, 4, 8, 5, 0, 0 };
u8 D_us_80180830[0x14] = { 2, 6, 6, 7, 8, 8, 8, 9, 6, 0xA, 5, 0xB, 7, 0xC, 6, 0xD, 0, 0, 0, 0 };
u8 D_us_80180844[0x14] = { 2, 6, 4, 7, 4, 8, 4, 9, 3, 0xA, 3, 0xB, 4, 0xC, 3, 0xD, 0, 0, 0, 0 };
u8 D_us_80180858[0x2C] = {
    4,
    0x1A,
    3,
    0x19,
    2,
    0x18,
    2,
    0x17,
    2,
    0xE,
    2,
    0xF,
    2,
    0x10,
    4,
    0x11,
    4,
    0x12,
    4,
    0x13,
    4,
    0x14,
    7,
    0x15,
    8,
    0x16,
    9,
    0x17,
    7,
    0x18,
    4,
    0x19,
    2,
    0x1A,
    2,
    0x1B,
    7,
    0x1C,
    7,
    0x1D,
    7,
    0x1E,
    0xFF,
    0,
};
u8 D_us_80180884[8] = { 1, 0x21, 2, 0x22, 3, 0x23, 0, 0 };
u8 D_us_8018088C[8] = { 6, 0x24, 6, 0x25, 0, 0, 0, 0 };
u8 D_us_80180894[8] = { 6, 0x26, 6, 0x27, 0x10, 0x28, 0xFF, 0 };
u8 D_us_8018089C[0x10] = { 3, 0x29, 3, 0x2A, 3, 0x2B, 3, 0x2C, 3, 0x2D, 3, 0x2E, 3, 0x2F, 0xFF, 0 };
u8 D_us_801808AC[0x10] = { 6, 0x29, 6, 0x2A, 6, 0x2B, 6, 0x2C, 6, 0x2D, 6, 0x2E, 6, 0x2F, 0, 0 };
u8 D_us_801808BC[0x18] = {
    2,
    0x37,
    2,
    0x38,
    2,
    0x39,
    2,
    0x3A,
    2,
    0x1B,
    2,
    0x1A,
    1,
    0x29,
    5,
    0x2A,
    5,
    0x2B,
    1,
    0x31,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801808D4[0x28] = {
    6,
    0x32,
    6,
    0x33,
    6,
    0x34,
    6,
    0x35,
    1,
    6,
    1,
    8,
    1,
    9,
    1,
    0xA,
    1,
    0xB,
    1,
    0xC,
    2,
    0xD,
    4,
    6,
    4,
    8,
    4,
    9,
    3,
    0xA,
    3,
    0xB,
    3,
    0xC,
    4,
    0xD,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801808FC[0x20] = {
    6,
    0x36,
    6,
    0x37,
    6,
    0x38,
    6,
    0x39,
    6,
    0x3A,
    4,
    0x3B,
    4,
    0x3C,
    4,
    0x3D,
    4,
    0x3E,
    6,
    0x3F,
    6,
    0x40,
    6,
    0x41,
    7,
    0x42,
    7,
    0x43,
    7,
    0x44,
    0xFF,
    0,
};
u8 D_us_8018091C[0xC] = { 0xA, 0x45, 0xA, 0x46, 0xA, 0x47, 0xA, 0x48, 0xA, 0x49, 0, 0 };
u8 D_us_80180928[0x10] = { 5, 0x4A, 4, 0x4B, 4, 0x4C, 4, 0x4D, 4, 0x4E, 4, 0x4F, 0xFF, 0, 0, 0 };
u8 D_us_80180938[0xC] = { 6, 0x51, 6, 0x52, 6, 0x53, 6, 0x54, 0xFF, 0, 0, 0 };
u8 D_us_80180944[0xC] = { 6, 0x54, 6, 0x55, 7, 0x56, 9, 0x57, 0, 0, 0, 0 };
u8 D_us_80180950[0x1C] = {
    2,
    0x64,
    3,
    0x63,
    4,
    0x62,
    4,
    0x61,
    5,
    0x60,
    6,
    0x5F,
    5,
    0x5E,
    4,
    0x5D,
    4,
    0x5C,
    2,
    0x5B,
    4,
    0x61,
    0xA,
    0x58,
    0xF,
    0x59,
    0xFF,
    0,
};
u8 D_us_8018096C[0x20] = {
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
    4,
    0x5F,
    3,
    0x60,
    2,
    0x61,
    2,
    0x62,
    2,
    0x63,
    2,
    0x64,
    3,
    0x65,
    5,
    0x66,
    8,
    0x67,
    0,
    0,
    0,
    0,
};
u8 D_us_8018098C[0x4C] = {
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
    8,
    0x43,
    8,
    0x44,
    8,
    0x31,
    8,
    0x4D,
    0xA,
    0x4E,
    6,
    0x5B,
    6,
    0x5C,
    6,
    0x5D,
    6,
    0x5E,
    6,
    0x5F,
    6,
    0x60,
    6,
    0x61,
    6,
    0x62,
    3,
    0x45,
    3,
    0x46,
    0xFF,
    0,
    0,
    0,
    0xC,
    0x6E,
    0xB,
    0x6F,
    0xA,
    0x70,
    9,
    0x71,
    8,
    0x72,
    7,
    0x73,
    6,
    0x74,
    5,
    0x78,
    5,
    0x79,
    7,
    0x7A,
    8,
    0x7B,
    0xA,
    0x7C,
    0xA,
    0x7D,
    0x34,
    0x7E,
    0xFF,
    0,
    0,
    0,
};
u8 D_us_801809D8[0x18] = {
    5,
    0x36,
    5,
    0x37,
    5,
    0x38,
    5,
    0x39,
    5,
    0x3A,
    5,
    0x3B,
    5,
    0x3C,
    5,
    0x3D,
    5,
    0x3C,
    5,
    0x3B,
    0xFF,
    0,
    0,
    0,
};
AnimateEntityFrame D_us_801809F0[4] = { { 0x18, 0x68 }, { 0x18, 0x69 }, { 0x18, 0x6A }, { 0xFF, 0 } };
AnimateEntityFrame D_us_801809F8[4] = { { 8, 0x6B }, { 8, 0x6C }, { 8, 0x6D }, { 0, 0 } };
u8 D_us_80180A00[0xC] = { 2, 1, 2, 2, 2, 3, 2, 2, 0, 0, 0, 0 };
u8 D_us_80180A0C[0xC] = { 2, 4, 2, 5, 2, 6, 2, 7, 0xFF, 0, 0, 0 };
AnimateEntityFrame D_us_80180A18[0xA] = {
    { 2, 1 },
    { 2, 2 },
    { 2, 3 },
    { 2, 2 },
    { 2, 3 },
    { 2, 4 },
    { 2, 5 },
    { 2, 6 },
    { 2, 7 },
    { 0xFF, 0 },
};
u8 D_us_80180A2C[0x1C] = {
    4,
    0x7F,
    5,
    0x80,
    6,
    0x81,
    7,
    0x82,
    8,
    0x83,
    7,
    0x84,
    6,
    0x85,
    5,
    0x86,
    4,
    0x87,
    3,
    0x88,
    2,
    0x89,
    2,
    0x8A,
    0,
    0,
    0,
    0,
};
u8 D_us_80180A48[0x64] = {
    0x1E,
    0x8B,
    8,
    0x8C,
    0x1E,
    0x8D,
    8,
    0x8C,
    0,
    0,
    0,
    0,
    2,
    0x40,
    0x20,
    0xF,
    0xF,
    2,
    0x50,
    0x20,
    0xF1,
    0xF,
    2,
    0x50,
    0x20,
    0xF1,
    0xF,
    0,
    2,
    0x60,
    0x20,
    0xF1,
    0xF,
    1,
    0x60,
    0x20,
    0xF,
    0xF,
    1,
    0x70,
    0x20,
    0xF1,
    0xF,
    1,
    0x70,
    0x20,
    0xF,
    0xF,
    0,
    0,
    0,
    0,
    2,
    0x40,
    0x20,
    0xF,
    0xF,
    2,
    0x50,
    0x20,
    0xF1,
    0xF,
    2,
    0x50,
    0x20,
    0xF,
    0xF,
    2,
    0x50,
    0x20,
    0xF1,
    0xF,
    2,
    0x50,
    0x20,
    0xF,
    0xF,
    2,
    0x60,
    0x20,
    0xF1,
    0xF,
    2,
    0x60,
    0x20,
    0xF,
    0xF,
    2,
    0x70,
    0x20,
    0xF1,
    0xF,
    2,
    0x70,
    0x20,
    0xF,
    0xF,
    0xFF,
    0,
    0,
};
s16 D_us_80180AAC[8] = { 0, 0x19, 0, 4, 0xC, -4, -0x18, 0 };
s16 D_us_80180ABC[8] = { 0, 7, 0, 4, 8, -4, -0x10, 0 };
s16 D_us_80180ACC[8] = { 0, 7, 0, 4, 8, -4, -0x10, 0 };
s16 D_us_80180ADC[4] = { 0, 0x40, 0xFF, 0 };
s16 D_us_80180AE4[4] = { 0x30, 0x1C, 0xFF, 0 };
u8 D_us_80180AEC[0x20] = {
    0xC,
    4,
    0xC,
    4,
    0xC,
    0xC,
    4,
    0x14,
    4,
    0xC,
    0x10,
    0xD,
    0xD,
    0x10,
    0xD,
    0xD,
    0x16,
    0,
    0x12,
    0,
    6,
    0,
    8,
    0,
    0xF6,
    0xFF,
    0x12,
    0,
    0xE6,
    0xFF,
    0x14,
    0,
};
u8 D_us_80180B0C[0x68] = {
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    0,
    3,
    3,
    3,
    4,
    0,
    2,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
    6,
    1,
    1,
    1,
    6,
    6,
    6,
    6,
    6,
    0,
    0,
    0,
    1,
    6,
    6,
    6,
    6,
    1,
    0,
    3,
    3,
    4,
    0,
    1,
    1,
    3,
    0,
    1,
    6,
    2,
    0,
    0,
    1,
    1,
    5,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    6,
    0,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    6,
    6,
    6,
    6,
    6,
    6,
    1,
    1,
    1,
    4,
    4,
    4,
    4,
    4,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
s32 D_us_80180B74 = 1;
// end pc_import_data.py
// clang-format on
#endif
