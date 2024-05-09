#include <SDL2/SDL_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "animation/animation.h"
#include "config/config.h"
#include "figure/figure.h"
#include "game/fog.h"
#include "game/game.h"
#include "game/level.h"
#include "game/log.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "screen/screen.h"
#include "time/time.h"
#include "types/types.h"
#include "handle/handle.h"
#include "map/loader.h"
#include "ui/notify.h"
#include "ui/ui.h"
#include "save/save.h"

static char save_file[256];

int time_seed (void)
{
    time_t timeval;
    unsigned char *ptr;
    unsigned seed;
    size_t i;

    timeval = time(NULL);
    ptr = (unsigned char *) &timeval;

    seed = 0;
    for (i = 0; i < sizeof timeval; i++)
        seed = seed * (10 + 2u) + ptr[i];

    return seed;
}

int main (int argc, char *argv[]) {
    srand(time_seed());

    config_set_game_path(SDL_GetBasePath());
    screen_init();
    input_keyboard_init();
    animation_load_all();
    ui_init();
    handle_init();
    map_load_csv("/home/andrew/krita_sources/islands.csv");
    game_init();
    game_log_init();

    if (argc < 2) {
        SDL_LogError(0, "provide save file as argv");
        return EXIT_FAILURE;
    }

    strcpy(save_file, argv[1]);
    SDL_LogInfo(0, "save file: %s", save_file);

    int quit = 0;

    /* figure_create(FIGURE_UNIT_KAMIKAZE, (vec2){.x=35,.y=5}, PLAYER_ONE); */
    /* figure_create(FIGURE_UNIT_KAMIKAZE, (vec2){.x=36,.y=5}, PLAYER_ONE); */
    /* figure_create(FIGURE_UNIT_KAMIKAZE, (vec2){.x=37,.y=5}, PLAYER_ONE); */

    /* figure_create(FIGURE_UNIT_SCARAB, (vec2){.x=36,.y=6}, PLAYER_TWO); */
    /* figure_create(FIGURE_UNIT_KAMIKAZE, (vec2){.x=37,.y=6}, PLAYER_TWO); */
    /* figure_create(FIGURE_UNIT_KAMIKAZE, (vec2){.x=38,.y=6}, PLAYER_TWO); */

    /* figure_create(FIGURE_UNIT_ASSASIN, (vec2){.x=10,.y=5}, PLAYER_ONE); */
    /* figure_create(FIGURE_UNIT_ASSASIN, (vec2){.x=11,.y=5}, PLAYER_ONE); */
    /* figure_create(FIGURE_UNIT_GUARDIAN, (vec2){.x=12,.y=5}, PLAYER_TWO); */
    /* figure_create(FIGURE_UNIT_GUARDIAN, (vec2){.x=13,.y=5}, PLAYER_TWO); */
    /* figure_create(FIGURE_UNIT_GUARDIAN, (vec2){.x=14,.y=5}, PLAYER_TWO); */
    /* figure_create(FIGURE_UNIT_GUARDIAN, (vec2){.x=14,.y=4}, PLAYER_TWO); */
    /* figure_create(FIGURE_UNIT_SURGEON, (vec2){.x=16,.y=5}, PLAYER_TWO); */

    /* figure_create_city((vec2){.x=13, .y=6})->player_id = PLAYER_TWO; */

    game_fog_toggle();

    SDL_Event event;
    while(quit == 0) {
        time_set_millis(SDL_GetTicks());
        input_mouse_update_state();

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_MOUSEMOTION:
                input_mouse_set_pos(event.motion.x, event.motion.y);
                break;
            case SDL_MOUSEBUTTONUP:
                input_mouse_click();
                break;
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_F12:
                        /* game_level_gen(); */
                        break;
                    case SDLK_F11:
                        /* game_fog_toggle(); */
                        break;
                    case SDLK_F2:
                        save_state(save_file);
                        ui_notify_show("game saved");
                        break;
                    case SDLK_F3:
                        save_load(save_file);
                        ui_notify_show("game loaded");
                        break;
                }
                break;
            default:
                break;
            }
        }

        input_keyboard_update();
        handle_keyboard_input();
        game_run();
        game_draw();
    }

    screen_destroy();

    return 0;
}
