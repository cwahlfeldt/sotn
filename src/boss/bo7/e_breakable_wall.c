// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo7.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/e_breakable_wall", EntityBreakableWallDebris);

INCLUDE_ASM("boss/bo7/nonmatchings/e_breakable_wall", EntityBreakableWall);
#else
// Same code and data as CHI's (checked with pc_asm_compare.py); bo7.h
// provides everything chi.h would.
#define CHI_H
#define g_EInitSecret D_us_80180454
extern EInit D_us_80180454;
#include "../../st/chi/en_breakable_wall.c"
#endif
