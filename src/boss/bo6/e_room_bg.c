// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo6.h"

#include "../../st/e_room_bg.h"

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_801804D8[];
extern u8 D_us_801804DC[];
u8 D_us_801804D8[] ALIGNED4 = {0x10, 0x0B, 0x00, 0x00};
u8 D_us_801804DC[] ALIGNED4 = {0x08, 0x0C, 0x08, 0x0D, 0x08, 0x0E, 0x08, 0x0F, 0x08, 0x10, 0x00, 0x00};
ObjInit2 BackgroundBlockInit[2] = {
    { 0x8001, 0x69, 0, 0, 0, 0, 0, 0, D_us_801804D8 },
    { 0x8001, 0x69, 0, 0, 0, 0, 0x30, 0, D_us_801804DC },
};
// end pc_import_data.py
// clang-format on
#endif
