#include <SDL2/SDL.h>
#include "video.h"
#include "log.h"

const int DEFAULT_WINDOW_W = 1024;
const int DEFAULT_WINDOW_H = 768;

const char *g_title = "state.io";

SDL_Window *g_Window = NULL;
SDL_DisplayMode *g_DisplayMode = NULL;

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
    
    if (SDL_GetCurrentDisplayMode(0, g_DisplayMode) != 0) {
        LogError("Unable to get display mode: %s");
        return SDL_FALSE;
    }
    return SDL_TRUE;
}

void VDO_Quit() {
    SDL_DestroyWindow(g_Window);
    g_Window = NULL;
}

SDL_Window * VDO_GetWindow() {
    return g_Window;
}

int VDO_GetWidth() {
    return g_DisplayMode->w;
}

int VDO_GetHeight() {
    return g_DisplayMode->h;
}