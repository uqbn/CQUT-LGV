/*!
 @file bsp_gpio.h
 @brief Board Support Package for General Purpose Input/Output.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

/* Define to prevent recursive inclusion */
#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include "bsp.h"

__STATIC_INLINE
void gpio_pin_set(GPIO_TypeDef *gpio, uint16_t pin)
{
  gpio->BSRR = (uint32_t)pin;
}

__STATIC_INLINE
void gpio_pin_reset(GPIO_TypeDef *gpio, uint16_t pin)
{
  gpio->BSRR = (uint32_t)pin << 0x10;
}

__STATIC_INLINE
void gpio_pin_write(GPIO_TypeDef *gpio, uint16_t pin, FlagStatus state)
{
  gpio->BSRR = (state == RESET) ? (uint32_t)pin << 0x10 : (uint32_t)pin;
}

__STATIC_INLINE
FlagStatus gpio_pin_read(GPIO_TypeDef *gpio, uint16_t pin)
{
  return (gpio->IDR & pin) ? SET : RESET;
}

__STATIC_INLINE
void gpio_pin_toggle(GPIO_TypeDef *gpio, uint16_t pin)
{
  uint32_t odr = gpio->ODR;
  gpio->BSRR = ((odr & pin) << 0x10) | (~odr & pin);
}

__STATIC_INLINE
void gpio_pin_lock(GPIO_TypeDef *gpio, uint16_t pin)
{
  /* Apply lock key write sequence */
  __IO uint32_t tmp = pin | GPIO_LCKR_LCKK;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  gpio->LCKR = tmp;
  /* Reset LCKx bit(s): LCKK='0' + LCK[15-0] */
  gpio->LCKR = pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  gpio->LCKR = tmp;
  /* Read LCKR register. This read is mandatory to complete key lock sequence */
  tmp = gpio->LCKR;
}

/* Enddef to prevent recursive inclusion */
#endif /* __BSP_GPIO_H__ */

/* END OF FILE */
