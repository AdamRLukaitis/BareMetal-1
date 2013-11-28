#include "bm/sht1x.h"
#include "bm/gpio.h"
#include "bm/delay.h"
#include <errno.h>

#define CLOCK_DELAY 2 // ~250 kHz

int sht1x_init_gpio(struct sht1x *sht1x)
{
    struct gpio pins[] =
    {
        {sht1x->data_gpio, GPIOF_OUT | GPIOF_OPEN_DRAIN | GPIOF_INIT_HIGH},
        {sht1x->sck_gpio, GPIOF_OUT | GPIOF_OPEN_DRAIN | GPIOF_INIT_LOW}
    };
    gpio_request(pins, 2);

    return 0;
}

int sht1x_start(struct sht1x *sht1x)
{
    gpio_set_value(sht1x->data_gpio, 1);
    gpio_set_value(sht1x->sck_gpio, 0);
    delay_us(4 * CLOCK_DELAY);

    gpio_set_value(sht1x->sck_gpio, 1);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->data_gpio, 0);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->sck_gpio, 0);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->sck_gpio, 1);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->data_gpio, 1);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->sck_gpio, 0);
    delay_us(CLOCK_DELAY);

    return 0;
}

int sht1x_stop(struct sht1x *sht1x)
{
    gpio_set_value(sht1x->sck_gpio, 1);
    gpio_set_value(sht1x->data_gpio, 1);
    return 0;
}

int sht1x_send(struct sht1x *sht1x, uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        gpio_set_value(sht1x->sck_gpio, 0);
        gpio_set_value(sht1x->data_gpio, (byte >> (7 - i)) & 0x1);
        delay_us(CLOCK_DELAY);

        gpio_set_value(sht1x->sck_gpio, 1);
        delay_us(CLOCK_DELAY);
    }

    gpio_set_value(sht1x->sck_gpio, 0);
    gpio_set_value(sht1x->data_gpio, 1);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->sck_gpio, 1);

    int result = 0;
    if (gpio_get_value(sht1x->data_gpio))
        result = -1;

    delay_us(CLOCK_DELAY);

    return result;
}

int sht1x_recieve(struct sht1x *sht1x, uint8_t *byte, uint8_t ack)
{
    *byte = 0;

    gpio_set_value(sht1x->data_gpio, 1);

    uint8_t result;
    for (uint8_t i = 0; i < 8; i++)
    {
        gpio_set_value(sht1x->sck_gpio, 0);
        delay_us(CLOCK_DELAY);
        gpio_set_value(sht1x->sck_gpio, 1);
        *byte |= (gpio_get_value(sht1x->data_gpio) ? 1 : 0) << (7 - i);
        delay_us(CLOCK_DELAY);
    }

    gpio_set_value(sht1x->sck_gpio, 0);
    gpio_set_value(sht1x->data_gpio, !ack);
    delay_us(CLOCK_DELAY);
    gpio_set_value(sht1x->sck_gpio, 1);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->sck_gpio, 0);
    gpio_set_value(sht1x->data_gpio, 1);

    return 0;
}


int sht1x_read_status(struct sht1x *sht1x, uint8_t *status)
{
    sht1x_start(sht1x);

    int st = sht1x_send(sht1x, SHT1X_COMMAND_READ_STATUS);

    if (st)
        st = -ENODEV;
    else
        st = sht1x_recieve(sht1x, status, 0);

    sht1x_stop(sht1x);

    return st;
}


int sht1x_init_struct(struct sht1x *sht1x, uint16_t sck_gpio, uint16_t data_gpio)
{
    sht1x->data_gpio = data_gpio;
    sht1x->sck_gpio = sck_gpio;
    return 0;
}

int sht1x_read_measure(struct sht1x *sht1x, uint8_t measurement, uint16_t *value)
{
    uint8_t command;
    uint16_t timeout;
    switch (measurement)
    {
    case SHT1X_MEASUREMENT_T:
        command = SHT1X_COMMAND_MEASURE_TEMP;
        timeout = 350;
        break;
    case SHT1X_MEASUREMENT_RH:
        command = SHT1X_COMMAND_MEASURE_HUMIDITY;
        timeout = 100;
        break;
    default:
        return -EINVAL;
    }

    sht1x_start(sht1x);
    if (sht1x_send(sht1x, command))
        return -ENODEV;

    gpio_set_value(sht1x->sck_gpio, 0);
    gpio_set_value(sht1x->data_gpio, 1);

    BM_INIT_TIMEOUT_WAIT();
    BM_TIMEOUT_WAIT_MS(gpio_get_value(sht1x->data_gpio), timeout);

    uint8_t result;
    sht1x_recieve(sht1x, &result, 1);
    *value = result << 8;
    sht1x_recieve(sht1x, &result, 1);
    *value |= result;
    sht1x_recieve(sht1x, &result, 0);

    sht1x_stop(sht1x);

    return 0;
}

float sht1x_calc_humidity(uint16_t rh, float temp, int resolution)
{
    float c1 = -2.0468f, c2, c3;
    float t1 = 0.01f, t2;
    float rh_l;

    switch (resolution)
    {
    case SHT1X_RESOLUTION_12_BIT:
        c2 = 0.0367;
        c3 = -1.5955e-6;
        t2 = 0.00008;
        break;
    case SHT1X_RESOLUTION_8_BIT:
        c2 = 0.5872;
        c3 = -4.0845e-4;
        t2 = 0.00128;
        break;
    default:
        return 0.0f / 0.0f;
    }

    rh_l = c1 + c2 * rh + c3 * rh * rh;
    return (temp - 25.0f) * (t1 + t2 * rh) + rh_l;
}

float sht1x_calc_temp(uint16_t temp, int resolution, float d1)
{
    float d2;

    switch (resolution)
    {
    case SHT1X_RESOLUTION_14_BIT:
        d2 = 0.01f;
        break;
    case SHT1X_RESOLUTION_12_BIT:
        d2 = 0.04f;
        break;
    default:
        return 0.0f / 0.0f;
    }

    return d1 + d2 * temp;
}

