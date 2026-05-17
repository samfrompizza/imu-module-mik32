#include "mik32_hal_irq.h"
#include "mik32_hal_timer32.h"
#include "timer_service.h"

void trap_handler(void)
{
    if (EPIC_CHECK_TIMER32_1())
    {
        HAL_Timer32_InterruptFlags_Clear(timer_service_imu_handle());
        timer_service_isr_on_imu_tick();
        HAL_EPIC_Clear(HAL_EPIC_TIMER32_1_MASK);
    }

    if (EPIC_CHECK_TIMER32_2())
    {
        HAL_Timer32_InterruptFlags_Clear(timer_service_display_handle());
        timer_service_isr_on_display_tick();
        HAL_EPIC_Clear(HAL_EPIC_TIMER32_2_MASK);
    }
}
