#include "log.h"
#include "array_list/array_list.h"
#include <SDL2/SDL_stdinc.h>

static array_list *storage;
static array_list *finalized;

void game_log_init() {
  storage = array_list_create(sizeof(char) * 100, 64);
  finalized = array_list_create(sizeof(char) * 100, 64);
}

void game_log(char *text, vec2 where) {
  game_log_msg m = {.pos = where};
  SDL_strlcpy(m.msg, text, 100);
  array_list_append(storage, &m);
}

void game_log_finalize() {
  array_list_reset(finalized);

  for (int i = 0; i < storage->len; i++)
    array_list_append(finalized, array_list_get(storage, i));

  array_list_reset(storage);
}

array_list *game_log_finalized_get() { return finalized; }
