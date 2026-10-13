// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo7.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/unk_13258", EntityBreakable);

INCLUDE_ASM("boss/bo7/nonmatchings/unk_13258", EntityBreakableDebris);
#else
// Same code and data as CHI's breakable (checked with pc_asm_compare.py),
// except that breaking it also plays the wall debris sound.
#define CANDLE_HIT_SFX2 SFX_WALL_DEBRIS_B
#define BREAKABLE_ZPRIORITY 112
#define PAL_BREAKABLE_DEBRIS 0x159
#include "../../st/e_breakable_with_lighting.h"
#endif
