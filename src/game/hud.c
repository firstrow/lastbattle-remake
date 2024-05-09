#include "hud.h"
#include <SDL2/SDL_stdinc.h>
#include <stdbool.h>

#include "animation/animation.h"
#include "array_list/array_list.h"
#include "figure/city.h"
#include "figure/figure.h"
#include "figure/movement.h"
#include "game/course.h"
#include "game/game.h"
#include "handle/handle.h"
#include "input/keyboard.h"
#include "screen/screen.h"
#include "ui/ui.h"

#include <SDL2/SDL_surface.h>

typedef void (*menu_item_callback)();
typedef struct {
    int index;
    char *text;
    menu_item_callback cb;
} menu_item;

static menu_item menu_items[10] = {0};
int menu_selected = 0;
int menu_total_items = 0;
figure *menu_selected_figure;

int leave_window_visible = 0;
int leave_window_selected = 0;
figure *leave_city = NULL;
figure *leave_window_unit = NULL;

int order_window_selected = 0;
figure *order_window_city = NULL;

static void draw_order_window() {
    require_fig(order_window_city);

    int idx = 0;
    ui_window *win = ui_create_window();

    figure unit;
    for (int i = 0; i < order_window_city->city_order_choices_len; i++) {
        int pos = idx * TILE_WIDTH + 10;
        unit = figure_unit_get(order_window_city->city_order_choices[i]);
        ui_window_draw(win, pos, 0, animation_get(unit.animation_id));
        if (order_window_selected % order_window_city->city_order_choices_len == idx)
            ui_window_draw(win, pos, 0, animation_get(ANIMATION_SELECTED_TILE));
        idx++;
    }

    ui_present_window_center(win);
}

static void draw_leave_window() {
    require_fig(leave_city);

    int idx = 0;
    ui_window *win = ui_create_window();

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *f = figure_get(i);
        if (!f->type)
            continue;
        if (f->stored_in_id != leave_city->id)
            continue;

        int pos = idx * TILE_WIDTH + 10;
        ui_window_draw(win, pos, 0, animation_get(f->animation_id));
        if (leave_window_selected % figure_stored_units(leave_city) == idx) {
            leave_window_unit = f;
            ui_window_draw(win, pos, 0, animation_get(ANIMATION_SELECTED_TILE));
        }

        idx++;
    }

    ui_present_window_center(win);
}

static void menu_hide() {
    menu_total_items = 0;
    handle_pop_callback();
}

static void leave_window_hide() {
    game_set_state_selected_figure(leave_city);
    handle_pop_callback();
    leave_city = NULL;
    leave_window_visible = 0;
}

static void order_window_hide() {
    order_window_city = NULL;
    handle_pop_callback();
}

static void order_window_callback() {
    if (input_key(KEY_CANCEL))
        order_window_hide();

    if (input_key(KEY_RIGHT))
        order_window_selected++;

    if (input_key(KEY_LEFT)) {
        order_window_selected--;
        if (order_window_selected <= 0)
            order_window_selected = order_window_city->city_order_choices_len;
    }

    if (input_key(KEY_ACTION)) {
        figure_unit u = order_window_city->city_order_choices[order_window_selected % order_window_city->city_order_choices_len];
        figure_city_make_order(order_window_city, u);
        order_window_hide();
        menu_hide();
        game_set_state_selected_figure(NULL);
    }
}

static void leave_window_callback() {
    if (input_key(KEY_CANCEL)) {
        leave_window_hide();
    }

    if (input_key(KEY_RIGHT))
        leave_window_selected++;
    if (input_key(KEY_LEFT)) {
        leave_window_selected--;
        if (leave_window_selected <= 0)
            leave_window_selected = figure_stored_units(menu_selected_figure);
    }

    if (input_key(KEY_ACTION)) {
        leave_window_hide();
        menu_hide();
        game_set_state_selected_figure(leave_window_unit);
    }
}

void game_hud_order_show() {
    if (order_window_city != NULL)
        return;

    figure *city = game_selected_figure();
    require_fig(city);

    if (game_state_get()->current_player != city->player_id)
        return;

    order_window_city = city;
    order_window_selected = 0;
    handle_push_callback(&order_window_callback);
}

void game_hud_leave_show() {
    if (leave_window_visible)
        return;

    figure *city = game_selected_figure();
    require_fig(city);

    if (game_state_get()->current_player != city->player_id)
        return;

    if (!figure_stored_units(city))
        return;

    leave_city = city;
    leave_window_visible = 1;
    leave_window_selected = 0;
    handle_push_callback(&leave_window_callback);

    if (figure_stored_units(city) == 1) {
        leave_window_hide();
        menu_hide();

        for (int i = 1; i < MAX_FIGURES; i++) {
            figure *f = figure_get(i);
            if (!f->type)
                continue;
            if (f->stored_in_id != city->id)
                continue;

            game_set_state_selected_figure(f);
            break;
        }
    }
}

static void menu_register(char *text, menu_item_callback cb) {
    menu_items[menu_total_items] = (menu_item){
        .text = text,
        .cb = cb,
    };
    menu_total_items++;
}

static void menu_callback() {
    if (input_key(KEY_CANCEL))
        menu_hide();
    if (input_key(KEY_SHOW_LOG))
        menu_hide();

    if (input_key(KEY_DOWN))
        menu_selected++;
    if (input_key(KEY_UP)) {
        menu_selected--;
        if (menu_selected <= 0)
            menu_selected = menu_total_items;
    }

    if (input_key(KEY_ACTION)) {
        if (menu_total_items == 0)
            return;

        short index = menu_selected % menu_total_items;
        menu_items[index].cb();
    }
}

static void draw_menu() {
    if (!menu_total_items)
        return;

    int step = 30;

    ui_window *win = ui_create_window();
    short index = menu_selected % menu_total_items;

    for (int i = 0; i < menu_total_items; i++) {
        if (menu_items[i].text == NULL)
            continue;

        if (i == index)
            ui_window_draw_surface(win, 0, i*step, ui_text("->", 0, UI_COLOR_WHITE));
        ui_window_draw_surface(win, 24, i*step, ui_text(menu_items[i].text, 0, UI_COLOR_WHITE));

    }

    ui_present_window_center(win);
}

static void menu_strip_selected() {
    figure *city = figure_get(menu_selected_figure->stored_in_id);

    for (int i = 0; i < city->city_order_choices_len; i++) {
        if (menu_selected_figure->unit == city->city_order_choices[i])
            return;
    }

    city->city_order_choices[city->city_order_choices_len] = menu_selected_figure->unit;
    city->city_order_choices_len++;

    figure_delete(menu_selected_figure);
    game_set_state_selected_figure(NULL);
    menu_hide();
}

static void menu_explode_selected() {
    if (menu_selected_figure->action_points == 0)
        return;

    int x, y;
    x = menu_selected_figure->pos.x;
    y = menu_selected_figure->pos.y;

    for (int i = 1; i < MAX_FIGURES; i++) {
        figure *unit = figure_get(i);
        if (unit->pos.x == x && unit->pos.y == y)
            figure_delete(unit);
    }

    game_set_state_selected_figure(NULL);
    menu_hide();
}

static void menu_pack_selected() {
    figure_movement_toggle_pack(menu_selected_figure);
    game_set_state_selected_figure(NULL);
    menu_hide();
}

static void menu_forbid_selected() {
    figure_city_order_forbid(menu_selected_figure);
    game_set_state_selected_figure(NULL);
    menu_hide();
}

static void menu_course_selected() {
    menu_hide();
    game_course_set(menu_selected_figure);
}

static void menu_course_clear() {
    menu_hide();
    game_course_cancel(menu_selected_figure);
}

void game_hud_menu_show() {
    if (menu_total_items > 0)
        return;

    menu_selected_figure = game_selected_figure();
    require_fig(menu_selected_figure);

    menu_selected = 0;
    menu_total_items = 0;
    SDL_memset(&menu_items, 0, sizeof(menu_items));

    if (menu_selected_figure->type == FIGURE_CITY) {
        menu_register("ORDER", &game_hud_order_show);
        menu_register("LEAVE", &game_hud_leave_show);
        menu_register("FORBID", &menu_forbid_selected);

        if (menu_selected_figure->course.x && menu_selected_figure->course.y)
            menu_register("DEPLOY CLEAR", &menu_course_clear);
        else
            menu_register("DEPLOY", &menu_course_selected);
    } else {
        menu_register("CANCEL", &menu_hide);
        if (menu_selected_figure->can_store_units > 0 && figure_stored_units(menu_selected_figure))
            menu_register("UNLOAD", &game_hud_leave_show);

        if (menu_selected_figure->unit == FIGURE_UNIT_SURGEON || menu_selected_figure->unit == FIGURE_UNIT_ANIMATOR) {
            if (menu_selected_figure->can_store_units)
                menu_register("PACK", &menu_pack_selected);
            else
                menu_register("UNPACK", &menu_pack_selected);
        }

        if (menu_selected_figure->course.x && menu_selected_figure->course.y)
            menu_register("COURSE CLEAR", &menu_course_clear);
        else
            menu_register("COURSE", &menu_course_selected);

        if (menu_selected_figure->stored_in_id) {
            figure *storage = figure_get(menu_selected_figure->stored_in_id);
            if (storage->type == FIGURE_CITY) {
                menu_register("STRIP", &menu_strip_selected);
                menu_register("EXPLODE", &menu_explode_selected);
            }
        }
    }

    if (menu_total_items == 0)
        return;

    handle_push_callback(&menu_callback);
}

static void log_visit() {
    if (menu_total_items == 0)
        return;

    array_list *glog = game_log_finalized_get();
    int i = menu_selected % menu_total_items;
    game_log_msg *m = (game_log_msg *)array_list_get(glog, i);
    screen_move_to(m->pos);
    game_player_get()->cursor = m->pos;
}

void game_hud_log_menu_show() {
    if (menu_total_items > 0)
        return;

    array_list *glog = game_log_finalized_get();
    if (glog->len == 0)
        return;

    menu_selected = 0;
    menu_total_items = 0;

    SDL_memset(&menu_items, 0, sizeof(menu_items));

    for (int i = 0; i < glog->len; i++) {
        game_log_msg *m = (game_log_msg *)array_list_get(glog, i);
        menu_register(m->msg, &log_visit);
    }

    handle_push_callback(&menu_callback);
}

static void draw_sf_info() {
    figure *sf = game_selected_figure();

    int step = 30;
    char buf[20];
    int i = -1;

    int x = 50;
    int y = 50;

    SDL_Color color = UI_COLOR_BLUE_LIGHT;

    if (game_player_get()->cursor.x <= 3 && game_player_get()->cursor.y <= 3)
        y = 60 * 5;

    if (sf == NULL)
        sf = figure_at(game_player_get()->cursor);

    if (sf && sf->player_id != game_player_get()->id)
        sf = NULL;

    if (sf == NULL) {
        ui_window *win = ui_create_window();
        sprintf(buf, "Player: %d", game_player_get()->id);
        ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));

        sprintf(buf, "Day: %d", game_state_get()->day);
        ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));

        sprintf(buf, "X: %d Y: %d", game_player_get()->cursor.x, game_player_get()->cursor.y);
        ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));

        ui_present_window(win, x, y);
        return;
    }

    if (!sf->type) {
        return;
    }

    ui_window *win = ui_create_window();

    if (sf && sf->type == FIGURE_CITY) {
        if (sf->city_order_enabled) {
            figure unit = figure_unit_get(sf->city_order);
            sprintf(buf, "Order: %s", unit.name);
            ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));

            sprintf(buf, "Days: %d", sf->city_order_days_left);
            ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));
        } else {
            sprintf(buf, "Order: none");
            ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));
        }

        sprintf(buf, "Robots: %d", figure_stored_units(sf));
        ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));

        if (sf->course.x > 0 || sf->course.y > 0) {
            sprintf(buf, "Deploy: %d : %d", sf->course.x, sf->course.y);
            ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));
        }
    }

    if (sf && sf->type != FIGURE_CITY) {
        sprintf(buf, "APs: %d", sf->action_points);
        ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 32, color));

        sprintf(buf, "Shield: %d / %d", sf->shield, sf->max_shield);
        ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));

        if (sf->course.x > 0 || sf->course.y > 0) {
            sprintf(buf, "Course: %d : %d", sf->course.x, sf->course.y);
            ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));
        }

        if (sf->unit == FIGURE_UNIT_SCARAB || sf->unit == FIGURE_UNIT_TORTOISE) {
            sprintf(buf, "Robots: %d", figure_stored_units(sf));
            ui_window_draw_surface(win, 5, ++i*step, ui_text(buf, 26, color));
        }
    }

    ui_present_window(win, x, y);
}

void game_hud_draw() {
    draw_sf_info();
    draw_menu();
    draw_leave_window();
    draw_order_window();

    // debugging
    /* sprintf(str, "Player: %d", game_state_get()->current_player); */
    /* ui_draw_text(24, ++i*step, str, 28); */
    /* sprintf(str, "Day: %d", game_state_get()->day); */
    /* ui_draw_text(24, ++i*step, str, 28); */

    /* ++i; */
    /* figure *c = figure_at(game_player_get()->cursor); */
    /* if (c && c->type == FIGURE_CITY) { */

    /*     sprintf(str, "orders: %d", c->city_order_choices_len); */
    /*     ui_draw_text(24, ++i*step, str, 28); */

    /*     for (int j = 0; j < c->city_order_choices_len; j++) { */
    /*         figure u = figure_unit_get(c->city_order_choices[j]); */
    /*         sprintf(str, "- %s", u.name); */
    /*         ui_draw_text(24, ++i*step, str, 28); */
    /*     } */
    /* } */

    /* if (sf->type == FIGURE_CITY) { */
    /*     sprintf(str, "CITY:"); */
    /*     ui_draw_text(24, ++i*step, str, 28); */

    /*     sprintf(str, "units: %d", figure_stored_units(sf)); */
    /*     ui_draw_text(24, ++i*step, str, 28); */
    /* } else { */
    /*     ui_draw_text(24, ++i*step, sf->name, 28); */

    /*     sprintf(str, "APs: %d", sf->action_points); */
    /*     ui_draw_text(24, ++i*step, str, 28); */

    /*     sprintf(str, "unit: #%d", sf->id); */
    /*     ui_draw_text(24, ++i*step, str, 28); */

    /*     sprintf(str, "shield: %d", sf->shield); */
    /*     ui_draw_text(24, ++i*step, str, 28); */

    /*     sprintf(str, "stored in: %d", sf->stored_in_id); */
    /*     ui_draw_text(24, ++i*step, str, 28); */

    /*     sprintf(str, "holds: %d", figure_stored_units(sf)); */
    /*     ui_draw_text(24, ++i*step, str, 28); */
    /* } */
}
