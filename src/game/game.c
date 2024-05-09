#include "game/game.h"

#include <SDL2/SDL.h>

#include "game/course.h"
#include "game/fog.h"
#include "game/hud.h"
#include "game/inter_screen.h"
#include "game/log.h"
#include "map/map.h"
#include "animation/animation.h"
#include "screen/screen.h"
#include "screen/draw.h"
#include "figure/figure.h"
#include "figure/movement.h"
#include "figure/combat.h"
#include "types/types.h"
#include "ui/ui.h"
#include "ui/notify.h"

static game_state state;

game_state* game_state_get() {
    return &state;
}

game_player *game_player_get() {
    return &state.player[state.current_player];
}

figure *game_selected_figure() {
    return game_player_get()->selected_figure;
}

void game_init() {
    state.current_player = PLAYER_ONE;
    state.day = 1;
    state.player[PLAYER_ONE] = (game_player){.id = 1, .selected_figure = NULL};
    state.player[PLAYER_TWO] = (game_player){.id = 2, .selected_figure = NULL};
}

static void deselect_figures() {
    const figure *sf = game_selected_figure();
    require_fig(sf);

    // what?
    figure *f = figure_get(sf->id);
    if (!f->type)
        game_set_state_selected_figure(NULL);
}

static void draw_figures() {
    const figure *sf = game_selected_figure();

    animation_t *cursor_selected = animation_get(ANIMATION_CURSOR_SELECTED);
    animation_t *black_tile = animation_get(ANIMATION_BLACK_TILE);

    // units
    animation_t *anim;
    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (!f->type)
            continue;
        if (!game_fog_can_see(f->pos))
            continue;
        if (f->stored_in_id > 0)
            continue;

        if (f->type == FIGURE_SHIP) {
            const map_tile_data *tile = map_get(f->pos.x, f->pos.y);
            if (tile->terrain_type == MAP_TILE_SHORE) {
                screen_draw_on_tile(black_tile, black_tile->index, f->pos);
            }
        }

        if (f->type == FIGURE_CITY && f->player_id > 0) {
            if (figure_stored_units(f) > 0)
                f->animation_id = ANIMATION_CITY;
            else
                f->animation_id = ANIMATION_CITY_EMPTY;
        }
        anim = animation_get(f->animation_id);

        screen_draw_on_tile(
            anim,
            animation_colorize(anim, f->player_id),
            f->pos
        );

        if (sf != NULL && f->id == sf->id)
            screen_draw_on_tile(cursor_selected, cursor_selected->index, f->pos);
    }

    // leaves city or ship
    if (sf && sf->stored_in_id > 0) {
        screen_draw_on_tile(black_tile, black_tile->index, sf->pos);
        animation_t *anim = animation_get(sf->animation_id);
        screen_draw_on_tile(
            anim,
            animation_colorize(anim, sf->player_id),
            sf->pos
        );
    }
}

static void draw_map() {
    // draw map tiles
    animation_t *water = animation_get(ANIMATION_WATER);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            screen_draw_on_tile(water, map_get(x, y)->tileset_idx,(vec2){.x=x, .y=y});
        }
    }
}

static void draw_cursor(){
    animation_t *selected_tile = animation_get(ANIMATION_SELECTED_TILE);
    animation_t *cursor_selected = animation_get(ANIMATION_CURSOR_SELECTED);
    const figure *sf = game_selected_figure();
    if (sf == NULL)
        screen_draw_on_tile(selected_tile, selected_tile->index, game_player_get()->cursor);
    else
        screen_draw_on_tile(cursor_selected, cursor_selected->index, game_player_get()->cursor);
}

void game_set_state_selected_figure(figure *f) {
    game_player_get()->selected_figure = f;
}

void game_select_next_unit(bool goback, int stored_in_id) {
    int i = 0;
    int steps = 0;

    figure *sf = game_selected_figure();
    if (sf != NULL && sf->type) {
        i = sf->id;
    }

    figure *f;
    while(steps<=MAX_FIGURES) {
        steps++;

        if (goback) {
            i--;
            if (i <= 0)
                i=MAX_FIGURES-1;
        } else {
            i++;
            if (i>=MAX_FIGURES)
                i=0;
        }

        f = figure_get(i);
        if (!f->type)
            continue;

        if (!stored_in_id) {
            if (f->stored_in_id)
                continue;
        } else {
            // leave window case
            if (f->stored_in_id != stored_in_id)
                continue;
        }

        if (f->player_id == game_state_get()->current_player) {
            screen_move_to(f->pos);
            game_player_get()->cursor = f->pos;
            return game_set_state_selected_figure(f);
        }
    }
}

void game_tile_selected(vec2 tile) {
    figure *f = figure_at(tile);
    figure *sf = game_selected_figure();

    if (sf == NULL) {
        if (f != NULL && f->player_id == game_state_get()->current_player) {
            game_set_state_selected_figure(f);
            if (f->type == FIGURE_CITY)
                game_hud_menu_show();
        }
    } else {
        if (tile.x == sf->pos.x && tile.y == sf->pos.y) {
            game_hud_menu_show();
        }

        if (sf->pos.x != tile.x || sf->pos.y != tile.y) {
            figure_movement_move(sf, tile);
            if (sf->action_points == 0)
                game_set_state_selected_figure(NULL);
        }
    }
}

void game_adavance_day() {
    state.day++;
}

void game_set_player(int p) {
    state.current_player = p;
}

void game_run() {
    if (!game_inter_screen_is_visible()) {
        deselect_figures();
        game_course_update();
        game_course_move();
        figure_combat_update();
    }
}

void game_draw() {
    screen_clear();

    if (game_inter_screen_is_visible()) {
        game_inter_screen_draw();
    } else {
        draw_map();
        draw_figures();
        game_fog_draw();
        draw_cursor();
        game_hud_draw();
    }

    ui_notify_draw();

    screen_present();
}

