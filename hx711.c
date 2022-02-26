#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "driver/gpio.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hx711.h"

#define HX711_CLK_PIN GPIO_NUM_26
#define HX711_DAT_PIN GPIO_NUM_25
#define HX711_TAG  "HX711"

static void hx711_usleep(long sleep);
static uint32_t zero_offset;

void hx711_init(void)
{
  gpio_config_t config = {
    .pin_bit_mask = (0x01 << HX711_DAT_PIN),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
  };
  gpio_config(&config);

  config.pin_bit_mask = (0x01 << HX711_CLK_PIN);
  config.mode = GPIO_MODE_OUTPUT;
  gpio_config(&config);
}

void hx711_deinit(void)
{
  gpio_reset_pin(HX711_CLK_PIN);
  gpio_reset_pin(HX711_DAT_PIN);
}

void hx711_powerdown(void)
{
  gpio_set_level(HX711_CLK_PIN, 1);
  usleep(60);
  gpio_set_level(HX711_CLK_PIN, 0);
}

void hx711_wait_for_ready(void)
{
  gpio_set_level(HX711_CLK_PIN, 0);
  while (gpio_get_level(HX711_DAT_PIN) != 0) {
    // vTaskDelay(1/ portTICK_PERIOD_MS);
    hx711_usleep(10);
  }
  return;
}

uint32_t hx711_measure(void)
{
  portDISABLE_INTERRUPTS();
  uint32_t weight = 0;
  hx711_wait_for_ready();
  for (int8_t i = 0; i < 24; i++) {
    gpio_set_level(HX711_CLK_PIN, 1);
    hx711_usleep(1);
    uint32_t v = gpio_get_level(HX711_DAT_PIN);
    weight += (v << (23-i));
    gpio_set_level(HX711_CLK_PIN, 0);
    hx711_usleep(1);
  }
  // 25th, 26th pulse
  for (uint8_t i = 0; i < 2; i++) {
    gpio_set_level(HX711_CLK_PIN, 1);
    hx711_usleep(1);
    gpio_set_level(HX711_CLK_PIN, 0);
    hx711_usleep(1);
  }

  // 27th
  gpio_set_level(HX711_CLK_PIN, 1);
  while(gpio_get_level(HX711_DAT_PIN) != 1) {}
  gpio_set_level(HX711_CLK_PIN, 0);
  portENABLE_INTERRUPTS();

  return (weight - zero_offset) & 0xffffff;
}

void hx711_set_zero_offset(uint32_t offset)
{
  zero_offset = offset;
}

uint32_t hx711_get_zero_offset(void)
{
  return zero_offset;
}

static void hx711_usleep(long sleep)
{
  usleep(sleep);
}
