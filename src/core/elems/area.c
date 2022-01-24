#include <SDL2/SDL.h>
#include <stdlib.h>
#include "area.h"
#include "player.h"
#include "../log.h"

enum ELE_AreaConstants {
    MAX_AREA_CNT = 31,
    MAX_AREA_VERTEX_CNT = 360
};

struct Area {
    int id;
    Player *conqueror;
    int capacity;
    int troop_cnt;

    SDL_Point center;
    int radius;
    SDL_Point *vertices;
    int vertex_cnt;
};

typedef struct Area Area;

Area* ELE_CreateArea(
    int id, Player *conqueror, int capacity, int troop_cnt,
    SDL_Point center, int radius, SDL_Point *vertices, int vertex_cnt
) {
    if (id >= MAX_AREA_CNT) {
        LogInfo("Areas reached maximum");
        return NULL;
    }
    if (vertex_cnt > MAX_AREA_VERTEX_CNT) {
        LogInfo("Area vertex_cnt too much");
        return NULL;
    }
    Area *new_area = malloc(sizeof(Area));
    new_area->id = id;
    new_area->conqueror = conqueror;
    new_area->capacity = capacity;
    new_area->troop_cnt = troop_cnt;
    new_area->center = center;
    new_area->radius = radius;

    new_area->vertices = malloc(sizeof(SDL_Point) * vertex_cnt);
    memcpy(new_area->vertices, vertices, sizeof(SDL_Point) * vertex_cnt);
    new_area->vertex_cnt = vertex_cnt;
    return new_area;
}

void ELE_DestroyArea(Area *area) {
    free(area->vertices);
    free(area);
}