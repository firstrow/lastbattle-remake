#include "course.h"

#include "figure/figure.h"
#include "figure/movement.h"
#include "game/fog.h"
#include "game/game.h"
#include "handle/handle.h"
#include "map/map.h"
#include "screen/screen.h"
#include "input/keyboard.h"
#include "time/time.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

bool active = false;
int day[3] = {};
figure *unit = NULL;
time_millis last_update = 0;
time_millis speed = 500;
vec2 cursor;

static void callback() {
    if (input_key(KEY_CANCEL)) {
        handle_pop_callback();
        screen_move_to(unit->pos);
        active = false;
        game_player_get()->cursor = unit->pos;
        game_set_state_selected_figure(unit);
    }

    if (input_key(KEY_UP))
        screen_camera_up();

    if (input_key(KEY_DOWN))
        screen_camera_down();

    if (input_key(KEY_LEFT))
        screen_camera_left();

    if (input_key(KEY_RIGHT))
        screen_camera_right();

    if (input_key(KEY_ACTION)) {
        active = false;
        handle_pop_callback();
        screen_move_to(unit->pos);
        game_set_state_selected_figure(unit);

        unit->course = game_player_get()->cursor;
        game_player_get()->cursor = unit->pos;
    }
}

static bool enemy_check(vec2 p) {
    vec2 neighbors[8] = {
        (vec2){.x=p.x-1, .y=p.y},
        (vec2){.x=p.x+1, .y=p.y},
        (vec2){.x=p.x,   .y=p.y-1},
        (vec2){.x=p.x,   .y=p.y+1},
        (vec2){.x=p.x-1, .y=p.y-1},
        (vec2){.x=p.x+1, .y=p.y-1},
        (vec2){.x=p.x-1, .y=p.y+1},
        (vec2){.x=p.x+1, .y=p.y+1},
    };

    for (int i = 0; i < 8; i++) {
        figure *f = figure_at(neighbors[i]);
        if (!f)
            continue;
        if (f->player_id != game_player_get()->id) {
            return true;
        }
    }

    return false;
}

static void move(figure *f) {
    if (time_get_millis() - last_update < speed)
        return;

    last_update = time_get_millis();

    if (f->action_points == 0)
        return;

    vec2 next_tile = {.x=f->pos.x, .y=f->pos.y};

    if (f->course.x > f->pos.x)
        next_tile.x++;
    if (f->course.x < f->pos.x)
        next_tile.x--;
    if (f->course.y > f->pos.y)
        next_tile.y++;
    if (f->course.y < f->pos.y)
        next_tile.y--;

    figure *unit_at_dest = figure_at(next_tile);
    bool can_move = figure_movement_can_move(f, next_tile);

    if (!can_move || unit_at_dest) {
        game_course_cancel(f);
        return;
    }

    f->pos = next_tile;
    f->action_points -= 1;
    f->stored_in_id = 0;
    figure_movement_update(f, next_tile);

    if (f->pos.x == f->course.x && f->pos.y == f->course.y)
        game_course_cancel(f);
}

void game_course_update() {
    int player_id = game_player_get()->id;
    if (day[player_id] == game_state_get()->day)
        return;

    cursor = game_player_get()->cursor;

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (!f->type)
            continue;
        if (f->player_id != player_id)
            continue;
        if (f->type == FIGURE_CITY)
            continue;
        if (f->course.x == 0 && f->course.y == 0)
            continue;
        if (f->action_points == 0)
            continue;

        move(f);
        screen_move_to(f->pos);
        /* game_player_get()->cursor = f->pos; */
        if (enemy_check(f->pos)) {
            game_course_cancel(f);
        }

        return;
    }

    day[player_id] = game_state_get()->day;
    game_player_get()->cursor = cursor;
    screen_move_to(cursor);
}

void game_course_move() {
    figure *f = game_selected_figure();
    require_fig(f);
    int player_id = game_player_get()->id;

    if (!f->type)
        return;
    if (f->type == FIGURE_CITY)
        return;
    if (f->player_id != player_id)
        return;
    if (f->course.x == 0 && f->course.y == 0)
        return;
    if (f->action_points == 0)
        return;
    move(f);
    game_player_get()->cursor = f->pos;
    if (enemy_check(f->pos))
        game_course_cancel(f);
    if (f->action_points == 0)
        game_set_state_selected_figure(NULL);
}

void game_course_set(figure *f) {
    if (active)
        return;

    require_fig(f);

    active = true;
    unit = f;
    handle_push_callback(&callback);
}

void game_course_cancel(figure *f) {
    f->course.x = 0;
    f->course.y = 0;
}
