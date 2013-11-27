#ifndef BAREMETAL_NRF24L01_H
#define BAREMETAL_NRF24L01_H

/*!
 * \defgroup drivers Drivers - Device drivers
 * \{
 * \defgroup nrf24l01 NRF24L01 - Single Chip 2.4GHz Transceiver
 * \{
 */

#include <stdint.h>
#include <bm/spi.h>

#define NRF24L01_CMD_R_REGISTER 0x00
#define NRF24L01_CMD_W_REGISTER 0x20
#define NRF24L01_CMD_R_RX_PAYLOAD 0x61
#define NRF24L01_CMD_W_TX_PAYLOAD 0xA0
#define NRF24L01_CMD_FLUSH_TX 0xE1
#define NRF24L01_CMD_FLUSH_RX 0xE2
#define NRF24L01_CMD_REUSE_TX_PL 0xE3
#define NRF24L01_CMD_ACTIVATE 0x50
#define NRF24L01_CMD_R_RX_PL_WID 0x60
#define NRF24L01_CMD_W_ACK_PAYLOAD 0xA8
#define NRF24L01_CMD_W_TX_PAYLOAD_NO_ACK 0xB0
#define NRF24L01_CMD_NOP 0xFF

#define NRF24L01_REG_CONFIG 0x00
#define NRF24L01_REG_EN_AA 0x01
#define NRF24L01_REG_EN_RXADDR 0x02
#define NRF24L01_REG_SETUP_AW 0x03
#define NRF24L01_REG_SETUP_RETR 0x04
#define NRF24L01_REG_RF_CH 0x05
#define NRF24L01_REG_RF_SETUP 0x06
#define NRF24L01_REG_STATUS 0x07
#define NRF24L01_REG_OBSERVE_TX 0x08
#define NRF24L01_REG_CD 0x09
#define NRF24L01_REG_RX_ADDR_P0 0x0A
#define NRF24L01_REG_RX_ADDR_P1 0x0B
#define NRF24L01_REG_RX_ADDR_P2 0x0C
#define NRF24L01_REG_RX_ADDR_P3 0x0D
#define NRF24L01_REG_RX_ADDR_P4 0x0E
#define NRF24L01_REG_RX_ADDR_P5 0x0F
#define NRF24L01_REG_TX_ADDR 0x10
#define NRF24L01_REG_RX_PW_P0 0x11
#define NRF24L01_REG_RX_PW_P1 0x12
#define NRF24L01_REG_RX_PW_P2 0x13
#define NRF24L01_REG_RX_PW_P3 0x14
#define NRF24L01_REG_RX_PW_P4 0x15
#define NRF24L01_REG_RX_PW_P5 0x16
#define NRF24L01_REG_FIFO_STATUS 0x17
#define NRF24L01_REG_DYNPD 0x1C
#define NRF24L01_REG_FEATURE 0x1D

// CONFIG
#define NRF24L01_MASK_RX_DR 0x40
#define NRF24L01_MASK_TX_DS 0x20
#define NRF24L01_MASK_MAX_RT 0x10
#define NRF24L01_EN_CRC 0x08
#define NRF24L01_CRCO 0x04
#define NRF24L01_PWR_UP 0x02
#define NRF24L01_PRIM_RX 0x01

// EN_AA
#define NRF24L01_ENAA_P5 0x20
#define NRF24L01_ENAA_P4 0x10
#define NRF24L01_ENAA_P3 0x08
#define NRF24L01_ENAA_P2 0x04
#define NRF24L01_ENAA_P1 0x02
#define NRF24L01_ENAA_P0 0x01

// EN_RXADDR
#define NRF24L01_ERX_P5 0x20
#define NRF24L01_ERX_P4 0x10
#define NRF24L01_ERX_P3 0x08
#define NRF24L01_ERX_P2 0x04
#define NRF24L01_ERX_P1 0x02
#define NRF24L01_ERX_P0 0x01

// RF_SETUP
#define NRF24L01_PLL_LOCK 0x10
#define NRF24L01_RF_DR 0x8
#define NRF24L01_RF_PWR 0x6
#define NRF24L01_LNA_HCURR 0x01

// STATUS
#define NRF24L01_RX_DR 0x40
#define NRF24L01_TX_DS 0x20
#define NRF24L01_MAX_RT 0x10
#define NRF24L01_RX_P_NO 0x0E
#define NRF24L01_TX_FULL 0x01

// FIFO_STATUS
#define NRF24L01_FIFO_TX_REUSE 0x40
#define NRF24L01_FIFO_TX_FULL 0x20
#define NRF24L01_FIFO_TX_EMPTY 0x10
#define NRF24L01_FIFO_RX_FULL 0x20
#define NRF24L01_FIFO_RX_EMPTY 0x01

// FEATURE
#define NRF24L01_EN_DPL 0x4
#define NRF24L01_EN_ACK_PAY 0x2
#define NRF24L01_EN_DYN_ACK 0x1

struct nrf24l01
{
    struct spi_client spi;
    uint16_t ce_gpio;
    uint16_t irq_gpio;
};

int nrf24l01_init_struct(struct spi_master* master, uint16_t cs_gpio, uint16_t ce_gpio, uint16_t irq_gpio, struct nrf24l01 *device);
int nrf24l01_power_up(struct nrf24l01 *device);
int nrf24l01_power_down(struct nrf24l01 *device);

int nrf24l01_read_register(struct nrf24l01 *device, uint8_t reg, uint8_t *data);
int nrf24l01_write_register(struct nrf24l01 *device, uint8_t reg, uint8_t data);

//! Read 40-bit registers.
int nrf24l01_read_address_register(struct nrf24l01 *device, uint8_t reg, uint8_t data[5]);
//! Write 40-bit registers.
int nrf24l01_write_address_register(struct nrf24l01 *device, uint8_t reg, uint8_t data[5]);

//! Setup of auto-retransmission.
int nrf24l01_setup_retransmit(struct nrf24l01 *device, uint8_t delay, uint8_t count);
//! Setup RF channel.
int nrf24l01_set_channel(struct nrf24l01 *device, uint8_t channel);
//! Get status.
int nrf24l01_get_status(struct nrf24l01 *device, uint8_t *status);
//! Clear IRQ flags.
int nrf24l01_clear_irq(struct nrf24l01 *device, uint8_t irq);
//! Set RX address for pipe.
int nrf24l01_set_rx_address(struct nrf24l01 *device, uint8_t pipe, uint8_t *addr);
//! Enable RX pipes.
int nrf24l01_enable_pipes(struct nrf24l01 *device, uint8_t pipes);
//! Set TX address.
int nrf24l01_set_tx_address(struct nrf24l01 *device, uint8_t addr[5]);
//! Set RX payload size.
int nrf24l01_set_payload_size(struct nrf24l01 *device, uint8_t pipe, uint8_t size);
//! Get FIFO address.
int nrf24l01_get_fifo_status(struct nrf24l01 *device, uint8_t *status);
//! Enable dynamic payload length on device.
int nrf24l01_enable_dynamic_size(struct nrf24l01 *device);
//! Enable dynamic payload length on pipes.
int nrf24l01_enable_dynamic_pipe_size(struct nrf24l01 *device, uint8_t pipes);
//! Enable dynamic payload length on device.
int nrf24l01_disable_dynamic_size(struct nrf24l01 *device);
//! Enable dynamic payload length on pipes.
int nrf24l01_disable_dynamic_pipe_size(struct nrf24l01 *device, uint8_t pipes);

//! Read RX payload.
int nrf24l01_read_payload(struct nrf24l01 *device, uint8_t size, uint8_t *data, uint8_t *pipe);
//! Write TX payload.
int nrf24l01_write_payload(struct nrf24l01 *device, uint8_t size, uint8_t *data);

//! Toggle FEATURE register writable. See datasheet for more info.
int nrf24l01_toggle_features(struct nrf24l01 *device);

//! Flush RX buffer.
int nrf24l01_flush_rx(struct nrf24l01 *device);
//! Flush TX buffer.
int nrf24l01_flush_tx(struct nrf24l01 *device);

//! Get RX payload size.
int nrf24l01_get_size(struct nrf24l01 *device, uint8_t *size);

//! Enter RX mode.
int nrf24l01_enter_rx(struct nrf24l01 *device);
//! Enter TX mode.
int nrf24l01_enter_tx(struct nrf24l01 *device);
//! Enter standby mode.
int nrf24l01_enter_standby(struct nrf24l01 *device);

//! Send data as PTX.
int nrf24l01_send(struct nrf24l01 *device, uint8_t *data, uint16_t size);
//! Recieve data.
int nrf24l01_receive_packet(struct nrf24l01 *device, uint8_t* pipe, uint8_t *data, uint8_t *size, uint16_t timeout);
// Ugly routine
int nrf24l01_receive(struct nrf24l01 *device, uint8_t* pipe, uint8_t *data, uint16_t size, uint16_t timeout);


//! \} \}

#endif
