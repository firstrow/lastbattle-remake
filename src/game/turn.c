#include "turn.h"

#include "array_list/array_list.h"
#include "figure/figure.h"
#include "figure/city.h"
#include "game/hud.h"
#include "screen/screen.h"
#include "types/types.h"
#include "game/game.h"
#include "game/log.h"
#include "ui/notify.h"

#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>

void game_turn_begin() {
    figure *storage;

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (!f->type)
            continue;
        if (f->player_id != game_player_get()->id)
            continue;

        f->action_points = f->max_action_points;
        if (f->shield == 0)
            f->action_points = 1;

        if (f->stored_in_id) {
            storage = figure_get(f->stored_in_id);
            switch (storage->unit) {
                case FIGURE_UNIT_SURGEON: f->shield++; break;
                case FIGURE_UNIT_ANIMATOR: f->shield += 3; break;
                default: break;
            }
            if (f->shield > f->max_shield)
                f->shield = f->max_shield;
        }

        // produce city units
        figure_city_order_update(f);
    }

    game_log_finalize();
    if (game_log_finalized_get()->len > 0) {
        /* game_hud_log_menu_show(); */
        char buf[24];
        sprintf(buf, "notifications: %lu", game_log_finalized_get()->len);
        ui_notify_show(buf);
    }
}

// FIXME: if fight in proggres - return
void game_turn_end() {
    screen_get_camera_offset(
        &game_player_get()->camera_x,
        &game_player_get()->camera_y
    );

    const game_state *gs = game_state_get();
    switch (gs->current_player) {
        case PLAYER_ONE:
            game_set_player(PLAYER_TWO);
            game_turn_begin();
            break;
        case PLAYER_TWO:
            game_set_player(PLAYER_ONE);
            game_adavance_day();
            game_turn_begin();
            break;
    }

    game_set_state_selected_figure(NULL);

    screen_set_camera_offset(
        game_player_get()->camera_x,
        game_player_get()->camera_y
    );
}
