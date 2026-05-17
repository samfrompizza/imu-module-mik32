#include "mode_compass.h"
#include "../drivers/display_service.h"
#include "../../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306_fonts.h"

#include <math.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define COMPASS_ARROW_LENGTH (24.0f)
#define COMPASS_ARROWHEAD_LENGTH (8.0f)
#define COMPASS_ARROWHEAD_HALF_ANG (0.5f)
#define COMPASS_DIAL_RADIUS (28U)

#define COMPASS_LABEL_RADIUS_OFFSET (6.0f)
#define COMPASS_NORTH_LABEL "N"

static inline int round_to_int(float v)
{
    return (int)(v + (v >= 0.0f ? 0.5f : -0.5f));
}

static void draw_dial(HAL_SSD1306_HandleTypeDef *display)
{
    ssd1306_DrawCircle(display,
                       DISPLAY_CENTER_U, DISPLAY_CENTER_V,
                       COMPASS_DIAL_RADIUS, White);
}

static void draw_arrow_shaft(HAL_SSD1306_HandleTypeDef *display,
                             int tip_u, int tip_v)
{
    ssd1306_Line(display,
                 (uint8_t)DISPLAY_CENTER_U, (uint8_t)DISPLAY_CENTER_V,
                 (uint8_t)tip_u, (uint8_t)tip_v,
                 White);
}

static void draw_arrow_head(HAL_SSD1306_HandleTypeDef *display,
                            int tip_u, int tip_v,
                            float du, float dv)
{
    float back_u = -du;
    float back_v = -dv;
    float c = cosf(COMPASS_ARROWHEAD_HALF_ANG);
    float s = sinf(COMPASS_ARROWHEAD_HALF_ANG);

    float left_u = back_u * c - back_v * s;
    float left_v = back_u * s + back_v * c;
    float right_u = back_u * c + back_v * s;
    float right_v = -back_u * s + back_v * c;

    int lu = tip_u + round_to_int(left_u * COMPASS_ARROWHEAD_LENGTH);
    int lv = tip_v + round_to_int(left_v * COMPASS_ARROWHEAD_LENGTH);
    int ru = tip_u + round_to_int(right_u * COMPASS_ARROWHEAD_LENGTH);
    int rv = tip_v + round_to_int(right_v * COMPASS_ARROWHEAD_LENGTH);

    ssd1306_Line(display, (uint8_t)tip_u, (uint8_t)tip_v,
                 (uint8_t)lu, (uint8_t)lv, White);
    ssd1306_Line(display, (uint8_t)tip_u, (uint8_t)tip_v,
                 (uint8_t)ru, (uint8_t)rv, White);
}

static void draw_north_label(HAL_SSD1306_HandleTypeDef *display,
                             float du, float dv)
{
    int label_u = DISPLAY_CENTER_U + round_to_int(du * (COMPASS_ARROW_LENGTH + COMPASS_LABEL_RADIUS_OFFSET)) - (Font_6x8.width / 2);
    int label_v = DISPLAY_CENTER_V + round_to_int(dv * (COMPASS_ARROW_LENGTH + COMPASS_LABEL_RADIUS_OFFSET)) - (Font_6x8.height / 2);

    if (label_u < 0)
        label_u = 0;
    if (label_v < 0)
        label_v = 0;
    if (label_u >= DISPLAY_WIDTH - Font_6x8.width)
        label_u = DISPLAY_WIDTH - Font_6x8.width - 1;
    if (label_v >= DISPLAY_HEIGHT - Font_6x8.height)
        label_v = DISPLAY_HEIGHT - Font_6x8.height - 1;

    ssd1306_SetCursor(display, (uint8_t)label_u, (uint8_t)label_v);
    ssd1306_WriteString(display, (char *)COMPASS_NORTH_LABEL, Font_6x8, White);
}

static void compass_enter(void) {}

static void compass_render(HAL_SSD1306_HandleTypeDef *display,
                           const imu_frame_t *frame)
{
    ssd1306_Fill(display, Black);
    draw_dial(display);

    if (frame == NULL)
    {
        return;
    }

    float h = frame->heading_rad;
    float du = -sinf(h);
    float dv = -cosf(h);

    int tip_u = DISPLAY_CENTER_U + round_to_int(du * COMPASS_ARROW_LENGTH);
    int tip_v = DISPLAY_CENTER_V + round_to_int(dv * COMPASS_ARROW_LENGTH);

    draw_arrow_shaft(display, tip_u, tip_v);
    draw_arrow_head(display, tip_u, tip_v, du, dv);
    draw_north_label(display, du, dv);
}

const mode_t MODE_COMPASS = {
    .name = "compass",
    .enter = compass_enter,
    .render = compass_render,
};
