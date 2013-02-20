#include "bm/delay.h"

static uint64_t tickCount = 0;

void delay_ms(uint16_t mS)
{
    uint64_t tickStop = tickCount + mS;
    while (tickCount < tickStop)
        system_nop();
}

uint64_t get_tick_count()
{
    return tickCount;
}

void tick()
{
    tickCount++;
}
