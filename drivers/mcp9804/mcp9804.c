#include "bm/mcp9804.h"
#include <bm/delay.h>
#include <errno.h>

int mcp9804_init_struct(struct i2c_adapter *adapter, struct mcp9804 *mcp9804, uint8_t address_pin_value)
{
    if (address_pin_value > 7)
        return -EINVAL;

    mcp9804->client.addr = MCP9804_BASE_ADDRESS + address_pin_value;
    mcp9804->client.adapter = adapter;

    return 0;
}

int mcp9804_power_on(struct mcp9804 *mcp9804)
{
    int result;
    result = i2c_read_word_data(&mcp9804->client, MCP9804_REGISTER_CONFIG);
    if (result < 0)
        return result;

    uint16_t config = (uint16_t)result;
    config &= ~MCP9804_CONFIG_SHDN;

    return i2c_write_word_data(&mcp9804->client, MCP9804_REGISTER_CONFIG, config);
}

int mcp9804_set_resolution(struct mcp9804 *mcp9804, uint8_t resolution)
{
    if (resolution > 3)
        return -EINVAL;

    return i2c_write_word_data(&mcp9804->client, MCP9804_REGISTER_RESOLUTION, resolution);
}

int mcp9804_read_temperature(struct mcp9804 *mcp9804, float *temperature)
{
    int result;
    result = i2c_read_word_data(&mcp9804->client, MCP9804_REGISTER_AMBIENT_TEMP);
    if (result < 0)
        return result;

    uint16_t temp = (uint16_t)result;
    *temperature = (((temp >> 12) & 0x1) ? -1.0f : 1.0f) * (((float)(temp & 0x7)) / 16.0f + (float)((temp >> 4) & 0xff));

    return 0;
}

int mcp9804_power_off(struct mcp9804 *mcp9804)
{
    int result;
    result = i2c_read_word_data(&mcp9804->client, MCP9804_REGISTER_CONFIG);
    if (result < 0)
        return result;

    uint16_t config = (uint16_t)result;
    config |= MCP9804_CONFIG_SHDN;

    return i2c_write_word_data(&mcp9804->client, MCP9804_REGISTER_CONFIG, config);
}
