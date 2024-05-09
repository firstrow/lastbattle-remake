#include "figure/figure.h"
#include "animation/animation.h"
#include "figure/city.h"
#include "types/types.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <string.h>

#define ap_mul 2
#define day_mul 0.5

static struct {
    figure figures[MAX_FIGURES];
} data;

static figure units[16] = {
    (figure){
        .type = FIGURE_ROBOT,
        .unit = FIGURE_UNIT_SURGEON,
        .name = "SURGEON",
        .animation_id = ANIMATION_UNIT_SURGEON,
        .max_action_points = 2 * ap_mul,
        .max_shield = 4,
        .days_to_make = 10 * day_mul,
        .can_store_units = 0,
    },
    (figure){
        .type = FIGURE_ROBOT,
        .unit = FIGURE_UNIT_ANIMATOR,
        .name = "ANIMATOR",
        .animation_id = ANIMATION_UNIT_ANIMATOR,
        .max_action_points = 1 * ap_mul,
        .max_shield = 5,
        .days_to_make = 19 * day_mul,
        .can_store_units = 0,
    },
    (figure){
        .type = FIGURE_ROBOT,
        .unit = FIGURE_UNIT_PREDATOR,
        .name = "PREDATOR",
        .animation_id = ANIMATION_UNIT_PREDATOR,
        .max_action_points = 2 * ap_mul,
        .max_shield = 24,
        .days_to_make = 41 * day_mul,
    },
    (figure){
        .type = FIGURE_ROBOT,
        .unit = FIGURE_UNIT_BUTCHER,
        .name = "BUTCHER",
        .animation_id = ANIMATION_UNIT_BUTCHER,
        .max_action_points = 1 * ap_mul,
        .max_shield = 17,
        .days_to_make = 34 * day_mul,
    },
    (figure){
        .type = FIGURE_ROBOT,
        .unit = FIGURE_UNIT_BURGLAR,
        .name = "BURGLAR",
        .animation_id = ANIMATION_UNIT_BURGLAR,
        .max_action_points = 1 * ap_mul,
        .max_shield = 10,
        .days_to_make = 21 * day_mul,
    },
    (figure){
        .type = FIGURE_ROBOT,
        .unit = FIGURE_UNIT_ASSASIN,
        .name = "ASSASIN",
        .animation_id = ANIMATION_UNIT_ASSASIN,
        .max_action_points = 3 * ap_mul,
        .max_shield = 6,
        .days_to_make = 12 * day_mul,
    },
    (figure){
        .type = FIGURE_ROBOT,
        .unit = FIGURE_UNIT_GUARDIAN,
        .name = "GUARDIAN",
        .animation_id = ANIMATION_UNIT_GUARDIAN,
        .max_action_points = 2 * ap_mul,
        .max_shield = 3,
        .days_to_make = 6 * day_mul,
    },
    (figure){
        .type = FIGURE_ROBOT,
        .unit = FIGURE_UNIT_GADFLY,
        .name = "GADFLY",
        .animation_id = ANIMATION_UNIT_GADFLY,
        .max_action_points = 4 * ap_mul,
        .max_shield = 2,
        .days_to_make = 6 * day_mul,
    },
    (figure){
        .type = FIGURE_SHIP,
        .unit = FIGURE_UNIT_MEGOZAVR,
        .name = "MEGOZAVR",
        .animation_id = ANIMATION_UNIT_MEGOZAVR,
        .max_action_points = 2 * ap_mul,
        .max_shield = 30,
        .days_to_make = 42 * day_mul,
    },
    (figure){
        .type = FIGURE_SHIP,
        .unit = FIGURE_UNIT_CORSAIR,
        .name = "CORSAIR",
        .animation_id = ANIMATION_UNIT_CORSAIR,
        .max_action_points = 2 * ap_mul,
        .max_shield = 24,
        .days_to_make = 33 * day_mul,
    },
    (figure){
        .type = FIGURE_SHIP,
        .unit = FIGURE_UNIT_SHARK,
        .name = "SHARK",
        .animation_id = ANIMATION_UNIT_SHARK,
        .max_action_points = 3 * ap_mul,
        .max_shield = 17,
        .days_to_make = 26 * day_mul,
    },
    (figure){
        .type = FIGURE_SHIP,
        .unit = FIGURE_UNIT_GHOST,
        .name = "GHOST",
        .animation_id = ANIMATION_UNIT_GHOST,
        .max_action_points = 3 * ap_mul,
        .max_shield = 4,
        .days_to_make = 20 * day_mul,
    },
    (figure){
        .type = FIGURE_SHIP,
        .unit = FIGURE_UNIT_TORPEDO,
        .name = "TORPEDO",
        .animation_id = ANIMATION_UNIT_TORPEDO,
        .max_action_points = 2 * ap_mul,
        .max_shield = 15,
        .days_to_make = 17 * day_mul,
    },
    (figure){
        .type = FIGURE_SHIP,
        .unit = FIGURE_UNIT_KAMIKAZE,
        .name = "KAMIKAZE",
        .animation_id = ANIMATION_UNIT_KAMIKAZE,
        .max_action_points = 5 * ap_mul,
        .max_shield = 1,
        .days_to_make = 10 * day_mul,
    },
    (figure){
        .type = FIGURE_SHIP,
        .unit = FIGURE_UNIT_SCARAB,
        .name = "SCARAB",
        .animation_id = ANIMATION_UNIT_SCARAB,
        .max_action_points = 2 * ap_mul,
        .max_shield = 4,
        .days_to_make = 15 * day_mul,
        .can_store_units = 5,
    },
    (figure){
        .type = FIGURE_SHIP,
        .unit = FIGURE_UNIT_TORTOISE,
        .name = "TORTOISE",
        .animation_id = ANIMATION_UNIT_TORTOISE,
        .max_action_points = 2 * ap_mul,
        .max_shield = 7,
        .days_to_make = 18 * day_mul,
        .can_store_units = 8,
    },
};

figure *figure_create(figure_unit unit, vec2 pos, int player_id) {
    int id = 0;

    for (int i = 1; i < MAX_FIGURES; i++) {
        if (!data.figures[i].type) {
            id = i;
            break;
        }
    }

    figure *f = &data.figures[id];

    for (int i = 0; i < 16; i++) {
        if (unit != units[i].unit) {
            continue;
        }

        SDL_memcpy(f, &units[i], sizeof(figure));
        f->id = id;
        f->pos = pos;
        f->player_id = player_id;
        f->action_points = f->max_action_points;
        f->shield = f->max_shield;
    }

    return f;
}

figure* figure_create_city(vec2 pos) {
    int id = 0;

    for (int i = 1; i < MAX_FIGURES; i++) {
        if (!data.figures[i].type) {
            id = i;
            break;
        }
    }

    figure *f = &data.figures[id];
    f->id = id;
    f->type = FIGURE_CITY;
    f->unit = FIGURE_UNIT_CITY;
    f->animation_id = ANIMATION_CITY_EMPTY;
    f->player_id = PLAYER_NONE;
    f->pos = pos;
    f->shield = 3;
    f->max_shield = 3;
    f->action_points = 0;
    f->max_action_points = 0;
    f->can_store_units = 3;
    strcpy(f->name, "CITY");
    figure_city_order_forbid(f);

    return f;
}

void figure_delete(figure *f) {
    SDL_LogInfo(0, "deleted figure: %d", f->id);
    int id = f->id;
    memset(f, 0, sizeof(figure));
    f->id = id;
}

figure *figure_get(int id) {
    return &data.figures[id];
}

figure* figure_at(vec2 tile) {
    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (!f->type)
            continue;
        if (f->stored_in_id)
            continue;
        if (f->pos.x == tile.x && f->pos.y == tile.y)
            return f;
    }
    return NULL;
}

int figure_stored_units(figure *unit) {
    int r = 0;
    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (!f->type)
            continue;
        if (f->stored_in_id == unit->id)
            r++;
    }
    return r;
}

figure figure_unit_get(figure_unit unit) {
    for (int i = 0; i < 16; i++) {
        if (unit == units[i].unit)
            return units[i];
    }
    SDL_LogError(0, "unit not found: %d", unit);
    return (figure){};
}
