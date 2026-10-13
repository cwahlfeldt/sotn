// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo0.h"

#include "../../st/e_room_bg.h"

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
u8 D_us_80180560[] ALIGNED4 = {0x10, 0x0B, 0x00, 0x00};
u8 D_us_80180564[] ALIGNED4 = {0x08, 0x0C, 0x08, 0x0D, 0x08, 0x0E, 0x08, 0x0F, 0x08, 0x10, 0x00, 0x00};
ObjInit2 BackgroundBlockInit[2] = {
    { 0x8001, 0x69, 0, 0, 0, 0, 0, 0, D_us_80180560 },
    { 0x8001, 0x69, 0, 0, 0, 0, 0x30, 0, D_us_80180564 },
};
// end pc_import_data.py
// clang-format on
#endif
