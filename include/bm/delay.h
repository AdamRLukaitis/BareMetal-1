#ifndef BAREMETAL_DELAY_H
#define BAREMETAL_DELAY_H

/*! \defgroup delay Delay - timing functions
 * \{
 */

#include <stdint.h>

//! Initialize timeout support macros.
#define BM_INIT_TIMEOUT_WAIT() uint64_t tick_stop;

/*! Wait for condition with timeout.
 * \param x condtion (e.g. x == 0).
 * \param mS timeout in milliseconds.
 */
#define BM_TIMEOUT_WAIT_MS(x, mS) \
    tick_stop = get_tick_count() + mS; \
    while((x) && (get_tick_count() < tick_stop)) system_nop(); \
    if(x)

/*! Wait for condition without timeout.
 * \param x condtion (e.g. x == 0).
 */
#define BM_WAIT(x) \
    while(x) system_nop();

/*! Init delay functions
 * \note This function must be implemented by platform port.
 * \returns 0 on success, negative error code otherwise.
 */
int delay_init();

/*! Delay for milliseconds
 * \param mS milliseconds.
 */
void delay_ms(uint16_t mS);

/*! Delay for microseconds
 * \param uS microseconds.
 * \note This function must be implemented by platform port.
 */
void delay_us(uint16_t uS);

/*! Get tick (milliseconds) count from system start.
 * \returns tick count from system start.
 */
uint64_t get_tick_count();

/*! System tick.
 * \note This function must be called by user program every millisecond,
 *        e.g. using systick interrupt in ARM Cortex MCU.
 */
void tick();
/*! Function used in delay and wait function loops.
 * \note In single thread this function do nothing.
 *        If some RTOS is used, this function may
 *         force task yielding.
 */
void system_nop();

//! \}

#endif
