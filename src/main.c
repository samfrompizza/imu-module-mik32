#include "mik32_hal_usart.h"
#include "mik32_hal_i2c.h"
#include "mik32_hal_irq.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_spi.h"
#include "string.h"
#include "stdlib.h"
#include "app/queue.h"
#include "app/circular_buffer.h"
#include "app/app_types.h"
#include "mik32_hal.h"
#include "drivers/oled_ssd1306.h"

static void SystemClock_Config();
static void USART_Init();
static void GPIO_Init();
static void spi_init();

USART_HandleTypeDef husart0;
SPI_HandleTypeDef hspi0;

int main()
{
    SystemClock_Config();
    GPIO_Init();
    USART_Init();
    spi_init();
    oled_init(&hspi0);

    while (1)
    {
        oled_test_screen();
    }
}

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void GPIO_Init()
{
    /**< Включить  тактирование GPIO */
    PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_0_M;
    PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_1_M;
    PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_2_M;

    /**< Включить  тактирование схемы формирования прерываний GPIO */
    PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_GPIO_IRQ_M;

    GPIO_InitTypeDef gpio = {0};

    gpio.Pin = OLED_DC_PIN;
    gpio.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;

    HAL_GPIO_Init(OLED_DC_PORT, &gpio);

    gpio.Pin = OLED_RES_PIN;
    HAL_GPIO_Init(OLED_RES_PORT, &gpio);

    gpio.Pin = OLED_CS_PIN;
    HAL_GPIO_Init(OLED_CS_PORT, &gpio);

    HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_PIN, GPIO_PIN_HIGH);
}

void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Enable;
    husart0.frame = Frame_8bit;
    husart0.parity_bit = Disable;
    husart0.parity_bit_inversion = Disable;
    husart0.bit_direction = LSB_First;
    husart0.data_inversion = Disable;
    husart0.tx_inversion = Disable;
    husart0.rx_inversion = Disable;
    husart0.swap = Disable;
    husart0.lbm = Disable;
    husart0.stop_bit = StopBit_1;
    husart0.mode = Asynchronous_Mode;
    husart0.xck_mode = XCK_Mode3;
    husart0.last_byte_clock = Disable;
    husart0.overwrite = Disable;
    husart0.rts_mode = AlwaysEnable_mode;
    husart0.channel_mode = Duplex_Mode;
    husart0.tx_break_mode = Disable;
    husart0.Interrupt.ctsie = Disable;
    husart0.Interrupt.eie = Disable;
    husart0.Interrupt.idleie = Disable;
    husart0.Interrupt.lbdie = Disable;
    husart0.Interrupt.peie = Disable;
    husart0.Interrupt.rxneie = Disable;
    husart0.Interrupt.tcie = Disable;
    husart0.Interrupt.txeie = Disable;
    husart0.Modem.rts = Disable;  // out
    husart0.Modem.cts = Disable;  // in
    husart0.Modem.dtr = Disable;  // out
    husart0.Modem.dcd = Disable;  // in
    husart0.Modem.dsr = Disable;  // in
    husart0.Modem.ri = Disable;   // in
    husart0.Modem.ddis = Disable; // out
    husart0.baudrate = 115200;

    husart0.dma_tx_request = Enable;
    husart0.dma_rx_request = Disable;

    HAL_USART_Init(&husart0);
}

static void spi_init()
{
    hspi0.Instance = SPI_0;

    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    hspi0.Init.CLKPhase = SPI_PHASE_ON;
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;

    hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV32;

    HAL_SPI_Init(&hspi0);
}