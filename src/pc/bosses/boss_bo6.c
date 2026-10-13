// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <cutscene.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include "../../boss/bo6/bo6.h"

extern AbbreviatedOverlay2 g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;

extern u8 D_us_8019A0C8;
extern u8 D_us_8019A0CC;
extern u8 D_us_8019A0D0[];
extern u8 D_us_8019A2E1[];
extern u8 D_us_8019A2FB[];
extern u8 D_8019ACC4[];

static void InitCutscenePc(void) {
    // the scripts refer to their switch variables, event lists and portraits
    // by PSX address. D_us_8019A2FB runs on into the event lists, and the
    // portraits (0x8019B528, 0x8019C2A8, 0x8019D028) lie in D_8019ACC4.
    static const CutsceneSymbolRange symbols[] = {
        {&D_us_8019A0C8, 0x8019A0C8, 1},
        {&D_us_8019A0CC, 0x8019A0CC, 1},
        {D_us_8019A0D0, 0x8019A0D0, 0x211},
        {D_us_8019A2E1, 0x8019A2E1, 0x1A},
        {D_us_8019A2FB, 0x8019A2FB, 0x1F9},
        {D_8019ACC4, 0x8019ACC4, 0x30E4},
    };
    CutscenePcAlloc(symbols, LEN(symbols));
}

void Psyz_ModuleStart(void* param) {
    Overlay* o = param;

    memcpy(o, &g_BossOverlay, sizeof(AbbreviatedOverlay2));
    PfnEntityUpdates = EntityUpdates;
    g_pStObjLayoutHorizontal = entityLayoutHorizontal;
    g_pStObjLayoutVertical = entityLayoutVertical;
    InitCutscenePc();
}

void Psyz_ModuleStop(void) {}
