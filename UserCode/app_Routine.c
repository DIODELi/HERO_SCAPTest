#include "app_Routine.h"

#include "drv_ADCSense.h"

static uint32_t PWMCount = 0; // 用于周期计数

/**
  * @brief  Callback function invoked when timer repetition period has elapsed
  * @param  hhrtim pointer to HAL HRTIM handle
  * @param  TimerIdx Timer index
  *                   This parameter can be one of the following values:
  *                   @arg HRTIM_TIMERINDEX_MASTER  for master timer
  *                   @arg HRTIM_TIMERINDEX_TIMER_A for timer A
  *                   @arg HRTIM_TIMERINDEX_TIMER_B for timer B
  *                   @arg HRTIM_TIMERINDEX_TIMER_C for timer C
  *                   @arg HRTIM_TIMERINDEX_TIMER_D for timer D
  *                   @arg HRTIM_TIMERINDEX_TIMER_E for timer E
  * @retval None
  */
void HAL_HRTIM_RepetitionEventCallback(HRTIM_HandleTypeDef *hhrtim,
                                       uint32_t TimerIdx)
{
    switch (TimerIdx)
    {
    case HRTIM_TIMERINDEX_MASTER:
        // 主定时器重复计数器溢出中断
        PWMCount++;
        ADC_Routine(PWMCount);
        break;

    default:
        break;
    }
}
