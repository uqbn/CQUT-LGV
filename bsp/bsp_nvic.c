/*!
 @file bsp_nvic.c
 @brief Board Support Package for Nested Vectored Interrupt Controller.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

#include "bsp_nvic.h"

void nvic_reset(void)
{
  __ASM volatile("cpsid i");
  NVIC_SystemReset();
}

/* END OF FILE */
