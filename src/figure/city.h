#ifndef CITY_H_
#define CITY_H_

#include "figure/figure.h"

figure* figure_city_select_defender(figure *city);
void figure_city_make_order(figure *city, figure_unit u);
void figure_city_order_forbid(figure *city);
figure* figure_city_order_update(figure *city);

#endif // CITY_H_
