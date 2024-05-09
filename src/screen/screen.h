#ifndef SCREEN_H_
#define SCREEN_H_

#include <SDL2/SDL.h>

#include "types/types.h"
#include "animation/animation.h"

#define TILE_WIDTH 58
#define TILE_HEIGHT 60

void screen_init();
void screen_destroy();
void screen_clear();
void screen_present();
vec2 screen_to_map_coords();
vec2 screen_map_to_screen(vec2); // returns coords on screen for specific cell
SDL_Renderer* screen_get_renderer();
SDL_Window* screen_get_window();
void screen_get_camera_offset(int *x, int *y);
void screen_set_camera_offset(int x, int y);
void screen_get_size(int *width, int *height);
void screen_set_scale(double s);
double screen_get_scale();
void screen_move_to(vec2 pos);

// camera
void screen_camera_up();
void screen_camera_down();
void screen_camera_left();
void screen_camera_right();

// drawing
void screen_draw_on_tile(animation_t* anim, int index, vec2 tile);
void screen_draw(animation_t* animation, int index, vec2 pixel_coords);

#endif // SCREEN_H_
