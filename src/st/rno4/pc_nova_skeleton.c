// SPDX-License-Identifier: AGPL-3.0-or-later
// PC only: RNO3's Nova Skeleton, whose code and data RNO4 shares. It stands
// in for the Nova Skeleton in unk_58A30.c, part of which is still asm; it
// has its own file because its static names clash with the Jack O'Bones'.
#include "rno4.h"

#define EntityNovaSkeletonDeathParts EntityBladeSoldierDeathParts
#define E_NOVA_DEATH_PARTS E_BLADE_SOLDIER_DEATH_PARTS
#define g_EInitNovaSkeleton2 D_us_80180C20
#include "../e_nova_skeleton.h"
