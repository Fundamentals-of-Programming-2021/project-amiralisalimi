#include <SDL2/SDL.h>
#include <time.h>
#include "game.h"
#include "video.h"
#include "log.h"
#include "elems/player.h"
#include "elems/area.h"
#include "elems/potion.h"
#include "elems/map.h"

enum GME_GameConstants {
    MAX_PLAYER_CNT = 15,
    MAX_AREA_CNT = 31
};

int GME_Init() {
    srand(time(NULL));
    Uint32 flags = SDL_INIT_VIDEO;
    if (SDL_Init(flags) != 0) {
        LogError("Unable to init sdl: %s");
        return -1;
    }
    if (VDO_Init() != 0) {
        return -1;
    }
    return 0;
}

void GME_Quit() {
    LogInfo("Gracefully quitting game");
    VDO_Quit();
    SDL_Quit();
}

void GME_Start() {
    GME_RetrievePlayers();
    GME_RetrieveAreas();
    GME_BuildRandMap();
}

const SDL_Color g_BackgroundColor = (SDL_Color){229, 229, 229, 255};
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

Player *g_Players[MAX_PLAYER_CNT];
Area *g_Areas[MAX_AREA_CNT];

void GME_RetrievePlayers() {
    memset(g_Players, 0, sizeof(g_Players));
    const char *players_filename = "../../bin/data/players.bin";
    SDL_RWops *players_file = SDL_RWFromFile(players_filename, "r+b");
    if (players_file == NULL) {
        LogInfo("Unable to open players data, going with the defaults");
        // Default Players
        g_Players[0] = ELE_CreatePlayer(0, "ArshiA", g_PlayerColors[0]);
        g_Players[1] = ELE_CreatePlayer(1, "ArshiA", g_PlayerColors[1]);
        g_Players[2] = ELE_CreatePlayer(2, "ArshiA", g_PlayerColors[2]);
        g_Players[3] = ELE_CreatePlayer(3, "ArshiA", g_PlayerColors[3]);
        g_Players[4] = ELE_CreatePlayer(4, "ArshiA", g_PlayerColors[4]);
        players_file = SDL_RWFromFile(players_filename, "w+b");
        if (players_file != NULL) {
            SDL_RWwrite(players_file, g_Players, sizeof(Player*), MAX_PLAYER_CNT);
            SDL_RWclose(players_file);
        } else {
            LogError("Unable to r/w players: %s");
        }
    } else {
        SDL_RWread(players_file, g_Players, sizeof(Player*), MAX_PLAYER_CNT);
        SDL_RWclose(players_file);
    }
}

void GME_RetrieveAreas() {
    const char *areas_filename = "../../bin/data/areas.bin";
    SDL_RWops *areas_file = SDL_RWFromFile(areas_filename, "r+b");
    if (areas_file == NULL) {
        LogInfo("Unable to open areas data");
        GME_BuildRandMap();
        areas_file = SDL_RWFromFile(areas_filename, "w+b");
        if (areas_file != NULL) {
            SDL_RWwrite(areas_file, g_Areas, sizeof(Area*), MAX_AREA_CNT);
            SDL_RWclose(areas_file);
        } else {
            LogError("Unable to r/w areas: %s");
        }
    } else {
        SDL_RWread(areas_file, g_Areas, sizeof(Area*), MAX_AREA_CNT);
        SDL_RWclose(areas_file);
    }
}

void GME_BuildRandMap() {
    memset(g_Areas, 0, sizeof(g_Areas));
    int w, h;
    VDO_GetWindowSize(&w, &h);
    int wsqcnt = 21;
    int sqsize = w / wsqcnt;
    int hsqcnt = h / sqsize;
    int min_radius = sqsize + 20;
    int max_radius = 2 * sqsize;
    int radius = (min_radius + max_radius) / 2;
    int amp = (max_radius - min_radius) / 2;
    int area_cnt = 0;
    double PI = acos(-1);
    for (int i = 1; i < wsqcnt - 1; i += 2) {
        for (int j = 1; j < hsqcnt - 1; j += 2) {
            int wst = i * sqsize;
            int hst = j * sqsize;
            SDL_Point center;
            center.x = rand() % sqsize + wst;
            center.y = rand() % sqsize + hst;
            int vertex_cnt = 360;
            SDL_Point vertices[vertex_cnt];
            /* Generate vertices */
            double amps[vertex_cnt];
            double phases[vertex_cnt];
            for (int vi = 0; vi < vertex_cnt; vi++) {
                amps[vi] = rand() * 1.0 / RAND_MAX / vertex_cnt / amp;
                phases[vi] = rand() * 1.0 / RAND_MAX * 2 * PI;
            }
            for (int vi = 0; vi < vertex_cnt; vi++) {
                double cur_radius = radius;
                for (int vj = 0; vj < vertex_cnt; vj++) {
                    cur_radius += amps[vj] * cos((vj + 1) * vi + phases[vj]);
                }
                vertices[vi].x = cos(vi / 180.0 * PI) * cur_radius;
                vertices[vi].y = cos(vi / 180.0 * PI) * cur_radius;
            }
            Area *area = ELE_CreateArea(
                area_cnt,
                NULL,
                ELE_GetAreaCapacityByRadius(radius),
                0,
                30,
                center,
                radius,
                vertices,
                vertex_cnt
            );
            g_Areas[area_cnt++] = area;
        }
    }
}