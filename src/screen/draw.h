#ifndef DRAW_H_
#define DRAW_H_

#include "types/types.h"
#include "animation/animation.h"

void screen_draw_on_tile(animation_t* anim, int anim_index, vec2 tile);
void screen_draw(animation_t* anim, int index, vec2 pixel_coords);

#endif // DRAW_H_
