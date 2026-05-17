#include "mode_level.h"
#include "../drivers/display_service.h"

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#define LEVEL_FLAT_THRESHOLD (0.05f)
#define LEVEL_LINE_HALF_LENGTH (96.0f)
#define LEVEL_CENTER_DOT_RADIUS (2U)

typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;
    bool visible;
} clipped_line_t;

static clipped_line_t clip_centered_line(float dx, float dy)
{
    clipped_line_t out = {0, 0, 0, 0, false};

    const float cx = (float)DISPLAY_CENTER_U;
    const float cy = (float)DISPLAY_CENTER_V;
    const float x_min = 0.0f;
    const float x_max = (float)(DISPLAY_WIDTH - 1);
    const float y_min = 0.0f;
    const float y_max = (float)(DISPLAY_HEIGHT - 1);

    float t_min = -LEVEL_LINE_HALF_LENGTH;
    float t_max = +LEVEL_LINE_HALF_LENGTH;

    const float p[4] = {-dx, dx, -dy, dy};
    const float q[4] = {cx - x_min, x_max - cx, cy - y_min, y_max - cy};

    for (int i = 0; i < 4; i++)
    {
        if (p[i] == 0.0f)
        {
            if (q[i] < 0.0f)
            {
                return out;
            }
            continue;
        }
        float r = q[i] / p[i];
        if (p[i] < 0.0f)
        {
            if (r > t_min)
                t_min = r;
        }
        else
        {
            if (r < t_max)
                t_max = r;
        }
    }
    if (t_min > t_max)
    {
        return out;
    }

    out.x1 = (int)(cx + t_min * dx + 0.5f);
    out.y1 = (int)(cy + t_min * dy + 0.5f);
    out.x2 = (int)(cx + t_max * dx + 0.5f);
    out.y2 = (int)(cy + t_max * dy + 0.5f);
    out.visible = true;
    return out;
}

static void draw_clipped_line(HAL_SSD1306_HandleTypeDef *display,
                              const clipped_line_t *line)
{
    if (!line->visible)
    {
        return;
    }
    ssd1306_Line(display,
                 (uint8_t)line->x1, (uint8_t)line->y1,
                 (uint8_t)line->x2, (uint8_t)line->y2,
                 White);
}

static void level_enter() {}

static void level_render(HAL_SSD1306_HandleTypeDef *display,
                         const imu_frame_t *frame)
{
    ssd1306_Fill(display, Black);

    ssd1306_DrawCircle(display,
                       DISPLAY_CENTER_U, DISPLAY_CENTER_V,
                       LEVEL_CENTER_DOT_RADIUS, White);

    if (frame == NULL)
    {
        return;
    }

    float u = frame->tilt_u;
    float v = frame->tilt_v;
    float mag2 = u * u + v * v;

    clipped_line_t line;
    if (mag2 < LEVEL_FLAT_THRESHOLD * LEVEL_FLAT_THRESHOLD)
    {
        line = clip_centered_line(1.0f, 0.0f);
    }
    else
    {
        float inv = 1.0f / sqrtf(mag2);
        float dx = -v * inv;
        float dy = u * inv;
        line = clip_centered_line(dx, dy);
    }

    draw_clipped_line(display, &line);
}

const mode_t MODE_LEVEL = {
    .name = "level",
    .enter = level_enter,
    .render = level_render,
};
