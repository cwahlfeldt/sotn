// SPDX-License-Identifier: AGPL-3.0-or-later
// PC only: data that upstream's C declares at block scope, where
// tools/pc_import_data.py cannot find it
#include "rbo6.h"

#ifdef VERSION_PC
// clang-format off
static u8 D_us_8018065C[] ALIGNED4 = {
    3, 1, 3, 2, 3, 3, 3, 4, 3, 5, 3, 6, 3, 7, 3, 8, 3, 9, 3, 10, 3, 11, 3, 12,
    3, 13, 0xFF, 0};
static u8 D_us_80180678[] ALIGNED4 = {
    2, 1, 2, 2, 2, 3, 2, 4, 2, 5, 2, 6, 2, 7, 2, 8, 2, 9, 2, 10, 2, 11, 2, 12,
    2, 13, 2, 14, 0xFF, 0, 0, 0};
static u8 D_us_80180698[] ALIGNED4 = {
    3, 1, 3, 2, 3, 3, 3, 4, 3, 5, 3, 6, 3, 7, 3, 8, 3, 9, 3, 10, 3, 11, 3, 12,
    3, 13, 0xFF, 0};

// func_us_8019D330_from_rcen's particles: animSet, unk5A, palette,
// blendMode, animation (the PC layout of its Borbo6ParticleData)
EntityConfig D_us_801806B4[] = {
    {0x8002, 0x57, 2, 0, D_us_80180678},
    {0x0E, 0x79, 0, 0x30, D_us_8018065C},
    {0x0E, 0x79, 0x0A, 0x30, D_us_8018065C},
    {0x8004, 0x58, 0x0E, 0x30, D_us_80180698},
};

// func_us_801A1150
u8 D_us_80180724[] ALIGNED4 = {1, 6, 1, 7, 0, 0, 0, 0};

// the body pieces (func_us_8019FB04)
static u8 D_us_801807B4[] ALIGNED4 = {0x30, 0x1C, 0x1E, 0x1D, 0, 0, 0, 0};
static u8 D_us_801807BC[] ALIGNED4 = {0x20, 0x1E, 0x20, 0x1F, 0, 0, 0, 0};
static u8 D_us_801807C4[] ALIGNED4 = {0x20, 0x20, 0x20, 0x21, 0, 0, 0, 0};
static u8 D_us_801807CC[] ALIGNED4 = {0x30, 0x22, 0x10, 0x23, 0, 0, 0, 0};
static u8 D_us_801807D4[] ALIGNED4 = {
    0x30, 0x24, 0x08, 0x25, 0x18, 0x26, 0x08, 0x25, 0, 0, 0, 0};
u8* D_us_801807F4[] = {
    D_us_801807B4, D_us_801807BC, D_us_801807C4, D_us_801807CC, D_us_801807D4,
};

// the heads' breath (func_us_801A1B38)
u8 D_us_801807EC[] ALIGNED4 = {1, 0x2B, 1, 0x2C, 0, 0, 0, 0};

// the centrepiece's sparks (func_us_801B2864)
u8 D_us_80181648[] ALIGNED4 = {
    2, 1, 2, 2, 2, 3, 2, 4, 2, 5, 2, 6, 2, 7, 2, 8, 2, 9, 2, 10, 2, 11, 2, 12,
    2, 13, 0xFF, 0};
// clang-format on
#endif
