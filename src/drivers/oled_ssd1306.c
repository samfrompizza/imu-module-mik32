#include <stdint.h>
#include "board_config.h"
#include "oled_ssd1306.h"
#include <string.h>
#include "mik32_hal_spi.h"
#include "mik32_hal_gpio.h"

static uint8_t fb[128 * 64 / 8];

static SPI_HandleTypeDef *oled_spi;

uint8_t oled_buffer[OLED_BUF_SIZE];

static inline void cs_low(void)
{
  HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_PIN, GPIO_PIN_LOW);
}

static inline void cs_high(void)
{
  HAL_GPIO_WritePin(OLED_CS_PORT, OLED_CS_PIN, GPIO_PIN_HIGH);
}

static inline void dc_low(void)
{
  HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_PIN, GPIO_PIN_LOW);
}

static inline void dc_high(void)
{
  HAL_GPIO_WritePin(OLED_DC_PORT, OLED_DC_PIN, GPIO_PIN_HIGH);
}

static inline void rst_low(void)
{
  HAL_GPIO_WritePin(OLED_RES_PORT, OLED_RES_PIN, GPIO_PIN_LOW);
}

static inline void rst_high(void)
{
  HAL_GPIO_WritePin(OLED_RES_PORT, OLED_RES_PIN, GPIO_PIN_HIGH);
}

static void oled_write_cmd(uint8_t cmd)
{
  cs_low();
  dc_low();
  HAL_SPI_Transmit(oled_spi, &cmd, 1, 100);
  cs_high();
}

static void oled_write_data(uint8_t *data, uint16_t size)
{
  cs_low();
  dc_high();
  HAL_SPI_Transmit(oled_spi, data, size, 100);
  cs_high();
}

static void oled_reset(void)
{
  rst_low();
  HAL_DelayMs(10);

  rst_high();
  HAL_DelayMs(10);
}

void oled_clear(void)
{
  memset(oled_buffer, 0x00, OLED_BUF_SIZE);
}

void oled_fill(void)
{
  memset(oled_buffer, 0xFF, OLED_BUF_SIZE);
}

void oled_draw_pixel(uint8_t x, uint8_t y, bool color)
{
  if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
    return;

  uint16_t index = x + (y / 8) * OLED_WIDTH;

  if (color)
    oled_buffer[index] |= (1 << (y % 8));
  else
    oled_buffer[index] &= ~(1 << (y % 8));
}

void oled_draw_bitmap_8x8(uint8_t x, uint8_t y, const uint8_t *bitmap)
{
  for (uint8_t row = 0; row < 8; row++)
  {
    uint8_t line = bitmap[row];

    for (uint8_t col = 0; col < 8; col++)
    {
      bool pixel = line & (1 << col);
      oled_draw_pixel(x + col, y + row, pixel);
    }
  }
}

void oled_update(void)
{
  for (uint8_t page = 0; page < 8; page++)
  {
    oled_write_cmd(0xB0 + page);

    oled_write_cmd(0x00);
    oled_write_cmd(0x10);

    oled_write_data(
        &oled_buffer[OLED_WIDTH * page],
        OLED_WIDTH);
  }
}

void oled_init(SPI_HandleTypeDef *hspi)
{
  oled_spi = hspi;

  oled_reset();

  oled_write_cmd(0xAE);

  oled_write_cmd(0x20);
  oled_write_cmd(0x00);

  oled_write_cmd(0xB0);

  oled_write_cmd(0xC8);

  oled_write_cmd(0x00);

  oled_write_cmd(0x10);

  oled_write_cmd(0x40);

  oled_write_cmd(0x81);
  oled_write_cmd(0x7F);

  oled_write_cmd(0xA1);

  oled_write_cmd(0xA6);

  oled_write_cmd(0xA8);
  oled_write_cmd(0x3F);

  oled_write_cmd(0xA4);

  oled_write_cmd(0xD3);
  oled_write_cmd(0x00);

  oled_write_cmd(0xD5);
  oled_write_cmd(0xF0);

  oled_write_cmd(0xD9);
  oled_write_cmd(0x22);

  oled_write_cmd(0xDA);
  oled_write_cmd(0x12);

  oled_write_cmd(0xDB);
  oled_write_cmd(0x20);

  oled_write_cmd(0x8D);
  oled_write_cmd(0x14);

  oled_write_cmd(0xAF);

  oled_clear();
  oled_update();
}

void oled_test_screen(void)
{
  oled_clear();

  for (uint8_t x = 0; x < OLED_WIDTH; x++)
  {
    oled_draw_pixel(x, x / 2, true);
  }

  oled_update();
}