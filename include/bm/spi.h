#ifndef BAREMETAL_SPI_H
#define BAREMETAL_SPI_H

#include <stdint.h>

#include "gpio.h"

/*! \defgroup spi SPI - Serial Peripheral Interface bus support
 * \{
 */


/*! Represent an SPI master adapter */
struct spi_master
{
    uint8_t bus_num;                               /*!< Implementation-specific SPI bus id */

    uint8_t direction;                             /*!< SPI bus direction */
#define SPI_DIR_BOTH  0x00                         /*!< Bidirectional SPI */
#define SPI_DIR_RX    0x01                         /*!< SPI receiver */
#define SPI_DIR_TX    0x02                         /*!< SPI transmitter */

    uint8_t mode;                                  /*!< SPI bus mode flags */
#define SPI_CPHA_HIGH 0x01
#define SPI_CPOL_HIGH 0x02
#define SPI_MODE_0    (0|0)
#define SPI_MODE_1    (0|SPI_CPHA_HIGH)
#define SPI_MODE_2    (SPI_CPOL_HIGH|0)
#define SPI_MODE_3    (SPI_CPOL_HIGH|SPI_CPHA_HIGH)
#define SPI_3WIRE     0x04                         /*!< MOSI and MISO at same pin */
#define SPI_LSB_FIRST 0x08                         /*!< Least significant bit of word is sent first */

    uint8_t bits_per_word;                         /*!< Number of bits in SPI word */
    uint32_t speed;                                /*!< Adapter speed */

    void *impl;                                    /*!< Implementation-specific data. */
};

/*! Represent an SPI slave device */
struct spi_client
{
    struct spi_master *master;                     /*!< SPI master adapter */
    uint16_t chip_select;                          /*!< CS pin GPIO */

    uint8_t flags;                                 /*!< Slave device flags */
#define SPI_NO_CS     0x01                         /*!< Device hasn't CS pin */
#define SPI_CS_HIGH   0x04                         /*!< Device CS pin is inverted */
};

/*! SPI message */
struct spi_message
{
    const void *tx_buf;                            /*!< TX buffer, if zero, 0 are transmitted to SPI bus */
    void *rx_buf;                                  /*!< RX buffer */

    uint16_t len;                                  /*!< Buffers size */
    uint8_t cs_change;                             /*!< Toggle CS after message */
    uint16_t delay_usecs;                          /*!< Delay for microseconds after message */
};

/*! Init SPI adapter.
 * \param master SPI adapter.
 * \returns 0 on success, negative error code otherwise.
 * \note This function must be implemented by platform port.
 */
int spi_init(struct spi_master *master);

/*! Process SPI messages synchronous.
 * CS deasserted (asserted) before transmission and asserted (deasserted) after all message are processed.
 * \param client SPI client.
 * \param messages SPI messages.
 * \param num number of messages.
 * \returns 0 on success, negative error code otherwise.
 * \note This function must be implemented by platform port.
 */
int spi_sync(struct spi_client *client, struct spi_message *messages, int num);

/*! Deinit SPI adapter.
 * \param master SPI adapter.
 * \returns 0 on success, negative error code otherwise.
 * \note This function must be implemented by platform port.
 */
int spi_deinit(struct spi_master *master);

/*! \} */

#endif
