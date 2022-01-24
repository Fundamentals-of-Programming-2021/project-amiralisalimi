#ifndef _GAME_H
#define _GAME_H

#include <SDL2/SDL.h>

const SDL_Color g_PlayerColors[] = {
    (SDL_Color){.r = 130, .g = 200, .b = 245, .a = 255},
    (SDL_Color){.r =  80, .g = 215, .b = 185, .a = 255},
    (SDL_Color){.r =  75, .g = 115, .b = 215, .a = 255},
    (SDL_Color){.r = 255, .g = 130, .b = 115, .a = 255},
    (SDL_Color){.r = 225, .g = 140, .b = 210, .a = 255},
    (SDL_Color){.r = 245, .g =  65, .b =  40, .a = 255},
    (SDL_Color){.r = 250, .g = 180, .b =  60, .a = 255},
    (SDL_Color){.r = 240, .g = 165, .b = 220, .a = 255},
    (SDL_Color){.r = 235, .g = 165, .b = 160, .a = 255},
    (SDL_Color){.r = 100, .g =  60, .b =  20, .a = 255},
    (SDL_Color){.r = 160, .g = 130, .b =  95, .a = 255},
    (SDL_Color){.r =   0, .g = 100, .b =  50, .a = 255},
    (SDL_Color){.r =  80, .g = 185, .b = 100, .a = 255},
    (SDL_Color){.r = 215, .g = 220, .b =  30, .a = 255},
    (SDL_Color){.r =  75, .g =   0, .b = 206, .a = 255}
};

extern int GME_Init(void);
extern void GME_Quit(void);

#endif /* _GAME_H */