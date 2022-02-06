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
    int id, Player **players, int player_cnt,
    Area **areas, int area_cnt
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
    new_map->player_cnt = player_cnt;
    new_map->area_cnt = area_cnt;
    new_map->players = malloc(sizeof(Player*) * player_cnt);
    new_map->areas = malloc(sizeof(Area*) * area_cnt);
    memcpy(new_map->players, players, sizeof(Player*) * player_cnt);
    memcpy(new_map->areas, areas, sizeof(Area*) * area_cnt);
    return new_map;
}

void ELE_DestroyMap(Map *map) {
    for (int i = 0; i < map->area_cnt; i++) free(map->areas[i]);
    for (int i = 0; i < map->player_cnt; i++) free(map->players[i]);
    free(map->areas);
    free(map->players);
    free(map);
}

void ELE_SaveMap(Map *map) {
    
}

int ELE_GetMapAreaCntSum(Map *map) {
    int sum = 0;
    for (int i = 0; i < map->player_cnt; i++) {
        sum += map->players[i]->area_cnt;
    }
    return sum;
}