#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include "game.h"
#include "video.h"
#include "log.h"
#include "elems/player.h"
#include "elems/area.h"
#include "elems/potion.h"
#include "elems/map.h"

#define RGBAColor(color) color.r, color.g, color.b, color.a

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
    if (TTF_Init() != 0) {
        return -1;
    }
    flags = IMG_INIT_PNG;
    if (IMG_Init(flags) == 0) {
        LogInfo("IMG_Error: %s", IMG_GetError());
        return -1;
    }
    return 0;
}

void GME_Quit() {
    LogInfo("Gracefully quitting game");
    VDO_Quit();
    SDL_Quit();
}

int GME_Start() {
    if (GME_RetrievePlayers() != 0) {
        return -1;
    }
    switch (GME_GetCurPlayer()) {
        case 1:
            return 0;
        case -1:
            return -1;
    }
    Player **players = GME_GetPlayers();
    Player player_arr[MAX_PLAYER_CNT] = {0};
    for (int i = 0; i < MAX_PLAYER_CNT; i++) {
        if (players[i] != NULL) {
            player_arr[i] = *players[i];
        } else {
            break;
        }
    }
    ELE_SavePlayers(player_arr, GME_GetPlayerCnt());
    GME_MapStart(0);
    return 0;
}

const SDL_Color g_BackgroundColor = (SDL_Color){229, 229, 229, 255};
const SDL_Color g_NullAreaColor = (SDL_Color){147, 147, 147, 255};
const SDL_Color g_BlackColor = (SDL_Color){0, 0, 0, 255};
const SDL_Color g_LightBlackColor = (SDL_Color){50, 50, 50, 255};
const SDL_Color g_WhiteColor = (SDL_Color){255, 255, 255, 255};
const SDL_Color g_BlueColor = (SDL_Color){0, 120, 230, 255};
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

Player *g_CurPlayer;

Map *g_CurMap;
int map_cnt = 0;

Player** GME_GetPlayers() {
    return g_Players;
}

int GME_GetPlayerCnt() {
    int player_cnt = MAX_PLAYER_CNT;
    for (int i = 0; i < MAX_PLAYER_CNT; i++) {
        if (g_Players[i] == NULL) {
            player_cnt = i;
            break;
        }
    }
    return player_cnt;
}

int GME_WriteTTF(SDL_Renderer *renderer, TTF_Font *font, const char *s, SDL_Color color,
        int x, int y) {
    SDL_Surface *text = TTF_RenderText_Solid(font, s, color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text);
    SDL_Rect obox = {x - text->w / 2, y - text->h / 2, text->w, text->h};
    SDL_Rect ibox = {0, 0, text->w, text->h};
    if (text == NULL || text_texture == NULL) {
        LogInfo("TTF_Error: %s", TTF_GetError());
        return -1;
    }
    SDL_RenderCopy(renderer, text_texture, &ibox, &obox);
    SDL_FreeSurface(text);
    SDL_DestroyTexture(text_texture);
    return 0;
}

int GME_GetCurPlayer() {
    g_CurPlayer = NULL;
    int w, h;
    VDO_GetWindowSize(&w, &h);
    SDL_Renderer *renderer = VDO_GetRenderer();
    char name[40] = " ";
    int name_ptr = 0;
    int quit = 0;
    SDL_Event e;
    SDL_Color text_color = g_NullAreaColor;
    SDL_StartTextInput();
    TTF_Font *font = TTF_OpenFont("bin/fonts/SourceCodePro.ttf", 24);
    SDL_Surface *icon = IMG_Load("bin/images/icon.png");
    if (icon == NULL) {
        SDL_FreeSurface(icon);
        LogInfo("IMG_Error: %s", IMG_GetError());
        return -1;
    }
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && name_ptr > 1) {
                    name[--name_ptr] = 0;
                } else if (e.key.keysym.sym == SDLK_RETURN) {
                    quit = 1;
                }
            } else if (e.type == SDL_TEXTINPUT) {
                if (name_ptr < 31)
                    strcat(name, e.text.text);
                name_ptr = strlen(name);
            } else if (e.type == SDL_QUIT) {
                return 1;
            }
        }
        boxRGBA(renderer, 0, 0, w, h, RGBAColor(g_BackgroundColor));
        int icon_w = SDL_min(icon->w, 200);
        int icon_h = SDL_min(icon->h, 200);
        SDL_Texture *icon_texture = SDL_CreateTextureFromSurface(renderer, icon);
        SDL_Rect icon_src = {0, 0, icon->w, icon->h};
        SDL_Rect icon_box = {w / 2 - icon_w / 2, h / 3 - icon_h / 2 - 50, icon_w, icon_h};
        SDL_RenderCopy(renderer, icon_texture, &icon_src, &icon_box);
        SDL_DestroyTexture(icon_texture);
        roundedBoxRGBA(renderer, w / 2 - 250, h / 2 - 20, w / 2 + 250, h / 2 + 20, 5,
            RGBAColor(g_NullAreaColor));
        if (GME_WriteTTF(renderer, font, name, g_WhiteColor, w / 2 - 5, h / 2) != 0) {
            return -1;
        }
        SDL_RenderPresent(renderer);
    }
    SDL_StopTextInput();
    SDL_FreeSurface(icon);
    TTF_CloseFont(font);
    int player_cnt = MAX_PLAYER_CNT;
    for (int i = 0; i < MAX_PLAYER_CNT; i++) {
        if (g_Players[i] != NULL && !strcmp(g_Players[i]->name, name + 1)) {
            g_CurPlayer = g_Players[i];
            break;
        }
        if (g_Players[i] == NULL) {
            player_cnt = i;
            break;
        }
    }
    if (g_CurPlayer == NULL) {
        g_CurPlayer = ELE_CreatePlayer(player_cnt, name + 1, g_PlayerColors[player_cnt], 0, 0);
        g_Players[player_cnt] = g_CurPlayer;
    }
    LogInfo("Player id %d", g_CurPlayer->id);
    return 0;
}

void GME_AreaConquer(Area *area, Player *player) {
    if (area->conqueror != NULL) {
        area->conqueror->area_cnt--;
    }
    player->area_cnt++;
    area->conqueror = player;
}

int GME_MapStart(Map *map) {
    if (map == NULL) {
        GME_BuildRandMap();
        Player *players[5];
        players[0] = g_Players[0];
        players[1] = g_Players[1];
        players[2] = g_Players[2];
        players[3] = g_Players[3];
        players[4] = g_CurPlayer;
        int area_cnt = MAX_AREA_CNT;
        for (int i = 0; i < MAX_AREA_CNT; i++) {
            if (g_Areas[i] == NULL) {
                area_cnt = i;
                break;
            }
        }
        g_CurMap = ELE_CreateMap(map_cnt, players, 5, g_Areas, area_cnt);
    } else {
        g_CurMap = map;
    }
    for (int i = 0; i < g_CurMap->player_cnt; i++) {
        int start_area = rand() % g_CurMap->area_cnt;
        for (int i = 0; i < 20; i++) {
            if (g_CurMap->areas[start_area]->conqueror == NULL) break;
            start_area = rand() % g_CurMap->area_cnt;
        }
        if (g_CurMap->areas[start_area]->conqueror != NULL) return -1;
        GME_AreaConquer(g_CurMap->areas[start_area], g_CurMap->players[i]);
    }
    LogInfo("Map id %d", g_CurMap->id);
    return GME_RenderGame();
}

void GME_MapQuit(Map *map) {
    ELE_DestroyMap(map);
}

int GME_RetrievePlayers() {
    memset(g_Players, 0, sizeof(g_Players));
    const char *players_filename = "bin/data/players.bin";
    SDL_RWops *players_file = SDL_RWFromFile(players_filename, "r+b");
    Player players[MAX_PLAYER_CNT] = {0};
    if (players_file == NULL) {
        LogInfo("Unable to open players data, going with the defaults");
        // Default Players
        g_Players[0] = ELE_CreatePlayer(0, "ArshiA", g_PlayerColors[0], 0, 0);
        g_Players[1] = ELE_CreatePlayer(1, "AArshiAA", g_PlayerColors[1], 0, 0);
        g_Players[2] = ELE_CreatePlayer(2, "AAArshiAAA", g_PlayerColors[2], 0, 0);
        g_Players[3] = ELE_CreatePlayer(3, "IArshiAI", g_PlayerColors[3], 0, 0);
        g_Players[4] = ELE_CreatePlayer(4, "IIArshiAII", g_PlayerColors[4], 0, 0);
    } else {
        SDL_RWread(players_file, players, sizeof(Player), MAX_PLAYER_CNT);
        SDL_RWclose(players_file);
        for (int i = 0; i < MAX_PLAYER_CNT; i++) {
            if (players[i].name[0] == 0) break;
            g_Players[i] = ELE_CreatePlayer(
                players[i].id, players[i].name, players[i].color,
                players[i].troop_rate, players[i].score);
        }
    }
    LogInfo("Player Retrieve Done");
    return 0;
}

int GME_RetrieveAreas(int new) {
    memset(g_Areas, 0, sizeof(g_Areas));
    const char *areas_filename = "bin/data/areas.bin";
    SDL_RWops *areas_file = (new ? NULL : SDL_RWFromFile(areas_filename, "r+b"));
    Area areas[MAX_AREA_CNT];
    if (areas_file == NULL || new) {
        LogInfo("Unable to open areas data");
        GME_BuildRandMap();
    } else {
        LogInfo("Read areas from file");
        SDL_RWread(areas_file, areas, sizeof(Area), MAX_AREA_CNT);
        SDL_RWclose(areas_file);
        for (int i = 0; i < MAX_AREA_CNT; i++) {
            g_Areas[i] = ELE_CreateArea(
                areas[i].id, areas[i].conqueror, areas[i].capacity,
                areas[i].troop_cnt, areas[i].troop_rate, areas[i].center, 
                areas[i].radius, areas[i].vertices, areas[i].vertex_cnt);
        }
    }
    LogInfo("Area Retrieve Done");
    return 0;
}

void GME_BuildRandMap() {
    memset(g_Areas, 0, sizeof(g_Areas));
    int w, h;
    VDO_GetWindowSize(&w, &h);
    int wsqcnt = 21;
    int sqsize = w / wsqcnt;
    int hsqcnt = h / sqsize;
    int min_radius = sqsize;
    int max_radius = 2 * sqsize - 15;
    int radius = (min_radius + max_radius) / 2;
    int amp = (max_radius - min_radius) / 2;
    int area_cnt = 0;
    double PI = acos(-1);
    for (int i = 2; i < wsqcnt - 2; i += 3) {
        for (int j = 2; j < hsqcnt - 2; j += 3) {
            if (rand() % 3 == 0) continue;
            int wst = i * sqsize;
            int hst = j * sqsize;
            SDL_Point center;
            center.x = rand() % sqsize + wst;
            center.y = rand() % sqsize + hst;
            int vertex_cnt = 360;
            SDL_Point vertices[vertex_cnt];
            /* Generate vertices */
            int wave_cnt = 30;
            double amps[wave_cnt];
            double phases[wave_cnt];
            for (int vi = 0; vi < wave_cnt; vi++) {
                amps[vi] = rand() * 1.0 / RAND_MAX / pow(vi + 1, 1.5) * 2 * amp;
                phases[vi] = rand() * 1.0 / RAND_MAX * 2 * PI;
            }
            for (int vi = 0; vi < vertex_cnt; vi++) {
                double alpha = 2 * PI * vi / vertex_cnt;
                double cur_radius = radius;
                for (int vj = 0; vj < wave_cnt; vj++) {
                    cur_radius += amps[vj] * cos((vj + 1) * alpha + phases[vj]);
                }
                vertices[vi].x = center.x + cos(alpha) * cur_radius;
                vertices[vi].y = center.y + sin(alpha) * cur_radius;
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
    LogInfo("Random Area Generation Done");
}

SDL_Color GME_ChangeAlpha(SDL_Color color, Uint8 alpha) {
    return (SDL_Color){color.r, color.g, color.b, alpha};
}

int GME_RenderGame() {
    int quit = 0;
    int w, h;
    VDO_GetWindowSize(&w, &h);
    SDL_Renderer *renderer = VDO_GetRenderer();
    SDL_Event e;
    Map *map = g_CurMap;
    Player *player = g_CurPlayer;
    Area **areas = map->areas;
    Area *selected = NULL;
    TTF_Font *font = TTF_OpenFont("bin/fonts/SourceCodeProBold.ttf", 18);
    if (font == NULL) return -1;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                for (int i = 0; i < map->area_cnt; i++) {
                    if (abs(x - areas[i]->center.x) + abs(y - areas[i]->center.y) < 15) {
                        if (selected == NULL && areas[i]->conqueror == player) {
                            selected = areas[i];
                        } else if (selected == areas[i]) {
                            selected = NULL;
                        } else {
                            // Attack
                        }
                        break;
                    }
                }
            }
        }
        boxRGBA(renderer, 0, 0, w, h, RGBAColor(g_BackgroundColor));
        for (int i = 0; i < map->area_cnt; i++) {
            ELE_ColorArea(areas[i], (areas[i] == selected ? g_BlueColor : g_BackgroundColor),
                (areas[i]->conqueror ? areas[i]->conqueror->color : g_NullAreaColor),
                (areas[i] == selected ? 5 : 2));
            filledCircleRGBA(renderer, areas[i]->center.x, areas[i]->center.y, 12,
                245, 245, 245, 255);
        }
        for (int i = 0; i < map->player_cnt; i++) {
            int x1 = w - 220, y1 = h - 90 - 80 * i;
            int x2 = w - 20, y2 = h - 20 - 80 * i;
            roundedBoxRGBA(renderer, x1, y1, x2, y2, 10,
                RGBAColor(GME_ChangeAlpha(g_NullAreaColor, 100)));
            if (GME_WriteTTF(renderer, font, map->players[i]->name, g_LightBlackColor,
                (x1 + x2) / 2, y1 + 20) != 0) {
                return -1;
            }
            int width = x2 - x1 - 10;
            width = 1.0 * width * map->players[i]->area_cnt / ELE_GetMapAreaCntSum(map);
            roundedBoxRGBA(renderer, x1 + 20, y2 - 22, x1 + 20 + width, y2 - 20, 2,
                RGBAColor(map->players[i]->color));
        }
        SDL_RenderPresent(renderer);
    }
    return 0;
}