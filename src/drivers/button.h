#ifndef DRIVERS_BUTTON_H
#define DRIVERS_BUTTON_H

#include <stdbool.h>

#define BUTTON_DEBOUNCE_SAMPLES   (3U)

void button_init(void);
void button_sample(void);
bool button_take_press(void);

#endif
