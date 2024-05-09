#ifndef UI_H_
#define UI_H_

#include "animation/animation.h"

#include <SDL2/SDL.h>

extern SDL_Color UI_COLOR_WHITE;
extern SDL_Color UI_COLOR_BLACK;
extern SDL_Color UI_COLOR_BLUE;
extern SDL_Color UI_COLOR_BLUE_LIGHT;

typedef struct {
    SDL_Surface *surface;
    int max_x;
    int max_y;
} ui_window;

typedef struct {
    char *text;
} ui_button;

void ui_init();
void ui_draw_text(int x, int y, char *text, int size);
ui_window *ui_create_window();
void ui_window_draw(ui_window *win, int x, int y, animation_t *anim);
void ui_present_window(ui_window *win, int x, int y);
void ui_present_window_center(ui_window *win);
SDL_Surface* ui_text(char *text, int size, SDL_Color color);
void ui_window_draw_surface(ui_window *win, int x, int y, SDL_Surface *surface);

#endif // UI_H_
