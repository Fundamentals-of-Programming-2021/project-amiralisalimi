#include <SDL2/SDL.h>
#include <stdlib.h>
#include "map.h"
#include "player.h"
#include "area.h"
#include "potion.h"
#include "../video.h"
#include "../log.h"

enum ELE_MapConstants {
    MAX_PLAYER_CNT = 15,
    MAX_AREA_CNT = 31,
    TROOP_RADIUS = 6
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
    new_map->troops_head = NULL;
    memcpy(new_map->players, players, sizeof(Player*) * player_cnt);
    memcpy(new_map->areas, areas, sizeof(Area*) * area_cnt);
    return new_map;
}

void ELE_DestroyMap(Map *map) {
    for (int i = 0; i < map->area_cnt; i++) ELE_DestroyArea(map->areas[i]);
    Troop *head;
    for (head = map->troops_head; head != NULL; head = head->next) {
        if (head->prev != NULL) {
            ELE_DestroyTroop(head->prev);
        }
    }
    if (head != NULL) ELE_DestroyTroop(head);
    for (int i = 0; i < map->player_cnt; i++) {
        map->players[i]->area_cnt = 0;
        map->players[i]->troop_cnt = 0;
        map->players[i]->attack_delay = 0;
    }
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

void ELE_AddTroopToMap(Map *map, Troop *troop) {
    if (troop == NULL) return;
    troop->next = troop->prev = NULL;
    if (map->troops_head == NULL) {
        map->troops_head = troop;
    } else {
        troop->next = map->troops_head;
        map->troops_head->prev = troop;
        map->troops_head = troop;
    }
}

Troop* ELE_RemoveTroopFromMap(Map *map, Troop *troop) {
    if (troop == NULL) return NULL;
    if (troop->prev != NULL)
        troop->prev->next = troop->next;
    if (troop->next != NULL)
        troop->next->prev = troop->prev;
    if (troop == map->troops_head)
        map->troops_head = troop->next;
    Troop *ret = troop->next;
    ELE_DestroyTroop(troop);
    return ret;
}

int ELE_Collide(Troop *first, Troop *second) {
    if (first == NULL || second == NULL) return 0;
    if (first->player == second->player) return 0;
    int x1 = first->x, y1 = first->y;
    int x2 = second->x, y2 = second->y;
    return (abs(x1 - x2) + abs(y1 - y2) < 2 * TROOP_RADIUS);
}

void ELE_HandleCollisions(Map *map) {
    int w, h;
    VDO_GetWindowSize(&w, &h);
    for (Troop *troop = map->troops_head; troop != NULL;) {
        if (troop->x < 0 || troop->y < 0 || troop->x > w || troop->y > h) {
            troop = ELE_RemoveTroopFromMap(map, troop);
        } else if (abs(troop->x - troop->dx) + abs(troop->y - troop->dy) < 40) {
            Area *src = troop->src, *dst = troop->dst;
            if (dst->conqueror == troop->player) {
                ++dst->troop_cnt;
            } else if (dst->troop_cnt == 0) {
                ++dst->troop_cnt;
                ELE_AreaConquer(dst, troop->player);
            } else {
                --dst->troop_cnt;
            }
            dst->troop_inc_delay = 10;
            troop = ELE_RemoveTroopFromMap(map, troop);
        } else {
            int found = 0;
            for (Troop *troop2 = troop->next; troop2 != NULL;) {
                if (ELE_Collide(troop, troop2)) {
                    troop2 = ELE_RemoveTroopFromMap(map, troop2);
                    found = 1;
                } else {
                    troop2 = troop2->next;
                }
            }
            if (found) {
                troop = ELE_RemoveTroopFromMap(map, troop);
            } else {
                troop = troop->next;
            }
        }
    }
}