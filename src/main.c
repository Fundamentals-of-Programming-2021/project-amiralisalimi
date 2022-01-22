#include "core/video.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    VDO_Init();
    SDL_Delay(5000);
    VDO_Quit();
    SDL_Quit();
    return 0;
}