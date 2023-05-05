/*!
 @file bsp_uart.h
 @brief Board Support Package for Universal Asynchronous Receiver/Transmitter.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

/* Define to prevent recursive inclusion */
#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "bsp.h"

__BEGIN_DECLS

void uart_enable(UART_HandleTypeDef *huart);
void uart_disable(UART_HandleTypeDef *huart);

void uart_dma_rerx(UART_HandleTypeDef *huart, uint16_t len);
void uart_dma_retx(UART_HandleTypeDef *huart, uint16_t len);

void uart_dma_init(UART_HandleTypeDef *huart);
void uart_dma_rx_init(UART_HandleTypeDef *huart);
void uart_dma_tx_init(UART_HandleTypeDef *huart);

void uart_dma_rx(UART_HandleTypeDef *huart, void *buf, uint16_t len);
void uart_dma_tx(UART_HandleTypeDef *huart, void *buf, uint16_t len);

__END_DECLS

/* Enddef to prevent recursive inclusion */
#endif /* __BSP_USART_H__ */

/* END OF FILE */
