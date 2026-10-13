// SPDX-License-Identifier: AGPL-3.0-or-later
// Applies the --window and --fullscreen command line options once psyz has
// created and shown its window. psyz owns the window, so it is looked up
// through SDL and fullscreen goes through psyz's own F4 toggle to keep its
// state in sync.
#include <SDL3/SDL.h>
#include "pc.h"

static SDL_Window* FindGameWindow(void) {
    SDL_Window* found = NULL;
    SDL_Window** windows;
    int count = 0;

    windows = SDL_GetWindows(&count);
    if (windows && count > 0) {
        found = windows[0];
    }
    SDL_free(windows);
    return found;
}

void WindowOpts_OnFrame(void) {
    static bool done;
    SDL_Window* window;
    SDL_Event event;

    if (done) {
        return;
    }
    if (!g_GameParams.windowWidth && !g_GameParams.fullscreen) {
        done = true;
        return;
    }
    window = FindGameWindow();
    if (!window || (SDL_GetWindowFlags(window) & SDL_WINDOW_HIDDEN)) {
        return; // not shown yet, try again next frame
    }
    done = true;
    if (g_GameParams.windowWidth) {
        SDL_SetWindowSize(
            window, g_GameParams.windowWidth, g_GameParams.windowHeight);
        SDL_SetWindowPosition(
            window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    if (g_GameParams.fullscreen) {
        SDL_zero(event);
        event.type = SDL_EVENT_KEY_DOWN;
        event.key.windowID = SDL_GetWindowID(window);
        event.key.scancode = SDL_SCANCODE_F4;
        event.key.down = true;
        SDL_PushEvent(&event);
    }
    if (g_GameParams.windowWidth) {
        int w, h;
        SDL_SyncWindow(window);
        SDL_GetWindowSize(window, &w, &h);
        INFOF("window resized to %dx%d", w, h);
    }
}
