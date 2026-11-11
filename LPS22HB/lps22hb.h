#ifndef LPS22HB_H
#define LPS22HB_H
#include "sensor_bus.h"
#define LPS22HB_ADDRESS_LOW 0x5cU
#define LPS22HB_ADDRESS_HIGH 0x5dU
#define LPS22HB_WHO_AM_I 0x0fU
#define LPS22HB_ID 0xb1U
#define LPS22HB_CTRL_REG1 0x10U
#define LPS22HB_CTRL_REG2 0x11U
#define LPS22HB_STATUS_REG 0x27U
#define LPS22HB_PRESS_OUT_XL 0x28U
#define LPS22HB_TEMP_OUT_L 0x2bU
#define LPS22HB_TEMP_READY 0x02U
#define LPS22HB_IF_ADD_INC 0x10U
#define LPS22HB_ONE_SHOT 0x01U
#define LPS22HB_SWRESET 0x04U
#define LPS22HB_POLL_LIMIT 1024U
#define LPS22HB_PRESS_READY 0x01U

typedef struct {
  sensor_bus_t bus;
  uint8_t address;
  uint8_t initialized;
} lps22hb_t;
int lps22hb_register_bus(lps22hb_t *dev, sensor_read_fn read, sensor_write_fn write);
int lps22hb_read_register(lps22hb_t *dev, uint8_t reg, uint8_t *data, uint16_t length);
int lps22hb_write_register(lps22hb_t *dev, uint8_t reg, const uint8_t *data, uint16_t length);
int lps22hb_init(lps22hb_t *dev);
/* Kazde citanie spusti nove jednorazove meranie; volat z hlavnej slucky. */
int lps22hb_read_pressure(lps22hb_t *dev, float *hpa);
int lps22hb_read_temperature(lps22hb_t *dev, float *celsius);
#endif
