#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "../log.h"

enum ELE_PlayerConstants {
    MAX_PLAYER_CNT = 15,
    MAX_NAME_LEN = 31
};

const SDL_Color g_PlayerColors[MAX_PLAYER_CNT] = {
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

int g_PlayerCnt = 0;

struct Player {
    int id;
    const char *name;
    int score;

    int area_cnt;
    SDL_Color color;
};

typedef struct Player Player;

Player* ELE_CreatePlayer(const char *s) {
    if (g_PlayerCnt >= MAX_PLAYER_CNT) {
        LogInfo("Players reached maximum");
        return NULL;
    }
    if (strlen(s) > MAX_NAME_LEN) {
        LogInfo("Player name too long");
        return NULL;
    }
    Player *new_player = malloc(sizeof(Player));
    new_player->id = g_PlayerCnt++;
    new_player->name = strdup(s);
    new_player->score = 0;
    new_player->area_cnt = 0;
    new_player->color = g_PlayerColors[new_player->id];
    ++g_PlayerCnt;
    return new_player;
}

void ELE_DestroyPlayer(Player *player) {
    free(player->name);
    free(player);
    --g_PlayerCnt;
}