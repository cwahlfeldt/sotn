// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef PC_AUTOSAVE_H
#define PC_AUTOSAVE_H

#include <game.h>
#include "pc.h"

void Autosave_Init(const struct InitGameParams* params);
void Autosave_OnFrame(void);
// a stage overlay is about to load (src/pc/stages/overlay.c)
void Autosave_OnStageLoad(void);
// called by ApplySaveData and StoreSaveData (src/save_mgr.h)
void Autosave_OnApply(SaveData* save);
void Autosave_OnStore(SaveData* save);

#endif
