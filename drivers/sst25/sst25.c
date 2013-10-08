#include "bm/sst25.h"
#include "bm/spi.h"
#include "bm/delay.h"
#include <errno.h>

int sst25_init_struct(struct spi_master* master, uint16_t cs_gpio, struct sst25 *sst25)
{
    sst25->spi.master = master;
    sst25->spi.chip_select = cs_gpio;
    sst25->spi.flags = 0;
    sst25->id.manufacturer = 0;
    sst25->id.capacity = 0;
    sst25->id.type = 0;
    return 0;
}

int sst25_read_id(struct sst25 *sst25)
{
    uint8_t op = SST25_OP_JEDEC_ID;
    uint8_t jedec[3];
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
            .rx_buf = jedec,
            .len = 3,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };

    int status = spi_sync(&sst25->spi, messages, 2);

    if (status)
        return status;

    sst25->id.manufacturer = jedec[0];
    sst25->id.type = jedec[1];
    sst25->id.capacity = jedec[2];

    return 0;
}

int sst25_get_status(struct sst25 *sst25, uint8_t *reg)
{
    uint8_t op = SST25_OP_RDSR;
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
            .rx_buf = reg,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };

    return spi_sync(&sst25->spi, messages, 2);
}


int sst25_wait_for_ready(struct sst25 *sst25, int timeout)
{
    uint8_t status = 0;
    int retval = 0;

    BM_INIT_TIMEOUT_WAIT();
    BM_TIMEOUT_WAIT_MS(((retval = sst25_get_status(sst25, &status)) == 0) && (status & SST25_STATUS_BUSY), timeout)
    return -ETIMEDOUT;

    if (retval)
        return retval;

    return 0;
}

int sst25_erase(struct sst25 *sst25, uint16_t addr, int type)
{
    uint32_t realaddr;
    uint8_t op;
    switch (type)
    {
    case SST25_ERASE_4K:
        op = SST25_OP_SECTOR_ERASE;
        realaddr = (addr & 0xFFF) << 12;
        break;
    case SST25_ERASE_32K:
        op = SST25_OP_32K_ERASE;
        realaddr = (addr & 0x1FF) << 15;
        break;
    case SST25_ERASE_64K:
        op = SST25_OP_64K_ERASE;
        realaddr = (addr & 0xFF) << 16;
        break;
    case SST25_ERASE_CHIP:
        op = SST25_OP_CHIP_ERASE;
        realaddr = 0x00;
        break;
    default:
        return -EINVAL;
        break;
    }
    int status = sst25_write_enable(sst25);
    if (status)
        return status;

    char command[4] = {op, (realaddr >> 16) & 0xFF, (realaddr >> 8) & 0xFF, (realaddr) & 0xFF};

    struct spi_message message =
    {
        .tx_buf = command,
        .rx_buf = 0,
        .len = (type == SST25_ERASE_CHIP) ? 1 : 4,
        .cs_change = 0,
        .delay_usecs = 0
    };

    status = spi_sync(&sst25->spi, &message, 1);
    if (status)
        return status;

    int timeout = 0;

    switch (type)
    {
    case SST25_ERASE_4K:
        timeout = SST25_TIMEOUT_SECTOR_ERASE;
        break;
    case SST25_ERASE_32K:
    case SST25_ERASE_64K:
        timeout = SST25_TIMEOUT_BLOCK_ERASE;
        timeout = SST25_TIMEOUT_BLOCK_ERASE;
        break;
    case SST25_ERASE_CHIP:
        timeout = SST25_TIMEOUT_CHIP_ERASE;
        break;
    }
    return sst25_wait_for_ready(sst25, timeout);
}


int sst25_read_data(struct sst25 *sst25, uint32_t addr, uint8_t *data, uint16_t size)
{
    uint8_t op = SST25_OP_READ;
    char command[4] = {op, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF};
    struct spi_message messages[2] =
    {
        {
            .tx_buf = command,
            .rx_buf = 0,
            .len = 4,
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

    return spi_sync(&sst25->spi, messages, 2);
}

int sst25_write_data(struct sst25 *sst25, uint32_t addr, uint8_t *data, uint16_t size)
{
    if (!size)
        return 0;

    int status = sst25_write_enable(sst25);
    if (status)
        return status;

    uint16_t start = 0;
    if ((addr & 0x1) || (size == 1))
    {
        char command[5] = {SST25_OP_BYTE_PROGRAM, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF, data[0]};
        struct spi_message message =
        {
            .tx_buf = command,
            .rx_buf = 0,
            .len = 5,
            .cs_change = 0,
            .delay_usecs = 0
        };
        start = 1;
        int status = spi_sync(&sst25->spi, &message, 1);
        if (status)
            return status;

        delay_us(10);
    }
    if (size - start == 0)
        return 0;

    uint32_t waddr = addr + start;
    char command[6] = {SST25_OP_AAI_WORD_PROGRAM, (waddr >> 16) & 0xFF, (waddr >> 8) & 0xFF, (waddr) & 0xFF, data[start], data[start + 1]};
    struct spi_message message =
    {
        .tx_buf = command,
        .rx_buf = 0,
        .len = 6,
        .cs_change = 0,
        .delay_usecs = 0
    };
    status = spi_sync(&sst25->spi, &message, 1);
    if (status)
        return status;

    status = sst25_wait_for_ready(sst25, 1);
    if (status)
        return status;

    int sended = start + 2;
    for (int i = 1; i < (size - start) / 2; i++)
    {
        char command[3] = {SST25_OP_AAI_WORD_PROGRAM, data[start + i * 2], data[start + i * 2 + 1]};
        struct spi_message message =
        {
            .tx_buf = command,
            .rx_buf = 0,
            .len = 3,
            .cs_change = 0,
            .delay_usecs = 0
        };
        int status = spi_sync(&sst25->spi, &message, 1);
        if (status)
            return status;

        status = sst25_wait_for_ready(sst25, 1);
        if (status)
            return status;

        sended += 2;
    }
    if (size - sended > 0)
    {
        char command[5] = {SST25_OP_BYTE_PROGRAM, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF, data[size - 1]};
        struct spi_message message =
        {
            .tx_buf = command,
            .rx_buf = 0,
            .len = 5,
            .cs_change = 0,
            .delay_usecs = 0
        };
        int status = spi_sync(&sst25->spi, &message, 1);
        if (status)
            return status;

        delay_us(10);
    }
    status = sst25_write_disable(sst25);
    if (status)
        return status;
    return 0;
}

int sst25_write_enable(struct sst25 *sst25)
{
    uint8_t wren = SST25_OP_WREN;
    struct spi_message message =
    {
        .tx_buf = &wren,
        .rx_buf = 0,
        .len = 1,
        .cs_change = 0,
        .delay_usecs = 0
    };
    return spi_sync(&sst25->spi, &message, 1);
}

int sst25_write_disable(struct sst25 *sst25)
{
    uint8_t wrdi = SST25_OP_WRDI;
    struct spi_message message =
    {
        .tx_buf = &wrdi,
        .rx_buf = 0,
        .len = 1,
        .cs_change = 0,
        .delay_usecs = 0
    };
    return spi_sync(&sst25->spi, &message, 1);
}

int sst25_unprotect(struct sst25 *sst25)
{
    int state;
    uint8_t reg;
    state = sst25_get_status(sst25, &reg);
    if (state)
        return state;
    if (reg & SST25_STATUS_BPL)
        return -EROFS;

    uint8_t op = SST25_OP_EWSR;
    struct spi_message message =
    {
        .tx_buf = &op,
        .rx_buf = 0,
        .len = 1,
        .cs_change = 0,
        .delay_usecs = 0
    };
    state = spi_sync(&sst25->spi, &message, 1);
    if (state)
        return state;

    op = SST25_OP_WRSR;
    reg &= ~(SST25_STATUS_BP0 | SST25_STATUS_BP1 | SST25_STATUS_BP2 | SST25_STATUS_BP3);

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
            .rx_buf = &reg,
            .len = 1,
            .cs_change = 0,
            .delay_usecs = 0
        }
    };
    state = spi_sync(&sst25->spi, messages, 2);
    if (state)
        return state;

    return 0;
}
