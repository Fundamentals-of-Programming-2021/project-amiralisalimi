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
    return GME_Menu();
}

const SDL_Color g_BackgroundColor = (SDL_Color){229, 229, 229, 255};
const SDL_Color g_GreyColor = (SDL_Color){147, 147, 147, 255};
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

int GME_Scoreboard() {
    int quit = 0;
    int sdl_quit = 0;
    int w, h;
    VDO_GetWindowSize(&w, &h);
    SDL_Event e;
    SDL_Renderer *renderer = VDO_GetRenderer();
    TTF_Font *font = TTF_OpenFont("bin/fonts/SourceCodePro.ttf", 24);
    int table_width = 500, table_height = 600;
    SDL_Rect table = {w / 2 - table_width / 2, h / 2 - table_height / 2, table_width, table_height};
    int back_btn_sz = 80;
    SDL_Rect back_btn = {30, h - 20 - back_btn_sz, back_btn_sz, back_btn_sz};
    int entry_margin = 15, entry_height = 20, name_width = 350, score_width = 120;
    int player_cnt = GME_GetPlayerCnt();
    Player **players = malloc(sizeof(Player*) * player_cnt);
    memcpy(players, g_Players, sizeof(Player*) * player_cnt);
    ELE_SortPlayersByScore(players, player_cnt);
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
                sdl_quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (back_btn.x <= x && x <= back_btn.x + back_btn.w &&
                    back_btn.y <= y && y <= back_btn.y + back_btn.h) {
                    quit = 1;
                    break;
                }
            }
        }
        boxRGBA(renderer, 0, 0, w, h, RGBAColor(g_BackgroundColor));
        roundedBoxRGBA(renderer, table.x, table.y, table.x + table.w, table.y + table.h, 10,
            RGBAColor(g_GreyColor));
        GME_WriteTTF(renderer, font, "Player", g_WhiteColor, table.x + entry_margin + name_width / 2,
            table.y + entry_margin + entry_height / 2);
        GME_WriteTTF(renderer, font, "Score", g_WhiteColor, table.x + table.w - entry_margin - score_width / 2,
            table.y + entry_margin + entry_height / 2);
        lineRGBA(renderer, table.x + entry_margin, table.y + 2 * entry_margin + entry_height,
            table.x + entry_margin + name_width - 4, table.y + 2 * entry_margin + entry_height,
            RGBAColor(g_WhiteColor));
        lineRGBA(renderer, table.x + table.w - entry_margin - score_width + 4,
            table.y + 2 * entry_margin + entry_height,
            table.x + table.w - entry_margin - 2, table.y + 2 * entry_margin + entry_height,
            RGBAColor(g_WhiteColor));
        for (int i = 0; i < player_cnt; i++) {
            Player *player = players[i];
            GME_WriteTTF(renderer, font, player->name, g_WhiteColor,
                table.x + entry_margin + name_width / 2,
                table.y + (i + 3) * entry_margin + (i + 1) * entry_height + entry_height / 2);
            char buffer[10];
            sprintf(buffer, "%d", player->score);
            GME_WriteTTF(renderer, font, buffer, g_WhiteColor,
                table.x + table.w - entry_margin - score_width / 2,
                table.y + (i + 3) * entry_margin + (i + 1) * entry_height + entry_height / 2);
        }
        roundedBoxRGBA(renderer, back_btn.x, back_btn.y, back_btn.x + back_btn.w, 
            back_btn.y + back_btn.h, 10, RGBAColor(g_GreyColor));
        SDL_RenderPresent(renderer);
    }
    free(players);
    if (sdl_quit) return 1;
    return 0;
}

int GME_Menu() {
    int quit = 0;
    int w, h;
    VDO_GetWindowSize(&w, &h);
    SDL_Event e;
    SDL_Renderer *renderer = VDO_GetRenderer();
    TTF_Font *font = TTF_OpenFont("bin/fonts/Aaargh.ttf", 32);
    int button_width = 400, button_height = 80, button_margin = 40;
    SDL_Rect new_game_btn = {w / 2 - button_width / 2, h / 2 - 3 * button_height / 2 - button_margin,
        button_width, button_height};
    SDL_Rect cont_game_btn = {w / 2 - button_width / 2, h / 2 - button_height / 2,
        button_width, button_height};
    SDL_Rect scoreboard_btn = {w / 2 - button_width / 2, h / 2 + button_height / 2 + button_margin,
        button_width, button_height};
    while (!quit) {
        int next = -1;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (new_game_btn.x <= x && x <= new_game_btn.x + new_game_btn.w &&
                    new_game_btn.y <= y && y <= new_game_btn.y + new_game_btn.h) {
                    next = 0;
                } else if (cont_game_btn.x <= x && x <= cont_game_btn.x + cont_game_btn.w &&
                    cont_game_btn.y <= y && y <= cont_game_btn.y + cont_game_btn.h) {
                    next = 1;
                } else if (scoreboard_btn.x <= x && x <= scoreboard_btn.x + scoreboard_btn.w &&
                    scoreboard_btn.y <= y && y <= scoreboard_btn.y + scoreboard_btn.h) {
                    next = 2;
                }
            }
        }
        if (next == 0) {
            if (GME_MapStart(0) == 1) {
                quit = 1;
            }
            GME_MapQuit(GME_GetCurMap());
        } else if (next == 1) {

        } else if (next == 2) {
            if (GME_Scoreboard() == 1) {
                quit = 1;
            }
        }
        boxRGBA(renderer, 0, 0, w, h, RGBAColor(g_BackgroundColor));
        roundedBoxRGBA(renderer, new_game_btn.x, new_game_btn.y,
            new_game_btn.x + new_game_btn.w, new_game_btn.y + new_game_btn.h,
            10, RGBAColor(g_GreyColor));
        GME_WriteTTF(renderer, font, "New Game", g_WhiteColor, new_game_btn.x + new_game_btn.w / 2,
            new_game_btn.y + new_game_btn.h / 2);
        roundedBoxRGBA(renderer, cont_game_btn.x, cont_game_btn.y,
            cont_game_btn.x + cont_game_btn.w, cont_game_btn.y + cont_game_btn.h,
            10, RGBAColor(g_GreyColor));
        GME_WriteTTF(renderer, font, "Continue Last Game", g_WhiteColor, cont_game_btn.x + cont_game_btn.w / 2,
            cont_game_btn.y + cont_game_btn.h / 2);
        roundedBoxRGBA(renderer, scoreboard_btn.x, scoreboard_btn.y,
            scoreboard_btn.x + scoreboard_btn.w, scoreboard_btn.y + scoreboard_btn.h,
            10, RGBAColor(g_GreyColor));
        GME_WriteTTF(renderer, font, "Scoreboard", g_WhiteColor, scoreboard_btn.x + scoreboard_btn.w / 2,
            scoreboard_btn.y + scoreboard_btn.h / 2);
        SDL_RenderPresent(renderer);
    }
    TTF_CloseFont(font);
    return 0;
}

Map* GME_GetCurMap() {
    return g_CurMap;
}

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
    SDL_Color text_color = g_GreyColor;
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
                } else if (e.key.keysym.sym == SDLK_RETURN && name_ptr > 1) {
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
            RGBAColor(g_GreyColor));
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
        if (player_cnt >= MAX_PLAYER_CNT) {
            LogInfo("Maximum number of players reached");
            return 1;
        }
        g_CurPlayer = ELE_CreatePlayer(player_cnt, name + 1, g_PlayerColors[player_cnt], 0);
        g_Players[player_cnt] = g_CurPlayer;
    }
    LogInfo("Player id %d", g_CurPlayer->id);
    return 0;
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
        ELE_AreaConquer(g_CurMap->areas[start_area], g_CurMap->players[i]);
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
        g_Players[0] = ELE_CreatePlayer(0, "ArshiA", g_PlayerColors[0], 0);
        g_Players[1] = ELE_CreatePlayer(1, "AArshiAA", g_PlayerColors[1], 0);
        g_Players[2] = ELE_CreatePlayer(2, "AAArshiAAA", g_PlayerColors[2], 0);
        g_Players[3] = ELE_CreatePlayer(3, "IArshiAI", g_PlayerColors[3], 0);
        g_Players[4] = ELE_CreatePlayer(4, "IIArshiAII", g_PlayerColors[4], 0);
    } else {
        SDL_RWread(players_file, players, sizeof(Player), MAX_PLAYER_CNT);
        SDL_RWclose(players_file);
        for (int i = 0; i < MAX_PLAYER_CNT; i++) {
            if (players[i].name[0] == 0) break;
            g_Players[i] = ELE_CreatePlayer(
                players[i].id, players[i].name, players[i].color,
                players[i].score);
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
    for (int i = 2; i < wsqcnt - 4; i += 3) {
        for (int j = 2; j < hsqcnt - 2; j += 3) {
            if (rand() % 5 == 0) continue;
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
                30,
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

void GME_Move(double x, double y, double size, int sx, int sy, int dx, int dy, double *nx, double *ny) {
    double PI = acos(-1), theta;
    if (sx != dx) {
        theta = atan(1.0 * (sy - dy) / (sx - dx));
    } else {
        theta = (sy < dy ? PI / 2 : -PI / 2);
    }
    if (sx > dx) theta += PI;
    double sinus = sin(theta);
    double cosinus = cos(theta);
    *nx = x + size * cosinus, *ny = y + size * sinus;
}

void GME_Move2(double x, double y, double size, double theta, double *nx, double *ny) {
    double sinus = sin(theta);
    double cosinus = cos(theta);
    *nx = x + size * cosinus, *ny = y + size * sinus;
}

int GME_RenderGame() {
    LogInfo("Start Render Game");
    int quit = 0;
    int w, h;
    VDO_GetWindowSize(&w, &h);
    SDL_Renderer *renderer = VDO_GetRenderer();
    SDL_Event e;
    int back_btn_sz = 80;
    SDL_Rect back_btn = {30, h - 20 - back_btn_sz, back_btn_sz, back_btn_sz};
    Map *map = g_CurMap;
    Player *player = g_CurPlayer;
    Area **areas = map->areas;
    Area *selected = NULL;
    TTF_Font *font = TTF_OpenFont("bin/fonts/SourceCodeProBold.ttf", 18);
    int troop_cnt = 0;
    int frame = 0;
    int sdl_quit = 0;
    Player *winner = NULL;
    while (!quit) {
        // Check Win
        int players = 0;
        for (int i = 0; i < map->player_cnt; i++) {
            if (map->players[i]->troop_cnt + map->players[i]->area_cnt != 0) {
                winner = map->players[i];
                ++players;
            }
        }
        if (players == 1) {
            break;
        } else {
            winner = NULL;
        }
        ++frame;
        if (selected != NULL && selected->conqueror != g_CurPlayer) selected = NULL;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
                sdl_quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (back_btn.x <= x && x <= back_btn.x + back_btn.w &&
                    back_btn.y <= y && y <= back_btn.y + back_btn.h) {
                    quit = 1;
                    break;
                }
                for (int i = 0; i < map->area_cnt; i++) {
                    if (abs(x - areas[i]->center.x) + abs(y - areas[i]->center.y) < 25) {
                        if (selected == NULL && areas[i]->conqueror == player) {
                            selected = areas[i];
                        } else if (selected == areas[i]) {
                            selected = NULL;
                        } else if (selected != NULL) {
                            selected->attack = areas[i];
                            selected = NULL;
                        }
                        break;
                    }
                }
            }
        }
        // Render Areas
        boxRGBA(renderer, 0, 0, w, h, RGBAColor(g_BackgroundColor));
        for (int i = 0; i < map->area_cnt; i++) {
            if (areas[i]->troop_inc_delay > 0) --areas[i]->troop_inc_delay;
            if (frame % areas[i]->troop_rate == 0 && areas[i]->troop_cnt < areas[i]->capacity
                && areas[i]->attack == NULL && areas[i]->conqueror != NULL && areas[i]->troop_inc_delay == 0) {
                ++areas[i]->troop_cnt;
            }
            if (areas[i]->troop_cnt == 0) areas[i]->attack = NULL;
            if (areas[i]->attack != NULL) {
                if (areas[i]->attack_delay) --areas[i]->attack_delay;
                else {
                    for (int it = 0; it < 5; it++) {
                        if (areas[i]->troop_cnt == 0) {
                            areas[i]->attack = NULL;
                            break;
                        }
                        areas[i]->troop_cnt--;
                        int sx = areas[i]->center.x, sy = areas[i]->center.y;
                        int dx = areas[i]->attack->center.x, dy = areas[i]->attack->center.y;
                        double x, y;
                        GME_Move(sx, sy, 10, sx, sy, dx, dy, &x, &y);
                        double vert_theta, PI = acos(-1);
                        if (sy != dy) {
                            vert_theta = SDL_atan(-1.0 * (sx - dx) / (sy - dy));
                        } else {
                            vert_theta = (sx > dx ? PI / 2 : -PI / 2);
                        }
                        if (sy < dy) vert_theta += PI;
                        if (it == 0) GME_Move2(x, y, 22, vert_theta, &x, &y);
                        if (it == 1) GME_Move2(x, y, 11, vert_theta, &x, &y);
                        if (it == 3) GME_Move2(x, y, 11, vert_theta + PI, &x, &y);
                        if (it == 4) GME_Move2(x, y, 22, vert_theta + PI, &x, &y);
                        Troop *troop = ELE_CreateTroop(troop_cnt++, areas[i]->conqueror, x, y, areas[i], areas[i]->attack, NULL, NULL);
                        ELE_AddTroopToMap(map, troop);
                    }
                    areas[i]->attack_delay = 25;
                }
            }

            ELE_ColorArea(areas[i], (areas[i] == selected ? g_BlueColor : g_BackgroundColor),
                (areas[i]->conqueror ? areas[i]->conqueror->color : g_GreyColor),
                (areas[i] == selected ? 5 : 2));
            filledCircleRGBA(renderer, areas[i]->center.x, areas[i]->center.y, 16,
                245, 245, 245, 255);
        }
        for (int i = 0; i < map->area_cnt; i++) {
            char buffer[5];
            sprintf(buffer, "%d", areas[i]->troop_cnt);
            GME_WriteTTF(renderer, font, buffer, g_BlackColor,
                areas[i]->center.x, areas[i]->center.y + 25);
        }
        // Render Player names
        for (int i = 0; i < map->player_cnt; i++) {
            int x1 = w - 220, y1 = h - 90 - 80 * i;
            int x2 = w - 20, y2 = h - 20 - 80 * i;
            roundedBoxRGBA(renderer, x1, y1, x2, y2, 10,
                RGBAColor(GME_ChangeAlpha(g_GreyColor, 100)));
            GME_WriteTTF(renderer, font, map->players[i]->name, g_LightBlackColor,
                (x1 + x2) / 2, y1 + 20);
            int width = x2 - x1 - 40;
            width = 1.0 * width * map->players[i]->area_cnt / ELE_GetMapAreaCntSum(map);
            roundedBoxRGBA(renderer, x1 + 20, y2 - 25, x1 + 20 + width, y2 - 20, 2,
                RGBAColor(map->players[i]->color));
        }
        // Render Troops
        for (Troop *troop = map->troops_head; troop != NULL; troop = troop->next) {
            GME_Move(troop->x, troop->y, 0.5, troop->sx, troop->sy, troop->dx, troop->dy, &troop->x, &troop->y);
            filledCircleRGBA(renderer, troop->x, troop->y, 6, RGBAColor(g_BackgroundColor));
            filledCircleRGBA(renderer, troop->x, troop->y, 5, RGBAColor(troop->player->color));
        }
        roundedBoxRGBA(renderer, back_btn.x, back_btn.y, back_btn.x + back_btn.w, 
            back_btn.y + back_btn.h, 10, RGBAColor(g_GreyColor));
        ELE_HandleCollisions(map);
        // AI
        for (int i = 0; i < map->player_cnt; i++) {
            if (map->players[i] == g_CurPlayer || map->players[i]->area_cnt == 0) continue;
            if (map->players[i]->attack_delay) {
                --map->players[i]->attack_delay;
                continue;
            }
            if (rand() % 240) continue;
            Player *player = map->players[i];
            int from = rand() % player->area_cnt;
            int to = rand() % map->area_cnt;
            Area *src = NULL, *dst = NULL;
            for (int i = 0; i < map->area_cnt; i++) {
                if (map->areas[i]->conqueror == player) {
                    if (from == 0) {
                        src = map->areas[i];
                        break;
                    } else {
                        --from;
                    }
                }
            }
            dst = map->areas[to];
            while (dst == src) {
                to = rand() % map->area_cnt;
                dst = map->areas[to];
            }
            src->attack = dst;
            player->attack_delay = 60;
        }
        SDL_RenderPresent(renderer);
    }
    TTF_CloseFont(font);
    if (sdl_quit) return 1;
    if (winner == NULL) return 0;
    for (int i = 0; i < map->player_cnt; i++) {
        if (map->players[i] == winner) {
            map->players[i]->score += map->player_cnt - 1;
        } else {
            map->players[i]->score -= 1;
        }
    }
    quit = 0;
    sdl_quit = 0;
    font = TTF_OpenFont("bin/fonts/SourceCodeProBold.ttf", 28);
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
                sdl_quit = 1;
            }
        }
        boxRGBA(renderer, 0, 0, w, h, RGBAColor(g_BackgroundColor));
        char buffer[50];
        sprintf(buffer, "%s won the game", winner->name);
        GME_WriteTTF(renderer, font, buffer, g_BlackColor, w / 2, h / 2);
        SDL_RenderPresent(renderer);
    }
    TTF_CloseFont(font);
    if (sdl_quit) return 1;
    return 0;
}