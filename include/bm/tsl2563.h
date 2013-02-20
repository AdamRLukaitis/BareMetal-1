#ifndef BAREMETAL_TSL2563_H
#define BAREMETAL_TSL2563_H

/*!
 * \defgroup drivers Drivers - Device drivers
 * \{
 * \defgroup tsl2563 TSL2563 - TAOS ambient light sensor.
 * \{
 */

#include <stdint.h>
#include <bm/i2c.h>

#define TSL2563_PIN_GND 0
#define TSL2563_PIN_FLOAT 1
#define TSL2563_PIN_VDD 2

#define TSL2563_ADDRESS_GND 0x29
#define TSL2563_ADDRESS_FLOAT 0x39
#define TSL2563_ADDRESS_VDD 0x49

#define TSL2563_INT_14MS 0x0
#define TSL2563_INT_101MS 0x1
#define TSL2563_INT_402MS 0x2
#define TSL2563_INT_MANUAL 0x3

#define TSL2563_GAIN_1X 0
#define TSL2563_GAIN_16X 1

#define TSL2563_REGISTER_CONTROL 0x80
#define TSL2563_REGISTER_TIMING 0x81
#define TSL2563_REGISTER_THRESHLOWLOW 0x82
#define TSL2563_REGISTER_THRESHLOWHIGH 0x83
#define TSL2563_REGISTER_THRESHHIGHLOW 0x84
#define TSL2563_REGISTER_THRESHHIGHHIGH 0x85
#define TSL2563_REGISTER_INTERRUPT 0x86
#define TSL2563_REGISTER_ID 0x8a
#define TSL2563_REGISTER_DATA0LOW 0x8c
#define TSL2563_REGISTER_DATA0HIGH 0x8d
#define TSL2563_REGISTER_DATA1LOW 0x8e
#define TSL2563_REGISTER_DATA1HIGH 0x8f

struct tsl2563
{
    struct i2c_client client;
};

int tsl2563_init_struct(struct i2c_adapter *adapter, struct tsl2563 *device, uint8_t addr_pin);

int tsl2563_power_on(struct tsl2563 *device);
int tsl2563_power_off(struct tsl2563 *device);

int tsl2563_set_integration_time(struct tsl2563 *device, uint8_t time);
int tsl2563_read_channel(struct tsl2563 *device, uint8_t channel, uint16_t *value);

float tsl2563_calc_lux(uint16_t channel0, uint16_t channel1, uint8_t gain);

//! \} \}

#endif
