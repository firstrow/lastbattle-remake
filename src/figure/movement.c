#include <stdbool.h>
#include <stdlib.h>

#include "animation/animation.h"
#include "game/course.h"
#include "game/game.h"
#include "game/log.h"
#include "map/map.h"
#include "figure/figure.h"
#include "figure/movement.h"
#include "figure/combat.h"
#include "time/time.h"

#include <SDL2/SDL.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

bool figure_movement_can_move(figure *f, vec2 coords) {
    const map_tile_data *tile = map_get(coords.x, coords.y);

    bool ground_or_shore = tile->terrain_type == MAP_TILE_GROUND || tile->terrain_type == MAP_TILE_SHORE;
    bool water_or_shore = tile->terrain_type == MAP_TILE_WATER || tile->terrain_type == MAP_TILE_SHORE;

    if (f->type == FIGURE_ROBOT && ground_or_shore)
        return true;

    if (f->type == FIGURE_SHIP && water_or_shore)
        return true;

    return false;
}

static int distance(vec2 start, vec2 end) {
    int dx = abs(start.x - end.x);
    int dy = abs(start.y - end.y);
    int d2 = 1;
    int d = 1;
    return (dx + dy) + (d2 - 2 * d) * MIN(dx, dy);
}

static bool move_unit_to_storage(figure *unit, figure *storage) {
    if (unit->type == FIGURE_SHIP && storage->type == FIGURE_SHIP)
        return false;
    if (figure_stored_units(unit) > 0 && (storage->unit == FIGURE_UNIT_SURGEON || storage->unit == FIGURE_UNIT_ANIMATOR)) {
        return false;
    }

    if (figure_stored_units(storage) < storage->can_store_units) {
        unit->stored_in_id = storage->id;
        unit->pos = storage->pos;
        game_set_state_selected_figure(NULL);
        return true;
    }
    return false;
}

static bool is_unpacked(figure *f) {
    if(f->unit == FIGURE_UNIT_SURGEON || f->unit == FIGURE_UNIT_ANIMATOR) {
        return f->can_store_units > 0;
    }
    return false;
}

static bool can_attack(figure *a, figure *b) {
    if (b->unit == FIGURE_UNIT_CITY) {
       switch (a->unit) {
           case FIGURE_UNIT_GUARDIAN:
           case FIGURE_UNIT_PREDATOR:
           case FIGURE_UNIT_BUTCHER:
           case FIGURE_UNIT_BURGLAR:
           case FIGURE_UNIT_ASSASIN:
               return true;
               break;
            default:
                return false;
                break;
       }
    }
    return true;
}

void figure_movement_move(figure *f, vec2 dest) {
    if (f == NULL)
        return;

    int dist = distance(f->pos, dest);
    if (dist > 1) {
        f->course = dest;
        return;
    }

    if (!figure_movement_can_move(f, dest))
        return;

    if (dist > f->action_points)
        return;

    if (f->action_points < 1)
        return;

    if (is_unpacked(f))
        return;

    figure *unit_at_dest = figure_at(dest);

    if (!unit_at_dest) {
        f->pos = dest;
        f->action_points -= dist;
        f->stored_in_id = 0;
        game_course_cancel(f);
        figure_movement_update(f, dest);
        return;
    }

    if (f->player_id != unit_at_dest->player_id) {
        if (!can_attack(f, unit_at_dest))
            return;

        f->attack_target_id = unit_at_dest->id;
        f->attack_last_update = time_get_millis();
        /* f->action_points -= dist; */
        f->action_points = 0;
        f->stored_in_id = 0;

        return;
    }

    if (unit_at_dest->can_store_units) {
        if (!move_unit_to_storage(f, unit_at_dest))
            return;
        f->action_points -= dist;
        figure_movement_update(f, dest);
    }
}

static void unpack(figure *f) {
    if (f->unit != FIGURE_UNIT_SURGEON && f->unit != FIGURE_UNIT_ANIMATOR)
        return;

    if (f->unit == FIGURE_UNIT_SURGEON)
        f->animation_id = ANIMATION_UNIT_SURGEON_UNPACKED;

    if (f->unit == FIGURE_UNIT_ANIMATOR)
        f->animation_id = ANIMATION_UNIT_ANIMATOR_UNPACKED;

    f->can_store_units = 1;
    f->action_points = 0;
}

static void pack(figure *f) {
    if (f->unit != FIGURE_UNIT_SURGEON && f->unit != FIGURE_UNIT_ANIMATOR)
        return;

    if(figure_stored_units(f) != 0)
        return;

    if (f->unit == FIGURE_UNIT_SURGEON)
        f->animation_id = ANIMATION_UNIT_SURGEON;

    if (f->unit == FIGURE_UNIT_ANIMATOR)
        f->animation_id = ANIMATION_UNIT_ANIMATOR;

    f->can_store_units = 0;
    f->action_points = 0;
}

void figure_movement_toggle_pack(figure *f) {
    require_fig(f);

    if (f->action_points < 1)
        return;

    if (f->unit != FIGURE_UNIT_SURGEON && f->unit != FIGURE_UNIT_ANIMATOR)
        return;

    if (f->can_store_units)
        pack(f);
    else
        unpack(f);
}

void figure_movement_update(figure *f, vec2 pos) {
    f->pos = pos;

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *unit = figure_get(i);
        if (!unit->type)
            continue;
        if (unit->stored_in_id != f->id)
            continue;

        unit->pos = f->pos;
    }
}
