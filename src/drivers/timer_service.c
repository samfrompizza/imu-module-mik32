#include "timer_service.h"
#include "button.h"
#include "mik32_hal_irq.h"

#define CPU_HZ (32000000U)
#define TIMER_TICK_HZ (1000000U)
#define TIMER_PRESCALER_VALUE ((CPU_HZ / TIMER_TICK_HZ) - 1U)

#define TIMER_TOP_FOR_HZ(period_hz) ((TIMER_TICK_HZ / (period_hz)) - 1U)

static TIMER32_HandleTypeDef s_imu_timer;
static TIMER32_HandleTypeDef s_disp_timer;

static volatile uint8_t s_imu_tick_pending = 0;
static volatile uint8_t s_display_tick_pending = 0;

static void timer_base_configure(TIMER32_HandleTypeDef *t,
                                 TIMER32_TypeDef *instance,
                                 uint32_t top)
{
    t->Instance = instance;
    t->Clock.Source = TIMER32_SOURCE_PRESCALER;
    t->Clock.Prescaler = TIMER_PRESCALER_VALUE;
    t->CountMode = TIMER32_COUNTMODE_FORWARD;
    t->Top = top;
    t->State = TIMER32_STATE_DISABLE;
    t->InterruptMask = 0;

    HAL_Timer32_Init(t);
}

void timer_service_init(void)
{
    timer_base_configure(&s_imu_timer,
                         TIMER32_1,
                         TIMER_TOP_FOR_HZ(TIMER_IMU_HZ));
    timer_base_configure(&s_disp_timer,
                         TIMER32_2,
                         TIMER_TOP_FOR_HZ(TIMER_DISPLAY_HZ));

    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER32_1_MASK | HAL_EPIC_TIMER32_2_MASK);
    HAL_IRQ_EnableInterrupts();

    HAL_Timer32_Base_Start_IT(&s_imu_timer);
    HAL_Timer32_Base_Start_IT(&s_disp_timer);
}

bool timer_service_take_imu_tick(void)
{
    if (s_imu_tick_pending)
    {
        s_imu_tick_pending = 0;
        return true;
    }
    return false;
}

bool timer_service_take_display_tick(void)
{
    if (s_display_tick_pending)
    {
        s_display_tick_pending = 0;
        return true;
    }
    return false;
}

TIMER32_HandleTypeDef *timer_service_imu_handle(void) { return &s_imu_timer; }
TIMER32_HandleTypeDef *timer_service_display_handle(void) { return &s_disp_timer; }

void timer_service_isr_on_imu_tick(void)
{
    s_imu_tick_pending = 1;
}

void timer_service_isr_on_display_tick(void)
{
    button_sample();
    s_display_tick_pending = 1;
}
