#include "bm/bmp085.h"
#include <bm/delay.h>
#include <bm/gpio.h>
#include <errno.h>

void bmp085_enter_reset(struct bmp085 *bmp085)
{
    gpio_request_one(bmp085->xclr_gpio, GPIOF_OUT);
    gpio_set_value(bmp085->xclr_gpio, 0);
}

void bmp085_exit_reset(struct bmp085 *bmp085)
{
    gpio_set_value(bmp085->xclr_gpio, 1);
    gpio_free_one(bmp085->xclr_gpio);
}

void bmp085_init_struct(struct i2c_adapter *adapter, struct bmp085 *bmp085, uint16_t gpio)
{
    bmp085->client.adapter = adapter;
    bmp085->client.addr = BMP085_I2C_ADDRESS;
    bmp085->xclr_gpio = gpio;
}

int bmp085_start_measure(struct bmp085 *bmp085, uint8_t measure)
{
    return i2c_write_byte_data(&bmp085->client, BMP085_CONTROL_REGISTER, measure);
}

int bmp085_read_measurement(struct bmp085 *bmp085, uint16_t *measurement)
{
    int32_t result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_MEASURE_ADDRESS)) < 0)
        return result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_MEASURE_ADDRESS)) < 0)
        return result;
    *measurement = (uint16_t)(result & 0x0000ffff);
    return 0;
}

int bmp085_measure_pressure(struct bmp085 *bmp085, uint32_t *pressure, uint8_t oss)
{
    if (oss > 3)
        return -EINVAL;

    int32_t result;
    if (result = bmp085_start_measure(bmp085, BMP085_PRESSURE_MEASURE + (oss << 6)))
        return result;

    delay_ms((3 << oss) + 2);

    uint16_t up;
    if (result = bmp085_read_measurement(bmp085, &up))
        return result;
    *pressure = (up << 8) >> (8 - oss);
    return 0;
}

int bmp085_measure_temperature(struct bmp085 *bmp085, uint16_t *temperature)
{
    int32_t result;
    if (result = bmp085_start_measure(bmp085, BMP085_TEMPERATURE_MEASURE))
        return result;

    delay_ms(5);

    if (result = bmp085_read_measurement(bmp085, temperature))
        return result;

    return 0;
}

int bmp085_read_coefficients(struct bmp085 *bmp085)
{
    int32_t result;

    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_AC1_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.ac1 = (int16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_AC2_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.ac2 = (int16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_AC3_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.ac3 = (int16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_AC4_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.ac4 = (uint16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_AC5_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.ac5 = (uint16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_AC6_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.ac6 = (uint16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_B1_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.b1 = (int16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_B2_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.b2 = (int16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_MB_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.mb = (int16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_MC_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.mc = (int16_t)result;
    if ((result = i2c_read_word_data(&bmp085->client, BMP085_EE_MD_ADDRESS)) < 0)
        return result;
    bmp085->coefficients.md = (int16_t)result;

    return 0;
}

void bmp085_calc(struct bmp085_coefficients * c, uint32_t up, uint16_t ut, uint32_t *pressure, int16_t *temperature, uint8_t oss)
{
    int32_t x1, x2, x3, b3, b5, b6, p;
    uint32_t  b4, b7;

    x1 = (ut - c->ac6) * c->ac5 >> 15;
    x2 = ((int32_t) c->mc << 11) / (x1 + c->md);
    b5 = x1 + x2;
    *temperature = (b5 + 8) >> 4;

    b6 = b5 - 4000;
    x1 = (c->b2 * (b6 * b6 >> 12)) >> 11;
    x2 = c->ac2 * b6 >> 11;
    x3 = x1 + x2;
    b3 = ((((int32_t) c->ac1 * 4 + x3) << oss) + 2) >> 2;
    x1 = c->ac3 * b6 >> 13;
    x2 = (c->b1 * (b6 * b6 >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (c->ac4 * (uint32_t)(x3 + 32768)) >> 15;
    b7 = ((uint32_t) up - b3) * (50000 >> oss);
    p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    *pressure = p + ((x1 + x2 + 3791) >> 4);
    /*int32_t  x1, x2, x3, b3, b5, b6, p;
    uint32_t  b4, b7;
    uint8_t oss = 3;

    x1 = ((ut - c->ac6) * c->ac5) / (1 << 15);
    x2 = (((int32_t) c->mc) << 11) / (x1 + c->md);
    b5 = x1 + x2;
    *temperature = (b5 + 8) / (1 << 4);

    b6 = b5 - 4000;
    x1 = (c->b2 * ((b6 * b6) / (1 << 12))) / (1 << 11);
    x2 = (c->ac2 * b6) / (1 << 11);
    x3 = x1 + x2;
    b3 = ((((int32_t) c->ac1 * 4 + x3) << oss) + 2) / (1 << 2);
    x1 = (c->ac3 * b6) / (1 << 13);
    x2 = (c->b1 * (b6 * b6 / (1 << 12))) / (1 << 16);
    x3 = ((x1 + x2) + 2) / (1 << 2);
    b4 = (c->ac4 * (uint32_t) (x3 + 32768)) / (1 << 15);
    b7 = ((uint32_t) up - b3) * (50000 >> oss);
    p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;

    x1 = (p / (1 << 8)) * (p / (1 << 8));
    x1 = (x1 * 3038) / (1 << 16);
    x2 = (-7357 * p) / (1 << 16);

    *pressure = p + ((x1 + x2 + 3791) / (1 << 4));*/
}

