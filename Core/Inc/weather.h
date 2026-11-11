#ifndef WEATHER_H
#define WEATHER_H
#include <stddef.h>
int relative_altitude(float pressure, float reference, float *meters);
int weather_csv(char *buffer, size_t capacity, float temperature, float humidity, float pressure, float altitude);
#endif
