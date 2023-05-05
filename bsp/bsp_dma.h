/*!
 @file bsp_dma.h
 @brief Board Support Package for Direct Memory Access.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

/* Define to prevent recursive inclusion */
#ifndef __BSP_DMA_H__
#define __BSP_DMA_H__

#include "bsp.h"

/* Wait Complete Transmit */
#define BSP_DMA_WAIT_TC(_) \
  while (!__HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_)))

#define BSP_DMA_FLAG_TC(_) __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_))
#define BSP_DMA_FLAG_HT(_) __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_))
#define BSP_DMA_FLAG_TE(_) __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_))
#if defined(__HAL_DMA_GET_FE_FLAG_INDEX)
#define BSP_DMA_FLAG_FE(_) __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_FE_FLAG_INDEX(_))
#endif /* __HAL_DMA_GET_FE_FLAG_INDEX */
#if defined(__HAL_DMA_GET_DME_FLAG_INDEX)
#define BSP_DMA_FLAG_DME(_) __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_DME_FLAG_INDEX(_))
#endif /* __HAL_DMA_GET_DME_FLAG_INDEX */

#define BSP_DMA_CLEAR_TC(_) __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_))
#define BSP_DMA_CLEAR_HT(_) __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_))
#define BSP_DMA_CLEAR_TE(_) __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_))
#if defined(__HAL_DMA_GET_FE_FLAG_INDEX)
#define BSP_DMA_CLEAR_FE(_) __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_FE_FLAG_INDEX(_))
#endif /* __HAL_DMA_GET_FE_FLAG_INDEX */
#if defined(__HAL_DMA_GET_DME_FLAG_INDEX)
#define BSP_DMA_CLEAR_DME(_) __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_DME_FLAG_INDEX(_))
#endif /* __HAL_DMA_GET_DME_FLAG_INDEX */

#if defined(__HAL_DMA_GET_FE_FLAG_INDEX) && defined(__HAL_DMA_GET_DME_FLAG_INDEX)
#define BSP_DMA_CLEAR_FLAG(_)                                 \
  do                                                          \
  {                                                           \
    /* Transfer complete interrupt mask */                    \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_));  \
    /* Half transfer complete interrupt mask */               \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_));  \
    /* Transfer error interrupt mask */                       \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_));  \
    /* FIFO error interrupt mask */                           \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_FE_FLAG_INDEX(_));  \
    /* Direct mode error interrupt mask */                    \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_DME_FLAG_INDEX(_)); \
  } while (0)
#elif defined(__HAL_DMA_GET_FE_FLAG_INDEX)
#define BSP_DMA_CLEAR_FLAG(_)                                \
  do                                                         \
  {                                                          \
    /* Transfer complete interrupt mask */                   \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_)); \
    /* Half transfer complete interrupt mask */              \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_)); \
    /* Transfer error interrupt mask */                      \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_)); \
    /* FIFO error interrupt mask */                          \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_FE_FLAG_INDEX(_)); \
  } while (0)
#elif defined(__HAL_DMA_GET_DME_FLAG_INDEX)
#define BSP_DMA_CLEAR_FLAG(_)                                 \
  do                                                          \
  {                                                           \
    /* Transfer complete interrupt mask */                    \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_));  \
    /* Half transfer complete interrupt mask */               \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_));  \
    /* Transfer error interrupt mask */                       \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_));  \
    /* Direct mode error interrupt mask */                    \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_DME_FLAG_INDEX(_)); \
  } while (0)
#else
#define BSP_DMA_CLEAR_FLAG(_)                                \
  do                                                         \
  {                                                          \
    /* Transfer complete interrupt mask */                   \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_)); \
    /* Half transfer complete interrupt mask */              \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_)); \
    /* Transfer error interrupt mask */                      \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_)); \
  } while (0)
#endif /* BSP_DMA_CLEAR_FLAG */

/* Enddef to prevent recursive inclusion */
#endif /* __BSP_DMA_H__ */

/* END OF FILE */
