// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

extern EInit g_EInitCommon;
#ifndef NON_MATCHING
INCLUDE_ASM("st/rdai/nonmatchings/func_us_801b1c34", func_us_801B1C34);
#else
// Narrows the camera to one part of a room once Alucard passes a point
// (ST0's func_801A8620 with RDAI's limits); params picks the limit
void func_us_801B1C34(Entity* self) {
    s32 params;
    s32 x;
    s16 dist;

    params = (s16)self->params;
    FntPrint("set:%04x\n", params);
    FntPrint("sx:%04x\n", g_Tilemap.left);
    FntPrint("ex:%04x\n", g_Tilemap.right);
    x = (u16)g_PlayerX - (g_Player.unk7C - (u16)PLAYER.posX.i.hi);
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCommon);
        self->animCurFrame = 1;
        self->animSet = 0;
        self->zPriority = 0xB0;
        break;
    case 1:
        dist = self->posY.i.hi - PLAYER.posY.i.hi;
        dist = abs(dist);
        if (dist >= 0x30) {
            break;
        }
        if (g_Player.status & PLAYER_STATUS_DEAD) {
            PLAYER.velocityX = 0;
            break;
        }
        switch (params) {
        case 0:
        case 11:
            if (x < 0x180 && g_PlayerX < 0x180) {
                g_Tilemap.x = 0x180;
                g_Tilemap.left++;
                self->step++;
            }
            break;
        case 2:
        case 4:
        case 13:
        case 14:
            if (x < 0x100 && g_PlayerX < 0x100) {
                g_Tilemap.x = 0x100;
                g_Tilemap.left++;
                self->step++;
            }
            break;
        case 1:
        case 10:
        case 12:
            if (x > 0x280 && g_PlayerX > 0x280) {
                g_Tilemap.width = 0x280;
                g_Tilemap.right--;
                self->step++;
            }
            break;
        case 3:
            if (x > 0x300 && g_PlayerX > 0x300) {
                g_Tilemap.width = 0x300;
                g_Tilemap.right--;
                self->step++;
            }
            break;
        case 5:
            if (x > 0x480 && g_PlayerX > 0x480) {
                g_Tilemap.width = 0x480;
                self->step++;
            }
            break;
        case 6:
        case 7:
        case 9:
            if (x < 0x80 && g_PlayerX < 0x80) {
                g_Tilemap.x = 0x80;
                self->step++;
            }
            break;
        case 8:
            if (x > 0x280 && g_PlayerX > 0x280) {
                g_Tilemap.width = 0x280;
                self->step++;
            }
            break;
        }
        break;
    }
}
#endif
