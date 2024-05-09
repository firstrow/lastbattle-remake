#ifndef GAME_H_
#define GAME_H_

#include "figure/figure.h"
#include "game/log.h"
#include "types/types.h"
#include <stdbool.h>

typedef struct {
    int id;
    int camera_x;
    int camera_y;
    vec2 cursor;
    figure *selected_figure;
} game_player;

typedef struct {
    int current_player;
    int day;
    game_player player[3];
} game_state;

void game_init();
void game_run();
void game_draw();
void game_adavance_day();
void game_set_player(int p);
void game_set_state_selected_figure(figure *f);
void game_tile_selected(vec2 tile);
void game_select_next_unit(bool goback, int stored_in_id);
game_state *game_state_get();
game_player *game_player_get();
figure *game_selected_figure();

#endif // GAME_H_
