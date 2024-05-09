#include "inter_screen.h"

#include <stdbool.h>
#include <stdio.h>

#include "game/game.h"
#include "game/turn.h"
#include "handle/handle.h"
#include "input/keyboard.h"
#include "screen/screen.h"
#include "types/types.h"
#include "ui/ui.h"

static bool visible;

static void callback() {
    if (input_key(KEY_END_TURN)) {
        handle_pop_callback();
        game_inter_screen_hide();
        game_turn_end();
    }
}

void game_inter_screen_show() {
    if (visible)
        return;

    visible = true;
    handle_push_callback(&callback);
}

void game_inter_screen_hide() {
    visible = false;
}

bool game_inter_screen_is_visible() {
    return visible;
}

void game_inter_screen_draw() {
    if (!visible)
        return;

    int x, y, w, h;
    screen_get_size(&w, &h);
    x = w - ((42 * 32));
    y = h / 2 - 48;

    int p;
    if (game_player_get()->id == PLAYER_ONE) {
        p = PLAYER_TWO;
    } else {
        p = PLAYER_ONE;
    }

    char str[100];
    sprintf(str, "PLAYER %d GET READY! PRESS ENTER TO START THE TURN!", p);

    ui_draw_text(x, y, str, 42);
}
