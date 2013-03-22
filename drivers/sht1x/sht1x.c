#include "bm/sht1x.h"
#include "bm/gpio.h"
#include "bm/delay.h"
#include <errno.h>

#define CLOCK_DELAY 50

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

int sht1x_send(struct sht1x *sht1x, uint8_t byte)
{
    struct gpio pin = {sht1x->data_gpio, GPIOF_IN};
    for (uint8_t i = 0; i < 8; i++)
    {
        gpio_set_value(sht1x->sck_gpio, 0);
        gpio_set_value(sht1x->data_gpio, (byte >> (7 - i)) & 0x1);
        delay_us(CLOCK_DELAY);

        gpio_set_value(sht1x->sck_gpio, 1);
        delay_us(CLOCK_DELAY);
    }

    gpio_set_value(sht1x->sck_gpio, 0);
    gpio_request(&pin, 1);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->sck_gpio, 1);
    delay_us(CLOCK_DELAY);

    int result = 0;
    if (gpio_get_value(sht1x->data_gpio))
        result = -1;
    pin.flags = GPIOF_OUT | GPIOF_OPEN_DRAIN;
    gpio_request(&pin, 1);
    gpio_set_value(sht1x->data_gpio, 1);

    return result;
}

int sht1x_recieve(struct sht1x *sht1x, uint8_t *byte, uint8_t ack)
{
    *byte = 0;
    struct gpio pin = {sht1x->data_gpio, GPIOF_IN};
    gpio_request(&pin, 1);
    for (uint8_t i = 0; i < 8; i++)
    {
        gpio_set_value(sht1x->sck_gpio, 0);
        delay_us(CLOCK_DELAY);

        gpio_set_value(sht1x->sck_gpio, 1);
        delay_us(CLOCK_DELAY);

        *byte |= (gpio_get_value(sht1x->data_gpio) ? 1 : 0) << (7 - i);
    }

    pin.flags = GPIOF_OUT | GPIOF_OPEN_DRAIN;
    gpio_request(&pin, 1);
    gpio_set_value(sht1x->sck_gpio, 0);
    gpio_set_value(sht1x->data_gpio, !ack);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->sck_gpio, 1);
    delay_us(CLOCK_DELAY);

    gpio_set_value(sht1x->data_gpio, 1);

    return 0;
}


int sht1x_read_status(struct sht1x *sht1x, uint8_t *status)
{
    sht1x_start(sht1x);
    if (sht1x_send(sht1x, SHT1X_COMMAND_READ_STATUS))
        return -ENODEV;

    return sht1x_recieve(sht1x, status, 0);
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
        timeout = 100;
        break;
    case SHT1X_MEASUREMENT_RH:
        command = SHT1X_COMMAND_MEASURE_HUMIDITY;
        timeout = 400;
        break;
    default:
        return -EINVAL;
    }

    sht1x_start(sht1x);
    if (sht1x_send(sht1x, command))
        return -ENODEV;

    struct gpio pin = {sht1x->data_gpio, GPIOF_IN};
    gpio_request(&pin, 1);

    gpio_set_value(sht1x->sck_gpio, 0);

    BM_INIT_TIMEOUT_WAIT();
    BM_TIMEOUT_WAIT_MS(gpio_get_value(sht1x->data_gpio), timeout);

    uint8_t result;
    sht1x_recieve(sht1x, &result, 1);
    *value = result << 8;
    sht1x_recieve(sht1x, &result, 0);
    *value |= result;

    return 0;
}
