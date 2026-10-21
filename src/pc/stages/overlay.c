// SPDX-License-Identifier: AGPL-3.0-or-later
#include <game.h>
#include <psyz/module.h>
#include "overlay.h"
#include "../pc.h"
#include "../spawn_point.h"
#include "../autosave.h"
#include "../framedump.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

// the file name Psyz_ModuleOpen gives an overlay, for messages
#if defined(__APPLE__)
#define OVERLAY_EXT ".dylib"
#elif defined(_WIN32)
#define OVERLAY_EXT ".dll"
#else
#define OVERLAY_EXT ".so"
#endif

static const char* OverlayPath(const char* name, char* buf, int len) {
    (void)buf;
    (void)len;
    return name;
}

// Psyz_ModuleOpen calls the overlay's Psyz_ModuleStart, which fills 'o'
static bool LoadOverlay(PsyzModule* current, const char* name, void* o) {
    char path[160];
    if (*current) {
        Psyz_ModuleClose(*current);
        *current = 0;
    }
    *current = Psyz_ModuleOpen(OverlayPath(name, path, sizeof(path)), o);
    if (!*current) {
        ERRORF("failed to load overlay '%s'", name);
        return false;
    }
    INFOF("loaded overlay '%s'", name);
    return true;
}

// The previous stage is closed before the next one opens (so a stage
// entered again starts from fresh data), which leaves g_api.o pointing
// into unloaded code when the next one fails: say what is missing and quit
// rather than crash on the next call into it.
static void MissingStage(const char* name) {
    char msg[256];

    snprintf(
        msg, sizeof(msg),
        "The program for the area '%s' (%s" OVERLAY_EXT " next to the game) is "
        "missing or could not be loaded, so the game cannot go on.\n\n"
        "Rebuild it with ./build.sh.",
        name, name);
    ERRORF("%s", msg);
    // scripted test runs have nobody to click the dialog away
    if (!FrameDump_IsScripted()) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                 "Castlevania: Symphony of the Night", msg, NULL);
    }
    exit(EXIT_FAILURE);
}

static PsyzModule CurrentStageOverlay;
bool LoadStageOverlay(const char* name, Overlay* o) {
    if (CurrentStageOverlay) {
        // the cutscene ranges point into the overlay being unloaded
        CutscenePcAlloc(NULL, 0);
    }
    if (!LoadOverlay(&CurrentStageOverlay, name, o)) {
        MissingStage(name);
        return false;
    }
    Autosave_OnStageLoad();
    SpawnPoint_HookOverlay(o);
    return true;
}

// A familiar whose overlay is missing: nothing appears and the game goes on
static void NoServantInit(InitializeMode mode) {}
static void NoServantUpdate(Entity* self) { DestroyEntity(self); }

static PsyzModule CurrentServantOverlay;
bool LoadServantOverlay(const char* name, ServantDesc* o) {
    PfnEntityUpdate* update;
    int i;

    if (LoadOverlay(&CurrentServantOverlay, name, o)) {
        return true;
    }
    o->Init = NoServantInit;
    update = &o->Update;
    for (i = 0; i < (sizeof(*o) - sizeof(o->Init)) / sizeof(*update); i++) {
        update[i] = NoServantUpdate;
    }
    return false;
}

static PsyzModule CurrentWeaponOverlay[2];
bool LoadWeaponOverlay(const char* name, unsigned handId, Weapon* o) {
    if (handId >= LEN(CurrentWeaponOverlay)) {
        ERRORF("hand ID %d not valid", handId);
        return false;
    }
    return LoadOverlay(&CurrentWeaponOverlay[handId], name, o);
}
