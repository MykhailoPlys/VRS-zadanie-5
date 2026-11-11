#include "lps22hb.h"

int lps22hb_register_bus(lps22hb_t *dev, sensor_read_fn read, sensor_write_fn write)
{
  if (!dev || !read || !write) return SENSOR_ARGUMENT;
  dev->bus.read = read;
  dev->bus.write = write;
  dev->address = LPS22HB_ADDRESS_LOW;
  dev->initialized = 0;
  return SENSOR_OK;
}

int lps22hb_read_register(lps22hb_t *dev, uint8_t reg, uint8_t *data, uint16_t length)
{
  if (!dev || !dev->bus.read || !data) return SENSOR_ARGUMENT;
  if (!length || length > 128 || reg > 0x7f || reg + length > 128) {
    return SENSOR_ARGUMENT;
  }
  return dev->bus.read(dev->address, reg, data, length);
}

int lps22hb_write_register(lps22hb_t *dev, uint8_t reg, const uint8_t *data, uint16_t length)
{
  if (!dev || !dev->bus.write || !data) return SENSOR_ARGUMENT;
  if (!length || length > 128 || reg > 0x7f || reg + length > 128) {
    return SENSOR_ARGUMENT;
  }
  return dev->bus.write(dev->address, reg, data, length);
}

static int wait_control_clear(lps22hb_t *dev, uint8_t mask)
{
  for (unsigned i = 0; i < LPS22HB_POLL_LIMIT; ++i) {
    uint8_t control;
    int result = lps22hb_read_register(dev, LPS22HB_CTRL_REG2, &control, 1);
    if (result) return result;
    if (!(control & mask)) return SENSOR_OK;
  }
  return SENSOR_TIMEOUT;
}

int lps22hb_init(lps22hb_t *dev)
{
  uint8_t id = 0, value;
  if (!dev || !dev->bus.read || !dev->bus.write) return SENSOR_ARGUMENT;
  dev->initialized = 0;
  /* SA0 moze byt na doske zapojene na GND alebo VDD. */
  dev->address = LPS22HB_ADDRESS_LOW;
  int result = lps22hb_read_register(dev, LPS22HB_WHO_AM_I, &id, 1);
  if (result || id != LPS22HB_ID) {
    dev->address = LPS22HB_ADDRESS_HIGH;
    result = lps22hb_read_register(dev, LPS22HB_WHO_AM_I, &id, 1);
  }
  if (result) return result;
  if (id != LPS22HB_ID) return SENSOR_ID;
  value = LPS22HB_SWRESET;
  result = lps22hb_write_register(dev, LPS22HB_CTRL_REG2, &value, 1);
  if (result) return result;
  result = wait_control_clear(dev, LPS22HB_SWRESET);
  if (result) return result;
  /* ODR=0, BDU=0, IF_ADD_INC=1. Po konverzii sa registre nemenia (9.5/9.6). */
  const uint8_t config[2] = {0, LPS22HB_IF_ADD_INC};
  result = lps22hb_write_register(dev, LPS22HB_CTRL_REG1, config, sizeof config);
  if (!result) dev->initialized = 1;
  return result;
}

static int read_output(lps22hb_t *dev, uint8_t bytes[5])
{
  uint8_t value = LPS22HB_IF_ADD_INC | LPS22HB_ONE_SHOT;
  int result = lps22hb_write_register(dev, LPS22HB_CTRL_REG2, &value, 1);
  if (result) return result;
  result = wait_control_clear(dev, LPS22HB_ONE_SHOT);
  if (result) return result;
  uint8_t status;
  result = lps22hb_read_register(dev, LPS22HB_STATUS_REG, &status, 1);
  if (result) return result;
  if (!(status & LPS22HB_PRESS_READY)) return SENSOR_NOT_READY;
  /* Jeden prenos: tlak XL/L/H a teplota L/H. Dalsia konverzia este nebezi. */
  return lps22hb_read_register(dev, LPS22HB_PRESS_OUT_XL, bytes, 5);
}

int lps22hb_read_pressure(lps22hb_t *dev, float *hpa)
{
  uint8_t bytes[5];
  if (!dev || !dev->initialized || !hpa) return SENSOR_ARGUMENT;
  int result = read_output(dev, bytes);
  if (result) return result;
  int32_t raw = (int32_t)((uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16));
  if (raw & 0x800000) raw -= 0x1000000;
  float value = raw / 4096.0f;
  if (value < 260.0f || value > 1260.0f) return SENSOR_RANGE;
  *hpa = value;
  return SENSOR_OK;
}

int lps22hb_read_temperature(lps22hb_t *dev, float *celsius)
{
  uint8_t bytes[5];
  if (!dev || !dev->initialized || !celsius) return SENSOR_ARGUMENT;
  int result = read_output(dev, bytes);
  if (result) return result;
  int32_t raw = bytes[3] | ((uint32_t)bytes[4] << 8);
  if (raw & 0x8000) raw -= 0x10000;
  *celsius = raw / 100.0f;
  return SENSOR_OK;
}
