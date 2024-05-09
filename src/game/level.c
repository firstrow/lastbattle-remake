#include "level.h"

#include "animation/animation.h"
#include "figure/figure.h"
#include "game/fog.h"
#include "game/game.h"
#include "map/map.h"
#include "screen/screen.h"
#include "types/types.h"

#include <SDL2/SDL.h>

int player1_segment;
int player2_segment;
figure *player1_city;
figure *player2_city;

const figure_unit starter_order_choices[5] = {
    FIGURE_UNIT_SCARAB,
    FIGURE_UNIT_KAMIKAZE,
    FIGURE_UNIT_CORSAIR,
    FIGURE_UNIT_GADFLY,
    FIGURE_UNIT_GUARDIAN,
};

#define orders_len 10

const figure_unit city_orders[orders_len] = {
    FIGURE_UNIT_GUARDIAN,
    FIGURE_UNIT_GUARDIAN,
    FIGURE_UNIT_SURGEON,
    FIGURE_UNIT_SURGEON,
    FIGURE_UNIT_ASSASIN,
    FIGURE_UNIT_ANIMATOR,
    FIGURE_UNIT_PREDATOR,
    FIGURE_UNIT_BUTCHER,
    FIGURE_UNIT_BURGLAR,
    FIGURE_UNIT_GADFLY,
};

const figure_unit port_orders[orders_len] = {
    FIGURE_UNIT_SCARAB,
    FIGURE_UNIT_SCARAB,
    FIGURE_UNIT_KAMIKAZE,
    FIGURE_UNIT_KAMIKAZE,
    FIGURE_UNIT_MEGOZAVR,
    FIGURE_UNIT_CORSAIR,
    FIGURE_UNIT_SHARK,
    FIGURE_UNIT_GHOST,
    FIGURE_UNIT_TORPEDO,
    FIGURE_UNIT_TORTOISE,
};

static void gen_player_starting_segments() {
    player1_city = NULL;
    player2_city = NULL;

    player1_segment = rand() % 4;
    player2_segment = rand() % 4;

    if (player1_segment == player2_segment)
        gen_player_starting_segments();
}

static void gen(int how_much, int tile_type, vec2 start, vec2 stop, int segment) {
    vec2 *tiles = SDL_calloc(1024*2, sizeof(vec2));
    int *tiles_taken = SDL_calloc(1024*2, sizeof(int));

    int n = 0;

    for (int y = start.y; y < stop.y; y++) {
        for (int x = start.x; x < stop.x; x++) {
            const map_tile_data *tile = map_get(x, y);
            if (tile->terrain_type == tile_type) {
                tiles[n] = (vec2){.x=x, .y=y};
                n++;
            }
        }
    }

    int idx;
    for (int i = 0; i < how_much; i++) {
        idx = rand() % n;
        if (tiles_taken[idx] == 1) {
            how_much++;
            continue;
        }
        tiles_taken[idx] = 1;
        figure *city = figure_create_city(tiles[idx]);
        city->animation_id = ANIMATION_CITY_ALIEN;

        if (tile_type == MAP_TILE_SHORE && segment == player1_segment && player1_city == NULL) {
            city->player_id = PLAYER_ONE;
            city->animation_id = ANIMATION_CITY;
            player1_city = city;
        }
        if (tile_type == MAP_TILE_SHORE && segment == player2_segment && player2_city == NULL) {
            city->player_id = PLAYER_TWO;
            city->animation_id = ANIMATION_CITY;
            player2_city = city;
        }
    }

    free(tiles);
    free(tiles_taken);
}

static void gen_bases() {
    gen(4, MAP_TILE_SHORE, (vec2){.x=0, .y=0}, (vec2){.x=50, .y=30}, 0);
    gen(4, MAP_TILE_SHORE, (vec2){.x=0, .y=30}, (vec2){.x=50, .y=60}, 1);
    gen(4, MAP_TILE_SHORE, (vec2){.x=49, .y=0}, (vec2){.x=100, .y=30}, 2);
    gen(4, MAP_TILE_SHORE, (vec2){.x=49, .y=30}, (vec2){.x=100, .y=60}, 3);

    gen(8, MAP_TILE_GROUND, (vec2){.x=0, .y=0}, (vec2){.x=50, .y=30}, 0);
    gen(8, MAP_TILE_GROUND, (vec2){.x=0, .y=30}, (vec2){.x=50, .y=60}, 1);
    gen(8, MAP_TILE_GROUND, (vec2){.x=49, .y=0}, (vec2){.x=100, .y=30}, 2);
    gen(8, MAP_TILE_GROUND, (vec2){.x=49, .y=30}, (vec2){.x=100, .y=60}, 3);
}

static void gen_units(figure *city) {
    figure_create(FIGURE_UNIT_GUARDIAN, city->pos, city->player_id)->stored_in_id = city->id;
    figure_create(FIGURE_UNIT_GUARDIAN, city->pos, city->player_id)->stored_in_id = city->id;
}

static int random_num(int min, int max) {
    return min + rand() % (max+1 - min);
}

static void gen_order_choices() {
    short max_units = 2;
    short max_ships = 2;

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (f->type != FIGURE_CITY)
            continue;

        const map_tile_data *tile = map_get(f->pos.x, f->pos.y);
        bool is_port = tile->terrain_type == MAP_TILE_SHORE;

        int units_num = random_num(1, max_units);
        int ships_num = random_num(1, max_ships);
        int claimed[16] = {0};
        SDL_memset(&claimed, 0, 16);

        for (int i = 0; i < units_num; i++) {
            int random_i = random_num(0, orders_len-1);
            int f_unit = city_orders[random_i];
            if (claimed[f_unit] == 1) {
                /* units_num++; */
                continue;
            }

            f->city_order_choices[f->city_order_choices_len] = f_unit;
            f->city_order_choices_len++;
            claimed[f_unit] = 1;
        }

        if (is_port) {
            for (int i = 0; i < ships_num; i++) {
                int random_i = random_num(0, orders_len-1);
                int f_unit = port_orders[random_i];
                if (claimed[f_unit] == 1) {
                    /* ships_num++; */
                    continue;
                }
                f->city_order_choices[f->city_order_choices_len] = f_unit;
                f->city_order_choices_len++;
                claimed[f_unit] = 1;
            }
        }
    }
}

void game_level_gen() {
    game_fog_reset(PLAYER_ONE);
    game_fog_reset(PLAYER_TWO);

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        figure_delete(f);
    }

    gen_player_starting_segments();
    gen_bases();
    gen_units(player1_city);
    gen_units(player2_city);
    gen_order_choices();

    SDL_memcpy(&player1_city->city_order_choices, &starter_order_choices, sizeof(figure_unit) * 5);
    SDL_memcpy(&player2_city->city_order_choices, &starter_order_choices, sizeof(figure_unit) * 5);
    player1_city->city_order_choices_len = 5;
    player2_city->city_order_choices_len = 5;

    game_state_get()->player[PLAYER_ONE].cursor = player1_city->pos;
    game_state_get()->player[PLAYER_TWO].cursor = player2_city->pos;
    game_state_get()->day = 1;

    // set camera offsets
    screen_move_to(player2_city->pos);
    screen_get_camera_offset(
        &game_state_get()->player[PLAYER_TWO].camera_x,
        &game_state_get()->player[PLAYER_TWO].camera_y
    );

    screen_move_to(player1_city->pos);
}
