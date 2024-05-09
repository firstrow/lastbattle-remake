#include <SDL2/SDL_log.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "array_list/array_list.h"
#include "screen/screen.h"
#include "config/config.h"
#include "animation/animation.h"

static array_list *texture_storage;
static array_list *animation_storage;

static void load(texture_t t) {
    char* fp = config_get_asset_path(t.filename);

    t.texture = IMG_LoadTexture(screen_get_renderer(), fp);
    t.surface = IMG_Load(fp);
    if (t.texture == NULL) {
        SDL_LogCritical(0, "failed to load asset texture: %s", t.filename);
        abort();
    }
    if (t.surface == NULL) {
        SDL_LogCritical(0, "failed to load asset surface: %s", t.filename);
        abort();
    }
    free(fp);

    t.cols = t.surface->w / t.cell_w;
    t.rows = t.surface->h / t.cell_h;

    array_list_set(texture_storage, &t, t.id);
}

void animation_load_all() {
    texture_storage = array_list_create(sizeof(texture_t), 128);
    animation_storage = array_list_create(sizeof(animation_t), 128);

    // textures
    load((texture_t){.id = TEXTURE_TILEMAP, .filename = "tilemap.png", .cell_w = 58, .cell_h = 60});
    load((texture_t){.id = TEXTURE_COMBAT, .filename = "combat.png", .cell_w = 58, .cell_h = 60});
    load((texture_t){.id = TEXTURE_HUD, .filename = "hud.png", .cell_w = 174, .cell_h = 96});

    // animations
    animation_define(ANIMATION_WATER, TEXTURE_TILEMAP, 0);
    animation_define(ANIMATION_SELECTED_TILE, TEXTURE_COMBAT, 4 * 16 + 0);
    animation_define(ANIMATION_CURSOR_SELECTED, TEXTURE_COMBAT, 4 * 16 + 2);
    animation_define(ANIMATION_UNIT_SURGEON, TEXTURE_COMBAT, 1);
    animation_define(ANIMATION_UNIT_ANIMATOR, TEXTURE_COMBAT, 2);
    animation_define(ANIMATION_UNIT_PREDATOR, TEXTURE_COMBAT, 3);
    animation_define(ANIMATION_UNIT_BUTCHER, TEXTURE_COMBAT, 4);
    animation_define(ANIMATION_UNIT_BURGLAR, TEXTURE_COMBAT, 5);
    animation_define(ANIMATION_UNIT_ASSASIN, TEXTURE_COMBAT, 6);
    animation_define(ANIMATION_UNIT_GUARDIAN, TEXTURE_COMBAT, 0);
    animation_define(ANIMATION_UNIT_GADFLY, TEXTURE_COMBAT, 7);
    animation_define(ANIMATION_UNIT_MEGOZAVR, TEXTURE_COMBAT, 8);
    animation_define(ANIMATION_UNIT_CORSAIR, TEXTURE_COMBAT, 9);
    animation_define(ANIMATION_UNIT_SHARK, TEXTURE_COMBAT, 10);
    animation_define(ANIMATION_UNIT_GHOST, TEXTURE_COMBAT, 11);
    animation_define(ANIMATION_UNIT_TORPEDO, TEXTURE_COMBAT, 12);
    animation_define(ANIMATION_UNIT_KAMIKAZE, TEXTURE_COMBAT, 13);
    animation_define(ANIMATION_UNIT_SCARAB, TEXTURE_COMBAT, 14);
    animation_define(ANIMATION_UNIT_TORTOISE, TEXTURE_COMBAT, 15);
    animation_define(ANIMATION_UNIT_SURGEON_UNPACKED, TEXTURE_COMBAT, 16 + 1);
    animation_define(ANIMATION_UNIT_ANIMATOR_UNPACKED, TEXTURE_COMBAT, 16 + 2);
    animation_define(ANIMATION_HUD, TEXTURE_HUD, 0);
    animation_define(ANIMATION_CITY_ALIEN, TEXTURE_COMBAT, 1 * 16 + 11);
    animation_define(ANIMATION_CITY_EMPTY, TEXTURE_COMBAT, 1 * 16 + 12);
    animation_define(ANIMATION_CITY, TEXTURE_COMBAT, 1 * 16 + 13);

    animation_define(ANIMATION_BLACK_TILE, TEXTURE_COMBAT, 16);
    animation_define(ANIMATION_FOG_TILE, TEXTURE_COMBAT, 1 * 16 + 10);
}

texture_t* animation_get_texture(texture_id_t id) {
    return (texture_t *)array_list_get(texture_storage, (size_t)id);
}

animation_t* animation_get(animation_id_t id) {
    return (animation_t *)array_list_get(animation_storage, (size_t)id);
}

void animation_define(animation_id_t aid, texture_id_t tid, int index) {
    animation_t anim = (animation_t){
        .id=aid,
        .texture_id=tid,
        .index=index,
    };
    array_list_set(animation_storage, &anim, (size_t)aid);
}

int animation_colorize(animation_t *unit, int player) {
    switch (unit->id) {
        // supports colors
        case ANIMATION_UNIT_SURGEON:
        case ANIMATION_UNIT_SURGEON_UNPACKED:
        case ANIMATION_UNIT_ANIMATOR:
        case ANIMATION_UNIT_ANIMATOR_UNPACKED:
        case ANIMATION_UNIT_PREDATOR:
        case ANIMATION_UNIT_BUTCHER:
        case ANIMATION_UNIT_BURGLAR:
        case ANIMATION_UNIT_ASSASIN:
        case ANIMATION_UNIT_GUARDIAN:
        case ANIMATION_UNIT_GADFLY:
        case ANIMATION_UNIT_MEGOZAVR:
        case ANIMATION_UNIT_CORSAIR:
        case ANIMATION_UNIT_SHARK:
        case ANIMATION_UNIT_GHOST:
        case ANIMATION_UNIT_TORPEDO:
        case ANIMATION_UNIT_KAMIKAZE:
        case ANIMATION_UNIT_SCARAB:
        case ANIMATION_UNIT_TORTOISE:
        case ANIMATION_CITY:
        case ANIMATION_CITY_EMPTY:
            break;
        default:
            return unit->index;
            break;
    }

    int offset = 0;
    if (player == 2)
        offset = 2*16;
    return unit->index + offset;
}
