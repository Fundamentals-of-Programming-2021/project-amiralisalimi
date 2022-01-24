#ifndef _AREA_H
#define _AREA_H

#include <SDL2/SDL.h>
#include "player.h"

struct Area;
typedef struct Area Area;

extern Area* ELE_CreateArea(
    int id, Player *conqueror, int capacity, int troop_cnt,
    SDL_Point center, int radius, SDL_Point *vertices, int vertex_cnt
);
extern void ELE_DestroyArea(Area *area);

#endif /* _AREA_H */