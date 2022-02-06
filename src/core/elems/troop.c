#include <SDL2/SDL.h>
#include <stdlib.h>
#include "troop.h"
#include "../log.h"

Troop* ELE_CreateTroop(
    int id, Player *player, int x, int y,
    int sx, int sy, int dx, int dy
) {
    Troop *new_troop = malloc(sizeof(Troop));
    new_troop->id = id;
    new_troop->player = player;
    new_troop->x = x;
    new_troop->y = y;
    new_troop->sx = sx;
    new_troop->sy = sy;
    new_troop->dx = dx;
    new_troop->dy = dy;
    return new_troop;
}

void ELE_DestroyTroop(Troop *troop) {
    free(troop);
}