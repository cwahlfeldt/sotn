// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <cutscene.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include "../../boss/rbo2/rbo2.h"

extern AbbreviatedOverlay g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;

extern u8 D_us_80181878[];
extern u8 D_8018C2A8[];

static void InitCutscenePc(void) {
    // the script refers to its event list (0x80181982, which D_us_80181878
    // runs on into) and portraits (0x8018D224, 0x8018DFA4, in D_8018C2A8)
    // by PSX address
    static const CutsceneSymbolRange symbols[] = {
        {D_us_80181878, 0x80181878, 0xA00},
        {D_8018C2A8, 0x8018C2A8, 0x2A7C},
    };
    CutscenePcAlloc(symbols, LEN(symbols));
}

void Psyz_ModuleStart(void* param) {
    Overlay* o = param;

    memcpy(o, &g_BossOverlay, sizeof(AbbreviatedOverlay));
    PfnEntityUpdates = EntityUpdates;
    g_pStObjLayoutHorizontal = entityLayoutHorizontal;
    g_pStObjLayoutVertical = entityLayoutVertical;
    InitCutscenePc();
}

void Psyz_ModuleStop(void) {}
