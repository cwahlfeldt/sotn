// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo7.h"
#define BG_BLOCK_NEEDS_SCALE
#include "../../st/e_room_bg.h"

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180484[];
extern u8 D_us_80180488[];
u8 D_us_80180484[] ALIGNED4 = {0x40, 0x01, 0xFF, 0x00};
u8 D_us_80180488[] ALIGNED4 = {0x02, 0x25, 0x02, 0x26, 0x02, 0x27, 0x02, 0x26, 0x00, 0x00, 0x00, 0x00};
ObjInit2 BackgroundBlockInit[2] = {
    { 6, 0x1FA, 0, 0, 0, 0, 0x10, 0, D_us_80180484 },
    { 0x8001, 0xC0, 0, 0, 0, 3, 0x30, 0, D_us_80180488 },
};
// end pc_import_data.py
// clang-format on
#endif
