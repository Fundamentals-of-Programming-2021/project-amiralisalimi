#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "../log.h"

enum ELE_PlayerConstants {
    MAX_NAME_LEN = 31,
    DEFAULT_TROOP_RATE = 30 /* Frame */
};

Player* ELE_CreatePlayer(int id, const char *name, SDL_Color color) {
    if (strlen(name) > MAX_NAME_LEN) {
        LogInfo("Player name too long");
        return NULL;
    }
    Player *new_player = malloc(sizeof(Player));
    new_player->id = id;
    new_player->name = strdup(name);
    new_player->score = 0;
    new_player->area_cnt = 0;
    new_player->color = color;
    new_player->troop_rate = DEFAULT_TROOP_RATE;
    return new_player;
}

void ELE_DestroyPlayer(Player *player) {
    free(player->name);
    free(player);
}