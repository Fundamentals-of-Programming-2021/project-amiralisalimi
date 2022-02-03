#ifndef _AREA_H
#define _AREA_H

#include <SDL2/SDL.h>
#include "player.h"

struct Area {
    int id;
    Player *conqueror;
    int capacity;
    int troop_cnt;
    int troop_rate;

    SDL_Point center;
    int radius;
    SDL_Point *vertices;
    int vertex_cnt;
};
typedef struct Area Area;

extern Area* ELE_CreateArea(
    int id, Player *conqueror, int capacity, int troop_cnt, int troop_rate,
    SDL_Point center, int radius, SDL_Point *vertices, int vertex_cnt
);
extern void ELE_DestroyArea(Area *area);

extern void ELE_ColorArea(
    Area *area,
    SDL_Color border_color, SDL_Color fill_color,
    int border_size
);

extern int ELE_GetAreaCapacityByRadius(int radius);

#endif /* _AREA_H */