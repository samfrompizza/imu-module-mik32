#include "mik32_hal.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_i2c.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_irq.h"

#include "drivers/board_config.h"
#include "drivers/button.h"
#include "drivers/display_service.h"
#include "drivers/imu_service.h"
#include "drivers/timer_service.h"
#include "app/mode_manager.h"

#define DISPLAY_BRIGHTNESS (10U)

SPI_HandleTypeDef spi;
I2C_HandleTypeDef hi2c;

static DMA_InitTypeDef dma_init_struct = {0};
static DMA_ChannelHandleTypeDef hdma_spi_tx;

static inline void cpu_wait_for_interrupt()
{
    __asm__ volatile("wfi");
}

static void system_clock_init()
{
    PCC_InitTypeDef cfg = {0};
    cfg.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    cfg.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    cfg.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    cfg.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    cfg.AHBDivider = 0;
    cfg.APBMDivider = 0;
    cfg.APBPDivider = 0;
    cfg.HSI32MCalibrationValue = 128;
    cfg.LSI32KCalibrationValue = 8;
    cfg.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    cfg.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&cfg);
}

static void gpio_clocks_init()
{
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();
}

static void i2c_init()
{
    hi2c.Instance = I2C_1;
    hi2c.Init.Mode = HAL_I2C_MODE_MASTER;
    hi2c.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c.Init.AutoEnd = I2C_AUTOEND_ENABLE;
    hi2c.Clock.PRESC = 1;
    hi2c.Clock.SCLL = 19;
    hi2c.Clock.SCLH = 9;
    hi2c.Clock.SCLDEL = 4;
    hi2c.Clock.SDADEL = 3;
    HAL_I2C_Init(&hi2c);
}

static void spi_init()
{
    spi.Instance = SPI_PORT;
    spi.Init.SPI_Mode = HAL_SPI_MODE_MASTER;
    spi.Init.CLKPhase = SPI_PHASE_OFF;
    spi.Init.CLKPolarity = SPI_POLARITY_LOW;
    spi.Init.ThresholdTX = 1;
    spi.Init.BaudRateDiv = SPI_BAUDRATE_DIV4;
    spi.Init.Decoder = SPI_DECODER_NONE;
    spi.Init.ManualCS = SPI_MANUALCS_ON;
    HAL_SPI_Init(&spi);
}

static void dma_for_spi_init()
{
    dma_init_struct.Instance = (DMA_CONFIG_TypeDef *)DMA_CONFIG;
    HAL_DMA_Init(&dma_init_struct);

    DMA_ChannelInitHandleTypeDef ch = {0};
    ch.Channel = DMA_CHANNEL_1;
    ch.Priority = DMA_CHANNEL_PRIORITY_MEDIUM;
    ch.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    ch.ReadSize = DMA_CHANNEL_SIZE_BYTE;
    ch.ReadInc = DMA_CHANNEL_INC_ENABLE;
    ch.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    ch.WriteSize = DMA_CHANNEL_SIZE_BYTE;
    ch.WriteInc = DMA_CHANNEL_INC_DISABLE;
    ch.WriteRequest = DMA_CHANNEL_SPI_1_REQUEST;

    hdma_spi_tx.dma = &dma_init_struct;
    hdma_spi_tx.ChannelInit = ch;
    HAL_DMA_ChannelEnable(&hdma_spi_tx);
}

static void heartbeat_led_init()
{
    GPIO_InitTypeDef io = {0};
    io.Pin = GPIO_PIN_7;
    io.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    io.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &io);
}

void configure_interrupts()
{
    __HAL_PCC_EPIC_CLK_ENABLE();
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER32_1_MASK);
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER32_2_MASK);
    HAL_EPIC_MaskLevelSet(HAL_EPIC_GPIO_IRQ_MASK);
    HAL_IRQ_EnableInterrupts();
}

int main()
{
    system_clock_init();
    gpio_clocks_init();
    heartbeat_led_init();
    button_init();

    i2c_init();
    spi_init();
    dma_for_spi_init();
    display_service_attach_dma(&hdma_spi_tx);

    if (display_service_init(&spi, DISPLAY_BRIGHTNESS) != HAL_OK)
    {
        while (1)
        {
            cpu_wait_for_interrupt();
        }
    }

    if (!imu_service_init())
    {
        HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, GPIO_PIN_HIGH);
        while (1)
        {
            cpu_wait_for_interrupt();
        }
    }

    mode_manager_init();
    timer_service_init();

    configure_interrupts();

    while (1)
    {
        if (timer_service_take_imu_tick())
        {
            imu_service_poll();
        }

        if (timer_service_take_display_tick())
        {
            if (button_take_press())
            {
                mode_manager_handle_button();
            }
            mode_manager_render(display_service_handle(), imu_service_get());
            display_service_flush();
        }
        HAL_DelayMs(10);

        cpu_wait_for_interrupt();
    }

    return 0;
}
