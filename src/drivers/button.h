#ifndef DRIVERS_BUTTON_H
#define DRIVERS_BUTTON_H

#include <stdbool.h>

#define BUTTON_DEBOUNCE_SAMPLES (3U)

void button_init();
void button_sample();
bool button_take_press();

#endif
