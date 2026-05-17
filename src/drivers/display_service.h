#ifndef DRIVERS_DISPLAY_SERVICE_H
#define DRIVERS_DISPLAY_SERVICE_H

#include <stdint.h>
#include "../../hardware/mik32-hal/utilities/Include/mik32_hal_ssd1306.h"

#define DISPLAY_WIDTH         (SSD1306_WIDTH)
#define DISPLAY_HEIGHT        (SSD1306_HEIGHT)
#define DISPLAY_CENTER_U      (DISPLAY_WIDTH / 2)
#define DISPLAY_CENTER_V      (DISPLAY_HEIGHT / 2)

void                       display_service_attach_dma(DMA_ChannelHandleTypeDef *hdma);
HAL_StatusTypeDef          display_service_init(SPI_HandleTypeDef *spi, uint8_t brightness);
HAL_SSD1306_HandleTypeDef *display_service_handle(void);
void                       display_service_clear(void);
HAL_StatusTypeDef          display_service_flush(void);

#endif
