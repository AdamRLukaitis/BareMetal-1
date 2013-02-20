#include "bm/i2c.h"
#include <errno.h>

//#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
//#define be_to_cpu16(x)
//#define cpu_to_be16(x)
//#else
#define be_to_cpu16(x) (x = ((x & 0xff00) >> 8) | ((x & 0xff) << 8))
#define cpu_to_be16(x) (x = ((x & 0xff00) >> 8) | ((x & 0xff) << 8))
//#endif

int i2c_action(const struct i2c_client *client, uint8_t command, char read, int size, uint8_t *values)
{
    if (size < 0 || size > I2C_MAX_BLOCK_SIZE)
        return -EINVAL;

    uint8_t msgbuf[size + 1];

    struct i2c_msg msg[2] =
    {
        {
            .addr = client->addr,
            .flags = 0,
            .len = read ? 1 : size + 1,
            .buf = read ? &command : msgbuf,
        },
        {
            .addr = client->addr,
            .flags = I2C_MSG_READ,
            .len = size,
            .buf = values,
        }
    };

    int msg_num = read ? 2 : 1;

    if (!read)
    {
        msgbuf[0] = command;
        int i;
        for (i = 0; i < size; i++)
        {
            msgbuf[i + 1] = values[i];
        }
    }

    if (size == 0 && read)
    {
        msg[0].flags = I2C_MSG_READ;
        msg[0].len = 1;
        msg[0].buf = values;
        msg_num = 1;
    }

    return i2c_transfer(client->adapter, msg, msg_num);
}

int32_t i2c_read_byte(const struct i2c_client *client)
{
    uint8_t result;
    int state = i2c_action(client, 0, 1, 0, &result);
    return state < 0 ? state : result;
}

int32_t i2c_write_byte(const struct i2c_client *client, uint8_t value)
{
    return i2c_action(client, 0, 0, 0, &value);
}

int32_t i2c_read_byte_data(const struct i2c_client *client, uint8_t command)
{
    uint8_t result;
    int state = i2c_action(client, command, 1, 1, &result);
    return state < 0 ? state : result;
}

int32_t i2c_write_byte_data(const struct i2c_client *client, uint8_t command, uint8_t value)
{
    return i2c_action(client, command, 0, 1, &value);
}

int32_t i2c_read_word_data(const struct i2c_client *client, uint8_t command)
{
    uint16_t result;
    int state = i2c_action(client, command, 1, 2, &result);
    be_to_cpu16(result);
    return state < 0 ? state : result;
}

int32_t i2c_write_word_data(const struct i2c_client *client, uint8_t command, uint16_t value)
{
    cpu_to_be16(value);
    return i2c_action(client, command, 0, 2, &value);
}

int32_t i2c_read_block_data(const struct i2c_client *client, uint8_t command, uint8_t length, uint8_t *values)
{
    return i2c_action(client, command, 1, length, values);
}

int32_t i2c_write_block_data(const struct i2c_client *client, uint8_t command, uint8_t length, const uint8_t *values)
{
    return i2c_action(client, command, 0, length, values);
}
