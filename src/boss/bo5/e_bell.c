// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo5.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/e_bell", EntityBell);
#else
// DAI's bell and its spawner (e_bell_spawner.c on PSX)
#define DAI_H
#include "../../st/dai/e_bell.c"
#endif
