#ifndef BAREMETAL_GPIO_H
#define BAREMETAL_GPIO_H

/*! \defgroup gpio GPIO - General Purpose Input/Output pin support
 * \{
 */

#include <stdint.h>

#define GPIOF_IN 0x0                //!< Pin configurated as input
#define GPIOF_OUT 0x1               //!< Pin configurated as output

#define GPIOF_ANALOG 0x2            //!< Pin configurated as analog pin

#define GPIOF_PULL 0x4              //!< Pin use internal pull up or pull down.
#define GPIOF_PULL_DOWN 0x4         //!< Pin use internal pull down.
#define GPIOF_PULL_UP 0xC           //!< Pin use internal pull up.

#define GPIOF_OPEN 0x4              //!< Pin is open drain or open source.
#define GPIOF_OPEN_DRAIN 0x4        //!< Pin is open drain.
#define GPIOF_OPEN_SOURCE 0xC       //!< Pin is open source.

#define GPIOF_ALTERNATIVE 0x10      //!< Pin used as alternative function pin.

#define GPIOF_INIT 0x20             //!< Pin is initialized by default value.
#define GPIOF_INIT_LOW 0x20         //!< Pin is initialized by default low.
#define GPIOF_INIT_HIGH 0x60        //!< Pin is initialized by default high.

//! Struct representing gpio initialization parameters.
struct gpio
{
    uint16_t gpio;                  //!< GPIO pin number.
    uint8_t flags;                  //!< GPIO initialization flags.
};

/*! Check GPIO is valid.
 * \param gpio pin number.
 * \returns none-zero if GPIO pin valid/available on platform, 0 otherwise.
 * \note This function must be implemented by platform port.
 */
int gpio_valid(uint16_t gpio);

/*! Request GPIOs for usage.
 * \param array requested pin array.
 * \param num pin count.
 * \returns 0 on success, negative error code otherwise.
 * \note This function must be implemented by platform port.
 */
int gpio_request(struct gpio *array, int num);
/*! Free GPIOs from usage.
 * \param array pin array.
 * \param num pin count.
 * \returns 0 on success, negative error code otherwise.
 * \note This function must be implemented by platform port.
 * \note If pin deinitialization not supported by platform, this function turns pin into Hi-Z state.
 */
int gpio_free(struct gpio *array, int num);

/*! Get GPIO pin value.
 * \param gpio pin number.
 * \returns 0 (low on pin) or positive (high on pin) value.
 * \note This function must be implemented by platform port.
 */
int gpio_get_value(uint16_t gpio);
/*! Set GPIO pin value.
 * \param gpio pin number.
 * \param value 0 or 1 - new pin value.
 * \note This function must be implemented by platform port.
 */
void gpio_set_value(uint16_t gpio, int value);

/*! Request one GPIO pin for usage.
 * \param gpio pin number.
 * \param flags pin initialization flags.
 * \returns 0 on success, negative error code otherwise.
 */
int gpio_request_one(uint16_t gpio, uint8_t flags);
/*! Free one GPIO pin from usage.
 * \param gpio pin number.
 * \returns 0 on success, negative error code otherwise.
 */
int gpio_free_one(uint16_t gpio);

//*! \}

#endif
