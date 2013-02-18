#include <bm/gpio.h>
#include <errno.h>

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>

void stm32_get_gpio_from_num(uint16_t gpio, uint16_t *pin, GPIO_TypeDef **port)
{
    *pin = 1 << (gpio % 16);
    switch(gpio / 16)
    {
    case 0:
        *port = GPIOA;
        break;
    case 1:
        *port = GPIOB;
        break;
    case 2:
        *port = GPIOC;
        break;
    case 3:
        *port = GPIOD;
        break;
    case 4:
        *port = GPIOE;
        break;
    case 5:
        *port = GPIOF;
        break;
    case 6:
        *port = GPIOG;
        break;
    }
}

int gpio_request(struct gpio *array, int num)
{
    GPIO_InitTypeDef GPIO_Config;
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
    for(int i = 0; i < num; i++)
    {
        if(!gpio_valid(array[i].gpio))
            return -EINVAL;

        uint16_t pin;
        GPIO_TypeDef *port;
        stm32_get_gpio_from_num(array[i].gpio, &pin, &port);
        GPIO_Config.GPIO_Pin = pin;

        if(array[i].flags & GPIOF_ALTERNATIVE)
        {
            if((array[i].flags & (GPIOF_OPEN | GPIOF_OPEN_DRAIN | GPIOF_OPEN_SOURCE)) == GPIOF_OPEN_SOURCE)
                return -ENOTSUP;

            if(array[i].flags & GPIOF_OPEN)
                GPIO_Config.GPIO_Mode = GPIO_Mode_AF_OD;
            else
                GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
        }
        else
        {
            if(array[i].flags & GPIOF_OUT)
            {
                if(array[i].flags & GPIOF_ANALOG)
                    return -ENOTSUP;

                if((array[i].flags & (GPIOF_OPEN | GPIOF_OPEN_DRAIN | GPIOF_OPEN_SOURCE)) == GPIOF_OPEN_SOURCE)
                    return -ENOTSUP;

                if((array[i].flags & (GPIOF_OPEN | GPIOF_OPEN_DRAIN | GPIOF_OPEN_SOURCE)) == GPIOF_OPEN_DRAIN)
                    GPIO_Config.GPIO_Mode = GPIO_Mode_Out_OD;
                else
                    GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;

            }
            else
            {
                if(!(array[i].flags & GPIOF_ANALOG))
                {
                    if(array[i].flags & GPIOF_PULL)
                    {
                        if((array[i].flags & (GPIOF_PULL | GPIOF_PULL_UP | GPIOF_PULL_DOWN)) == GPIOF_PULL_UP)
                            GPIO_Config.GPIO_Mode = GPIO_Mode_IPU;
                        else
                            GPIO_Config.GPIO_Mode = GPIO_Mode_IPD;
                    }
                    else
                        GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
                }
                else
                    GPIO_Config.GPIO_Mode = GPIO_Mode_AIN;
            }
        }

        GPIO_Init(port, &GPIO_Config);

        if(array[i].flags & GPIOF_INIT)
        {
            if((array[i].flags & (GPIOF_INIT | GPIOF_INIT_LOW | GPIOF_INIT_HIGH)) == GPIOF_INIT_HIGH)
                GPIO_SetBits(port, pin);
            else
                GPIO_ResetBits(port, pin);
        }
    }
    return 0;
}

int gpio_free(struct gpio *array, int num)
{
    return 0;
}

int gpio_get_value(uint16_t gpio)
{
    uint16_t pin;
    GPIO_TypeDef *port;
    stm32_get_gpio_from_num(gpio, &pin, &port);
    return GPIO_ReadInputDataBit(port, pin);
}

void gpio_set_value(uint16_t gpio, int value)
{
    uint16_t pin;
    GPIO_TypeDef *port;
    stm32_get_gpio_from_num(gpio, &pin, &port);
    GPIO_WriteBit(port, pin, value ? Bit_SET : Bit_RESET);
}

int gpio_valid(uint16_t gpio)
{
    if(gpio > 111)
        return 0;
    else
        return 1;
}
