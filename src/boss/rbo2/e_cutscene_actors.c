// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rbo2.h"

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/e_cutscene_actors", CutsceneCameraPan);
#else
void CutsceneCameraPan(s16 target) {
    s16 delta;

    delta = target - g_unkGraphicsStruct.unk14;
    if (delta > 1) {
        g_unkGraphicsStruct.unk14++;
    } else if (delta < -1) {
        g_unkGraphicsStruct.unk14--;
    } else {
        g_unkGraphicsStruct.unk14 = target;
    }
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("boss/rbo2/nonmatchings/e_cutscene_actors", func_us_801A8FC0_from_bo6);
#else
// if the player is transformed, transform alucard back
// into alucard form
bool func_us_801A8FC0_from_bo6(void) {
    if (g_Player.status & PLAYER_STATUS_TRANSFORM) {
        g_Player.padSim = PAD_NONE;
        if (g_Timer & 1) {
            if (g_Player.status & PLAYER_STATUS_BAT_FORM) {
                g_Player.padSim = PAD_R1;
            } else if (g_Player.status & PLAYER_STATUS_MIST_FORM) {
                g_Player.padSim = PAD_L1;
            } else if (g_Player.status & PLAYER_STATUS_WOLF_FORM) {
                g_Player.padSim = PAD_R2;
            }
        }
        return true;
    }
    return false;
}
#endif
