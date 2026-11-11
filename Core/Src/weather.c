#include "weather.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>

int relative_altitude(float pressure, float reference, float *meters)
{
  if (!meters || !isfinite(pressure) || !isfinite(reference) || pressure < 260.0f || pressure > 1260.0f || reference < 260.0f || reference > 1260.0f) return -1;
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
  if (!buffer || !capacity || !isfinite(temperature) || !isfinite(humidity) || !isfinite(pressure) || !isfinite(altitude) ||
      temperature < -40 || temperature > 120 || humidity < 0 || humidity > 100 || pressure < 260 || pressure > 1260 || fabsf(altitude) > 50000) return -1;
  int32_t t = rounded(temperature, 10), h = rounded(humidity, 1);
  int32_t p = rounded(pressure, 100), a = rounded(altitude, 100);
  const char *tsign = t < 0 ? "-" : "", *asign = a < 0 ? "-" : "";
  if (t < 0) t = -t;
  if (a < 0) a = -a;
  int length = snprintf(buffer, capacity, "%s%ld.%01ld,%ld,%ld.%02ld,%s%ld.%02ld\r\n",
      tsign, (long)(t / 10), (long)(t % 10), (long)h, (long)(p / 100), (long)(p % 100), asign, (long)(a / 100), (long)(a % 100));
  return length < 0 || (size_t)length >= capacity ? -1 : length;
}
