#ifndef SENSOR_BUS_H
#define SENSOR_BUS_H
#include <stdint.h>
/* Adresa je 7-bitova, navrat 0 znamena uspesny prenos. */
typedef int (*sensor_read_fn)(uint8_t, uint8_t, uint8_t *, uint16_t);
typedef int (*sensor_write_fn)(uint8_t, uint8_t, const uint8_t *, uint16_t);
typedef struct {
  sensor_read_fn read;
  sensor_write_fn write;
} sensor_bus_t;
enum { SENSOR_OK = 0, SENSOR_ARGUMENT = -1, SENSOR_ID = -5,
       SENSOR_NOT_READY = -6, SENSOR_CALIBRATION = -7, SENSOR_RANGE = -8 };
#endif
