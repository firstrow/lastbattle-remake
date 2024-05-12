#include "ui.h"
#include "animation/animation.h"
#include "config/config.h"
#include "game/game.h"
#include "screen/screen.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

TTF_Font *font = NULL;
int font_size = 24;

SDL_Color UI_COLOR_WHITE = {255, 255, 255};
SDL_Color UI_COLOR_BLACK = {0, 0, 0};
SDL_Color UI_COLOR_BLUE = {0, 192, 192};
SDL_Color UI_COLOR_BLUE_LIGHT = {0, 255, 255};

void ui_init() {
  TTF_Init();

  font = TTF_OpenFont(config_get_asset_path("cnc_inet.ttf"), 24);
  if (font == NULL) {
    SDL_LogError(0, "failed to load font");
    abort();
  }
}

void ui_draw_text(int x, int y, char *text, int size) {
  TTF_SetFontSize(font, size);
  SDL_Surface *surface_text =
      TTF_RenderText_Shaded(font, text, UI_COLOR_WHITE, UI_COLOR_BLACK);
  SDL_Texture *message =
      SDL_CreateTextureFromSurface(screen_get_renderer(), surface_text);
  if (message == NULL) {
    SDL_LogError(0, "failed to allocate mem for ui_draw_message");
    abort();
  }

  SDL_Rect message_rect =
      (SDL_Rect){.x = x, .y = y, .w = surface_text->w, .h = surface_text->h};
  SDL_RenderCopy(screen_get_renderer(), message, NULL, &message_rect);

  SDL_FreeSurface(surface_text);
  SDL_DestroyTexture(message);
  TTF_SetFontSize(font, font_size);
}

ui_window *ui_create_window() {
  ui_window *win = SDL_malloc(sizeof(ui_window));
  win->surface = SDL_CreateRGBSurface(0, 600, 600, 32, 0x00ff0000, 0x0000ff00,
                                      0x000000ff, 0xff000000);
  return win;
}

void ui_window_draw(ui_window *win, int x, int y, animation_t *anim) {
  texture_t *texture = animation_get_texture(anim->texture_id);
  int index = animation_colorize(anim, game_player_get()->id);

  int src_x = (index % texture->cols) * texture->cell_w;
  int src_y = floor(index / texture->cols) * texture->cell_h;

  SDL_Rect src_rect = {
      .w = texture->cell_w, .h = texture->cell_h, .x = src_x, .y = src_y};
  SDL_Rect dst_rect = {
      .w = texture->cell_w, .h = texture->cell_h, .x = x, .y = y};

  SDL_BlitScaled(texture->surface, &src_rect, win->surface, &dst_rect);

  if (x + texture->cell_w > win->max_x)
    win->max_x = x + texture->cell_w;
  if (y + texture->cell_h > win->max_y)
    win->max_y = y + texture->cell_h;
}

SDL_Surface *ui_text(char *text, int size, SDL_Color color) {
  if (size == 0)
    size = font_size;
  TTF_SetFontSize(font, size);
  return TTF_RenderText_Solid(font, text, color);
}

void ui_window_draw_surface(ui_window *win, int x, int y,
                            SDL_Surface *surface) {
  SDL_Rect dst_rect = {.w = surface->w, .h = surface->h, .x = x, .y = y};
  SDL_BlitScaled(surface, NULL, win->surface, &dst_rect);

  if (x + surface->w > win->max_x)
    win->max_x = x + surface->w;
  if (y + surface->h > win->max_y)
    win->max_y = y + surface->h;

  SDL_FreeSurface(surface);
}

void ui_present_window(ui_window *win, int x, int y) {
  int bg_width = win->max_x + 60;
  int bg_height = win->max_y + 60;

  texture_t *hud_texture = animation_get_texture(TEXTURE_HUD);
  SDL_Rect src_rect = {
      .w = hud_texture->cell_w, .h = hud_texture->cell_h, .x = 0, .y = 0};
  SDL_Rect dst_rect = {.w = bg_width, .h = bg_height, .x = 0, .y = 0};

  // bg
  SDL_Surface *bg_surface =
      SDL_CreateRGBSurface(0, bg_width, bg_height, 32, 0x00ff0000, 0x0000ff00,
                           0x000000ff, 0xff000000);
  SDL_BlitScaled(hud_texture->surface, &src_rect, bg_surface, &dst_rect);

  // window data
  src_rect = (SDL_Rect){.w = win->max_x, .h = win->max_y, .x = 0, .y = 0};
  dst_rect = (SDL_Rect){.w = win->max_x, .h = win->max_y, .x = 20, .y = 20};
  SDL_BlitScaled(win->surface, &src_rect, bg_surface, &dst_rect);

  // draw texture
  SDL_Texture *texture =
      SDL_CreateTextureFromSurface(screen_get_renderer(), bg_surface);
  dst_rect = (SDL_Rect){.w = bg_width, .h = bg_height, .x = x, .y = y};
  SDL_RenderCopy(screen_get_renderer(), texture, NULL, &dst_rect);

  SDL_FreeSurface(win->surface);
  SDL_FreeSurface(bg_surface);
  SDL_DestroyTexture(texture);
  SDL_free(win);
}

void ui_present_window_center(ui_window *win) {
  int offset = 10;
  int bg_width = win->max_x + offset * 4;
  int bg_height = win->max_y + offset * 4;

  int sw, sc;
  SDL_GetWindowSize(screen_get_window(), &sw, &sc);

  int x = sw / 2 - bg_width / 2;
  int y = sc / 2 - bg_height / 2 - TILE_HEIGHT * 2;

  ui_present_window(win, x, y);
}
