// SPDX-License-Identifier: AGPL-3.0-or-later
// Debug helper for headless testing: periodically writes the PSX display area
// to PNG files and optionally quits after a number of frames.
//   SOTN_SHOT_DIR=<dir>     enable dumping into <dir>
//   SOTN_SHOT_EVERY=<n>     dump every n frames (default 60)
//   SOTN_SHOT_INTERP=1      also dump the in-between frame psyz drew on a
//                           120 Hz display, as frame_<n>_i.png
//   SOTN_SHOT_MMPX=1        also dump the last MMPX input and passes
//                           (--filter mmpx) as frame_<n>_m<pass>.png
//   SOTN_EXIT_AFTER=<n>     request quit after n frames
//   SOTN_INPUT=<script>     drive pad 1 from a script of comma-separated
//                           "<frames>:<buttons>" steps, buttons joined by '+'
//                           (up down left right cross circle square triangle
//                           l1 l2 r1 r2 start select none), e.g.
//                           "120:none,90:right,15:right+cross"
//   SOTN_GOD=<n>            keep Alucard's HP and MP full (MP max at least
//                           99); n > 1 also sets STR to n
//   SOTN_EQUIP=<l>[,<r>[,<head>[,<body>[,<cape>[,<acc1>[,<acc2>]]]]]]
//                           equip these item ids when the stage sets up the
//                           gear (hands index g_EquipDefs, the rest
//                           g_AccessoryDefs); "-" keeps a slot as it is
//   SOTN_FAMILIAR=<n>       summon familiar n (1 bat .. 7 nose demon) there too
//   SOTN_FAMILIAR_EXP=<n>   give that familiar n EXP (level n / 100 + 1)
//   SOTN_RELICS=<i>[,<i>..] give these relics (RELIC_* indexes, e.g. 12,13
//                           for Gravity Boots and Leap Stone) there too;
//                           -<i> takes one away (--stage starts with all)
//   SOTN_SUBWEAPON=<n>      set the subweapon (SUBWPN_*, 2 = axe) and keep
//                           hearts full
//   SOTN_HP=<n>             start with n HP (game over tests)
//   SOTN_EVENTS=<i>[,<i>..] mark these TIMEATTACK_EVENT_* ids as done (a boss
//                           beaten, a scene seen), so their boss teleports
//                           and scenes are skipped; 26 is the first meeting
//                           with Maria in NO0's clock room. "all" marks 0-26
//   SOTN_CASTLE_FLAGS=<i>[,<i>..] set these g_CastleFlags entries to 1 once
//                           the stage starts playing (e.g. 0xD8, met Maria
//                           in CEN)
//   SOTN_SEEK=<id>          steer the pad script's left/right toward the first
//                           entity with this id, facing it when in reach;
//                           @<slot> follows a slot instead (the Doppleganger
//                           is slot 64 with id 0)
//   SOTN_KILL=<id>:<frame>[,<id>:<frame>..] flag every entity with this id
//                           as dead at that frame, to reach death sequences
//                           quickly
//   SOTN_FAST=1             don't wait for vblank, so scripted runs go as
//                           fast as the machine can draw (audio is skipped)
//   SOTN_VRAM_AT=<frame>    write the whole VRAM (1024x512 16-bit, raw) to
//                           <SOTN_SHOT_DIR or .>/vram_<frame>.bin
//   SOTN_TRACE=<s>[,<s>..]  print these entity slots to stderr every
//                           SOTN_TRACE_EVERY frames (default 30), from frame
//                           SOTN_TRACE_FROM (default 0)
#include <game.h>
#include <items.h>
#include <psyz/input.h>
#include <psyz/video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "framedump.h"

extern bool g_IsQuitRequested;

static const char* s_ShotDir;
static int s_ShotEvery = 60;
static bool s_ShotInterp;
static bool s_ShotMmpx;
static int s_ExitAfter;
static int s_Frame;
static bool s_Init;
static int s_GodStr; // SOTN_GOD: keep HP full; a value > 1 also sets STR

// SOTN_SUBWEAPON: keep hearts full
static bool s_InfHearts;
// SOTN_CASTLE_FLAGS, until applied
static const char* s_CastleFlags;
// SOTN_KILL
static const char* s_Kills;

#define MAX_INPUT_STEPS 8192
typedef struct {
    int frames;
    u16 buttons;
} InputStep;
static InputStep s_Steps[MAX_INPUT_STEPS];
static int s_StepCount;
static int s_StepIndex;
static int s_StepFrame;
static int s_SeekId = -1;   // SOTN_SEEK
static int s_SeekSlot = -1; // SOTN_SEEK=@<slot>

static u16 ParseButtons(const char* s, size_t len) {
    static const struct {
        const char* name;
        u16 mask;
    } names[] = {
        {"select", 0x0001}, {"start", 0x0008},    {"up", 0x0010},
        {"right", 0x0020},  {"down", 0x0040},     {"left", 0x0080},
        {"l2", 0x0100},     {"r2", 0x0200},       {"l1", 0x0400},
        {"r1", 0x0800},     {"triangle", 0x1000}, {"circle", 0x2000},
        {"cross", 0x4000},  {"square", 0x8000},   {"none", 0x0000},
    };
    u16 mask = 0;
    size_t i, start = 0;
    int k;

    for (i = 0; i <= len; i++) {
        if (i == len || s[i] == '+') {
            for (k = 0; k < LEN(names); k++) {
                if (strlen(names[k].name) == i - start &&
                    !strncmp(names[k].name, s + start, i - start)) {
                    mask |= names[k].mask;
                }
            }
            start = i + 1;
        }
    }
    return mask;
}

static void ParseInputScript(const char* script) {
    const char* p = script;
    while (*p && s_StepCount < MAX_INPUT_STEPS) {
        const char* end = strchr(p, ',');
        const char* colon;
        size_t len = end ? (size_t)(end - p) : strlen(p);
        colon = memchr(p, ':', len);
        if (colon) {
            s_Steps[s_StepCount].frames = atoi(p);
            s_Steps[s_StepCount].buttons =
                ParseButtons(colon + 1, len - (colon + 1 - p));
            s_StepCount++;
        }
        if (!end) {
            break;
        }
        p = end + 1;
    }
}

// SOTN_SEEK: walk toward an entity, since scripts cannot follow a boss
static u16 Seek(u16 buttons) {
    Entity* e;
    int i;
    int dx;

    if (s_SeekSlot >= 0) {
        e = &g_Entities[s_SeekSlot];
        if (!e->entityId && !e->hitPoints) {
            return buttons;
        }
        if (e->flags & FLAG_DEAD || e->hitPoints <= 0) {
            return buttons; // let the script walk on after the kill
        }
    } else {
        for (i = 0, e = g_Entities; i < LEN(g_Entities); i++, e++) {
            if (e->entityId == s_SeekId) {
                break;
            }
        }
        if (i == LEN(g_Entities)) {
            return buttons;
        }
    }
    buttons &= ~(0x0020 | 0x0080);
    dx = e->posX.i.hi - PLAYER.posX.i.hi;
    if (dx < -24 || (dx < 0 && !PLAYER.facingLeft)) {
        buttons |= 0x0080; // left
    } else if (dx > 24 || (dx > 0 && PLAYER.facingLeft)) {
        buttons |= 0x0020; // right
    }
    return buttons;
}

static void ApplyInputScript(void) {
    char frame[4];
    u16 buttons = 0;

    if (!s_StepCount) {
        return;
    }
    while (s_StepIndex < s_StepCount &&
           s_StepFrame >= s_Steps[s_StepIndex].frames) {
        s_StepIndex++;
        s_StepFrame = 0;
    }
    if (s_StepIndex < s_StepCount) {
        buttons = s_Steps[s_StepIndex].buttons;
        s_StepFrame++;
    }
    if (s_SeekId >= 0 || s_SeekSlot >= 0) {
        buttons = Seek(buttons);
    }
    // digital pad frame: status, kind, then active-low button bits
    frame[0] = 0x00;
    frame[1] = 0x41;
    frame[2] = (char)~(buttons & 0xFF);
    frame[3] = (char)~(buttons >> 8);
    Psyz_PadsSet(0, frame, sizeof(frame));
}

static u32 Crc32(u32 crc, const u8* data, size_t len) {
    static u32 table[256];
    static bool ready;
    size_t i;
    int k;

    if (!ready) {
        for (i = 0; i < 256; i++) {
            u32 c = (u32)i;
            for (k = 0; k < 8; k++) {
                c = (c & 1) ? 0xEDB88320 ^ (c >> 1) : c >> 1;
            }
            table[i] = c;
        }
        ready = true;
    }
    crc = ~crc;
    for (i = 0; i < len; i++) {
        crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return ~crc;
}

static void PutU32BE(u8* p, u32 v) {
    p[0] = v >> 24;
    p[1] = v >> 16;
    p[2] = v >> 8;
    p[3] = v;
}

static void WriteChunk(FILE* f, const char* type, const u8* data, u32 len) {
    u8 hdr[8];
    u8 crcbuf[4];
    u32 crc;

    PutU32BE(hdr, len);
    memcpy(hdr + 4, type, 4);
    fwrite(hdr, 1, 8, f);
    if (len) {
        fwrite(data, 1, len, f);
    }
    crc = Crc32(0, (const u8*)type, 4);
    crc = Crc32(crc, data, len);
    PutU32BE(crcbuf, crc);
    fwrite(crcbuf, 1, 4, f);
}

// Minimal PNG encoder: 8-bit RGB, zlib stream made of stored blocks.
static bool WritePng(const char* path, const u8* rgb, int w, int h) {
    static const u8 sig[] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    size_t raw_len = (size_t)(w * 3 + 1) * h;
    size_t nblocks = (raw_len + 0xFFFE) / 0xFFFF;
    size_t z_len = 2 + raw_len + nblocks * 5 + 4;
    u8* raw = malloc(raw_len);
    u8* z = malloc(z_len);
    u8 ihdr[13];
    size_t i, pos, left;
    u32 a = 1, b = 0;
    FILE* f;
    int y;

    if (!raw || !z) {
        free(raw);
        free(z);
        return false;
    }
    for (y = 0; y < h; y++) {
        raw[y * (w * 3 + 1)] = 0;
        memcpy(raw + y * (w * 3 + 1) + 1, rgb + y * w * 3, w * 3);
    }
    pos = 0;
    z[pos++] = 0x78;
    z[pos++] = 0x01;
    for (i = 0, left = raw_len; left > 0;) {
        u16 n = left > 0xFFFF ? 0xFFFF : (u16)left;
        z[pos++] = left == n ? 1 : 0;
        z[pos++] = n & 0xFF;
        z[pos++] = n >> 8;
        z[pos++] = ~n & 0xFF;
        z[pos++] = (~n >> 8) & 0xFF;
        memcpy(z + pos, raw + i, n);
        pos += n;
        i += n;
        left -= n;
    }
    for (i = 0; i < raw_len; i++) {
        a = (a + raw[i]) % 65521;
        b = (b + a) % 65521;
    }
    PutU32BE(z + pos, (b << 16) | a);
    pos += 4;

    PutU32BE(ihdr, w);
    PutU32BE(ihdr + 4, h);
    ihdr[8] = 8;  // bit depth
    ihdr[9] = 2;  // RGB
    ihdr[10] = 0; // deflate
    ihdr[11] = 0; // filter
    ihdr[12] = 0; // no interlace

    f = fopen(path, "wb");
    if (f) {
        fwrite(sig, 1, sizeof(sig), f);
        WriteChunk(f, "IHDR", ihdr, sizeof(ihdr));
        WriteChunk(f, "IDAT", z, (u32)pos);
        WriteChunk(f, "IEND", NULL, 0);
        fclose(f);
    }
    free(raw);
    free(z);
    return f != NULL;
}

static int s_VramAt = -1; // SOTN_VRAM_AT

static void DumpVram(void) {
    RECT rect = {0, 0, 1024, 512};
    u16* vram = malloc(1024 * 512 * sizeof(u16));
    char path[512];
    FILE* f;

    if (!vram) {
        return;
    }
    DrawSync(0);
    StoreImage(&rect, (u_long*)vram);
    DrawSync(0);
    snprintf(path, sizeof(path), "%s/vram_%06d.bin",
             s_ShotDir ? s_ShotDir : ".", s_Frame);
    if ((f = fopen(path, "wb"))) {
        fwrite(vram, sizeof(u16), 1024 * 512, f);
        fclose(f);
    }
    free(vram);
}

static void DumpInterp(void) {
    char path[512];
    int w, h;
    u8* rgb = Psyz_VideoAllocCapturedInterpFrame(&w, &h);

    if (rgb) {
        snprintf(path, sizeof(path), "%s/frame_%06d_i.png", s_ShotDir, s_Frame);
        WritePng(path, rgb, w, h);
        free(rgb);
    }
}

static void DumpMmpx(void) {
    char path[512];
    int pass, w, h;
    u8* rgb;

    for (pass = 0; (rgb = Psyz_VideoAllocCapturedFilterPass(pass, &w, &h));
         pass++) {
        snprintf(path, sizeof(path), "%s/frame_%06d_m%d.png", s_ShotDir,
                 s_Frame, pass);
        WritePng(path, rgb, w, h);
        free(rgb);
    }
}

static void DumpDisplay(void) {
    DISPENV env;
    RECT rect;
    u16* vram;
    u8* rgb;
    char path[512];
    int i, n;

    GetDispEnv(&env);
    rect = env.disp;
    if (rect.w <= 0 || rect.h <= 0 || rect.w > 1024 || rect.h > 512) {
        return;
    }
    n = rect.w * rect.h;
    vram = malloc(n * sizeof(u16));
    rgb = malloc(n * 3);
    if (vram && rgb) {
        DrawSync(0);
        StoreImage(&rect, (u_long*)vram);
        DrawSync(0);
        for (i = 0; i < n; i++) {
            u16 c = vram[i];
            rgb[i * 3 + 0] = (u8)((c & 0x1F) << 3);
            rgb[i * 3 + 1] = (u8)(((c >> 5) & 0x1F) << 3);
            rgb[i * 3 + 2] = (u8)(((c >> 10) & 0x1F) << 3);
        }
        snprintf(path, sizeof(path), "%s/frame_%06d.png", s_ShotDir, s_Frame);
        WritePng(path, rgb, rect.w, rect.h);
    }
    free(vram);
    free(rgb);
}

extern RelicDesc g_RelicDefs[];

void FrameDump_OnInitGear(void) {
    const char* env = getenv("SOTN_EQUIP");
    char* end;
    long id;

    int i;

    for (i = 0; env && *env && i < NUM_ITEM_SLOTS; i++) {
        if (*env == '-') {
            end = (char*)env + 1;
        } else {
            g_Status.equipment[LEFT_HAND_SLOT + i] = strtol(env, &end, 0);
        }
        env = *end == ',' ? end + 1 : NULL;
    }
    env = getenv("SOTN_FAMILIAR");
    if (env && *env) {
        id = strtol(env, NULL, 0);
        for (i = 0; i < LEN(g_Status.relics); i++) {
            if (g_RelicDefs[i].unk0C == id) {
                g_Status.relics[i] = RELIC_FLAG_FOUND | RELIC_FLAG_ACTIVE;
            }
        }
    }
    env = getenv("SOTN_FAMILIAR_EXP");
    if (env && *env && getenv("SOTN_FAMILIAR")) {
        id = strtol(getenv("SOTN_FAMILIAR"), NULL, 0) - 1;
        if (id >= 0 && id < NUM_FAMILIARS) {
            g_Status.statsFamiliars[id].exp = strtol(env, NULL, 0);
            g_Status.statsFamiliars[id].level =
                g_Status.statsFamiliars[id].exp / 100 + 1;
        }
    }
    env = getenv("SOTN_RELICS");
    while (env && *env) {
        if (*env == '-') {
            // -<i> takes the relic away, so its orb appears in the stage
            id = strtol(env + 1, &end, 0);
            if (id >= 0 && id < LEN(g_Status.relics)) {
                g_Status.relics[id] = 0;
            }
        } else {
            id = strtol(env, &end, 0);
            if (id >= 0 && id < LEN(g_Status.relics)) {
                g_Status.relics[id] = RELIC_FLAG_FOUND | RELIC_FLAG_ACTIVE;
            }
        }
        env = *end == ',' ? end + 1 : NULL;
    }
    env = getenv("SOTN_HP");
    if (env && *env) {
        g_Status.hp = strtol(env, NULL, 0);
    }
    env = getenv("SOTN_EVENTS");
    while (env && *env) {
        if (!strncmp(env, "all", 3)) {
            for (i = 0; i < NUM_TIMEATTACK_EVENTS; i++) {
                if (!g_Settings.timeAttackRecords[i]) {
                    g_Settings.timeAttackRecords[i] = 1;
                }
            }
            break;
        }
        id = strtol(env, &end, 0);
        if (id >= 0 && id < LEN(g_Settings.timeAttackRecords) &&
            !g_Settings.timeAttackRecords[id]) {
            g_Settings.timeAttackRecords[id] = 1;
        }
        env = *end == ',' ? end + 1 : NULL;
    }
    env = getenv("SOTN_SUBWEAPON");
    if (env && *env) {
        g_Status.subWeapon = strtol(env, NULL, 0);
        g_Status.hearts = g_Status.heartsMax = 99;
        s_InfHearts = true;
    }
}

static int s_TraceSlots[16];
static int s_TraceCount;
static int s_TraceEvery = 30;
static int s_TraceFrom;

static void TraceEntities(void) {
    Entity* e;
    int i;

    s32 relics = 0, flags = 0, map = 0, events = 0;

    if (!s_TraceCount || s_Frame < s_TraceFrom || s_Frame % s_TraceEvery) {
        return;
    }
    // counts that a save and load must keep
    for (i = 0; i < LEN(g_Status.relics); i++) {
        relics += (g_Status.relics[i] & RELIC_FLAG_FOUND) != 0;
    }
    for (i = 0; i < LEN(g_CastleFlags); i++) {
        flags += g_CastleFlags[i] != 0;
    }
    for (i = 0; i < LEN(g_CastleMap); i++) {
        map += g_CastleMap[i] != 0;
    }
    for (i = 0; i < LEN(g_Settings.timeAttackRecords); i++) {
        events += g_Settings.timeAttackRecords[i] != 0;
    }
    fprintf(stderr,
            "TRACE f=%d state=%d step=%d/%d stage=%02X room=%d,%d hearts=%d "
            "player=%d,%d hp=%d mp=%d spells=%X status=%X gold=%d "
            "relics=%d flags=%d map=%d events=%d time=%d:%02d:%02d "
            "familiar=%d lv=%d exp=%d\n",
            s_Frame, g_GameState, g_GameStep, g_GameEngineStep, g_StageId,
            g_Tilemap.left, g_Tilemap.top, g_Status.hearts,
            g_Tilemap.scrollX.i.hi + PLAYER.posX.i.hi,
            g_Tilemap.scrollY.i.hi + PLAYER.posY.i.hi, g_Status.hp,
            g_Status.mp, g_Status.spellsLearnt, g_Player.status,
            g_Status.gold, relics, flags, map, events, g_Status.timerHours,
            g_Status.timerMinutes, g_Status.timerSeconds, g_Servant,
            g_Servant > 0 ? g_Status.statsFamiliars[g_Servant - 1].level : 0,
            g_Servant > 0 ? g_Status.statsFamiliars[g_Servant - 1].exp : 0);
    for (i = 0; i < s_TraceCount; i++) {
        e = &g_Entities[s_TraceSlots[i]];
        fprintf(stderr,
                "TRACE f=%d slot=%d id=%02X step=%d/%d pos=%d,%d vel=%X,%X "
                "anim=%d/%d hp=%d flags=%08X\n",
                s_Frame, s_TraceSlots[i], e->entityId, e->step, e->step_s,
                e->posX.i.hi, e->posY.i.hi, e->velocityX, e->velocityY, e->pose,
                e->poseTimer, e->hitPoints, e->flags);
    }
}

// Play_Reset clears the castle flags after the gear is set up, so they are
// set on the first frame past it
static void ApplyCastleFlags(void) {
    const char* env = s_CastleFlags;
    char* end;
    long id;

    if (!env || g_GameState != Game_Play || g_GameStep == Play_Reset) {
        return;
    }
    while (env && *env) {
        id = strtol(env, &end, 0);
        if (id >= 0 && id < LEN(g_CastleFlags)) {
            g_CastleFlags[id] = 1;
        }
        env = *end == ',' ? end + 1 : NULL;
    }
    s_CastleFlags = NULL;
}

static void ApplyKill(void) {
    const char* p;
    char* end;
    Entity* e;
    long id;
    int i;

    for (p = s_Kills; p && *p; p = strchr(p, ',') ? strchr(p, ',') + 1 : NULL) {
        id = strtol(p, &end, 0);
        if (*end != ':' || atoi(end + 1) != s_Frame) {
            continue;
        }
        for (i = 0, e = g_Entities; i < LEN(g_Entities); i++, e++) {
            if (e->entityId == id) {
                e->flags |= FLAG_DEAD;
            }
        }
    }
}

bool FrameDump_IsScripted(void) {
    return getenv("SOTN_INPUT") || getenv("SOTN_EXIT_AFTER");
}

void FrameDump_OnFrame(void) {
    const char* env;

    if (!s_Init) {
        s_Init = true;
        s_ShotDir = getenv("SOTN_SHOT_DIR");
        if ((env = getenv("SOTN_SHOT_EVERY")) && atoi(env) > 0) {
            s_ShotEvery = atoi(env);
        }
        s_ShotInterp = (env = getenv("SOTN_SHOT_INTERP")) && atoi(env);
        s_ShotMmpx = (env = getenv("SOTN_SHOT_MMPX")) && atoi(env);
        if ((env = getenv("SOTN_EXIT_AFTER"))) {
            s_ExitAfter = atoi(env);
        }
        if ((env = getenv("SOTN_INPUT"))) {
            ParseInputScript(env);
        }
        if ((env = getenv("SOTN_GOD"))) {
            s_GodStr = atoi(env) > 0 ? atoi(env) : 1;
        }
        if ((env = getenv("SOTN_TRACE"))) {
            while (*env && s_TraceCount < LEN(s_TraceSlots)) {
                s_TraceSlots[s_TraceCount++] = strtol(env, (char**)&env, 0);
                if (*env == ',') {
                    env++;
                } else {
                    break;
                }
            }
        }
        if ((env = getenv("SOTN_TRACE_EVERY")) && atoi(env) > 0) {
            s_TraceEvery = atoi(env);
        }
        if ((env = getenv("SOTN_TRACE_FROM"))) {
            s_TraceFrom = atoi(env);
        }
        s_CastleFlags = getenv("SOTN_CASTLE_FLAGS");
        s_Kills = getenv("SOTN_KILL");
        if ((env = getenv("SOTN_SEEK"))) {
            if (*env == '@') {
                s_SeekSlot = strtol(env + 1, NULL, 0);
                if (s_SeekSlot >= LEN(g_Entities)) {
                    s_SeekSlot = -1;
                }
            } else {
                s_SeekId = strtol(env, NULL, 0);
            }
        }
        if ((env = getenv("SOTN_VRAM_AT"))) {
            s_VramAt = atoi(env);
        }
        if ((env = getenv("SOTN_FAST")) && atoi(env)) {
            Psyz_VideoSetVsyncMode(PSYZ_VSYNC_LIMITLESS);
        }
    }
    s_Frame++;
    ApplyInputScript();
    ApplyCastleFlags();
    ApplyKill();
    TraceEntities();
    if (s_InfHearts) {
        g_Status.hearts = g_Status.heartsMax;
    }
    if (s_GodStr) {
        // for testing fights to the end with a level 1 Alucard
        g_Status.hp = g_Status.hpMax;
        if (g_Status.mpMax < 99) {
            g_Status.mpMax = 99; // Soul Steal costs 50
        }
        g_Status.mp = g_Status.mpMax;
        if (s_GodStr > 1) {
            g_Status.statsBase[STAT_STR] = s_GodStr;
        }
    }
    if (s_Frame == s_VramAt) {
        DumpVram();
    }
    if (s_ShotDir && s_Frame % s_ShotEvery == 0) {
        DumpDisplay();
        if (s_ShotInterp) {
            DumpInterp();
        }
        if (s_ShotMmpx) {
            DumpMmpx();
        }
    }
    if (s_ExitAfter > 0 && s_Frame >= s_ExitAfter) {
        g_IsQuitRequested = true;
    }
}
