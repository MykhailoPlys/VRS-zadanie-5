#ifndef HTS221_H
#define HTS221_H
#include "sensor_bus.h"
#define HTS221_ADDRESS 0x5fU
#define HTS221_WHO_AM_I 0x0fU
#define HTS221_ID 0xbcU
#define HTS221_AV_CONF 0x10U
#define HTS221_CTRL_REG1 0x20U
#define HTS221_CTRL_REG2 0x21U
#define HTS221_STATUS_REG 0x27U
#define HTS221_HUMIDITY_OUT_L 0x28U
#define HTS221_TEMP_OUT_L 0x2aU
#define HTS221_CALIBRATION 0x30U
#define HTS221_AUTO_INCREMENT 0x80U
#define HTS221_POWER_ON 0x80U
#define HTS221_BDU 0x04U
#define HTS221_ODR_1HZ 0x01U
#define HTS221_AVG_T16_H32 0x1bU
#define HTS221_TEMP_READY 0x01U
#define HTS221_HUM_READY 0x02U

typedef struct {
  sensor_bus_t bus;
  float h0, h1, t0, t1;
  int16_t h0_out, h1_out, t0_out, t1_out;
  uint8_t initialized;
} hts221_t;
int hts221_register_bus(hts221_t *dev, sensor_read_fn read, sensor_write_fn write);
int hts221_read_register(hts221_t *dev, uint8_t reg, uint8_t *data, uint16_t length);
int hts221_write_register(hts221_t *dev, uint8_t reg, const uint8_t *data, uint16_t length);
int hts221_init(hts221_t *dev);
int hts221_read_temperature(hts221_t *dev, float *celsius);
int hts221_read_humidity(hts221_t *dev, float *percent);
#endif
