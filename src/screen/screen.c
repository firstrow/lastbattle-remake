#include "screen/screen.h"

#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "animation/animation.h"
#include "game/game.h"
#include "input/mouse.h"
#include "config/config.h"
#include "map/map.h"
#include "types/types.h"

static struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} SDL;

static double scale;

/* int WINDOW_WIDTH = 2400; */
/* int WINDOW_HEIGHT = 1400; */

int WINDOW_WIDTH = 1798;
int WINDOW_HEIGHT = 1020;

int tile_width;
int tile_height;

static int camera_x, camera_y;

void screen_init() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL.window = SDL_CreateWindow(
        "The Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    /* SDL_SetWindowFullscreen(SDL.window, SDL_WINDOW_FULLSCREEN_DESKTOP); */
    /* SDL_GetWindowSize(SDL.window, &WINDOW_WIDTH, &WINDOW_HEIGHT); */

    // create rendere
    SDL.renderer = SDL_CreateRenderer(
        SDL.window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    // NOTE: might cause rendering artifacts for pixel-art style
    /* const char* scale_quality = "nearest"; */
    /* SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scale_quality); */
    scale = 1;
    tile_width = 58 * scale;
    tile_height = 60 * scale;
}

void screen_clear() {
    SDL_SetRenderDrawColor(SDL.renderer, 10, 10, 10, 0);
    SDL_RenderClear(SDL.renderer);
}

void screen_present() {
    SDL_RenderPresent(SDL.renderer);
}

void screen_destroy() {
    SDL_DestroyWindow(SDL.window);
    SDL_DestroyRenderer(SDL.renderer);
}

SDL_Renderer* screen_get_renderer() {
    return SDL.renderer;
}

SDL_Window* screen_get_window() {
    return SDL.window;
}

// Get x,y coords for tile that is under mouse cursor
vec2 screen_to_map_coords() {
    // cell on screen
    const mouse *m = input_mouse_get();
    int mx = m->x / (tile_width) - (camera_x / tile_width);
    int my = m->y / (tile_height) - (camera_y / tile_height);

    // camera offset
    return (vec2){.x = mx, .y = my};
}

// Returns coordinates of pixels on screen
vec2 screen_map_to_screen(vec2 v) {
    int mx = v.x * tile_width + camera_x;
    int my = v.y * tile_height + camera_y;
    return (vec2){.x = mx, .y = my};
}

void screen_camera_up() {
    vec2 c = game_player_get()->cursor;
    if (c.y > 0) {
        c.y--;
        game_player_get()->cursor.y--;
        int cam_offset = camera_y / tile_height;
        if (c.y <= abs(cam_offset) && c.y > 0)
            camera_y += tile_height;
    }
}

void screen_camera_down() {
    vec2 c = game_player_get()->cursor;
    if (c.y < MAP_HEIGHT-1) {
        c.y++;
        game_player_get()->cursor.y++;
        int tiles_vertically = WINDOW_HEIGHT / tile_height;
        int cam_offset = camera_y / tile_height;
        if (c.y >= abs(cam_offset) + tiles_vertically-1 && c.y < MAP_HEIGHT - 1)
            camera_y -= tile_height;
    }
}

void screen_camera_left() {
    vec2 c = game_player_get()->cursor;
    if (c.x > 0) {
        c.x--;
        game_player_get(c)->cursor = c;
        int cam_offset = camera_x / tile_width;
        if (c.x <= abs(cam_offset) && c.x > 0)
            camera_x += tile_width;
    }
}

void screen_camera_right() {
    vec2 c = game_player_get()->cursor;
    if (c.x < MAP_WIDTH - 1) {
        c.x++;
        game_player_get(c)->cursor = c;
        int tiles_horizontally = WINDOW_WIDTH / tile_width;
        int cam_offset = camera_x / tile_width;
        if (c.x >= abs(cam_offset) + tiles_horizontally-1 && c.x < MAP_WIDTH - 1)
            camera_x -= tile_width;
    }
}

void screen_get_camera_offset(int *x, int *y) {
    *x = camera_x;
    *y = camera_y;
}

void screen_get_size(int *width, int *height) {
    *width = WINDOW_WIDTH;
    *height = WINDOW_HEIGHT;
}

void screen_set_camera_offset(int x, int y) {
    camera_x = x;
    camera_y = y;
}

void screen_set_scale(double s) {
    scale += s;
    tile_width = 58 * scale;
    tile_height = 60 * scale;
    camera_x = 0;
    camera_y = 0;
    screen_move_to(game_player_get()->cursor);
}

double screen_get_scale() {
    return scale;
}

void screen_move_to(vec2 pos) {
    int tiles_horizontally = WINDOW_WIDTH / tile_width;
    int tiles_vertically = WINDOW_HEIGHT / tile_height;
    int tx = (pos.x - (tiles_horizontally / 2));
    int ty = (pos.y - (tiles_vertically / 2));
    if (tx < 0 ) tx = 0;
    if (ty < 0 ) ty = 0;

    if (tx + tiles_horizontally > MAP_WIDTH)
        tx = MAP_WIDTH - tiles_horizontally;
    if (ty + tiles_vertically > MAP_HEIGHT)
        ty = MAP_HEIGHT - tiles_vertically;

    camera_x = tx * tile_width * -1;
    camera_y = ty * tile_height * -1;
}

