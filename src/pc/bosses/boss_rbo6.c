// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <cutscene.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include "../../boss/rbo6/rbo6.h"

extern Overlay g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;

extern u8 D_us_80181720[];
extern u8 D_us_801819E7[];
extern u8 D_8019032C[];

static void InitCutscenePc(void) {
    // the scripts refer to their event lists (0x80181D13, 0x80181D27, in
    // D_us_801819E7) and portraits (0x8019113C, 0x80191EBC, in D_8019032C)
    // by PSX address
    static const CutsceneSymbolRange symbols[] = {
        {D_us_80181720, 0x80181720, 0x2C7},
        {D_us_801819E7, 0x801819E7, 0x2BC9},
        {D_8019032C, 0x8019032C, 0x4B34},
    };
    CutscenePcAlloc(symbols, LEN(symbols));
}

void Psyz_ModuleStart(void* param) {
    Overlay* o = param;

    memcpy(o, &g_BossOverlay, sizeof(Overlay));
    PfnEntityUpdates = EntityUpdates;
    g_pStObjLayoutHorizontal = entityLayoutHorizontal;
    g_pStObjLayoutVertical = entityLayoutVertical;
    InitCutscenePc();
}

void Psyz_ModuleStop(void) {}
