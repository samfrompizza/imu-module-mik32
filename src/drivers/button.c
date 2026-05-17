#include "button.h"
#include "board_config.h"
#include "mik32_hal_gpio.h"

static volatile uint8_t s_stable_level   = 0;
static volatile uint8_t s_match_counter  = 0;
static volatile uint8_t s_last_sample    = 0;
static volatile uint8_t s_pending_press  = 0;

static inline uint8_t read_raw_level(void)
{
    return (BTN_PIN_PORT->STATE >> BTN_PIN_NUM) & 0x1U;
}

void button_init(void)
{
    GPIO_InitTypeDef io = {0};
    io.Pin  = (1U << BTN_PIN_NUM);
    io.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    io.Pull = HAL_GPIO_PULL_DOWN;
    HAL_GPIO_Init(BTN_PIN_PORT, &io);

    s_stable_level  = read_raw_level();
    s_match_counter = 0;
    s_last_sample   = s_stable_level;
    s_pending_press = 0;
}

void button_sample(void)
{
    uint8_t level = read_raw_level();

    if (level == s_last_sample) {
        if (s_match_counter < BUTTON_DEBOUNCE_SAMPLES) {
            s_match_counter++;
        }
    } else {
        s_last_sample   = level;
        s_match_counter = 1;
    }

    if (s_match_counter >= BUTTON_DEBOUNCE_SAMPLES && level != s_stable_level) {
        if (s_stable_level == 0 && level == 1) {
            s_pending_press = 1;
        }
        s_stable_level = level;
    }
}

bool button_take_press(void)
{
    if (s_pending_press) {
        s_pending_press = 0;
        return true;
    }
    return false;
}
