#include <bm/delay.h>
#include <bm/gpio.h>
#include <bm/spi.h>

#include <stm32f10x.h>
#include <stm32f10x_spi.h>

#include <errno.h>

int spi_init(struct spi_master *master)
{
    SPI_TypeDef *device = 0;
    switch (master->bus_num)
    {
    case 1:
        device = SPI1;
        break;
    case 2:
        device = SPI2;
        break;
    case 3:
        device = SPI3;
        break;
    default:
        return -EINVAL;
        break;
    }

    if (master->mode & SPI_3WIRE)
        return -ENOTSUP;

    SPI_InitTypeDef conf;

    if (master->speed >= SystemCoreClock / 2)
        conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    else if (master->speed >= SystemCoreClock / 4)
        conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    else if (master->speed >= SystemCoreClock / 8)
        conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    else if (master->speed >= SystemCoreClock / 16)
        conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    else if (master->speed >= SystemCoreClock / 32)
        conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    else if (master->speed >= SystemCoreClock / 64)
        conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    else if (master->speed >= SystemCoreClock / 128)
        conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    else
        conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;

    conf.SPI_CPHA = (master->mode & SPI_CPHA_HIGH) ? SPI_CPHA_2Edge : SPI_CPHA_1Edge;
    conf.SPI_CPOL = (master->mode & SPI_CPOL_HIGH) ? SPI_CPOL_High : SPI_CPOL_Low;
    conf.SPI_CRCPolynomial = 0;

    if (master->bits_per_word == 8)
        conf.SPI_DataSize = SPI_DataSize_8b;
    else if (master->bits_per_word == 16)
        conf.SPI_DataSize = SPI_DataSize_16b;
    else
        return -ENOTSUP;

    switch (master->direction)
    {
    case SPI_DIR_RX:
        conf.SPI_Direction = SPI_Direction_1Line_Rx;
        break;
    case SPI_DIR_TX:
        conf.SPI_Direction = SPI_Direction_1Line_Tx;
        break;
    case SPI_DIR_BOTH:
        conf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        break;
    }

    conf.SPI_FirstBit = (master->mode & SPI_LSB_FIRST) ? SPI_FirstBit_LSB : SPI_FirstBit_MSB;
    conf.SPI_Mode = SPI_Mode_Master;
    conf.SPI_NSS = SPI_NSS_Soft;

    SPI_Init(device, &conf);
    SPI_Cmd(device, ENABLE);

    return 0;
}

/*struct spi_transfer
{
    const void *tx_buf;
    void *rx_buf;

    uint16_t len;
    uint8_t cs_change;
    uint8_t bits_per_word;
    uint16_t delay_usecs;
    uint32_t speed;
};*/

int spi_sync(struct spi_client *client, struct spi_message *messages, int num)
{
    SPI_TypeDef *device = 0;
    switch (client->master->bus_num)
    {
    case 1:
        device = SPI1;
        break;
    case 2:
        device = SPI2;
        break;
    case 3:
        device = SPI3;
        break;
    default:
        return -EINVAL;
        break;
    }

    uint8_t size = 0;

    if (client->master->bits_per_word == 8)
        size = 1;
    else if (client->master->bits_per_word == 16)
        size = 2;

    if (!(client->flags & SPI_NO_CS))
    {
        if (client->flags & SPI_CS_HIGH)
            gpio_set_value(client->chip_select, 1);
        else
            gpio_set_value(client->chip_select, 0);
    }
    for (int i = 0; i < num; i++)
    {
        if (messages[i].speed || messages[i].bits_per_word)
            return -ENOTSUP;

        for (int j = 0; j < messages[i].len; j++)
        {
            if (messages[i].tx_buf)
                SPI_I2S_SendData(device, (size == 2) ? ((uint16_t*)messages[i].tx_buf)[j] : ((uint8_t*)messages[i].tx_buf)[j]);
            else
                SPI_I2S_SendData(device, 0x0);

            BM_WAIT(SPI_I2S_GetFlagStatus(device, SPI_I2S_FLAG_TXE) == RESET);
            BM_WAIT(SPI_I2S_GetFlagStatus(device, SPI_I2S_FLAG_RXNE) == RESET);

            if (messages[i].rx_buf)
            {
                if (size == 2)
                    ((uint16_t*)messages[i].rx_buf)[j] = SPI_I2S_ReceiveData(device) & 0xffff;
                else
                    ((uint8_t*)messages[i].rx_buf)[j] = SPI_I2S_ReceiveData(device) & 0xff;
            }
            else
                SPI_I2S_ReceiveData(device);
        }
        if (messages[i].cs_change && !(client->flags & SPI_NO_CS))
        {
            if (client->flags & SPI_CS_HIGH)
                gpio_set_value(client->chip_select, 0);
            else
                gpio_set_value(client->chip_select, 1);
        }
        if (messages[i].delay_usecs)
            delay_us(messages[i].delay_usecs);
    }
    if (!(client->flags & SPI_NO_CS))
    {
        if (client->flags & SPI_CS_HIGH)
            gpio_set_value(client->chip_select, 0);
        else
            gpio_set_value(client->chip_select, 1);
    }
    return 0;
}

int spi_deinit(struct spi_master *master)
{
    SPI_TypeDef *device = 0;
    switch (master->bus_num)
    {
    case 1:
        device = SPI1;
        break;
    case 2:
        device = SPI2;
        break;
    case 3:
        device = SPI3;
        break;
    default:
        return -EINVAL;
        break;
    }

    SPI_Cmd(device, DISABLE);
    return 0;
}
