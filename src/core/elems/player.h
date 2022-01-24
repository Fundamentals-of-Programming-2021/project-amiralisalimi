#ifndef _PLAYER_H
#define _PLAYER_H

#include <SDL2/SDL.h>

struct Player {
    int id;
    char *name;
    int score;

    int area_cnt;
    SDL_Color color;
};
typedef struct Player Player;

extern Player* ELE_CreatePlayer(int id, const char *s);
extern void ELE_DestroyPlayer(Player *player);

#endif /* _PLAYER_H */