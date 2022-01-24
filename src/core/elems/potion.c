#include <SDL2/SDL.h>
#include <stdlib.h>
#include "potion.h"

struct Potion {
    int id;
    int type;
    int seconds;

    SDL_Point center;
};

Potion* ELE_CreatePotion(
    int id, int type, int seconds,
    SDL_Point center, SDL_Texture *icon
) {
    Potion *new_potion = malloc(sizeof(Potion));
    new_potion->id = id;
    new_potion->type = type;
    new_potion->seconds = seconds;
    new_potion->center = center;
    return new_potion;
}

void ELE_DestroyPotion(Potion *potion) {
    free(potion);
}