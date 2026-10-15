// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef PC_SPAWN_POINT_H
#define PC_SPAWN_POINT_H

#include <game.h>
#include "pc.h"

int SpawnPoint_StageForName(const char* name);
void SpawnPoint_Init(const struct InitGameParams* params);
void SpawnPoint_OnFrame(void);
void SpawnPoint_HookOverlay(Overlay* o);
// put the player at x, y (pixels from the top left of the room) in the room a
// save is loading into, rather than in a save room's coffin
void SpawnPoint_PlaceAfterLoad(s32 x, s32 y, s32 facingLeft);

#endif
