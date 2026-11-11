#include "weather.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>

int relative_altitude(float pressure, float reference, float *meters)
{
  if (!meters || !isfinite(pressure) || !isfinite(reference)) return -1;
  if (pressure < 260.0f || pressure > 1260.0f ||
      reference < 260.0f || reference > 1260.0f) {
    return -1;
  }
  /* Standardna atmosfera, referencia je tlak pri spusteni na zemi. */
  *meters = 44330.0f * (1.0f - powf(pressure / reference, 0.19029496f));
  return 0;
}

static int32_t rounded(float value, float scale)
{
  float scaled = value * scale;
  return (int32_t)(scaled < 0 ? scaled - 0.5f : scaled + 0.5f);
}

int weather_csv(char *buffer, size_t capacity, float temperature, float humidity, float pressure, float altitude)
{
  if (!buffer || !capacity) return -1;
  if (!isfinite(temperature) || !isfinite(humidity) ||
      !isfinite(pressure) || !isfinite(altitude)) {
    return -1;
  }
  if (temperature < -40 || temperature > 120 || humidity < 0 || humidity > 100 ||
      pressure < 260 || pressure > 1260 || fabsf(altitude) > 50000) {
    return -1;
  }

  int32_t temperature_scaled = rounded(temperature, 10);
  int32_t humidity_rounded = rounded(humidity, 1);
  int32_t pressure_scaled = rounded(pressure, 100);
  int32_t altitude_scaled = rounded(altitude, 100);
  const char *temperature_sign = temperature_scaled < 0 ? "-" : "";
  const char *altitude_sign = altitude_scaled < 0 ? "-" : "";
  if (temperature_scaled < 0) temperature_scaled = -temperature_scaled;
  if (altitude_scaled < 0) altitude_scaled = -altitude_scaled;

  int length = snprintf(buffer, capacity, "%s%ld.%01ld,%ld,%ld.%02ld,%s%ld.%02ld\r\n",
      temperature_sign, (long)(temperature_scaled / 10), (long)(temperature_scaled % 10),
      (long)humidity_rounded, (long)(pressure_scaled / 100), (long)(pressure_scaled % 100),
      altitude_sign, (long)(altitude_scaled / 100), (long)(altitude_scaled % 100));
  if (length < 0 || (size_t)length >= capacity) return -1;
  return length;
}
