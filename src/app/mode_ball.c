#include "mode_ball.h"
#include "../drivers/display_service.h"

#include <stdint.h>

#define BALL_RADIUS_PX (3)
#define BALL_GRAVITY_ACCEL (320.0f)
#define BALL_FRICTION (0.1f)
#define BALL_BOUNCE_KEEP (0.8f)
#define BALL_DT_SEC (1.0f / 8.0f)

#define BALL_MIN_U (BALL_RADIUS_PX + 1)
#define BALL_MAX_U (DISPLAY_WIDTH - 1 - BALL_RADIUS_PX)
#define BALL_MIN_V (BALL_RADIUS_PX + 1)
#define BALL_MAX_V (DISPLAY_HEIGHT - 1 - BALL_RADIUS_PX)

typedef struct
{
    float pos_u;
    float pos_v;
    float vel_u;
    float vel_v;
} ball_state_t;

static ball_state_t s_ball;

static void ball_reset()
{
    s_ball.pos_u = (float)DISPLAY_CENTER_U;
    s_ball.pos_v = (float)DISPLAY_CENTER_V;
    s_ball.vel_u = 0.0f;
    s_ball.vel_v = 0.0f;
}

static void integrate_axis(float *pos, float *vel, float accel, float min, float max)
{
    *vel += accel * BALL_DT_SEC;
    *vel *= (1.0f - BALL_FRICTION);

    float new_pos = *pos + *vel * BALL_DT_SEC;

    if (new_pos < min)
    {
        new_pos = min;
        if (*vel < 0.0f)
        {
            *vel = -*vel * BALL_BOUNCE_KEEP;
        }
    }
    else if (new_pos > max)
    {
        new_pos = max;
        if (*vel > 0.0f)
        {
            *vel = -*vel * BALL_BOUNCE_KEEP;
        }
    }

    if (new_pos < min)
        new_pos = min;
    if (new_pos > max)
        new_pos = max;
    *pos = new_pos;
}

static void ball_enter()
{
    ball_reset();
}

static void ball_render(HAL_SSD1306_HandleTypeDef *display,
                        const imu_frame_t *frame)
{
    ssd1306_Fill(display, Black);

    if (frame != NULL)
    {
        float au = frame->tilt_u * BALL_GRAVITY_ACCEL;
        float av = frame->tilt_v * BALL_GRAVITY_ACCEL;
        integrate_axis(&s_ball.pos_u, &s_ball.vel_u, au, BALL_MIN_U, BALL_MAX_U);
        integrate_axis(&s_ball.pos_v, &s_ball.vel_v, av, BALL_MIN_V, BALL_MAX_V);
    }

    ssd1306_DrawRectangle(display,
                          0, 0,
                          DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1,
                          White);

    int x = (int)(s_ball.pos_u + 0.5f);
    int y = (int)(s_ball.pos_v + 0.5f);
    if (x < BALL_MIN_U)
        x = BALL_MIN_U;
    if (y < BALL_MIN_V)
        y = BALL_MIN_V;
    if (x > BALL_MAX_U)
        x = BALL_MAX_U;
    if (y > BALL_MAX_V)
        y = BALL_MAX_V;

    ssd1306_FillCircle(display,
                       (uint8_t)x, (uint8_t)y,
                       BALL_RADIUS_PX, White);
}

const mode_t MODE_BALL = {
    .name = "ball",
    .enter = ball_enter,
    .render = ball_render,
};
