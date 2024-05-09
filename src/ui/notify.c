#include "notify.h"

#include "screen/screen.h"
#include "time/time.h"
#include "ui/ui.h"
#include <string.h>

static time_millis duration = 3000;
static time_millis stop = 0;
static char _str[64];

void ui_notify_draw() {
    if (time_get_millis() > stop)
        return;

    int x, y, w, h;
    screen_get_size(&w, &h);
    x = 42;
    y = h - 60;
    ui_draw_text(x, y, _str, 32);
}

void ui_notify_show(char *text) {
    strcpy(_str, text);
    stop = time_get_millis() + duration;
}
