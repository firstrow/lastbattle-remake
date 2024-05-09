#include "save.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "figure/figure.h"
#include "buffer/buffer.h"
#include "game/fog.h"
#include "game/inter_screen.h"
#include "map/map.h"
#include "game/game.h"
#include "screen/screen.h"
#include "types/types.h"

#include <SDL2/SDL_log.h>

int size = 250000;

static void save_figures(buffer *buf) {
    for (int i = 0; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);

        buffer_write_i32(buf, f->id);
        buffer_write_i32(buf, f->player_id);
        buffer_write_raw(buf, f->name, 12);
        buffer_write_i32(buf, f->animation_id);
        buffer_write_i32(buf, f->type);
        buffer_write_i32(buf, f->unit);
        buffer_write_i32(buf, f->max_action_points);
        buffer_write_i32(buf, f->max_shield);
        buffer_write_i32(buf, f->days_to_make);
        buffer_write_i32(buf, f->pos.x);
        buffer_write_i32(buf, f->pos.y);
        buffer_write_i32(buf, f->course.x);
        buffer_write_i32(buf, f->course.y);
        buffer_write_i32(buf, f->action_points);
        buffer_write_i32(buf, f->shield);
        buffer_write_i32(buf, f->attack_target_id);
        buffer_write_i32(buf, f->attack_last_update);
        buffer_write_i32(buf, f->attack_seq);
        buffer_write_i8(buf, f->can_store_units);
        buffer_write_i32(buf, f->stored_in_id);
        buffer_write_i8(buf, f->city_order_enabled);
        buffer_write_i32(buf, f->city_order_days_left);
        buffer_write_i32(buf, f->city_order_choices_len);
        buffer_write_i32(buf, f->city_order);
        buffer_write_raw(buf, f->city_order_choices, sizeof(int) * 5);
    }
}

static void save_game(buffer *buf) {
    buffer_write_i32(buf, game_state_get()->current_player);
    buffer_write_i32(buf, game_state_get()->day);
    buffer_write_i8(buf, game_inter_screen_is_visible());

    for (int i = 1; i <= 2; i++) {
        buffer_write_i32(buf, game_state_get()->player[i].id);
        buffer_write_i32(buf, game_state_get()->player[i].camera_x);
        buffer_write_i32(buf, game_state_get()->player[i].camera_y);
        buffer_write_i32(buf, game_state_get()->player[i].cursor.x);
        buffer_write_i32(buf, game_state_get()->player[i].cursor.y);
    }
}

static void load_game_state(buffer *buf) {
    game_state_get()->current_player = buffer_read_i32(buf);
    game_state_get()->day = buffer_read_i32(buf);
    int inter_screen_v = buffer_read_i8(buf);
    if (inter_screen_v)
        game_inter_screen_show();

    for (int i = 1; i <= 2; i++) {
        game_state_get()->player[i].id = buffer_read_i32(buf);
        game_state_get()->player[i].camera_x = buffer_read_i32(buf);
        game_state_get()->player[i].camera_y = buffer_read_i32(buf);
        game_state_get()->player[i].cursor.x = buffer_read_i32(buf);
        game_state_get()->player[i].cursor.y = buffer_read_i32(buf);

        game_state_get()->player[i].selected_figure = NULL;
    }

    screen_move_to(game_player_get()->cursor);
}

static void load_figures(buffer *buf) {
    for (int i = 0; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);

        f->id = buffer_read_i32(buf);
        f->player_id = buffer_read_i32(buf);
        buffer_read_raw(buf, f->name, 12);
        f->animation_id = buffer_read_i32(buf);
        f->type = buffer_read_i32(buf);
        f->unit = buffer_read_i32(buf);
        f->max_action_points = buffer_read_i32(buf);
        f->max_shield = buffer_read_i32(buf);
        f->days_to_make = buffer_read_i32(buf);
        f->pos.x = buffer_read_i32(buf);
        f->pos.y = buffer_read_i32(buf);
        f->course.x = buffer_read_i32(buf);
        f->course.y = buffer_read_i32(buf);
        f->action_points = buffer_read_i32(buf);
        f->shield = buffer_read_i32(buf);
        f->attack_target_id = buffer_read_i32(buf);
        f->attack_last_update = buffer_read_i32(buf);
        f->attack_seq = buffer_read_i32(buf);
        f->can_store_units = buffer_read_i8(buf);
        f->stored_in_id = buffer_read_i32(buf);
        f->city_order_enabled = buffer_read_i8(buf);
        f->city_order_days_left = buffer_read_i32(buf);
        f->city_order_choices_len = buffer_read_i32(buf);
        f->city_order = buffer_read_i32(buf);
        buffer_read_raw(buf, f->city_order_choices, sizeof(int) * 5);
    }
}

static void save_fog(buffer *buf) {
    for (int i = 1; i <= 2; i++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                fog_cell c = game_fog_get(x, y, i);
                buffer_write_i32(buf, c.tile.x);
                buffer_write_i32(buf, c.tile.y);
                buffer_write_u8(buf, c.visible);
                buffer_write_u8(buf, c.visited);
                buffer_write_i32(buf, c.player_id);
                buffer_write_i32(buf, c.animation_id);
            }
        }
    }
}

static void load_fog(buffer *buf) {
    for (int i = 1; i <= 2; i++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                fog_cell c = (fog_cell){};

                c.tile.x = buffer_read_i32(buf);
                c.tile.y = buffer_read_i32(buf);
                c.visible = buffer_read_u8(buf);
                c.visited = buffer_read_u8(buf);
                c.player_id = buffer_read_i32(buf);
                c.animation_id = buffer_read_i32(buf);

                game_fog_set(x, y, i, c);
            }
        }
    }
}

static void allocate_buffer(buffer *buf) {
    void *data = malloc(size);
    memset(data, 0, sizeof(size));
    buffer_init(buf, data, size);
}

void save_state(char *file) {
    save_game_state state = (save_game_state){
        .figures = &(buffer){},
        .game_state = &(buffer){},
        .game_fog = &(buffer){},
    };
    allocate_buffer(state.figures);
    allocate_buffer(state.game_state);
    allocate_buffer(state.game_fog);

    save_figures(state.figures);
    save_game(state.game_state);
    save_fog(state.game_fog);

    FILE *fp = fopen(file, "wb");
    if (!fp) {
        SDL_LogError(0, "failed to open save file");
        abort();
    }
    fwrite(state.figures->data, 1, size, fp);
    fwrite(state.game_state->data, 1, size, fp);
    fwrite(state.game_fog->data, 1, size, fp);
    fclose(fp);

    free(state.figures->data);
    free(state.game_state->data);
    free(state.game_fog->data);
}

void save_load(char *file) {
    // reset
    for (int i = 0; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        figure_delete(f);
    }
    game_fog_reset(PLAYER_ONE);
    game_fog_reset(PLAYER_TWO);

    // load
    save_game_state state = (save_game_state){
        .figures = &(buffer){},
        .game_state = &(buffer){},
        .game_fog = &(buffer){},
    };
    allocate_buffer(state.figures);
    allocate_buffer(state.game_state);
    allocate_buffer(state.game_fog);

    FILE *fp = fopen(file, "rb");
    if (!fp) {
        SDL_LogError(0, "failed to read file");
        abort();
    }
    fread(state.figures->data, 1, size, fp);
    fread(state.game_state->data, 1, size, fp);
    fread(state.game_fog->data, 1, size, fp);
    fclose(fp);

    load_figures(state.figures);
    load_game_state(state.game_state);
    load_fog(state.game_fog);

    free(state.figures->data);
    free(state.game_state->data);
    free(state.game_fog->data);
}
