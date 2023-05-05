/*!
 @file bsp_uart.c
 @brief Board Support Package for Universal Asynchronous Receiver/Transmitter.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

#include "bsp_uart.h"

void uart_enable(UART_HandleTypeDef *huart)
{
  /* Enable UART */
  __HAL_UART_ENABLE(huart);
}

void uart_disable(UART_HandleTypeDef *huart)
{
  /* Disable UART */
  __HAL_UART_DISABLE(huart);
}

void uart_dma_rerx(UART_HandleTypeDef *huart, uint16_t len)
{
  /* Disable UART */
  __HAL_UART_DISABLE(huart);

  do
  {
    /* Disable the specified DMA Stream */
    __HAL_DMA_DISABLE(huart->hdmarx);
  } while (huart->hdmarx->Instance->CCR & DMA_CCR_EN);

  /* Clear all flag */
  BSP_DMA_CLEAR_FLAG(huart->hdmarx);

  /* DMA stream x number of data register */
  huart->hdmarx->Instance->CNDTR = len;

  /* Enable the specified DMA Stream */
  __HAL_DMA_ENABLE(huart->hdmarx);
  /* Enable UART */
  __HAL_UART_ENABLE(huart);
}

void uart_dma_retx(UART_HandleTypeDef *huart, uint16_t len)
{
  /* Disable UART */
  __HAL_UART_DISABLE(huart);

  do
  {
    /* Disable the specified DMA Stream */
    __HAL_DMA_DISABLE(huart->hdmatx);
  } while (huart->hdmatx->Instance->CCR & DMA_CCR_EN);

  /* Clear all flag */
  BSP_DMA_CLEAR_FLAG(huart->hdmatx);

  /* DMA stream x number of data register */
  huart->hdmatx->Instance->CNDTR = len;

  /* Enable the specified DMA Stream */
  __HAL_DMA_ENABLE(huart->hdmatx);
  /* Enable UART */
  __HAL_UART_ENABLE(huart);
}

void uart_dma_rx_init(UART_HandleTypeDef *huart)
{
  /* DMA Enable Receiver */
  SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

  /* Enable UART */
  __HAL_UART_ENABLE(huart);
  /* Enalbe idle interrupt */
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

  do
  {
    /* Disable the specified DMA Stream */
    __HAL_DMA_DISABLE(huart->hdmarx);
  } while (huart->hdmarx->Instance->CCR & DMA_CCR_EN);

  /* Clear all flag */
  BSP_DMA_CLEAR_FLAG(huart->hdmarx);

  /* DMA stream x peripheral address register */
  huart->hdmarx->Instance->CPAR = (size_t) & (huart->Instance->DR);
}

void uart_dma_tx_init(UART_HandleTypeDef *huart)
{
  /* DMA Enable Transmitter */
  SET_BIT(huart->Instance->CR3, USART_CR3_DMAT);

  /* Enable UART */
  __HAL_UART_ENABLE(huart);
  /* Enable transmission complete interrupt */
  // __HAL_UART_ENABLE_IT(huart, UART_IT_TC);

  do
  {
    /* Disable the specified DMA Stream */
    __HAL_DMA_DISABLE(huart->hdmatx);
  } while (huart->hdmatx->Instance->CCR & DMA_CCR_EN);

  /* Clear all flag */
  BSP_DMA_CLEAR_FLAG(huart->hdmatx);

  /* DMA stream x peripheral address register */
  huart->hdmatx->Instance->CPAR = (size_t) & (huart->Instance->DR);
}

void uart_dma_init(UART_HandleTypeDef *huart)
{
  /* DMA Enable Receiver and Transmitter */
  uart_dma_rx_init(huart);
  uart_dma_tx_init(huart);
}

void uart_dma_rx(UART_HandleTypeDef *huart, void *buf, uint16_t len)
{
  do
  {
    /* Disable the specified DMA Stream */
    __HAL_DMA_DISABLE(huart->hdmarx);
  } while (huart->hdmarx->Instance->CCR & DMA_CCR_EN);

  /* Clear all flag */
  BSP_DMA_CLEAR_FLAG(huart->hdmarx);

  /* DMA stream x memory address register   */
  huart->hdmarx->Instance->CMAR = (size_t)(buf);
  /* DMA stream x number of data register */
  huart->hdmarx->Instance->CNDTR = len;

  /* Enable the specified DMA Stream */
  __HAL_DMA_ENABLE(huart->hdmarx);
}

void uart_dma_tx(UART_HandleTypeDef *huart, void *buf, uint16_t len)
{
  do
  {
    /* Disable the specified DMA Stream */
    __HAL_DMA_DISABLE(huart->hdmatx);
  } while (huart->hdmatx->Instance->CCR & DMA_CCR_EN);

  /* Clear all flag */
  BSP_DMA_CLEAR_FLAG(huart->hdmatx);

  /* DMA stream x memory 0 address register   */
  huart->hdmatx->Instance->CMAR = (size_t)(buf);
  /* DMA stream x number of data register */
  huart->hdmatx->Instance->CNDTR = len;

  /* Enable the specified DMA Stream */
  __HAL_DMA_ENABLE(huart->hdmatx);
}

/* END OF FILE */
