#include "fog.h"
#include "animation/animation.h"
#include "game/game.h"
#include "map/map.h"
#include "figure/figure.h"
#include "screen/screen.h"
#include "types/types.h"

static fog_cell fog[3][MAP_HEIGHT][MAP_WIDTH];
static bool render;

void game_fog_update(int player_id) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            fog[player_id][y][x].visible = false;
        }
    }

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (!f->type)
            continue;
        if (f->player_id != player_id)
            continue;
        if (f->stored_in_id > 0)
            continue;

        vec2 p = f->pos;
        vec2 neighbors[9] = {
            (vec2){.x=p.x, .y=p.y},
            (vec2){.x=p.x-1, .y=p.y},
            (vec2){.x=p.x+1, .y=p.y},
            (vec2){.x=p.x,   .y=p.y-1},
            (vec2){.x=p.x,   .y=p.y+1},
            (vec2){.x=p.x-1, .y=p.y-1},
            (vec2){.x=p.x+1, .y=p.y-1},
            (vec2){.x=p.x-1, .y=p.y+1},
            (vec2){.x=p.x+1, .y=p.y+1},
        };

        fog[player_id][p.y][p.x].visible = true;
        fog[player_id][p.y][p.x].visited = true;

        int x, y;
        for (int i = 0; i < 9; i++) {
            if (!map_check_bounds(neighbors[i]))
                continue;

            x = neighbors[i].x;
            y = neighbors[i].y;
            fog[player_id][y][x].visible = true;
            fog[player_id][y][x].visited = true;

            figure *n = figure_at(neighbors[i]);
            if (n && n->player_id != player_id) {
                fog[player_id][y][x].player_id = n->player_id;
                fog[player_id][y][x].animation_id = n->animation_id;
            } else {
                fog[player_id][y][x].player_id = 0;
                fog[player_id][y][x].animation_id = 0;
            }
        }
    }
}

void game_fog_reset(int player_id) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            memset(&fog[player_id][y][x], 0, sizeof(fog_cell));
        }
    }
}

void game_fog_draw() {
    game_fog_update(PLAYER_ONE);
    game_fog_update(PLAYER_TWO);

    if (!render)
        return;

    animation_t *black_tile = animation_get(ANIMATION_BLACK_TILE);
    animation_t *fog_tile = animation_get(ANIMATION_FOG_TILE);

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            fog_cell c = fog[game_state_get()->current_player][y][x];

            if (!c.visible && !c.visited)
                screen_draw_on_tile(black_tile, black_tile->index, (vec2){.x=x, .y=y});

            if (!c.visible && c.visited) {
                if (c.animation_id > 0) {
                    animation_t *anim = animation_get(c.animation_id);
                    screen_draw_on_tile(anim, animation_colorize(anim, c.player_id), (vec2){.x=x, .y=y});
                }
                screen_draw_on_tile(fog_tile, fog_tile->index, (vec2){.x=x, .y=y});
            }
        }
    }
}

bool game_fog_can_see(vec2 where) {
    if (!render)
        return true;

    int x = where.x;
    int y = where.y;
    return fog[game_state_get()->current_player][y][x].visible;
}

fog_cell game_fog_get(int x, int y, int player_id) {
    return fog[player_id][y][x];
}

void game_fog_set(int x, int y, int player_id, fog_cell c) {
    fog[player_id][y][x] = c;
}

void game_fog_toggle() {
    render = !render;
}
