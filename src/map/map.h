#ifndef MAP_H_
#define MAP_H_

#include "types/types.h"
#include "stdbool.h"

#define MAP_WIDTH 100
#define MAP_HEIGHT 60

enum {
    MAP_TILE_WATER = 1,
    MAP_TILE_GROUND = 2,
    MAP_TILE_SHORE = 3,
};

typedef struct {
    int tileset_idx;
    int terrain_type; // water, ground, shore
} map_tile_data;

void map_reset();
const map_tile_data *map_get(int x, int y);
void map_set_tile_type(int type, int x, int y);
void map_set_tileset_idx(int idx, int x, int y);
void map_set_terrain_type(int type, int x, int y);
bool map_check_bounds(vec2 pos);

#endif // MAP_H_
