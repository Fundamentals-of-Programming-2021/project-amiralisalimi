#ifndef _TROOP_H
#define _TROOP_H

#include <SDL2/SDL.h>
#include "player.h"

struct Troop {
    int id;
    Player *player;
    int x, y;
    int sx, sy;
    int dx, dy;
};
typedef struct Troop Troop;

extern Troop* ELE_CreateTroop(
    int id, Player *player, int x, int y,
    int sx, int sy, int dx, int dy
);
extern void ELE_DestroyTroop(Troop *troop);

#endif /* _TROOP_H */