#include "mode_welcome.h"
#include "../drivers/display_service.h"
#include "../../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306_fonts.h"

#include <stdint.h>

#define WELCOME_TITLE_TEXT "IMU MODULE"
#define WELCOME_HINT_TEXT "Press BTN to start"

#define WELCOME_TITLE_X (8)
#define WELCOME_TITLE_Y (16)
#define WELCOME_HINT_X (10)
#define WELCOME_HINT_Y (44)

#define WELCOME_FRAME_PADDING (2)

static void draw_frame_border(HAL_SSD1306_HandleTypeDef *display)
{
    ssd1306_DrawRectangle(display,
                          WELCOME_FRAME_PADDING,
                          WELCOME_FRAME_PADDING,
                          DISPLAY_WIDTH - 1 - WELCOME_FRAME_PADDING,
                          DISPLAY_HEIGHT - 1 - WELCOME_FRAME_PADDING,
                          White);
}

static void draw_static_text(HAL_SSD1306_HandleTypeDef *display)
{
    ssd1306_SetCursor(display, WELCOME_TITLE_X, WELCOME_TITLE_Y);
    ssd1306_WriteString(display, (char *)WELCOME_TITLE_TEXT, Font_11x18, White);

    ssd1306_SetCursor(display, WELCOME_HINT_X, WELCOME_HINT_Y);
    ssd1306_WriteString(display, (char *)WELCOME_HINT_TEXT, Font_6x8, White);
}

static void welcome_enter(void)
{
    // nothing to do
}

static void welcome_render(HAL_SSD1306_HandleTypeDef *display,
                           const imu_frame_t *frame)
{
    (void)frame;
    ssd1306_Fill(display, Black);
    draw_frame_border(display);
    draw_static_text(display);
}

const mode_t MODE_WELCOME = {
    .name = "welcome",
    .enter = welcome_enter,
    .render = welcome_render,
};
