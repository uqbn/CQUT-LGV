/*!
 @file io.h
 @brief io
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

/* Define to prevent recursive inclusion */
#ifndef __IO_H__
#define __IO_H__

#include "bsp.h"

#define hio huart1 /* uart, connect to the upper computer */
#define IO_IRQHandler USART1_IRQHandler
extern UART_HandleTypeDef hio;

#define IO_BUFSIZ 0x100
typedef union buf_u
{
  char s[IO_BUFSIZ];
  uint8_t u[IO_BUFSIZ];
  float f[IO_BUFSIZ >> 2];
} buf_u;

__BEGIN_DECLS

void io_init(void);
void io_printf(const char *format, ...);
void io_justfloat(uint8_t n, ...);

__END_DECLS

/* Enddef to prevent recursive inclusion */
#endif /* __IO_H__ */

/* END OF FILE */
