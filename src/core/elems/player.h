#ifndef _PLAYER_H
#define _PLAYER_H

#include <SDL2/SDL.h>

struct Player;
typedef struct Player Player;

extern Player* ELE_CreatePlayer(const char *s);
extern void ELE_DestroyPlayer(Player *player);

#endif /* _PLAYER_H */