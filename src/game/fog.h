#ifndef FOG_H_
#define FOG_H_

#include <stdbool.h>
#include "animation/animation.h"
#include "types/types.h"
#include "figure/figure.h"

typedef struct {
    vec2 tile;
    bool visible;
    bool visited;
    int player_id;
    animation_id_t animation_id;
} fog_cell;

void game_fog_update(int player_id);
void game_fog_draw();
void game_fog_toggle();
bool game_fog_can_see(vec2 where);
void game_fog_reset(int player_id);
void game_fog_set(int x, int y, int player_id, fog_cell c);
fog_cell game_fog_get(int x, int y, int player_id);

#endif // FOG_H_
