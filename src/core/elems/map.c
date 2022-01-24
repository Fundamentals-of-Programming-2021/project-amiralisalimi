#include <SDL2/SDL.h>
#include <stdlib.h>
#include "map.h"
#include "player.h"
#include "area.h"
#include "potion.h"
#include "../log.h"

enum ELE_MapConstants {
    MAX_PLAYER_CNT = 15,
    MAX_AREA_CNT = 31
};

Map* ELE_CreateMap(
    int id, Player *players, int player_cnt,
    Area *areas, int area_cnt
) {
    if (player_cnt > MAX_PLAYER_CNT) {
        LogInfo("Players too much");
        return NULL;
    }
    if (area_cnt > MAX_AREA_CNT) {
        LogInfo("Areas too much");
        return NULL;
    }
    Map *new_map = malloc(sizeof(Map));
    new_map->id = id;
    new_map->players = malloc(sizeof(Player) * player_cnt);
    new_map->player_cnt = player_cnt;
    new_map->areas = malloc(sizeof(Area) * area_cnt);
    new_map->area_cnt = area_cnt;
    return new_map;
}

void ELE_DestroyMap(Map *map) {
    free(map->players);
    free(map->areas);
    free(map);
}