#ifndef _MAP_H
#define _MAP_H

#include "player.h"
#include "area.h"

struct Map {
    Player *players;
    int player_cnt;

    Area *areas;
    int area_cnt;
};
typedef struct Map Map;

extern Map* ELE_CreateMap(
    Player *players, int player_cnt,
    Area *areas, int area_cnt
);
extern void ELE_DestroyMap(Map *map);

#endif /* _MAP_H */