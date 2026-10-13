// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo0.h"

#include "../../st/e_marionette.h"

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
EInit g_EInitMarionette = { 0x8004, 0, 0x48, 0x205, 0x31 };
// splat labels this inside g_EInitOwlKnight; BO0 has no Slinger
EInit g_EInitSlinger = { 0, 0x8008, 0, 0x49, 0x2CB };
// end pc_import_data.py
// clang-format on
#endif
