#include <SDL2/SDL.h>
#include "video.h"
#include "log.h"

const int DEFAULT_WINDOW_W = 1024;
const int DEFAULT_WINDOW_H = 768;

const char *g_title = "state.io";

SDL_Window *g_Window = NULL;

SDL_bool VDO_Init() {
    Uint32 flags = SDL_WINDOW_SHOWN;
    g_Window = SDL_CreateWindow(
        g_title,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        DEFAULT_WINDOW_W,
        DEFAULT_WINDOW_H,
        flags
    );

    if (g_Window == NULL) {
        LogError("Unable to create window: %s");
        return SDL_FALSE;
    }

    int w, h;
    SDL_GetWindowSize(g_Window, &w, &h);
    LogInfo("Window %dx%d", w, h);

    return SDL_TRUE;
}

void VDO_Quit() {
    SDL_DestroyWindow(g_Window);
    g_Window = NULL;
}

SDL_Window * VDO_GetWindow() {
    return g_Window;
}

void VDO_GetWindowSize(int *w, int *h) {
    SDL_GetWindowSize(g_Window, w, h);
}