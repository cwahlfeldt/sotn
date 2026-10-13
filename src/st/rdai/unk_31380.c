// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#include "../e_room_bg.h"

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
extern u8 D_us_8018092C[];
u8 D_us_8018092C[] ALIGNED4 = {0x40, 0x02, 0xFF, 0x00};
ObjInit2 BackgroundBlockInit[1] = { { 6, 0x1FA, 0, 0, 0, 0, 0x10, 0, D_us_8018092C } };
// end pc_import_data.py
// clang-format on
#endif
