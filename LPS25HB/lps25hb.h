#ifndef LPS25HB_H
#define LPS25HB_H
#include "sensor_bus.h"
#define LPS25HB_ADDRESS_LOW 0x5cU
#define LPS25HB_ADDRESS_HIGH 0x5dU
#define LPS25HB_WHO_AM_I 0x0fU
#define LPS25HB_ID 0xbdU
#define LPS25HB_RES_CONF 0x10U
#define LPS25HB_CTRL_REG1 0x20U
#define LPS25HB_CTRL_REG2 0x21U
#define LPS25HB_STATUS_REG 0x27U
#define LPS25HB_PRESS_OUT_XL 0x28U
#define LPS25HB_TEMP_OUT_L 0x2bU
#define LPS25HB_TEMP_READY 0x01U
#define LPS25HB_AUTO_INCREMENT 0x80U
#define LPS25HB_POWER_ON 0x80U
#define LPS25HB_ODR_1HZ 0x10U
#define LPS25HB_BDU 0x04U
#define LPS25HB_PRESS_READY 0x02U
#define LPS25HB_RES_HIGH 0x0fU

typedef struct {
  sensor_bus_t bus;
  uint8_t address;
  uint8_t initialized;
} lps25hb_t;
int lps25hb_register_bus(lps25hb_t *dev, sensor_read_fn read, sensor_write_fn write);
int lps25hb_read_register(lps25hb_t *dev, uint8_t reg, uint8_t *data, uint16_t length);
int lps25hb_write_register(lps25hb_t *dev, uint8_t reg, const uint8_t *data, uint16_t length);
int lps25hb_init(lps25hb_t *dev);
int lps25hb_read_pressure(lps25hb_t *dev, float *hpa);
int lps25hb_read_temperature(lps25hb_t *dev, float *celsius);
#endif
