/*!
 @file bsp.h
 @brief Board Support Package.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

/* Define to prevent recursive inclusion */
#ifndef __BSP_H__
#define __BSP_H__

#include "main.h"

/* extern "C" */
#undef __END_DECLS
#undef __BEGIN_DECLS
#if defined(__cplusplus)
#define __BEGIN_DECLS \
  extern "C" {
#define __END_DECLS \
  }
#else
#define __END_DECLS
#define __BEGIN_DECLS
#endif /* __cplusplus */

/* CMSIS compiler specific defines */
#ifndef __ASM
#define __ASM __asm
#endif
#ifndef __INLINE
#define __INLINE inline
#endif
#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif
#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif
#ifndef __NO_RETURN
#define __NO_RETURN __attribute__((__noreturn__))
#endif
#ifndef __USED
#define __USED __attribute__((used))
#endif
#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif
#ifndef __PACKED
#define __PACKED __attribute__((packed, aligned(1)))
#endif
#ifndef __PACKED_STRUCT
#define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))
#endif
#ifndef __PACKED_UNION
#define __PACKED_UNION union __attribute__((packed, aligned(1)))
#endif
#ifndef __ALIGNED
#define __ALIGNED(x) __attribute__((aligned(x)))
#endif
#ifndef __RESTRICT
#define __RESTRICT __restrict
#endif

/* include submodules */
#include "bsp_systick.h"
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_dma.h"

/* Using the operating system */
#if defined(USED_OS) && USED_OS
#include "cmsis_os.h"
#endif /* USED_OS */

/* Enddef to prevent recursive inclusion */
#endif /* __BSP_H__ */

/* END OF FILE */
