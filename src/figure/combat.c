#include "combat.h"
#include "animation/animation.h"
#include "figure/city.h"
#include "figure/figure.h"
#include "figure/movement.h"
#include "game/game.h"
#include "game/log.h"
#include "time/time.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <stdio.h>

#define COMBAT_SPEED 10

typedef struct {
    figure_unit unit;
    int hit[17];
} hit_points;

static hit_points hit_points_table[10] = {
    (hit_points) {
        .unit = FIGURE_UNIT_PREDATOR,
        .hit = {3,3,3,2,2,3,3,3,3,3,3,3,3,3,3,3,3},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_BUTCHER,
        .hit = {4,4,2,4,2,4,4,3,4,4,4,4,4,4,4,4,4},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_BURGLAR,
        .hit = {1,1,3,3,2,1,1,1,3,3,2,1,1,1,1,1,1},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_ASSASIN,
        .hit = {2,2,1,1,1,2,2,3,1,1,1,2,2,3,3,2,1},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_GUARDIAN,
        .hit = {1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,1},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_MEGOZAVR,
        .hit = {3,3,2,2,2,4,3,4,3,3,3,4,4,4,4,4,0},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_CORSAIR,
        .hit = {2,2,1,2,1,2,3,3,2,3,2,3,3,3,4,4,0},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_SHARK,
        .hit = {2,2,1,2,2,2,2,3,1,2,2,5,8,2,3,3,0},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_GHOST,
        .hit = {7,7,1,7,7,2,2,7,8,8,4,1,8,8,8,8,0},
    },
    (hit_points) {
        .unit = FIGURE_UNIT_TORPEDO,
        .hit = {1,1,3,4,1,2,2,1,3,3,2,2,1,1,1,1,0},
    },
};

static int random_num(int min, int max) {
    return min + rand() % (max+1 - min);
}

static int random_hit_point(int min, figure *attacker, figure *target) {
    if (attacker->type == FIGURE_CITY)
        return random_num(0, 1);

    int max = 1;
    for (int i = 0; i < 10; i++) {
        if (hit_points_table[i].unit == attacker->unit) {
            max = hit_points_table[i].hit[target->unit];
            break;
        }
    }
    return min + rand() % (max + 1 - min);
}

static void unit_destroyed(figure *unit) {
    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (f->type && f->stored_in_id == unit->id) {
            SDL_LogInfo(0, "destroyed with storage: %s %d", f->name, f->id);
            figure_delete(f);
        }
    }
    figure_delete(unit);
}

void figure_combat_update() {
    char buf[64];

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (!f->type)
            continue;

        if (f->attack_target_id == 0)
            continue;

        if (time_get_millis() - f->attack_last_update < COMBAT_SPEED)
            return;

        figure *target = figure_get(f->attack_target_id);
        figure *attacker, *defender;

        if (target->type == FIGURE_CITY)
            target = figure_city_select_defender(target);

        if (f->attack_seq % 2 == 0) {
            attacker = f;
            defender = target;
        } else {
            attacker = target;
            defender = f;
        }

        int hit = random_hit_point(0, attacker, defender);

        // decrease chance of MISS to 20% if you attack first.
        if (f->attack_seq == 0 && hit == 0) {
            if (random_num(1, 10) > 8)
                hit = 1;
        }

        defender->shield -= hit;
        SDL_LogInfo(0, "%s attacks %s with %d points. shield: %d", attacker->name, defender->name, hit, defender->shield);

        if (defender->shield >= 0) {
            f->attack_last_update = time_get_millis();
            f->attack_seq++;
            return;
        }

        if (defender->type == FIGURE_CITY) {
            // log msg
            if (defender->player_id > 0 && defender->player_id != game_player_get()->id) {
                sprintf(buf, "city captured by %s", f->name);
                game_log(buf, target->pos);
            }

            defender->player_id = f->player_id;
            defender->shield = defender->max_shield;
            defender->animation_id = ANIMATION_CITY;
            f->stored_in_id = defender->id;
            f->attack_target_id = 0;
            f->attack_last_update = 0;
            f->attack_seq = 0;
            f->pos = defender->pos;
            game_set_state_selected_figure(NULL);
            return;
        }

        // log msg
        if (defender->player_id > 0 && attacker->player_id > 0) {
            if (defender->player_id != game_player_get()->id)
                sprintf(buf, "LOST: %s killed by %s", defender->name, attacker->name);
            else
                sprintf(buf, "WON: %s killed by %s", defender->name, attacker->name);
            game_log(buf, defender->pos);
        }

        if (defender->stored_in_id) {
            SDL_LogInfo(0, "defender stored in");
            f->attack_target_id = defender->stored_in_id;
            unit_destroyed(defender);
            return;
        }

        f->attack_target_id = 0;
        f->attack_last_update = 0;
        f->attack_seq = 0;

        figure_movement_update(f, target->pos);
        unit_destroyed(defender);
    }
}
