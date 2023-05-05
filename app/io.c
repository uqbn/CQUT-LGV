/*!
 @file io.c
 @brief io
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

#include "io.h"

static buf_u io_rx[2];
static buf_u io_tx[1];

static struct
{
  uint8_t *buf0;
  uint8_t *buf1;
} stat[1] = {{
  .buf0 = io_rx[0].u,
  .buf1 = io_rx[1].u,
}};

void io_init(void)
{
  stat->buf0 = io_rx[0].u;
  stat->buf1 = io_rx[1].u;
  uart_dma_init(&hio);
  uart_dma_rx(&hio, stat->buf0 + 1, IO_BUFSIZ - 1);
}

#include <stdio.h>
#include <stdarg.h>

void io_printf(const char *fmt, ...)
{
  va_list ap;
  char *p = io_tx->s;

  va_start(ap, fmt);
  uint8_t n = (uint8_t)vsnprintf(p, IO_BUFSIZ, fmt, ap);
  va_end(ap);

  /* USART transmit by DMA Stream */
  uart_dma_tx(&hio, p, n);
  /* Wait Complete Transmit flag to be set */
  BSP_DMA_WAIT_TC(hio.hdmatx);
}

void io_justfloat(uint8_t n, ...)
{
  va_list ap;
  float *p = io_tx->f;
  float *q = p + n;

  va_start(ap, n);
  while (p != q)
  {
    *p++ = (float)va_arg(ap, double);
  }
  *p = (1.0F / 0.0F);
  va_end(ap);

  /* USART transmit by DMA Stream */
  uart_dma_tx(&hio, io_tx->u, (uint16_t)(sizeof(float) * (n + 1)));
  /* Wait Complete Transmit flag to be set */
  BSP_DMA_WAIT_TC(hio.hdmatx);
}

#include "exec.h"

/**
 * @brief This function handles USART global interrupt.
*/
void IO_IRQHandler(void)
{
  /* USART Status register */
  if (hio.Instance->SR & UART_FLAG_RXNE)
  {
    __HAL_UART_CLEAR_PEFLAG(&hio); /* Clears the UART PE pending flag */
  }
  else if (hio.Instance->SR & UART_FLAG_IDLE)
  {
    __HAL_UART_CLEAR_PEFLAG(&hio); /* Clears the UART PE pending flag */

    __HAL_DMA_DISABLE(hio.hdmarx);
    if (READ_BIT(exec->uart, IO_RXBF_MSK))
    {
      /* Current memory buffer used is Memory 1 */
      exec->io = stat->buf1;
      exec->io[0] = (uint8_t)(IO_BUFSIZ - 1 - hio.hdmarx->Instance->CNDTR);

      hio.hdmarx->Instance->CMAR = (size_t)(stat->buf0 + 1);
      hio.hdmarx->Instance->CNDTR = IO_BUFSIZ - 1;

      CLEAR_BIT(exec->uart, IO_RXBF_MSK);
    }
    else
    {
      /* Current memory buffer used is Memory 0 */
      exec->io = stat->buf0;
      exec->io[0] = (uint8_t)(IO_BUFSIZ - 1 - hio.hdmarx->Instance->CNDTR);

      hio.hdmarx->Instance->CMAR = (size_t)(stat->buf1 + 1);
      hio.hdmarx->Instance->CNDTR = IO_BUFSIZ - 1;

      SET_BIT(exec->uart, IO_RXBF_MSK);
    }
    __HAL_DMA_ENABLE(hio.hdmarx);

    SET_BIT(exec->uart, IO_RXEN_MSK);
  }
  else
  {
    __HAL_UART_CLEAR_PEFLAG(&hio); /* Clears the UART PE pending flag */
  }
  // else if (hio.Instance->SR & UART_FLAG_TC)
  // {
  //   __HAL_UART_CLEAR_FLAG(&hio, UART_FLAG_TC);
  // }
}

/* END OF FILE */
