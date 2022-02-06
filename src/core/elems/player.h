#ifndef _PLAYER_H
#define _PLAYER_H

#include <SDL2/SDL.h>

struct Player {
    int id;
    char name[32];
    int score;

    int area_cnt;
    SDL_Color color;

    int troop_rate;
};
typedef struct Player Player;

extern Player* ELE_CreatePlayer(
    int id, const char *name, SDL_Color color, int troop_rate, int score);
extern void ELE_DestroyPlayer(Player *player);

extern int ELE_SavePlayers(Player *players, int player_cnt);

#endif /* _PLAYER_H */