#pragma once

#include <stdbool.h>

typedef enum {
  K_UNPRESSED,
  K_HOLD,
  K_PRESSED,
  K_RELEASED,
} k_state;

typedef enum {
    KEY_CANCEL,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_LEAVE_WINDOW,
    KEY_PACK_UNPACK,
    KEY_SELECT_NEXT,
    KEY_END_TURN,
    KEY_ACTION,
    KEY_SCALE_INC,
    KEY_SCALE_DEC,
    KEY_SHOW_LOG,
} INPUT_KEY;

void input_keyboard_init();
void input_keyboard_update();
bool input_key(INPUT_KEY k);
bool input_key_hold(INPUT_KEY k);
