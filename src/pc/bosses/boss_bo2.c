// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <cutscene.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include "../../boss/bo2/bo2.h"

extern AbbreviatedOverlay g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;

extern u8 D_us_80181CCC[];
extern u8 D_us_80181DC5[];
extern u8 cutscene_portrait_alucard[];
extern u8 cutscene_portrait_richter[];

static void InitCutscenePc(void) {
    // the scripts refer to their events and the portraits by PSX address
    static const CutsceneSymbolRange symbols[] = {
        {D_us_80181CCC, 0x80181CCC, 0xF9},
        {D_us_80181DC5, 0x80181DC5, 0x87},
        {cutscene_portrait_alucard, 0x80191E14, 0xD80},
        {cutscene_portrait_richter, 0x80192B94, 0xD80},
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
