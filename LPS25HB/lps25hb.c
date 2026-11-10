#include "lps25hb.h"

int lps25hb_register_bus(lps25hb_t *dev, sensor_read_fn read, sensor_write_fn write)
{
  if (!dev || !read || !write) return SENSOR_ARGUMENT;
  dev->bus.read = read;
  dev->bus.write = write;
  dev->address = LPS25HB_ADDRESS_LOW;
  dev->initialized = 0;
  return SENSOR_OK;
}

int lps25hb_read_register(lps25hb_t *dev, uint8_t reg, uint8_t *data, uint16_t length)
{
  if (!dev || !dev->bus.read || !data || !length || length > 128 || reg > 0x7f || reg + length > 128) return SENSOR_ARGUMENT;
  return dev->bus.read(dev->address, reg | (length > 1 ? LPS25HB_AUTO_INCREMENT : 0), data, length);
}

int lps25hb_write_register(lps25hb_t *dev, uint8_t reg, const uint8_t *data, uint16_t length)
{
  if (!dev || !dev->bus.write || !data || !length || length > 128 || reg > 0x7f || reg + length > 128) return SENSOR_ARGUMENT;
  return dev->bus.write(dev->address, reg | (length > 1 ? LPS25HB_AUTO_INCREMENT : 0), data, length);
}

int lps25hb_init(lps25hb_t *dev)
{
  uint8_t id = 0, value;
  if (!dev || !dev->bus.read || !dev->bus.write) return SENSOR_ARGUMENT;
  dev->initialized = 0;
  /* SA0 moze byt na doske zapojene na GND alebo VDD. */
  dev->address = LPS25HB_ADDRESS_LOW;
  int result = lps25hb_read_register(dev, LPS25HB_WHO_AM_I, &id, 1);
  if (result || id != LPS25HB_ID) {
    dev->address = LPS25HB_ADDRESS_HIGH;
    result = lps25hb_read_register(dev, LPS25HB_WHO_AM_I, &id, 1);
  }
  if (result) return result;
  if (id != LPS25HB_ID) return SENSOR_ID;
  value = 0;
  result = lps25hb_write_register(dev, LPS25HB_CTRL_REG1, &value, 1);
  if (result) return result;
  result = lps25hb_write_register(dev, LPS25HB_CTRL_REG2, &value, 1);
  if (result) return result;
  value = LPS25HB_RES_HIGH;
  result = lps25hb_write_register(dev, LPS25HB_RES_CONF, &value, 1);
  if (result) return result;
  value = LPS25HB_POWER_ON | LPS25HB_ODR_1HZ | LPS25HB_BDU;
  result = lps25hb_write_register(dev, LPS25HB_CTRL_REG1, &value, 1);
  if (!result) dev->initialized = 1;
  return result;
}

int lps25hb_read_pressure(lps25hb_t *dev, float *hpa)
{
  uint8_t status, bytes[3];
  if (!dev || !dev->initialized || !hpa) return SENSOR_ARGUMENT;
  int result = lps25hb_read_register(dev, LPS25HB_STATUS_REG, &status, 1);
  if (result) return result;
  if (!(status & LPS25HB_PRESS_READY)) return SENSOR_NOT_READY;
  result = lps25hb_read_register(dev, LPS25HB_PRESS_OUT_XL, bytes, 3);
  if (result) return result;
  int32_t raw = (int32_t)((uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16));
  if (raw & 0x800000) raw -= 0x1000000;
  float value = raw / 4096.0f;
  if (value < 260.0f || value > 1260.0f) return SENSOR_RANGE;
  *hpa = value;
  return SENSOR_OK;
}

int lps25hb_read_temperature(lps25hb_t *dev, float *celsius)
{
  uint8_t status, bytes[2];
  if (!dev || !dev->initialized || !celsius) return SENSOR_ARGUMENT;
  int result = lps25hb_read_register(dev, LPS25HB_STATUS_REG, &status, 1);
  if (result) return result;
  if (!(status & LPS25HB_TEMP_READY)) return SENSOR_NOT_READY;
  result = lps25hb_read_register(dev, LPS25HB_TEMP_OUT_L, bytes, 2);
  if (result) return result;
  int32_t raw = bytes[0] | ((uint32_t)bytes[1] << 8);
  if (raw & 0x8000) raw -= 0x10000;
  *celsius = 42.5f + raw / 480.0f;
  return SENSOR_OK;
}
