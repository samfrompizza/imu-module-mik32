#include "mode_debug.h"
#include "../drivers/display_service.h"
#include "../../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306_fonts.h"

#include <stdint.h>
#include <stdbool.h>

#define DEBUG_LINE_HEIGHT (10)
#define DEBUG_LINE_X (0)
#define DEBUG_LINE0_Y (0)
#define DEBUG_MAX_LINE_LEN (22)

static char *append_string(char *p, const char *s)
{
    while (*s)
    {
        *p++ = *s++;
    }
    return p;
}

static char *append_signed4(char *p, int16_t value)
{
    uint16_t magnitude;
    if (value < 0)
    {
        *p++ = '-';
        magnitude = (uint16_t)(-(int32_t)value);
    }
    else
    {
        *p++ = '+';
        magnitude = (uint16_t)value;
    }
    if (magnitude > 9999U)
    {
        magnitude = 9999U;
    }
    *p++ = (char)('0' + (magnitude / 1000U) % 10U);
    *p++ = (char)('0' + (magnitude / 100U) % 10U);
    *p++ = (char)('0' + (magnitude / 10U) % 10U);
    *p++ = (char)('0' + (magnitude) % 10U);
    return p;
}

static char *append_digit(char *p, uint8_t digit)
{
    if (digit > 9U)
        digit = 9U;
    *p++ = (char)('0' + digit);
    return p;
}

static void write_line(HAL_SSD1306_HandleTypeDef *display, uint8_t row, const char *text)
{
    ssd1306_SetCursor(display, DEBUG_LINE_X, DEBUG_LINE0_Y + row * DEBUG_LINE_HEIGHT);
    ssd1306_WriteString(display, (char *)text, Font_6x8, White);
}

static void format_axis_line(char *buf, const char *label,
                             int16_t x, int16_t y, int16_t z)
{
    char *p = buf;
    p = append_string(p, label);
    p = append_string(p, " X");
    p = append_signed4(p, x);
    p = append_string(p, " Y");
    p = append_signed4(p, y);
    p = append_string(p, " Z");
    p = append_signed4(p, z);
    *p = '\0';
}

static void format_euler_line(char *buf, int16_t h, int16_t r, int16_t p_angle)
{
    char *p = buf;
    p = append_string(p, "EUL H");
    p = append_signed4(p, h);
    p = append_string(p, " R");
    p = append_signed4(p, r);
    p = append_string(p, " P");
    p = append_signed4(p, p_angle);
    *p = '\0';
}

static void format_calib_line(char *buf,
                              uint8_t sys, uint8_t gyr,
                              uint8_t acc, uint8_t mag)
{
    char *p = buf;
    p = append_string(p, "CAL S");
    p = append_digit(p, sys);
    p = append_string(p, " G");
    p = append_digit(p, gyr);
    p = append_string(p, " A");
    p = append_digit(p, acc);
    p = append_string(p, " M");
    p = append_digit(p, mag);
    *p = '\0';
}

static void debug_enter()
{
    // nothing to do
}

static void debug_render(HAL_SSD1306_HandleTypeDef *display,
                         const imu_frame_t *frame)
{
    ssd1306_Fill(display, Black);

    if (frame == NULL)
    {
        write_line(display, 0, "no IMU data yet");
        return;
    }

    char line[DEBUG_MAX_LINE_LEN];

    format_axis_line(line, "ACC",
                     frame->raw_accel_x,
                     frame->raw_accel_y,
                     frame->raw_accel_z);
    write_line(display, 0, line);

    format_axis_line(line, "GRV",
                     frame->raw_gravity_x,
                     frame->raw_gravity_y,
                     frame->raw_gravity_z);
    write_line(display, 1, line);

    format_axis_line(line, "MAG",
                     frame->raw_mag_x,
                     frame->raw_mag_y,
                     frame->raw_mag_z);
    write_line(display, 2, line);

    format_euler_line(line,
                      frame->raw_euler_h,
                      frame->raw_euler_r,
                      frame->raw_euler_p);
    write_line(display, 3, line);

    format_calib_line(line,
                      frame->calib_sys,
                      frame->calib_gyro,
                      frame->calib_accel,
                      frame->calib_mag);
    write_line(display, 4, line);
}

const mode_t MODE_DEBUG = {
    .name = "debug",
    .enter = debug_enter,
    .render = debug_render,
};
