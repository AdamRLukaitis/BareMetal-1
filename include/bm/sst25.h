#ifndef BAREMETAL_SST25_H
#define BAREMETAL_SST25_H

/*!
 * \defgroup drivers Drivers - Device drivers
 * \{
 * \defgroup sst25 SST25 - SPI Serial Flash
 * \{
 */

#include <stdint.h>
#include <bm/spi.h>

#define SST25_OP_WRSR 0x01
#define SST25_OP_BYTE_PROGRAM 0x02
#define SST25_OP_READ 0x03
#define SST25_OP_WRDI 0x04
#define SST25_OP_RDSR 0x05
#define SST25_OP_WREN 0x06
#define SST25_OP_HS_READ 0x0B
#define SST25_OP_SECTOR_ERASE 0x20
#define SST25_OP_EWSR 0x50
#define SST25_OP_32K_ERASE 0x52
#define SST25_OP_CHIP_ERASE 0x60
#define SST25_OP_EBSY 0x70
#define SST25_OP_DBSY 0x80
#define SST25_OP_RDID 0x90
#define SST25_OP_JEDEC_ID 0x9F
#define SST25_OP_RDID_2 0xAB
#define SST25_OP_AAI_WORD_PROGRAM 0xAD
#define SST25_OP_CHIP_ERASE_2 0xC7
#define SST25_OP_64K_ERASE 0xD8

#define SST25_TIMEOUT_SECTOR_ERASE 30
#define SST25_TIMEOUT_BLOCK_ERASE 30
#define SST25_TIMEOUT_CHIP_ERASE 60

#define SST25_STATUS_BUSY 0x01
#define SST25_STATUS_WEL 0x02
#define SST25_STATUS_BP0 0x04
#define SST25_STATUS_BP1 0x08
#define SST25_STATUS_BP2 0x10
#define SST25_STATUS_BP3 0x20
#define SST25_STATUS_AAI 0x40
#define SST25_STATUS_BPL 0x80

#define SST25_ERASE_4K 0
#define SST25_ERASE_32K 1
#define SST25_ERASE_64K 2
#define SST25_ERASE_CHIP 3

struct sst25_jedec_id
{
    uint8_t manufacturer;
    uint8_t type;
    uint8_t capacity;
};

struct sst25
{
    struct spi_client spi;
    struct sst25_jedec_id id;
};

int sst25_init_struct(struct spi_master* master, uint16_t cs_gpio, struct sst25 *sst25);

int sst25_read_id(struct sst25 *sst25);
int sst25_wait_for_ready(struct sst25 *sst25, int timeout);
int sst25_get_status(struct sst25 *sst25, uint8_t *reg);
int sst25_unprotect(struct sst25 *sst25);

int sst25_read_data(struct sst25 *sst25, uint32_t addr, uint8_t *data, uint16_t size);
int sst25_write_data(struct sst25 *sst25, uint32_t addr, uint8_t *data, uint16_t size);

int sst25_erase(struct sst25 *sst25, uint16_t addr, int type);

int sst25_write_enable(struct sst25 *sst25);
int sst25_write_disable(struct sst25 *sst25);

//! \} \}

#endif
