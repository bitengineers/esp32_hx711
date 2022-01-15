#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

  void hx711_init(void);
  void hx711_deinit(void);

  void hx711_powerdown(void);

  void hx711_wait_for_ready(void);
  uint32_t hx711_measure(void);

  uint32_t hx711_get_zero_offset(void);
  void hx711_set_zero_offset(uint32_t offset);

#ifdef __cplusplus
}
#endif // __cplusplus
