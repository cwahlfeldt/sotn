// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo6.h"

extern u8 D_us_801805E4[];
extern u8 D_us_801805E8[];
extern u16 D_us_801805EC[];

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo6/nonmatchings/e_lock_camera", PlayerIsWithinHitbox);

INCLUDE_ASM("boss/rbo6/nonmatchings/e_lock_camera", EntityLockCamera);
#else
// RBO6's lock camera tables have no names yet
#define entityLockCameraHitbox D_us_801805E4
#define entityLockCameraData D_us_801805E8
#define entityLockCameraTilemapProps D_us_801805EC
#include "../../st/e_lock_camera.h"
#undef entityLockCameraHitbox
#undef entityLockCameraData
#undef entityLockCameraTilemapProps
#endif

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u8 D_us_801805E4[4] = { 0x20, 0x20, 0x20, 0x20 };
u8 D_us_801805E8[4] = { 1, 0, 0, 0 };
u16 D_us_801805EC[8] = { 0x100, 0xFB, 0x200, 0x1FB, 0, 0, 0x300, 0x300 };
// end pc_import_data.py
// clang-format on
#endif
