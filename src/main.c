#include "core/game.h"

int main() {
    if (GME_Init() != 0) {
        GME_Quit();
        return 1;
    }
    atexit(GME_Quit);
    SDL_Delay(5000);
    return 0;
}