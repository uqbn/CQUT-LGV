/*!
 @file exec.h
 @brief Execution implementation.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

/* Define to prevent recursive inclusion */
#ifndef __EXEC_H__
#define __EXEC_H__

#include "timeslice.h"
#include <stdint.h>

#define KEY0_MSK (1 << 0)
#define KEY1_MSK (1 << 1)
#define KEYS_MSK (KEY1_MSK | KEY0_MSK)
#define IO_RXEN_MSK (1 << 0)
#define IO_RXBF_MSK (1 << 1)
typedef struct exec_s
{
  short uart;
  short exti;
  uint8_t *io;
  float x;
  float y;
  float z;
} exec_s;
extern exec_s exec[1];

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* Enddef to prevent recursive inclusion */
#endif /* __EXEC_H__ */

/* END OF FILE */
