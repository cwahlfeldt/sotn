// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo5.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo5/nonmatchings/e_clouds", EntityClouds);
#else
#include "../../st/e_clouds.h"
#endif
