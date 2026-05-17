#include "display_service.h"
#include "board_config.h"

#define DEFAULT_BRIGHTNESS (10U)

static HAL_SSD1306_HandleTypeDef s_display;

void display_service_attach_dma(DMA_ChannelHandleTypeDef *hdma)
{
    s_display.hdmatx = hdma;
}

HAL_StatusTypeDef display_service_init(SPI_HandleTypeDef *spi, uint8_t brightness)
{
    s_display.Init.Interface = HAL_SPI;
    s_display.Init.Spi = spi;
    s_display.Init.SSD1306_DC_Port = OLED_DC_PORT;
    s_display.Init.SSD1306_DC_Pin = OLED_DC_PIN;
    s_display.Init.SSD1306_Reset_Port = OLED_RES_PORT;
    s_display.Init.SSD1306_Reset_Pin = OLED_RES_PIN;
    s_display.Init.SSD1306_CS_Port = OLED_CS_PORT;
    s_display.Init.SSD1306_CS_Pin = OLED_CS_PIN;

    HAL_StatusTypeDef st = ssd1306_Init(&s_display, brightness ? brightness : DEFAULT_BRIGHTNESS);
    if (st != HAL_OK)
    {
        return st;
    }
    ssd1306_Fill(&s_display, Black);
    return ssd1306_UpdateScreen(&s_display);
}

HAL_SSD1306_HandleTypeDef *display_service_handle()
{
    return &s_display;
}

void display_service_clear()
{
    ssd1306_Fill(&s_display, Black);
}

HAL_StatusTypeDef display_service_flush()
{
    return ssd1306_UpdateScreen(&s_display);
}
