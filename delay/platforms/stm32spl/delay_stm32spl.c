#include <bm/delay.h>
#include <limits.h>

#if defined STM32F1
# include <stm32f10x.h>
# include <stm32f10x_tim.h>
#elif defined STM32F4
# include <stm32f4xx.h>
# include <stm32f4xx_tim.h>
#endif

void delay_us(uint16_t uS)
{
    uint16_t start = TIM_GetCounter(TIM14);
    while ((uint16_t)(TIM_GetCounter(TIM14) - start) <= uS) system_nop();
}

int delay_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Period = UINT16_MAX;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM14, ENABLE);
    return 0;
}
