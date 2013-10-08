#ifndef BAREMETAL_I2C_H
#define BAREMETAL_I2C_H

#include <stdint.h>

#define I2C_MAX_BLOCK_SIZE 32

/*! \defgroup i2c I2C - Inter-Integrated Circuit protocol support
 * \{
 */

/*! Represent an I2C master adapter */
struct i2c_adapter
{
    uint8_t bus_num;     /*!< Implementation-specific bus id */
    uint32_t speed;      /*!< Adapter speed in Hz.  */
    int timeout;         /*!< Operations time out in system ticks (usually tick = ms). */
    void *impl;          /*!< Implementation-specific data. */
};

/*! Represent an I2C client (slave device on I2C bus) */
struct i2c_client
{
    uint8_t addr;                 /*!< 7-bit client address. In <b>lower</b> bits. */
    struct i2c_adapter *adapter;  /*!< Adapter to which client is connected. */
};

/*! I2C transaction segment beginning with START */
struct i2c_msg
{
    uint8_t addr;                   /*!< 7-bit client address. In <b>lower</b> bits. */
    uint8_t flags;                  /*!< Message flags. */
#define I2C_MSG_READ 0x01           /*!< Indicates that we are reading from client. */
    uint16_t len;                   /*!< Payload length. */
    uint8_t *buf;                   /*!< Message payload. */
};

/*! Initialize adapter for communication.
 * \param adap I2C adapter.
 * \returns 0 on success, negative error code otherwise.
 * \note This function must be implemented by platform port.
 */
int i2c_init_adapter(struct i2c_adapter *adap);

/*! Initialize adapter for communication.
 * \param adap I2C adapter.
 * \returns 0 on success, negative error code otherwise.
 * \note This function must be implemented by platform port.
 */
int i2c_deinit_adapter(struct i2c_adapter *adap);

/*! Transfer I2C messages.
 * \param adap I2C adapter.
 * \param msgs message array.
 * \param num message count.
 * \returns 0 on success, negative error code otherwise.
 */
/*!
 * Function that transfers messages to I2C bus.
 * Every message starts with <b>START</b> condition.
 * After <b>all</b> messages have been transferred, <b>STOP</b> condition is generated.
 * \note This function must be implemented by platform port.
 */
int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num);

/*! Read single byte from slave.
 * \param client I2C client.
 * \returns byte on success, negative error code otherwise.
 */
int32_t i2c_read_byte(const struct i2c_client *client);
/*! Write single byte from slave.
 * \param client I2C client.
 * \returns 0 on success, negative error code otherwise.
 * \param value value that must be written.
 */
int32_t i2c_write_byte(const struct i2c_client *client, uint8_t value);

/*! Read single byte from slave using command code.
 * \param client I2C client.
 * \param command command that sended to client before reading (e.g. register addres).
 * \returns byte on success, negative error code otherwise.
 */
int32_t i2c_read_byte_data(const struct i2c_client *client, uint8_t command);
/*! Write single byte to slave using command code.
 * \param client I2C client.
 * \param command command that sended to client before writing (e.g. register addres).
 * \param value value that must be written.
 * \returns 0 on success, negative error code otherwise.
 */
int32_t i2c_write_byte_data(const struct i2c_client *client, uint8_t command, uint8_t value);

/*! Read two bytes from slave using command code.
 * \param client I2C client.
 * \param command command that sended to client before reading (e.g. register addres).
 * \returns word in native endianness on success, negative error code otherwise.
 */
/*!
 * \note It is implied that the most significant byte (MSB) is sended first.
 */
int32_t i2c_read_word_data(const struct i2c_client *client, uint8_t command);
/*! Write two bytes to slave using command code.
 * \param client I2C client.
 * \param command command that sended to client before writing (e.g. register addres).
 * \param value value that must be written.
 * \returns 0 on success, negative error code otherwise.
 */
/*!
 * \note The most significant byte (MSB) is sended first.
 */
int32_t i2c_write_word_data(const struct i2c_client *client, uint8_t command, uint16_t value);

/*! Read block data from slave using command code.
 * \param client I2C client.
 * \param command command that sended to client before reading.
 * \param length block data length
 * \param values array, in which data will be written.
 * \returns 0 on success, negative error code otherwise.
 */
int32_t i2c_read_block_data(const struct i2c_client *client, uint8_t command, uint8_t length, uint8_t *values);
/*! Write block data to slave using command code.
 * \param client I2C client.
 * \param command command that sended to client before reading.
 * \param length block data length
 * \param values data array.
 * \returns 0 on success, negative error code otherwise.
 */
int32_t i2c_write_block_data(const struct i2c_client *client, uint8_t command, uint8_t length, const uint8_t *values);

/*! \} */

#endif
