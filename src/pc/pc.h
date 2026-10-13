// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef PC_H
#define PC_H

#define LEN(x) ((s32)(sizeof(x) / sizeof(*(x))))

#ifndef VERSION_PC
#define VERSION_PC
#endif
#define VERSION_US
#ifdef VERSION
#undef VERSION
#endif
#define VERSION "us"

#include <stdio.h>
#include <types.h>
#include <psyz/log.h>
#include <psyz/types.h>

#define DISP_WIDTH 256
#define DISP_HEIGHT 256

#define VRAM_W 1024
#define VRAM_H 512
#define VRAM_STRIDE 2048

enum TestMode {
    NO_TEST,
    TEST_SNDLIB,
};
struct InitGameParams {
    const char* diskPath;
    enum TestMode testMode;
    int stage;
    int player;
    int demo;
    int scale;
    const char* recordPath;
    const char* replayPath;
    const char* spawnPoint;
    bool exitAfterReplay;
    bool replayBoundlessFramerate;
    int windowWidth;  // initial window size in points, 0 = psyz default
    int windowHeight;
    bool fullscreen;
    bool noInterp; // --no-interp: one presented frame per game frame
    int filter;    // --filter: a PsyzFilterMode (pixel-art magnification)
    bool noCrop;   // --no-crop: keep the black bars the game draws around
    bool scaleSet; // --scale was given
};

struct FileOpenRead {
    const char* filename;
    FILE* file;
    size_t length;
    void* param;
};

struct FileAsString {
    const char* filename;
    const char* content;
    size_t length;
    void* param;
};

typedef struct FileUseContent {
    const char* filename;
    void* content;
    size_t length;
    void* param;
} FileLoad;

extern struct InitGameParams g_GameParams;
bool InitGame(struct InitGameParams* params);
void MainGame(void);
void ResetGame(void);

// Controller input record/replay (src/pc/replay.c)
void Replay_Init(const struct InitGameParams* params);
void Replay_Reset(void);
void Replay_OnFrame(void);
bool Replay_DidDrift(void);
bool Replay_DemoAborted(void);

// --window / --fullscreen handling (src/pc/window_opts.c)
void WindowOpts_OnFrame(void);

bool FileOpenRead(
    bool (*cb)(const struct FileOpenRead*), const char* filename, void* param);
int FileReadToBuf(const char* filename, void* dst, int offset, size_t maxlen);
bool FileAsString(bool (*cb)(const struct FileAsString* file),
                  const char* filename, void* param);
bool FileUseContent(bool (*cb)(const struct FileUseContent* file, void* param),
                    const char* filename, void* param);

typedef struct {
    const void* pcAddr; // equivalent of symbol address on PC
    unsigned psxAddr;   // equivalent of symbol address on PSX
    unsigned size;      // symbol size in bytes
} CutsceneSymbolRange;

void CutscenePcAlloc(const CutsceneSymbolRange* symbols, int count);
u8* CutsceneAddrToPc(u32 psxAddr);

#endif
