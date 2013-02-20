#include "bm/tsl2563.h"
#include <bm/delay.h>
#include <math.h>
#include <errno.h>

int tsl2563_init_struct(struct i2c_adapter *adapter, struct tsl2563 *device, uint8_t addr_pin)
{
    switch (addr_pin)
    {
    case TSL2563_PIN_GND:
        device->client.addr = TSL2563_ADDRESS_GND;
        break;
    case TSL2563_PIN_FLOAT:
        device->client.addr = TSL2563_ADDRESS_FLOAT;
        break;
    case TSL2563_PIN_VDD:
        device->client.addr = TSL2563_ADDRESS_VDD;
        break;
    default:
        return -EINVAL;
    }
    device->client.adapter = adapter;
    return 0;
}

int tsl2563_set_integration_time(struct tsl2563 *device, uint8_t time)
{
    if (time > 4)
        return -EINVAL;

    uint8_t timing;
    int result = i2c_read_byte_data(&device->client, TSL2563_REGISTER_TIMING);
    if (result < 0)
        return result;
    timing = (((uint8_t)result) & 0xfc) | (time & 0x3);
    return i2c_write_byte_data(&device->client, TSL2563_REGISTER_TIMING, timing);
}

int tsl2563_set_gain(struct tsl2563 *device, uint8_t gain)
{
    if (gain > 1)
        return -EINVAL;

    uint8_t timing;
    int result = i2c_read_byte_data(&device->client, TSL2563_REGISTER_TIMING);
    if (result < 0)
        return result;
    timing = (((uint8_t)result) & 0xef) | ((gain & 0x1) << 4);
    return i2c_write_byte_data(&device->client, TSL2563_REGISTER_TIMING, timing);
}

int tsl2563_read_channel(struct tsl2563 *device, uint8_t channel, uint16_t *value)
{
    if (channel > 1)
        return -EINVAL;

    uint8_t base_addr = channel ? TSL2563_REGISTER_DATA1LOW : TSL2563_REGISTER_DATA0LOW;

    int result = i2c_read_byte_data(&device->client, base_addr);
    if (result < 0)
        return result;
    *value = ((uint16_t)result) & 0xff;

    result = i2c_read_byte_data(&device->client, base_addr + 1);
    if (result < 0)
        return result;
    *value |= (((uint16_t)result) & 0xff) << 8;

    return 0;
}

int tsl2563_power_on(struct tsl2563 *device)
{
    return i2c_write_byte_data(&device->client, TSL2563_REGISTER_CONTROL, 0x3);
}

int tsl2563_power_off(struct tsl2563 *device)
{
    return i2c_write_byte_data(&device->client, TSL2563_REGISTER_CONTROL, 0x0);
}

float tsl2563_calc_lux(uint16_t channel0, uint16_t channel1, uint8_t gain)
{
    float ch0 = channel0, ch1 = channel1;

    float ratio = ch1 / ch0;

    /*if(gain)
    {
        ch0 /= 16.0f;
        ch1 /= 16.0f;
    }*/

    float result = 0;

    if (ratio <= 0.5f)
        result = 0.0304f * ch0 - 0.062f * ch0 * (powf(ratio, 1.4f));
    else if (ratio <= 0.61f)
        result = 0.0224f * ch0 - 0.031f * ch1;
    else if (ratio <= 0.8f)
        result = 0.0128f * ch0 - 0.0153f * ch1;
    else if (ratio <= 1.3f)
        result = 0.00146f * ch0 - 0.00112f * ch1;
    else
        result = 0;

    return result;
}
