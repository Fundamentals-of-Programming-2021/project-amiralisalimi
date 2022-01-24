#ifndef _POTION_H
#define _POTION_H

#include <SDL2/SDL.h>

struct Potion {
    int id;
    int type;
    int seconds;

    SDL_Point center;
};
typedef struct Potion Potion;

extern Potion* ELE_CreatePotion(
    int id, int type, int seconds,
    SDL_Point center, SDL_Texture *icon
);
extern void ELE_DestroyPotion(Potion *potion);

#endif /* _POTION_H */