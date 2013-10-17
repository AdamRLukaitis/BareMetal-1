#include "bm/nrf24l01.h"
#include "bm/spi.h"
#include "bm/delay.h"
#include "bm/gpio.h"
#include <errno.h>

int nrf24l01_init_struct(struct spi_master* master, uint16_t cs_gpio, uint16_t ce_gpio, uint16_t irq_gpio, struct nrf24l01 *device)
{
    device->spi.master = master;
    device->spi.chip_select = cs_gpio;
    device->spi.flags = 0;
    device->ce_gpio = ce_gpio;
    device->irq_gpio = irq_gpio;
    return 0;
}

int nrf24l01_power_up(struct nrf24l01 *device)
{
    uint8_t reg;
    int status;

    status = nrf24l01_read_register(device, NRF24L01_REG_CONFIG, &reg);
    if (status)
        return status;

    reg |= NRF24L01_PWR_UP;

    return nrf24l01_write_register(device, NRF24L01_REG_CONFIG, reg);
}

int nrf24l01_power_down(struct nrf24l01 *device)
{
    uint8_t reg;
    int status;

    status = nrf24l01_read_register(device, NRF24L01_REG_CONFIG, &reg);
    if (status)
        return status;

    reg &= ~(NRF24L01_PWR_UP);

    return nrf24l01_write_register(device, NRF24L01_REG_CONFIG, reg);
}

int nrf24l01_read_register(struct nrf24l01 *device, uint8_t reg, uint8_t *data)
{
    uint8_t op = NRF24L01_CMD_R_REGISTER | (reg & 0x1F);
    struct spi_message messages[2] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        },
        {
            .tx_buf = 0,
            .rx_buf = data,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };
    return spi_sync(&device->spi, messages, 2);
}

int nrf24l01_write_register(struct nrf24l01 *device, uint8_t reg, uint8_t data)
{
    uint8_t op = NRF24L01_CMD_W_REGISTER | (reg & 0x1F);
    struct spi_message messages[2] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        },
        {
            .tx_buf = &data,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };
    return spi_sync(&device->spi, messages, 2);
}

int nrf24l01_read_address_register(struct nrf24l01 *device, uint8_t reg, uint8_t data[5])
{
    uint8_t op = NRF24L01_CMD_R_REGISTER | (reg & 0x1F);
    struct spi_message messages[2] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        },
        {
            .tx_buf = 0,
            .rx_buf = data,
            .len = 5,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };
    return spi_sync(&device->spi, messages, 2);
}

int nrf24l01_write_address_register(struct nrf24l01 *device, uint8_t reg, uint8_t data[5])
{
    uint8_t op = NRF24L01_CMD_W_REGISTER | (reg & 0x1F);
    struct spi_message messages[2] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        },
        {
            .tx_buf = data,
            .rx_buf = 0,
            .len = 5,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };
    return spi_sync(&device->spi, messages, 2);
}

/*!
 * \arg delay - auto retransmit delay (delay defined from end of transmission to start
                of next transmission) T = (delay + 1) * 250 us. From 0 to 15.
 * \arg count - auto retransmit count. From 0 to 15
 *
 */
int nrf24l01_setup_retransmit(struct nrf24l01 *device, uint8_t delay, uint8_t count)
{
    uint8_t reg = ((delay & 0xF) << 4) | (count & 0xF);
    return nrf24l01_write_register(device, NRF24L01_REG_SETUP_RETR, reg);
}

/*!
 * \arg channel - RF channel, from 0 to 127. F = 2400 + channel (MHz)
 */
int nrf24l01_set_channel(struct nrf24l01 *device, uint8_t channel)
{
    uint8_t reg = channel & 0x7F;
    return nrf24l01_write_register(device, NRF24L01_REG_RF_CH, reg);
}

int nrf24l01_get_status(struct nrf24l01 *device, uint8_t *status)
{
    uint8_t op = NRF24L01_CMD_NOP;
    struct spi_message messages[1] =
    {
        {
            .tx_buf = &op,
            .rx_buf = status,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };
    return spi_sync(&device->spi, messages, 1);
}

/*!
 * \arg irq - bitset of interrupt flags (device_RX_DR, NRF24L01_TX_DS, NRF24L01_MAX_RT)
 */
int nrf24l01_clear_irq(struct nrf24l01 *device, uint8_t irq)
{
    return nrf24l01_write_register(device, NRF24L01_REG_STATUS, irq);
}

/*!
 * \arg pipe - pipe number, from 0 to 5
 * \arg addr - address, for pipe 0 and 1 it should be 5-byte array, for pipes 2-5 - pointer to 1-byte address.
 */
int nrf24l01_set_rx_address(struct nrf24l01 *device, uint8_t pipe, uint8_t *addr)
{
    if ((pipe == 0) || (pipe == 1))
        return nrf24l01_write_address_register(device, NRF24L01_REG_RX_ADDR_P0 + pipe, addr);
    else if ((pipe > 2) && (pipe < 6))
        return nrf24l01_write_register(device, NRF24L01_REG_RX_ADDR_P0 + pipe, *addr);

    return -EINVAL;
}

int nrf24l01_set_tx_address(struct nrf24l01 *device, uint8_t addr[5])
{
    return nrf24l01_write_address_register(device, NRF24L01_REG_TX_ADDR, addr);
}

/*!
 * pipe - pipe number, from 0 to 5
 * size - payload size from 0 to 32, 0 - pipe disabled.
 */
int nrf24l01_set_payload_size(struct nrf24l01 *device, uint8_t pipe, uint8_t size)
{
    if ((pipe > 5) || (size > 32))
        return -EINVAL;

    return nrf24l01_write_register(device, NRF24L01_REG_RX_PW_P0 + pipe, size);
}

int nrf24l01_get_fifo_status(struct nrf24l01 *device, uint8_t *status)
{
    return nrf24l01_read_register(device, NRF24L01_REG_FIFO_STATUS, status);
}

int nrf24l01_enable_dynamic_size(struct nrf24l01 *device)
{
    uint8_t reg;
    int status;

    status = nrf24l01_read_register(device, NRF24L01_REG_FEATURE, &reg);

    if (status)
        return status;

    reg |= NRF24L01_EN_DPL;

    status = nrf24l01_write_register(device, NRF24L01_REG_FEATURE, reg);

    return status;
}

/*!
 * \arg - biset of pipes. e.g. 0x01 - pipe 0, 0x3F - all pipes.
 */
int nrf24l01_enable_dynamic_pipe_size(struct nrf24l01 *device, uint8_t pipes)
{
    uint8_t reg;
    int status;

    status = nrf24l01_read_register(device, NRF24L01_REG_DYNPD, &reg);

    if (status)
        return status;

    reg |= pipes & 0x3F;

    status = nrf24l01_write_register(device, NRF24L01_REG_DYNPD, reg);

    return status;
}

int nrf24l01_disable_dynamic_size(struct nrf24l01 *device)
{
    uint8_t reg;
    int status;

    status = nrf24l01_read_register(device, NRF24L01_REG_FEATURE, &reg);

    if (status)
        return status;

    reg &= ~NRF24L01_EN_DPL;

    status = nrf24l01_write_register(device, NRF24L01_REG_FEATURE, reg);

    return status;
}

/*!
 * \arg - biset of pipes. e.g. 0x01 - pipe 0, 0x3F - all pipes.
 */
int nrf24l01_disable_dynamic_pipe_size(struct nrf24l01 *device, uint8_t pipes)
{
    uint8_t reg;
    int status;

    status = nrf24l01_read_register(device, NRF24L01_REG_DYNPD, &reg);

    if (status)
        return status;

    reg &= ~(pipes & 0x3F);

    status = nrf24l01_write_register(device, NRF24L01_REG_DYNPD, reg);

    return status;
}

/*!
 * \arg pipe - pipe, to which the payload belongs.
 *             Values >5 means that RX payload FIFO is empty and data are invalid.
 */

int nrf24l01_read_payload(struct nrf24l01 *device, uint8_t size, uint8_t *data, uint8_t *pipe)
{
    if (size > 32)
        return -EINVAL;

    uint8_t op = NRF24L01_CMD_R_RX_PAYLOAD;
    uint8_t status;
    struct spi_message messages[2] =
    {
        {
            .tx_buf = &op,
            .rx_buf = &status,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        },
        {
            .tx_buf = 0,
            .rx_buf = data,
            .len = size,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };

    int result = spi_sync(&device->spi, messages, 2);

    *pipe = (status >> 1) & 0x7;

    return result;
}

int nrf24l01_write_payload(struct nrf24l01 *device, uint8_t size, uint8_t *data)
{
    if (size > 32)
        return -EINVAL;

    uint8_t op = NRF24L01_CMD_W_TX_PAYLOAD;
    struct spi_message messages[2] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        },
        {
            .tx_buf = data,
            .rx_buf = 0,
            .len = size,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };

    return spi_sync(&device->spi, messages, 2);
}

int nrf24l01_toggle_features(struct nrf24l01 *device)
{
    uint8_t op = NRF24L01_CMD_ACTIVATE;
    uint8_t feature = 0x73;
    struct spi_message messages[2] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        },
        {
            .tx_buf = &feature,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };

    return spi_sync(&device->spi, messages, 2);
}


int nrf24l01_flush_rx(struct nrf24l01 *device)
{
    uint8_t op = NRF24L01_CMD_FLUSH_RX;
    struct spi_message messages[1] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };

    return spi_sync(&device->spi, messages, 1);
}

int nrf24l01_flush_tx(struct nrf24l01 *device)
{
    uint8_t op = NRF24L01_CMD_FLUSH_TX;
    struct spi_message messages[1] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };

    return spi_sync(&device->spi, messages, 1);
}

int nrf24l01_get_size(struct nrf24l01 *device, uint8_t *size)
{
    uint8_t op = NRF24L01_CMD_R_RX_PL_WID;
    struct spi_message messages[2] =
    {
        {
            .tx_buf = &op,
            .rx_buf = 0,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        },
        {
            .tx_buf = 0,
            .rx_buf = size,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };
    return spi_sync(&device->spi, messages, 2);
}

int nrf24l01_enter_rx(struct nrf24l01 *device)
{
    uint8_t reg;
    int status;

    status = nrf24l01_read_register(device, NRF24L01_REG_CONFIG, &reg);
    if (status)
        return status;

    reg |= NRF24L01_PRIM_RX;

    status = nrf24l01_write_register(device, NRF24L01_REG_CONFIG, reg);

    if (status)
        return status;

    gpio_set_value(device->ce_gpio, 1);

    return 0;
}

int nrf24l01_enter_tx(struct nrf24l01 *device)
{
    uint8_t reg;
    int status;

    status = nrf24l01_read_register(device, NRF24L01_REG_CONFIG, &reg);
    if (status)
        return status;

    reg &= ~NRF24L01_PRIM_RX;

    status = nrf24l01_write_register(device, NRF24L01_REG_CONFIG, reg);

    if (status)
        return status;

    gpio_set_value(device->ce_gpio, 1);

    return 0;
}

int nrf24l01_enter_standby(struct nrf24l01 *device)
{
    gpio_set_value(device->ce_gpio, 0);
    return 0;
}

int nrf24l01_send(struct nrf24l01 *device, uint8_t *data, uint16_t size)
{
    uint16_t sended = 0;
    uint8_t status_reg;
    int status;

    status = nrf24l01_get_status(device, &status_reg);
    if (status)
        return status;

    if (status_reg & NRF24L01_TX_FULL)
        return -EBUSY;

    status = nrf24l01_enter_tx(device);
    if (status)
        return status;

    while (sended < size)
    {
        uint8_t psize = (size - sended > 32) ? 32 : size - sended;

        status = nrf24l01_write_payload(device, psize, data + sended);
        if (status)
            return status;

        //TODO: Timeout?
        for (;;)
        {
            status = nrf24l01_get_status(device, &status_reg);
            if (status)
                return status;

            if (status_reg & NRF24L01_MAX_RT)
            {
                nrf24l01_clear_irq(device, NRF24L01_MAX_RT);
                nrf24l01_flush_tx(device);
                nrf24l01_enter_standby(device);
                return -ETIMEDOUT;
            }
            if (status_reg & NRF24L01_TX_DS)
                break;

            system_nop();
        }

        sended += psize;
    }

    return nrf24l01_enter_standby(device);
}

/*!
 * \arg pipe - pipe on which data has been received.
 * \arg data - output data, the size of this array must be at least 32 bytes.
 * \arg size - received data size.
 * \arg timeout - receive timeout. 0 - means wait forever
 */
int nrf24l01_receive(struct nrf24l01 *device, uint8_t* pipe, uint8_t *data, uint8_t *size, uint16_t timeout)
{
    uint64_t tickStop = get_tick_count() + timeout;
    uint8_t status_reg;
    int status;

    status = nrf24l01_enter_rx(device);
    if (status)
        return status;

    while ((timeout = 0) || (get_tick_count() < tickStop))
    {
        status = nrf24l01_get_fifo_status(device, &status_reg);
        if (status)
            return status;

        if (status_reg & NRF24L01_FIFO_RX_EMPTY)
        {
            system_nop();
            continue;
        }

        status = nrf24l01_get_size(device, size);
        if (status)
            return status;

        status = nrf24l01_read_payload(device, *size, data, pipe);
        if (status)
            return status;

        break;
    }

    if (get_tick_count() >= tickStop)
        return -ETIMEDOUT;

    return nrf24l01_enter_standby(device);
}

/*!
 * \arg - biset of pipes. e.g. 0x01 - pipe 0, 0x3F - all pipes.
 */
int nrf24l01_enable_pipes(struct nrf24l01 *device, uint8_t pipes)
{
    uint8_t reg;
    int status;
    status = nrf24l01_read_register(device, NRF24L01_REG_EN_RXADDR, &reg);

    if (status)
        return status;

    reg &= ~(pipes & 0x3F);

    return nrf24l01_write_register(device, NRF24L01_REG_EN_RXADDR, reg);
}

