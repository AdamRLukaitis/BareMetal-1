#include "bm/gpio.h"
#include <errno.h>

int gpio_request_one(uint16_t gpio, uint8_t flags)
{
    struct gpio pin = {gpio, flags};
    gpio_request(&pin, 1);
}

int gpio_free_one(uint16_t gpio)
{
    struct gpio pin = {gpio, 0};
    gpio_free(&pin, 1);
}
