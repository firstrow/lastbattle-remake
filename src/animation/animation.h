#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>

// enum of all textures
typedef enum {
    TEXTURE_TILEMAP = 1,
    TEXTURE_COMBAT = 2,
    TEXTURE_HUD = 3,
} texture_id_t;

typedef enum {
    ANIMATION_WATER,
    ANIMATION_SELECTED_TILE,
    ANIMATION_CURSOR_SELECTED,

    ANIMATION_UNIT_SURGEON,
    ANIMATION_UNIT_ANIMATOR,
    ANIMATION_UNIT_PREDATOR,
    ANIMATION_UNIT_BUTCHER,
    ANIMATION_UNIT_BURGLAR,
    ANIMATION_UNIT_ASSASIN,
    ANIMATION_UNIT_GUARDIAN,
    ANIMATION_UNIT_GADFLY,
    ANIMATION_UNIT_MEGOZAVR,
    ANIMATION_UNIT_CORSAIR,
    ANIMATION_UNIT_SHARK,
    ANIMATION_UNIT_GHOST,
    ANIMATION_UNIT_TORPEDO,
    ANIMATION_UNIT_KAMIKAZE,
    ANIMATION_UNIT_SCARAB,
    ANIMATION_UNIT_TORTOISE,

    ANIMATION_UNIT_SURGEON_UNPACKED,
    ANIMATION_UNIT_ANIMATOR_UNPACKED,

    ANIMATION_HUD,
    ANIMATION_CITY,
    ANIMATION_CITY_EMPTY,
    ANIMATION_CITY_ALIEN,
    ANIMATION_BLACK_TILE,
    ANIMATION_FOG_TILE,
} animation_id_t;

typedef struct {
    // user defined before loading
    texture_id_t id;
    char* filename;
    int cell_w; // grid cell width
    int cell_h; // grid cell height
    int cols;
    int rows;

    // autoloaded
    SDL_Texture *texture;
    SDL_Surface *surface;
} texture_t;

typedef struct {
    animation_id_t id;
    texture_id_t texture_id;
    int index;
} animation_t;

void animation_load_all();
void animation_define(animation_id_t aid, texture_id_t tid, int index);
animation_t* animation_get(animation_id_t id);
texture_t* animation_get_texture(texture_id_t id);
int animation_colorize(animation_t *unit, int player);

#endif // ANIMATION_H_
