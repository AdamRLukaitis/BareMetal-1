#include <bm/delay.h>
#include <limits.h>
#include <stm32f10x.h>
#include <stm32f10x_tim.h>

void delay_us(uint16_t uS)
{
    uint16_t start = TIM_GetCounter(TIM17);
    while ((uint16_t)(TIM_GetCounter(TIM17) - start) <= uS) system_nop();
}

int delay_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Period = UINT16_MAX;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM17, ENABLE);
    return 0;
}
