#include <stdint.h>
#include "board_config.h"
#include "oled_ssd1306.h"

static inline void oled_cs_low(void);
static inline void oled_cs_high(void);
static inline void oled_dc_low(void);
static inline void oled_dc_high(void);
static inline void oled_rst_low(void);
static inline void oled_rst_high(void);

static uint8_t fb[128 * 64 / 8];