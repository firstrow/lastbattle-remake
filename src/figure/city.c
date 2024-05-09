#include <SDL2/SDL_log.h>
#include <stdio.h>
#include <stdlib.h>

#include "city.h"
#include "figure/figure.h"
#include "game/log.h"
#include "types/types.h"

#include <SDL2/SDL.h>

// FIXME: select by asc or desc shield
figure *figure_city_select_defender(figure *city) {
  if (city->type != FIGURE_CITY)
    SDL_LogError(0, "figure %d is not 'city type", city->id);

  for (int i = 1; i < MAX_FIGURES; i++) {
    figure *f = figure_get(i);
    if (!f->type)
      continue;
    if (f->stored_in_id == city->id)
      return f;
  }

  return city;
}

void figure_city_make_order(figure *city, figure_unit u) {
  if (city->city_order == u)
    return;

  figure unit = figure_unit_get(u);

  city->city_order_enabled = true;
  city->city_order = u;
  city->city_order_days_left = unit.days_to_make;
}

void figure_city_order_forbid(figure *city) {
  city->city_order_enabled = false;
  city->city_order = -1;
  city->city_order_days_left = 0;
}

figure *figure_city_order_update(figure *city) {
  if (city->type != FIGURE_CITY)
    return NULL;
  if (!city->city_order_enabled)
    return NULL;

  city->city_order_days_left--;
  if (city->city_order_days_left == 0) {
    if (figure_stored_units(city) >= city->can_store_units) {
      city->city_order_days_left = 1;

      char buf[50];
      sprintf(buf, "city full. can not build more units.");
      game_log(buf, city->pos);

      return NULL;
    }

    figure *f = figure_create(city->city_order, city->pos, city->player_id);
    f->stored_in_id = city->id;
    f->pos = city->pos;
    f->course = city->course;

    figure_unit order = city->city_order;
    city->city_order = -1;
    figure_city_make_order(city, order);

    char buf[50];
    sprintf(buf, "new unit built: %s", f->name);
    game_log(buf, city->pos);
  }

  return NULL;
}
