#include <SDL2/SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include "area.h"
#include "player.h"
#include "../video.h"
#include "../log.h"

enum ELE_AreaConstants {
    MAX_AREA_VERTEX_CNT = 360
};

Area* ELE_CreateArea(
    int id, Player *conqueror, int capacity, int troop_cnt, int troop_rate,
    SDL_Point center, int radius, SDL_Point *vertices, int vertex_cnt
) {
    if (vertex_cnt > MAX_AREA_VERTEX_CNT) {
        LogInfo("Area vertex_cnt too much");
        return NULL;
    }
    Area *new_area = malloc(sizeof(Area));
    new_area->id = id;
    new_area->conqueror = conqueror;
    new_area->capacity = capacity;
    new_area->troop_cnt = troop_cnt;
    new_area->center = center;
    new_area->radius = radius;

    new_area->vertices = malloc(sizeof(SDL_Point) * vertex_cnt);
    memcpy(new_area->vertices, vertices, sizeof(SDL_Point) * vertex_cnt);
    new_area->vertex_cnt = vertex_cnt;
    return new_area;
}

void ELE_DestroyArea(Area *area) {
    free(area->vertices);
    free(area);
}

void ELE_ColorArea(
    Area *area,
    SDL_Color border_color, SDL_Color fill_color,
    int border_size
) {
    Sint16 *vertices_x = malloc(sizeof(Sint16) * area->vertex_cnt);
    Sint16 *vertices_y = malloc(sizeof(Sint16) * area->vertex_cnt);
    for (int i = 0; i < area->vertex_cnt; i++) {
        vertices_x[i] = area->vertices[i].x;
        vertices_y[i] = area->vertices[i].y;
    }
    SDL_Renderer *renderer = VDO_GetRenderer();
    /* Border color */
    filledPolygonRGBA(
        renderer,
        vertices_x,
        vertices_y,
        area->vertex_cnt,
        border_color.r,
        border_color.g,
        border_color.b,
        border_color.a
    );
    
    /* Exclude border size */
    for (int i = 0; i < area->vertex_cnt; i++) {
        int dx = area->vertices[i].x - area->center.x;
        int dy = area->vertices[i].y - area->center.y;
        double tan = 1.0 * dy / dx;
        double theta = SDL_atan(tan);
        double sin = SDL_sin(theta);
        double cos = SDL_cos(theta);

        vertices_x[i] += (dx > 0 ?
                        -border_size * cos :
                        border_size * cos);
        vertices_y[i] += (dy > 0 ?
                        -border_size * sin :
                        border_size * sin);
    }
    /* Fill color */
    filledPolygonRGBA(
        renderer,
        vertices_x,
        vertices_y,
        area->vertex_cnt,
        fill_color.r,
        fill_color.g,
        fill_color.b,
        fill_color.a
    );

    free(vertices_x);
    free(vertices_y);
}

int ELE_GetAreaCapacityByRadius(int radius) {
    const int NORMAL_RADIUS = 75;
    return round(2.0 * radius / NORMAL_RADIUS) * 25;
}