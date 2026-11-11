#include "hts221.h"

static int16_t signed16(const uint8_t *bytes)
{
  int32_t value = bytes[0] | ((uint32_t)bytes[1] << 8);
  if (value & 0x8000) value -= 0x10000;
  return (int16_t)value;
}

int hts221_register_bus(hts221_t *dev, sensor_read_fn read, sensor_write_fn write)
{
  if (!dev || !read || !write) return SENSOR_ARGUMENT;
  dev->bus.read = read;
  dev->bus.write = write;
  dev->initialized = 0;
  return SENSOR_OK;
}

int hts221_read_register(hts221_t *dev, uint8_t reg, uint8_t *data, uint16_t length)
{
  if (!dev || !dev->bus.read || !data) return SENSOR_ARGUMENT;
  if (!length || length > 128 || reg > 0x7f || reg + length > 128) {
    return SENSOR_ARGUMENT;
  }
  return dev->bus.read(HTS221_ADDRESS, reg | (length > 1 ? HTS221_AUTO_INCREMENT : 0), data, length);
}

int hts221_write_register(hts221_t *dev, uint8_t reg, const uint8_t *data, uint16_t length)
{
  if (!dev || !dev->bus.write || !data) return SENSOR_ARGUMENT;
  if (!length || length > 128 || reg > 0x7f || reg + length > 128) {
    return SENSOR_ARGUMENT;
  }
  return dev->bus.write(HTS221_ADDRESS, reg | (length > 1 ? HTS221_AUTO_INCREMENT : 0), data, length);
}

int hts221_init(hts221_t *dev)
{
  uint8_t id, value, cal[16];
  if (!dev || !dev->bus.read || !dev->bus.write) return SENSOR_ARGUMENT;
  dev->initialized = 0;
  int result = hts221_read_register(dev, HTS221_WHO_AM_I, &id, 1);
  if (result) return result;
  if (id != HTS221_ID) return SENSOR_ID;
  value = 0;
  result = hts221_write_register(dev, HTS221_CTRL_REG1, &value, 1);
  if (result) return result;
  result = hts221_write_register(dev, HTS221_CTRL_REG2, &value, 1);
  if (result) return result;
  value = HTS221_AVG_T16_H32;
  result = hts221_write_register(dev, HTS221_AV_CONF, &value, 1);
  if (result) return result;
  result = hts221_read_register(dev, HTS221_CALIBRATION, cal, sizeof cal);
  if (result) return result;
  dev->h0 = cal[0] / 2.0f;
  dev->h1 = cal[1] / 2.0f;
  /* Dva horne bity teplot su ulozene v spolocnom registri 0x35. */
  dev->t0 = (cal[2] | ((uint16_t)(cal[5] & 3) << 8)) / 8.0f;
  dev->t1 = (cal[3] | ((uint16_t)(cal[5] & 12) << 6)) / 8.0f;
  dev->h0_out = signed16(&cal[6]);
  dev->h1_out = signed16(&cal[10]);
  dev->t0_out = signed16(&cal[12]);
  dev->t1_out = signed16(&cal[14]);
  if (dev->h0_out == dev->h1_out || dev->t0_out == dev->t1_out ||
      dev->h0 >= dev->h1 || dev->h1 > 100.0f || dev->t0 >= dev->t1) {
    return SENSOR_CALIBRATION;
  }
  value = HTS221_POWER_ON | HTS221_BDU | HTS221_ODR_1HZ;
  result = hts221_write_register(dev, HTS221_CTRL_REG1, &value, 1);
  if (!result) dev->initialized = 1;
  return result;
}

static int read_sample(hts221_t *dev, uint8_t reg, uint8_t ready, int16_t *raw)
{
  uint8_t status, bytes[2];
  int result = hts221_read_register(dev, HTS221_STATUS_REG, &status, 1);
  if (result) return result;
  if (!(status & ready)) return SENSOR_NOT_READY;
  result = hts221_read_register(dev, reg, bytes, 2);
  if (!result) *raw = signed16(bytes);
  return result;
}

int hts221_read_temperature(hts221_t *dev, float *celsius)
{
  int16_t raw;
  if (!dev || !dev->initialized || !celsius) return SENSOR_ARGUMENT;
  if (dev->t1_out == dev->t0_out) return SENSOR_CALIBRATION;
  int result = read_sample(dev, HTS221_TEMP_OUT_L, HTS221_TEMP_READY, &raw);
  if (result) return result;
  float value = dev->t0 + ((float)raw - dev->t0_out) * (dev->t1 - dev->t0) / ((float)dev->t1_out - dev->t0_out);
  if (value < -40.0f || value > 120.0f) return SENSOR_RANGE;
  *celsius = value;
  return SENSOR_OK;
}

int hts221_read_humidity(hts221_t *dev, float *percent)
{
  int16_t raw;
  if (!dev || !dev->initialized || !percent) return SENSOR_ARGUMENT;
  if (dev->h1_out == dev->h0_out) return SENSOR_CALIBRATION;
  int result = read_sample(dev, HTS221_HUMIDITY_OUT_L, HTS221_HUM_READY, &raw);
  if (result) return result;
  float value = dev->h0 + ((float)raw - dev->h0_out) * (dev->h1 - dev->h0) / ((float)dev->h1_out - dev->h0_out);
  if (value < 0.0f) value = 0.0f;
  if (value > 100.0f) value = 100.0f;
  *percent = value;
  return SENSOR_OK;
}
