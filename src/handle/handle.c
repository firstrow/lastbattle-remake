#include <SDL2/SDL_keycode.h>
#include <stdlib.h>

#include "game/hud.h"
#include "game/turn.h"
#include "game/game.h"
#include "game/inter_screen.h"
#include "handle/handle.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "screen/screen.h"
#include "time/time.h"
#include "types/types.h"
#include "figure/figure.h"
#include "figure/movement.h"

#include <SDL2/SDL.h>

static handler_state state;

static int end_turn_pressed = 0;

static void default_handler() {
    if (input_key(KEY_CANCEL)) {
        if (end_turn_pressed > 0) {
            end_turn_pressed = 0;
            return;
        }

        if (game_selected_figure() != NULL)
            game_set_state_selected_figure(NULL);
    }

    if (input_key(KEY_SELECT_NEXT))
        game_select_next_unit((SDL_GetModState() & KMOD_SHIFT) > 0, 0);

    if (input_key_hold(KEY_UP))
        screen_camera_up();

    if (input_key_hold(KEY_DOWN))
        screen_camera_down();

    if (input_key_hold(KEY_LEFT))
        screen_camera_left();

    if (input_key_hold(KEY_RIGHT))
        screen_camera_right();

    if (input_key(KEY_PACK_UNPACK))
        figure_movement_toggle_pack(game_selected_figure());

    if (input_key(KEY_LEAVE_WINDOW))
        game_hud_leave_show();

    if (input_key(KEY_ACTION))
        game_tile_selected(game_player_get()->cursor);

    if (input_key(KEY_SHOW_LOG))
        game_hud_log_menu_show();

    if (input_key(KEY_SCALE_INC))
        screen_set_scale(0.1);
    if (input_key(KEY_SCALE_DEC))
        screen_set_scale(-0.1);

    if (input_key(KEY_END_TURN)) {
        end_turn_pressed++;
        if (end_turn_pressed >= 2) {
            end_turn_pressed = 0;
            game_inter_screen_show();
        }
    }
}

void handle_init() {
    state.pos = 0;
    state.input_callbacks[state.pos] = &default_handler;
}

void handle_keyboard_input() {
    state.input_callbacks[state.pos]();
}

void handle_push_callback(event_cb_t cb) {
    state.pos++;
    state.input_callbacks[state.pos] = cb;
}

void handle_pop_callback() {
    state.input_callbacks[state.pos] = NULL;
    state.pos--;
}
