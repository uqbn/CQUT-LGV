/*!
 @file bsp_systick.h
 @brief Board Support Package for System Timer.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

/* Define to prevent recursive inclusion */
#ifndef __BSP_SYSTICK_H__
#define __BSP_SYSTICK_H__

#include "bsp.h"

__BEGIN_DECLS

void delay_ctor(void);
void delay_dtor(void);
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);

__END_DECLS

/* Enddef to prevent recursive inclusion */
#endif /* __BSP_SYSTICK_H__ */

/* END OF FILE */
