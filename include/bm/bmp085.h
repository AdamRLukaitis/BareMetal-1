#ifndef BAREMETAL_BMP085_H
#define BAREMETAL_BMP085_H

/*!
 * \defgroup drivers Drivers - Device drivers
 * \{
 * \defgroup bmp085 BMP085 - Pressure/Temperature sensor.
 * \{
 */

#include <stdint.h>
#include <bm/i2c.h>

#define BMP085_I2C_ADDRESS 0x77

#define BMP085_EE_AC1_ADDRESS 0xAA
#define BMP085_EE_AC2_ADDRESS 0xAC
#define BMP085_EE_AC3_ADDRESS 0xAE
#define BMP085_EE_AC4_ADDRESS 0xB0
#define BMP085_EE_AC5_ADDRESS 0xB2
#define BMP085_EE_AC6_ADDRESS 0xB4
#define BMP085_EE_B1_ADDRESS 0xB6
#define BMP085_EE_B2_ADDRESS 0xB8
#define BMP085_EE_MB_ADDRESS 0xBA
#define BMP085_EE_MC_ADDRESS 0xBC
#define BMP085_EE_MD_ADDRESS 0xBE
#define BMP085_MEASURE_ADDRESS 0xF6

#define BMP085_CONTROL_REGISTER 0xF4

#define BMP085_TEMPERATURE_MEASURE 0x2E
#define BMP085_PRESSURE_MEASURE 0x34

struct bmp085_coefficients
{
    int16_t ac1;
    int16_t ac2;
    int16_t ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t b1;
    int16_t b2;
    int16_t mb;
    int16_t mc;
    int16_t md;
};

struct bmp085
{
    struct i2c_client client;
    struct bmp085_coefficients coefficients;
    uint16_t xclr_gpio;
};

void bmp085_enter_reset(struct bmp085 *bmp085);
void bmp085_exit_reset(struct bmp085 *bmp085);

void bmp085_init_struct(struct i2c_adapter *adapter, struct bmp085 *bmp085, uint16_t xclr);

int bmp085_start_measure(struct bmp085 *bmp085, uint8_t measure);
int bmp085_read_measurement(struct bmp085 *bmp085, uint16_t *measurement);

int bmp085_measure_pressure(struct bmp085 *bmp085, uint32_t *pressure, uint8_t oss);
int bmp085_measure_temperature(struct bmp085 *bmp085, uint16_t *temperature);

int bmp085_read_coefficients(struct bmp085 *bmp085);

void bmp085_calc(struct bmp085_coefficients * c, uint32_t up, uint16_t ut, uint32_t *pressure, int16_t *temperature, uint8_t oss);

//! \} \}

#endif
