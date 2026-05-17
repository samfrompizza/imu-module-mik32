#include "mode_manager.h"

#include "mode_welcome.h"
#include "mode_debug.h"
#include "mode_level.h"
#include "mode_compass.h"
#include "mode_ball.h"

extern const mode_t MODE_WELCOME;
extern const mode_t MODE_DEBUG;
extern const mode_t MODE_LEVEL;
extern const mode_t MODE_COMPASS;
extern const mode_t MODE_BALL;

static const mode_t *const MODES[APP_MODE_COUNT] = {
    [APP_MODE_WELCOME] = &MODE_WELCOME,
    [APP_MODE_DEBUG] = &MODE_DEBUG,
    [APP_MODE_LEVEL] = &MODE_LEVEL,
    [APP_MODE_COMPASS] = &MODE_COMPASS,
    [APP_MODE_BALL] = &MODE_BALL,
};

#define FIRST_CYCLED_MODE APP_MODE_DEBUG

static app_mode_id_t g_current = APP_MODE_WELCOME;

static void enter_mode(app_mode_id_t id)
{
    g_current = id;
    if (MODES[id]->enter)
    {
        MODES[id]->enter();
    }
}

static app_mode_id_t next_cycled(app_mode_id_t current)
{
    app_mode_id_t next = (app_mode_id_t)(current + 1);
    if (next >= APP_MODE_COUNT)
    {
        next = FIRST_CYCLED_MODE;
    }
    return next;
}

void mode_manager_init()
{
    enter_mode(APP_MODE_WELCOME);
}

void mode_manager_handle_button()
{
    app_mode_id_t next;
    if (g_current == APP_MODE_WELCOME)
    {
        next = FIRST_CYCLED_MODE;
    }
    else
    {
        next = next_cycled(g_current);
    }
    enter_mode(next);
}

void mode_manager_render(HAL_SSD1306_HandleTypeDef *display,
                         const imu_frame_t *frame)
{
    if (MODES[g_current]->render)
    {
        MODES[g_current]->render(display, frame);
    }
}

app_mode_id_t mode_manager_current()
{
    return g_current;
}
