#ifndef FIGURE_H_
#define FIGURE_H_

#include <stdbool.h>
#include <stdint.h>

#include "animation/animation.h"
#include "time/time.h"
#include "types/types.h"

#define MAX_FIGURES 1024

#define require_fig(f) { if (!f) { return; } };

typedef enum {
    FIGURE_DEAD = 0,
    FIGURE_ROBOT = 1,
    FIGURE_SHIP = 2,
    FIGURE_CITY = 3,
} figure_type;

// do not edit order of units
typedef enum {
    FIGURE_UNIT_SURGEON = 0,
    FIGURE_UNIT_ANIMATOR = 1,
    FIGURE_UNIT_PREDATOR = 2,
    FIGURE_UNIT_BUTCHER = 3,
    FIGURE_UNIT_BURGLAR = 4,
    FIGURE_UNIT_ASSASIN = 5,
    FIGURE_UNIT_GUARDIAN = 6,
    FIGURE_UNIT_GADFLY = 7,
    FIGURE_UNIT_MEGOZAVR = 8,
    FIGURE_UNIT_CORSAIR = 9,
    FIGURE_UNIT_SHARK = 10,
    FIGURE_UNIT_GHOST = 11,
    FIGURE_UNIT_TORPEDO = 12,
    FIGURE_UNIT_KAMIKAZE = 13,
    FIGURE_UNIT_SCARAB = 14,
    FIGURE_UNIT_TORTOISE = 15,
    FIGURE_UNIT_CITY = 16,
} figure_unit;

typedef struct {
    int id;
    int player_id;
    char name[12];
    animation_id_t animation_id;
    figure_type type;
    figure_unit unit;
    int max_action_points;
    int max_shield;
    int days_to_make;

    vec2 pos;
    vec2 course;
    int action_points;
    int shield;
    int attack_target_id;
    time_millis attack_last_update;
    int attack_seq;

    unsigned short can_store_units;
    int stored_in_id;

    bool city_order_enabled;
    int city_order_days_left;
    int city_order_choices_len;
    figure_unit city_order;
    figure_unit city_order_choices[5];
} figure;

typedef struct {
    short action_points;
    int attack;
    short shield;
    short days_to_build;
} unit_props;

figure* figure_create(figure_unit unit, vec2 pos, int player_id);
figure* figure_create_city(vec2 pos);
void figure_delete(figure *f);
figure* figure_get(int id);
figure* figure_at(vec2 tile);
figure figure_unit_get(figure_unit unit);
int figure_stored_units(figure *unit);
void figure_combat_action(int current_time);

#endif // FIGURE_H_
