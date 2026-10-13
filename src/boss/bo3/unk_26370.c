// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo3.h"

extern EInit g_EInitInteractable;
extern s32 D_us_80180D48;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_26370", func_us_801A6370);
#else
// Scylla's music: the boss theme when the fight starts, Crystal Teardrops
// again once she is dead
void func_us_801A6370(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        if (g_api.TimeAttackController(
                TIMEATTACK_EVENT_SCYLLA_DEFEAT, TIMEATTACK_GET_RECORD)) {
            DestroyEntity(self);
            return;
        }
        g_api.PlaySfx(SET_UNK_90);
        // fallthrough
    case 1:
        if (D_us_80180D48 & SCYLLA_FIGHT_STARTED) {
            stopMusicFlag = false;
            currentMusicId = MU_FESTIVAL_OF_SERVANTS;
            self->step++;
        }
        break;
    case 2:
        if (!g_api.func_80131F68()) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        // fallthrough
    case 3:
        if (D_us_80180D48 & SCYLLA_DEFEATED) {
            g_api.PlaySfx(0x91);
            currentMusicId = MU_CRYSTAL_TEARDROPS;
            stopMusicFlag = false;
            self->step++;
        }
        break;
    case 4:
        if (D_us_80180D48 & SCYLLA_REWARD_SPAWNED) {
            stopMusicFlag = true;
            currentMusicId = MU_CRYSTAL_TEARDROPS;
            self->step++;
        }
        break;
    case 5:
        if (!g_api.func_80131F68()) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;
    }
}
#endif

extern u16 D_us_80180C66;
#ifndef NON_MATCHING
INCLUDE_ASM("boss/bo3/nonmatchings/unk_26370", func_us_801A6568);
#else
// The Scylla Wyrm's music, in the room before Scylla's
void func_us_801A6568(Entity* self) {
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        if (g_CastleFlags[SCYLLA_WYRM_DEFEATED]) {
            DestroyEntity(self);
            return;
        }
        // fallthrough
    case 1:
        if (D_us_80180C66 & 2) {
            stopMusicFlag = true;
            currentMusicId = MU_FESTIVAL_OF_SERVANTS;
            self->step++;
        }
        break;
    case 2:
        if (!g_api.func_80131F68()) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        // fallthrough
    case 3:
        if (g_CastleFlags[SCYLLA_WYRM_DEFEATED]) {
            g_api.PlaySfx(SET_UNK_90);
            stopMusicFlag = true;
            currentMusicId = MU_CRYSTAL_TEARDROPS;
            self->step = 4;
        }
        break;
    case 4:
        if (!g_api.func_80131F68()) {
            stopMusicFlag = false;
            g_api.PlaySfx(currentMusicId);
            self->step++;
        }
        break;
    }
}
#endif
