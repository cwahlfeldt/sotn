// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo5.h"

// NON_MATCHING builds get the spawner from DAI's e_bell.c, see e_bell.c
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/e_bell_spawner", EntityBellSpawner);
#endif
