#ifndef HANDLE_H_
#define HANDLE_H_

#include <SDL2/SDL.h>

typedef void (*event_cb_t)();

typedef struct {
    event_cb_t input_callbacks[8];
    int pos;
} handler_state;

void handle_init();
void handle_keyboard_input();
void handle_push_callback(event_cb_t cb);
void handle_pop_callback();

#endif // HANDLE_H_
