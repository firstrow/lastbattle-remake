#ifndef SAVE_H_
#define SAVE_H_

#include "buffer/buffer.h"

typedef struct {
    buffer *figures;
    buffer *game_state;
    buffer *game_fog;
} save_game_state;

void save_state(char *file);
void save_load(char *file);

#endif // SAVE_H_
