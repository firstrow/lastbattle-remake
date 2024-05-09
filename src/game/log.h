#ifndef LOG_H_
#define LOG_H_

#include "array_list/array_list.h"
#include "types/types.h"

typedef struct {
    vec2 pos;
    char msg[128];
} game_log_msg;

void game_log_init();
void game_log_finalize();
void game_log(char *text, vec2 where);
array_list *game_log_finalized_get();

#endif // LOG_H_
