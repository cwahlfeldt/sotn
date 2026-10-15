// SPDX-License-Identifier: AGPL-3.0-or-later
// Saves the game each time Alucard enters a room, once he stands on the ground
// in his own form. It goes to the memory card slot the game was loaded from or
// last saved to, else to the first free one on the first card. A save made
// outside a save room keeps where he stood in the save header's reserved bytes
// (nothing else reads them), and loading it puts him back there instead of in
// a coffin.
//   SOTN_AUTOSAVE=0/1   turn it off, or on for --stage, replays and scripted
//                       runs (off by default there so tests leave cards alone)
#include "autosave.h"
#include "dra.h"
#include "framedump.h"
#include "spawn_point.h"
#include <psyz/system.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void StoreSaveData(SaveData* save, s32 block, s32 cardIcon);
void MakeMemcardPath(char* dstSaveName, s32 block);

#define POS_MAGIC "PCAS"

// where Alucard stood when a save was made outside a save room
typedef struct {
    char magic[4];
    s16 stageId;
    s16 roomLeft; // g_Tilemap.left/top, as SaveInfo.roomX/roomY
    s16 roomTop;
    s16 x; // pixels from the top left of the room
    s16 y;
    s16 facingLeft;
} SavePos;
STATIC_ASSERT(sizeof(SavePos) <= sizeof(((MemcardHeader*)0)->reserve),
              "SavePos must fit in MemcardHeader.reserve");

static bool s_Enabled;
static s32 s_RoomStage = -1;
static s32 s_RoomLeft = -1;
static s32 s_RoomTop = -1;
static bool s_Pending; // entered a room that is not saved yet
static SavePos s_Applied;
static bool s_AppliedValid;
static bool s_WarnedFull;
// one memory card block, the size of a save file: StoreSaveData clears all of
// it, more than a SaveData
static u8 s_Block[0x2000];

void Autosave_Init(const struct InitGameParams* params) {
    const char* env = getenv("SOTN_AUTOSAVE");

    s_Enabled = params->stage < 0 && params->spawnPoint == NULL &&
                params->replayPath == NULL && params->demo < 0 &&
                !FrameDump_IsScripted();
    if (env && *env) {
        s_Enabled = atoi(env) != 0;
    }
}

static bool IsSaveRoom(void) { return D_8003C708.flags & FLAG_UNK_20; }

static bool ReadPos(const SaveData* save, SavePos* pos) {
    memcpy(pos, save->header.reserve, sizeof(*pos));
    return !memcmp(pos->magic, POS_MAGIC, sizeof(pos->magic)) &&
           pos->stageId == save->info.stage &&
           pos->roomLeft == save->info.roomX &&
           pos->roomTop == save->info.roomY;
}

void Autosave_OnApply(SaveData* save) {
    s_AppliedValid = ReadPos(save, &s_Applied);
}

// File select renames a save by applying and storing it again: keep the
// position it had
void Autosave_OnStore(SaveData* save) {
    if (s_AppliedValid && s_Applied.stageId == save->info.stage &&
        s_Applied.roomLeft == save->info.roomX &&
        s_Applied.roomTop == save->info.roomY) {
        memcpy(save->header.reserve, &s_Applied, sizeof(s_Applied));
    }
}

void Autosave_OnStageLoad(void) {
    bool valid = s_AppliedValid;

    s_AppliedValid = false;
    if (valid && D_8003C730 == 1 && g_StageId == s_Applied.stageId &&
        g_Tilemap.left == s_Applied.roomLeft &&
        g_Tilemap.top == s_Applied.roomTop) {
        SpawnPoint_PlaceAfterLoad(
            s_Applied.x, s_Applied.y, s_Applied.facingLeft);
    }
}

// the areas of the castle; not the prologue, the title, bosses, cutscene-only
// or test stages
static bool StageAllowsSaving(s32 stageId) {
    switch (stageId) {
    case STAGE_NO0:
    case STAGE_NO1:
    case STAGE_LIB:
    case STAGE_CAT:
    case STAGE_NO2:
    case STAGE_CHI:
    case STAGE_DAI:
    case STAGE_NP3:
    case STAGE_NO3:
    case STAGE_NO4:
    case STAGE_ARE:
    case STAGE_TOP:
    case STAGE_NZ0:
    case STAGE_NZ1:
    case STAGE_WRP:
    case STAGE_RNO0:
    case STAGE_RNO1:
    case STAGE_RLIB:
    case STAGE_RCAT:
    case STAGE_RNO2:
    case STAGE_RCHI:
    case STAGE_RDAI:
    case STAGE_RNO3:
    case STAGE_RNO4:
    case STAGE_RARE:
    case STAGE_RTOP:
    case STAGE_RNZ0:
    case STAGE_RNZ1:
    case STAGE_RWRP:
        return true;
    default:
        return false;
    }
}

static bool CanSave(void) {
    return g_GameState == Game_Play && g_GameStep == Play_Default &&
           g_GameEngineStep == Engine_Normal && D_8003C730 == 0 &&
           !g_CutsceneHasControl && g_PauseAllowed &&
           g_PlayableCharacter == PLAYER_ALUCARD && g_Status.hp > 0 &&
           !(g_Player.status &
             (PLAYER_STATUS_TRANSFORM | PLAYER_STATUS_DEAD)) &&
           !g_Player.demo_timer &&
           (PLAYER.step == Player_Stand || PLAYER.step == Player_Walk ||
            PLAYER.step == Player_Crouch) &&
           StageAllowsSaving(g_StageId);
}

static void CardPath(char* path, size_t len, s32 port, const char* name) {
    char card[64];

    snprintf(card, sizeof(card), "bu%1d0:%s", port, name);
    Psyz_AdjustPath(path, card, (int)len);
}

// what the game would list on that card, to not go past its 15 blocks
static s32 CountCardFiles(s32 port) {
    char dir[256];
    DIR* d;
    struct dirent* e;
    s32 n = 0;

    CardPath(dir, sizeof(dir), port, "");
    d = opendir(dir);
    if (!d) {
        return 0;
    }
    while ((e = readdir(d))) {
        if (e->d_name[0] != '.' && e->d_type == DT_REG) {
            n++;
        }
    }
    closedir(d);
    return n;
}

static bool PickSlot(s32* port, s32* block) {
    char name[32];
    char path[256];
    s32 i;

    if (D_80097924 >= 0 && D_8006C378 >= 0) {
        *port = D_80097924;
        *block = D_8006C378;
        return true;
    }
    if (CountCardFiles(0) >= BLOCK_PER_CARD) {
        return false;
    }
    for (i = 0; i < BLOCK_PER_CARD; i++) {
        MakeMemcardPath(name, i);
        CardPath(path, sizeof(path), 0, name);
        if (access(path, F_OK) != 0) {
            *port = 0;
            *block = i;
            return true;
        }
    }
    return false;
}

static void Save(void) {
    char name[32];
    char path[256];
    char tmp[256];
    s32 port, block;
    SavePos pos;
    FILE* f;
    bool ok;

    if (!PickSlot(&port, &block)) {
        if (!s_WarnedFull) {
            s_WarnedFull = true;
            WARNF("autosave: no free memory card block on the first card");
        }
        return;
    }
    StoreSaveData((SaveData*)s_Block, block, block & 0xF);
    if (!IsSaveRoom()) {
        // a save room's own load puts Alucard in its coffin
        memcpy(pos.magic, POS_MAGIC, sizeof(pos.magic));
        pos.stageId = (s16)g_StageId;
        pos.roomLeft = (s16)g_Tilemap.left;
        pos.roomTop = (s16)g_Tilemap.top;
        pos.x = (s16)g_PlayerX;
        pos.y = (s16)g_PlayerY;
        pos.facingLeft = PLAYER.facingLeft;
        memcpy(((SaveData*)s_Block)->header.reserve, &pos, sizeof(pos));
    }

    // write next to the save and rename, so a crash never leaves half a save;
    // the card listing skips dot files
    MakeMemcardPath(name, block);
    CardPath(path, sizeof(path), port, name);
    snprintf(name, sizeof(name), ".autosave");
    CardPath(tmp, sizeof(tmp), port, name);
    f = fopen(tmp, "wb");
    if (!f) {
        WARNF("autosave: cannot write %s", tmp);
        return;
    }
    ok = fwrite(s_Block, 1, sizeof(s_Block), f) == sizeof(s_Block);
    ok = !fclose(f) && ok;
    if (!ok || rename(tmp, path)) {
        WARNF("autosave: cannot write %s", path);
        remove(tmp);
        return;
    }
    // later saves, autosaves or not, go to the same slot
    D_80097924 = port;
    D_8006C378 = block;
    INFOF("autosaved to %s (stage %02X, room %d,%d)", path, g_StageId,
          g_Tilemap.left, g_Tilemap.top);
}

void Autosave_OnFrame(void) {
    if (!s_Enabled) {
        return;
    }
    if (g_GameState != Game_Play) {
        // the next game starts from scratch
        s_RoomStage = s_RoomLeft = s_RoomTop = -1;
        s_Pending = false;
        return;
    }
    if (g_GameStep != Play_Default) {
        return;
    }
    if (g_StageId != s_RoomStage || g_Tilemap.left != s_RoomLeft ||
        g_Tilemap.top != s_RoomTop) {
        s_RoomStage = g_StageId;
        s_RoomLeft = g_Tilemap.left;
        s_RoomTop = g_Tilemap.top;
        s_Pending = true;
    }
    if (s_Pending && CanSave()) {
        s_Pending = false;
        Save();
    }
}
