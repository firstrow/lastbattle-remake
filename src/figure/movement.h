#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <stdbool.h>

#include "types/types.h"
#include "figure/figure.h"

void figure_movement_move(figure *f, vec2 coords);
void figure_movement_update(figure *f, vec2 pos);
void figure_movement_toggle_pack(figure *f);
bool figure_movement_can_move(figure *f, vec2 coords);

#endif // MOVEMENT_H_
