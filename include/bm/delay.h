#ifndef BAREMETAL_DELAY_H 
#define BAREMETAL_DELAY_H

/*! \defgroup delay Delay - timing functions
 * \{
 */

#include <stdint.h>

//! Initialize timeout support macros.
#define BM_INIT_TIMEOUT_WAIT() uint64_t tickStop;

/*! Wait for condition with timeout.
 * \param x condtion (e.g. x == 0).
 * \param mS timeout in milliseconds.
 */
#define BM_TIMEOUT_WAIT_MS(x, mS) \
    tickStop = getTickCount() + mS; \
    while((x) && (getTickCount() < tickStop)) systemNOP(); \
    if(x)

/*! Wait for condition without timeout.
 * \param x condtion (e.g. x == 0).
 */
#define BM_WAIT(x) \
    while(x) systemNOP();

/*! Delay for milliseconds
 * \param mS milliseconds.
 */
void delayMS(uint16_t mS);

/*! Get tick (milliseconds) count from system start.
 * \returns tick count from system start.
 */
uint64_t getTickCount();

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
void systemNOP();

//! \}

#endif 
