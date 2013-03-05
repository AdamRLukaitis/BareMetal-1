#ifndef BAREMETAL_SPI_H
#define BAREMETAL_SPI_H

#include "gpio.h"

/*! \defgroup spi SPI - Serial Peripheral Interface bus support
 * \{
 */


struct spi_master
{
    uint8_t bus_num;

    uint8_t direction;
#define SPI_DIR_BOTH  0x00
#define SPI_DIR_RX    0x01
#define SPI_DIR_TX    0x02

    uint8_t mode;
#define SPI_CPHA_HIGH 0x01
#define SPI_CPOL_HIGH 0x02
#define SPI_MODE_0    (0|0)
#define SPI_MODE_1    (0|SPI_CPHA_HIGH)
#define SPI_MODE_2    (SPI_CPOL_HIGH|0)
#define SPI_MODE_3    (SPI_CPOL_HIGH|SPI_CPHA_HIGH)
#define SPI_3WIRE     0x04
#define SPI_LSB_FIRST 0x08

    uint8_t bits_per_word;
    uint32_t speed;
};

struct spi_client
{
    struct spi_master *master;
    uint16_t chip_select;

    uint8_t flags;
#define SPI_NO_CS     0x01
#define SPI_CS_HIGH   0x04
};

struct spi_message
{
    const void *tx_buf;
    void *rx_buf;

    uint16_t len;
    uint8_t cs_change;
    uint8_t bits_per_word;
    uint16_t delay_usecs;
    uint32_t speed;
};

int spi_init(struct spi_master *master);
int spi_sync(struct spi_client *client, struct spi_message *messages, int num);
int spi_deinit(struct spi_master *master);

/*! \} */

#endif
