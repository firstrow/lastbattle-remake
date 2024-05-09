#include "draw.h"
#include "screen.h"

void screen_draw_on_tile(animation_t* anim, int anim_index, vec2 tile) {
    screen_draw(anim, anim_index, screen_map_to_screen(tile));
}

void screen_draw(animation_t* anim, int index, vec2 pixel_coords) {
    texture_t *texture = animation_get_texture(anim->texture_id);
    int src_x = (index % texture->cols) * texture->cell_w;
    int src_y = floor(index / texture->cols) * texture->cell_h;

    double scale = screen_get_scale();

    SDL_Rect src_rect = {.w=texture->cell_w, .h=texture->cell_h, .x=src_x, .y=src_y};
    SDL_Rect dst_rect = {.w=texture->cell_w*scale, .h=texture->cell_h*scale, .x=pixel_coords.x, .y=pixel_coords.y};
    SDL_RenderCopy(screen_get_renderer(), texture->texture, &src_rect, &dst_rect);
}
