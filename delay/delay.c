#include "bm/delay.h" 

static uint64_t tickCount = 0;

void delayMS(uint16_t mS)
{
    uint64_t tickStop = tickCount + mS;
    while(tickCount < tickStop)
       systemNOP();
}

uint64_t getTickCount()
{
    return tickCount;
}

void tick()
{
    tickCount++;
}
