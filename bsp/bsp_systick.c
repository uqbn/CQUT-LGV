/*!
 @file bsp_systick.c
 @brief Board Support Package for System Timer.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

#include "bsp_systick.h"

__WEAK
void delay_ctor(void)
{
  SysTick->VAL = SysTick_VAL_CURRENT_Msk;
  SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

__WEAK
void delay_dtor(void)
{
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
}

#undef SYSTICK_ENABLED
#define SYSTICK_ENABLED() (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk)

__WEAK
void delay_us(uint32_t nus)
{
  uint32_t cnt = 0;
  nus = (SystemCoreClock / 1000000) * nus;
  /* SysTick Current Value Register */
  uint32_t cache = SysTick->VAL;
  do
  {
    uint32_t value = SysTick->VAL;
    if (value == cache)
    {
      continue;
    }
    if (value > cache)
    {
      cnt += SysTick->LOAD;
    }
    cnt += cache - value;
    cache = value;
  } while (SYSTICK_ENABLED() && cnt < nus);
}

__WEAK
void delay_ms(uint32_t nms)
{
  uint32_t cnt = 0;
  nms = (SystemCoreClock / 1000) * nms;
  /* SysTick Current Value Register */
  uint32_t cache = SysTick->VAL;
  do
  {
    uint32_t value = SysTick->VAL;
    if (value == cache)
    {
      continue;
    }
    if (value > cache)
    {
      cnt += SysTick->LOAD;
    }
    cnt += cache - value;
    cache = value;
  } while (SYSTICK_ENABLED() && cnt < nms);
}

/* END OF FILE */
