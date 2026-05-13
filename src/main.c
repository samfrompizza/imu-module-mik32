#include "mik32_hal.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_i2c.h"
#include "mik32_hal_dma.h"
#include "../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306.h"
#include "mik32_memory_map.h"
#include "scr1_timer.h"
#include "drivers/board_config.h"
#include "../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306_fonts.h"
#include "../lib/BNO055_SensorAPI/bno055.h"
#include "drivers/bno055_port.h"

static void SystemClock_Config();
static void GPIO_Init();
static void I2C_Init();
void SPI_Init(SPI_HandleTypeDef *spi);
static void DMA_Init();
void display_Init();
void BNO055_Init();

void BNO055_delay_msek(u32 msek);
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

void itoa_hex_u8(uint8_t val, char *str);

SPI_HandleTypeDef spi;
I2C_HandleTypeDef hi2c;
USART_HandleTypeDef husart0; 
HAL_SSD1306_HandleTypeDef display;

// DMA handle for SPI
DMA_ChannelHandleTypeDef hdma_spi_tx;
static DMA_InitTypeDef dma_init_struct = {0};

int main() {
    SystemClock_Config();
    GPIO_Init();

    I2C_Init();
    HAL_I2C_Init(&hi2c);

    display_Init();
    DMA_Init();
    SPI_Init(&spi);
    display.hdmatx = &hdma_spi_tx;  // Link DMA channel to display handle

    BNO055_Init();

    ssd1306_Init(&display, 10);
    ssd1306_Fill(&display, Black);

    uint8_t chip_id = 0;
    if (bno055_read_chip_id(&chip_id) == BNO055_SUCCESS) {
        char str[5];
        itoa_hex_u8(chip_id, str);
        ssd1306_WriteString(&display, str, Font_16x24, White);
        ssd1306_UpdateScreen(&display);
    }

    while (1) {
        HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, GPIO_PIN_HIGH);
        HAL_DelayMs(500);
        HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, GPIO_PIN_LOW);
        HAL_DelayMs(500);
    }

    return 0;
}

void BNO055_Init() {
    struct bno055_t bno055;
    bno055.bus_write = BNO055_I2C_bus_write;
    bno055.bus_read  = BNO055_I2C_bus_read;
    bno055.delay_msec = BNO055_delay_msek;
    bno055.dev_addr = BNO055_I2C_ADDR2;

    if (bno055_init(&bno055) != BNO055_SUCCESS) {
        HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, GPIO_PIN_HIGH);
        while (1);
    }

    bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
    BNO055_delay_msek(10);
}

void itoa_hex_u8(uint8_t val, char *str) {
    const char hex_chars[] = "0123456789ABCDEF";
    str[0] = '0';
    str[1] = 'x';
    str[2] = hex_chars[(val >> 4) & 0x0F];
    str[3] = hex_chars[val & 0x0F];
    str[4] = '\0';
}

void display_Init() {
    spi.Instance = SPI_PORT;

    display.Init.Interface = HAL_SPI;
    display.Init.Spi = &spi;
    display.Init.SSD1306_DC_Port = OLED_DC_PORT;
    display.Init.SSD1306_DC_Pin = OLED_DC_PIN;
    display.Init.SSD1306_Reset_Port = OLED_RES_PORT;
    display.Init.SSD1306_Reset_Pin = OLED_RES_PIN;
    display.Init.SSD1306_CS_Port = OLED_CS_PORT;
    display.Init.SSD1306_CS_Pin = OLED_CS_PIN;
}

void SPI_Init(SPI_HandleTypeDef *spi) {
    spi->Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    spi->Init.CLKPhase = SPI_PHASE_OFF;
    spi->Init.CLKPolarity = SPI_POLARITY_LOW;
    spi->Init.ThresholdTX = 1;

    spi->Init.BaudRateDiv = SPI_BAUDRATE_DIV4;
    spi->Init.Decoder = SPI_DECODER_NONE;
    spi->Init.ManualCS = SPI_MANUALCS_ON;
    HAL_SPI_Init(spi);
}

static void DMA_Init(void) {
    // Initialize DMA controller
    dma_init_struct.Instance = (DMA_CONFIG_TypeDef *)DMA_CONFIG;
    HAL_DMA_Init(&dma_init_struct);
    
    DMA_ChannelInitHandleTypeDef ChannelInit = {0};
    ChannelInit.Channel = DMA_CHANNEL_1;
    ChannelInit.Priority = DMA_CHANNEL_PRIORITY_MEDIUM;
    ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;
    ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;
    ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;
    ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    ChannelInit.WriteRequest = DMA_CHANNEL_SPI_1_REQUEST;
    
    // Link channel to DMA controller and enable
    hdma_spi_tx.dma = &dma_init_struct;
    hdma_spi_tx.ChannelInit = ChannelInit;
    HAL_DMA_ChannelEnable(&hdma_spi_tx);
}

static void GPIO_Init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_2, &GPIO_InitStruct);
}

static void SystemClock_Config() {
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

void I2C_Init() {
    hi2c.Instance = I2C_1;
    hi2c.Init.Mode = HAL_I2C_MODE_MASTER;

    hi2c.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c.Init.AutoEnd = I2C_AUTOEND_ENABLE;

    /* Настройка частоты */
    hi2c.Clock.PRESC = 1;
    hi2c.Clock.SCLL  = 19;
    hi2c.Clock.SCLH  = 9;
    hi2c.Clock.SCLDEL = 4;
    hi2c.Clock.SDADEL = 3;
}