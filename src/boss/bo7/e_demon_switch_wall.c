// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo7.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo7/nonmatchings/e_demon_switch_wall", UpdateFallingPebble);

INCLUDE_ASM("boss/bo7/nonmatchings/e_demon_switch_wall", EntityDemonSwitch);

INCLUDE_ASM("boss/bo7/nonmatchings/e_demon_switch_wall", EntityDemonSwitchWall);
#else
// Same code and data as CHI's (checked with pc_asm_compare.py) apart from
// the switch trigger, see STAGE_IS_BO7 there.
#define CHI_H
#define g_EInitSecret D_us_80180454
extern EInit D_us_80180454;
#include "../../st/chi/en_demon_switch_wall.c"
#endif
