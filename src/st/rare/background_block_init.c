// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rare.h"

// PSX still keeps this table as raw data; the PC build needs it as C since
// the entries hold pointers
#ifdef VERSION_PC
static u8 anim0[] = {16, 11, 0, 0};
static u8 anim1[] = {8, 12, 8, 13, 8, 14, 8, 15, 8, 16, 0, 0};
static u8 anim2[] = {16, 19, 255, 0};
static u8 anim3[] = {16, 13, 255, 0};
static u8 anim4[] = {16, 14, 255, 0};

ObjInit2 BackgroundBlockInit[] = {
    {0x8002, 105, 0, 0, 0x0, 0x0, BLEND_NO, 0, anim0},
    {0x8002, 105, 0, 0, 0x0, 0x0, BLEND_TRANSP | BLEND_ADD, 0, anim1},
    {0x8002, 108, 0, 0, 0x0, 0x0, BLEND_NO, 0, anim2},
    {0x8002, 108, 0, 0, 0x0, 0x0, BLEND_NO, 0, anim3},
    {0x8002, 108, 0, 0, 0x0, 0x0, BLEND_NO, 0, anim4},
};
#endif
