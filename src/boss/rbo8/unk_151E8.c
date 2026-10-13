// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo8.h"

#include "../../st/e_room_bg.h"

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_80180ABC[];
u8 D_us_80180ABC[] ALIGNED4 = {0x40, 0x01, 0xFF, 0x00};
ObjInit2 BackgroundBlockInit[1] = { { 6, 0x1FA, 0, 0, 0, 0, 0x10, 0, D_us_80180ABC } };
// end pc_import_data.py
// clang-format on
#endif
