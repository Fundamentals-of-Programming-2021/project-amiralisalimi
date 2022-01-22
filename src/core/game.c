#include <SDL2/SDL.h>
#include "game.h"
#include "video.h"
#include "log.h"

int GME_Init() {
    Uint32 flags = SDL_INIT_VIDEO;
    if (SDL_Init(flags) != 0) {
        LogError("Unable to init sdl: %s");
        return -1;
    }
    if (VDO_Init() != 0) {
        return -1;
    }
    return 0;
}

void GME_Quit() {
    LogInfo("Gracefully quitting game");
    VDO_Quit();
    SDL_Quit();
}