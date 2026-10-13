// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <cutscene.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include "../../boss/mar/mar.h"

extern AbbreviatedOverlay g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;

extern u8 cutscene_data[];
extern u8 cutscene_alucard[];
extern u8 cutscene_maria[];

static void InitCutscenePc(void) {
    // the script refers to its events and the portraits by PSX address
    static const CutsceneSymbolRange symbols[] = {
        {cutscene_data, 0x80181424, 0x15EF - 0x1424},
        {cutscene_alucard, 0x801815F0, 0xD80},
        {cutscene_maria, 0x80182370, 0xD80},
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
