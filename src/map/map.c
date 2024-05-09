#include "map.h"
#include "types/types.h"
#include <SDL2/SDL_stdinc.h>
#include <stdbool.h>

static map_tile_data map_data[MAP_HEIGHT][MAP_WIDTH];

void map_reset() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map_data[y][x] = (map_tile_data){.tileset_idx=0, .terrain_type=MAP_TILE_WATER};
        }
    }
}

const map_tile_data *map_get(int x, int y) {
    return &map_data[y][x];
}

void map_set_tile_type(int type, int x, int y) {
    map_data[y][x].terrain_type = type;
}

void map_set_tileset_idx(int idx, int x, int y) {
    map_data[y][x].tileset_idx = idx;
}

void map_set_terrain_type(int type, int x, int y) {
    map_data[y][x].terrain_type = type;
}

bool map_check_bounds(vec2 pos) {
    if (pos.x < 0 || pos.y < 0)
        return false;

    if (pos.x > MAP_WIDTH - 1 || pos.y > MAP_HEIGHT - 1)
        return false;

    return true;
}
