// SPDX-License-Identifier: AGPL-3.0-or-later
#include "bo0.h"
#include "../../st/pfn_entity_update.h"

// PC only: the PSX build takes the header from the data segment

extern RoomHeader rooms[];
extern SpriteParts* spriteBanks[];
extern u_long* cluts[];
extern RoomDef rooms_layers[];
extern GfxBank* gfxBanks[];

AbbreviatedOverlay g_Overlay = {
    .Update = Update,
    .HitDetection = HitDetection,
    .UpdateRoomPosition = UpdateRoomPosition,
    .InitRoomEntities = InitRoomEntities,
    .rooms = rooms,
    .spriteBanks = spriteBanks,
    .cluts = cluts,
    .objLayoutHorizontal = &OBJ_LAYOUT_HORIZONTAL,
    .tileLayers = rooms_layers,
    .gfxBanks = gfxBanks,
    .UpdateStageEntities = UpdateStageEntities,
};

#ifdef VERSION_PC
// clang-format off
// begin pc_import_data.py
EInit g_EInitCommon = { 0, 0, 0, 0, 3 };
EInit g_EInitInteractable = { 0, 0, 0, 0, 5 };
EInit g_EInitSpawner = { 0, 0, 0, 0, 4 };
// end pc_import_data.py
// clang-format on
#endif
