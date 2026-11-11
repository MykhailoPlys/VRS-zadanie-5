#ifndef SENSOR_BUS_H
#define SENSOR_BUS_H
#include <stdint.h>
/* Adresa je 7-bitova, navrat 0 znamena uspesny prenos. */
typedef int (*sensor_read_fn)(uint8_t address, uint8_t reg,
                              uint8_t *data, uint16_t length);
typedef int (*sensor_write_fn)(uint8_t address, uint8_t reg,
                               const uint8_t *data, uint16_t length);
typedef struct {
  sensor_read_fn read;
  sensor_write_fn write;
} sensor_bus_t;
enum {
  SENSOR_OK = 0,
  SENSOR_ARGUMENT = -1,
  SENSOR_TIMEOUT = -4,
  SENSOR_ID = -5,
  SENSOR_NOT_READY = -6,
  SENSOR_CALIBRATION = -7,
  SENSOR_RANGE = -8
};
#endif
