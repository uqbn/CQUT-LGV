/*!
 @file exec.c
 @brief Execution implementation.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

#include "exec.h"
#include "bsp.h"
#include "io.h"

#include "dma.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"

exec_s exec[1] = {{
  .exti = 0,
  .uart = 0,
  .x = 0,
  .y = 0,
  .z = 0,
}};

static void exec_led(void *argv)
{
  gpio_pin_toggle(LEDG_GPIO_Port, LEDG_Pin);
  HAL_IWDG_Refresh(&hiwdg);
  (void)argv;
}

static struct
{
  int32_t total;
  int32_t delta;
  float speed;
  float write;
} motor[4] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
};

typedef struct pid4_s
{
  float kp;     //!< proportional constant
  float ki;     //!< integral constant
  float kd;     //!< derivative constant
  float e[4];   //!< error input
  float fdb[4]; //!< cache feedback
  float sum[4]; //!< (integral) output item sum
  float out[4]; //!< controller output
  float summax; //!< maximum final output
  float outmax; //!< maximum integral output
} pid4_s;

void pid4_out(pid4_s *ctx)
{
  for (int i = 0; i < 4; ++i)
  {
    float const e = motor[i].write - motor[i].speed;
    float const sum = ctx->ki * e;
    /* when the limit of integration is exceeded or */
    /* the direction of integration is the same, the integration stops. */
    if ((-ctx->summax < ctx->sum[i] && ctx->sum[i] < ctx->summax) || ctx->sum[i] * sum < 0)
    {
      /* sum = K_i \sum^k_{i=0} e(i) */
      ctx->sum[i] += sum;
    }
    /* avoid derivative kick, fdb[k-1]-fdb[k] */
    /* out = K_p e(k) + sum + K_d [fdb(k-1)-fdb(k)] */
    ctx->out[i] = ctx->kp * e + ctx->sum[i] + ctx->kd * (ctx->fdb[i] - motor[i].speed);
    /* output limiter */
    if (ctx->outmax < ctx->out[i])
    {
      ctx->out[i] = ctx->outmax;
    }
    else if (ctx->out[i] < -ctx->outmax)
    {
      ctx->out[i] = -ctx->outmax;
    }
    /* cache data */
    ctx->fdb[i] = motor[i].speed;
    ctx->e[i] = e;
  }
}

void mecanum_odometer(float *x, float *y, float *w)
{
  *x = (motor[0].total - motor[1].total - motor[2].total + motor[3].total) * 0.23561944901923448F / (4 * 13 * 30) * 0.25F;
  *y = (motor[0].total + motor[1].total - motor[2].total - motor[3].total) * 0.23561944901923448F / (4 * 13 * 30) * 0.25F;
  *w = (motor[0].total + motor[1].total + motor[2].total + motor[3].total) * 0.23561944901923448F / (4 * 13 * 30) * 1.25F;
}

void mecanum_nagtive(float *x, float *y, float *w)
{
  *x = (motor[0].speed - motor[1].speed - motor[2].speed + motor[3].speed) * 0.25F;
  *y = (motor[0].speed + motor[1].speed - motor[2].speed - motor[3].speed) * 0.25F;
  *w = (motor[0].speed + motor[1].speed + motor[2].speed + motor[3].speed) * 1.25F;
}

void mecanum_postive(float x, float y, float w)
{
  motor[0].write = +x + y + w * 0.2;
  motor[1].write = -x + y + w * 0.2;
  motor[2].write = -x - y + w * 0.2;
  motor[3].write = +x - y + w * 0.2;
}

void motor_set_pwm(int16_t m1, int16_t m2, int16_t m3, int16_t m4)
{
#undef MOTOR_SET
#define MOTOR_SET(id, value)                                   \
  do                                                           \
  {                                                            \
    FlagStatus in1 = RESET;                                    \
    FlagStatus in2 = RESET;                                    \
    if (value < -100)                                          \
    {                                                          \
      in1 = SET;                                               \
    }                                                          \
    else if (value > 100)                                      \
    {                                                          \
      in2 = SET;                                               \
    }                                                          \
    else                                                       \
    {                                                          \
      value = 0;                                               \
    }                                                          \
    if (value < 0)                                             \
    {                                                          \
      value = -value;                                          \
    }                                                          \
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_##id, value);    \
    gpio_pin_write(M##id##IN1_GPIO_Port, M##id##IN1_Pin, in1); \
    gpio_pin_write(M##id##IN2_GPIO_Port, M##id##IN2_Pin, in2); \
  } while (0)
  MOTOR_SET(1, m1);
  MOTOR_SET(2, m2);
  MOTOR_SET(3, m3);
  MOTOR_SET(4, m4);
#undef MOTOR_SET
}

static pid4_s pid4 = {
  .outmax = 10000,
  .summax = 10000,
  .kp = 50000,
  .ki = 500,
  .kd = 0,
};

static uint16_t M0_count = 0;
static uint16_t M1_count = 0;
static uint16_t M2_count = 0;
static uint16_t M3_count = 0;
void mecanum_calibrate(void)
{
  motor[0].total = 0;
  motor[1].total = 0;
  motor[2].total = 0;
  motor[3].total = 0;
  M0_count = __HAL_TIM_GET_COUNTER(&htim2);
  M1_count = __HAL_TIM_GET_COUNTER(&htim3);
  M2_count = __HAL_TIM_GET_COUNTER(&htim4);
  M3_count = __HAL_TIM_GET_COUNTER(&htim5);
}

static void exec_encoder(void *argv)

{
  (void)argv;

#undef ENCODER_READ
#define ENCODER_READ(id, htim)                     \
  do                                               \
  {                                                \
    uint16_t count = __HAL_TIM_GET_COUNTER(&htim); \
    motor[id].delta = M##id##_count - count;       \
    if (motor[id].delta < -0x8000)                 \
    {                                              \
      motor[id].delta += 0x10000;                  \
    }                                              \
    else if (0x8000 < motor[id].delta)             \
    {                                              \
      motor[id].delta -= 0x10000;                  \
    }                                              \
    motor[id].total += motor[id].delta;            \
    M##id##_count = count;                         \
  } while (0)
  ENCODER_READ(0, htim2);
  ENCODER_READ(1, htim3);
  ENCODER_READ(2, htim4);
  ENCODER_READ(3, htim5);
#undef ENCODER_READ

#undef ENCODER_CALC
#define ENCODER_CALC(id)                                                    \
  do                                                                        \
  {                                                                         \
    motor[id].speed = motor[id].delta * 23.56194490192345F / (4 * 13 * 30); \
  } while (0)
  ENCODER_CALC(0);
  ENCODER_CALC(1);
  ENCODER_CALC(2);
  ENCODER_CALC(3);
#undef ENCODER_CALC

  if (READ_BIT(exec->uart, IO_RXEN_MSK))
  {
    if (exec->io[1] == 0xFF && exec->io[2] == 0xFF)
    {
      exec->x = *(int16_t *)(exec->io + 3) * 0.001F;
      exec->y = *(int16_t *)(exec->io + 5) * 0.001F;
      exec->z = *(int16_t *)(exec->io + 7) * 0.001F;
      mecanum_postive(exec->x, exec->y, exec->z);
    }
    else if (exec->io[1] == 0xFF && exec->io[2] == 0xFE)
    {
      mecanum_calibrate();
    }
    else if (exec->io[1] == 0xFF && exec->io[2] == 0xFD)
    {
      pid4.kp = *(uint16_t *)(exec->io + 3);
      pid4.ki = *(uint16_t *)(exec->io + 5);
      pid4.kd = *(uint16_t *)(exec->io + 7);
    }
    CLEAR_BIT(exec->uart, IO_RXEN_MSK);
  }

  pid4_out(&pid4);
  motor_set_pwm(pid4.out[0], pid4.out[1], pid4.out[2], pid4.out[3]);

  float x, y, w;
  mecanum_nagtive(&x, &y, &w);
  static unsigned char buffer[20] = {0xFF, 0xFF};
  *(int16_t *)(buffer + 2) = x * 1000;
  *(int16_t *)(buffer + 4) = y * 1000;
  *(int16_t *)(buffer + 6) = w * 1000;
  mecanum_odometer((float *)(buffer + 8), (float *)(buffer + 12), (float *)(buffer + 16));
  uart_dma_tx(&huart1, buffer, 20);
}

static void exec_io(void *argv)
{
  // io_printf("%g,%g,%g,%g\n", pid4.e[0], pid4.e[1], pid4.e[2], pid4.e[3]);
  // io_printf("%g,%g,%g,%g\n", pid4.sum[0], pid4.sum[1], pid4.sum[2], pid4.sum[3]);
  // io_printf("%g,%g,%g,%g\n", pid4.out[0], pid4.out[1], pid4.out[2], pid4.out[3]);
  // io_printf("%g,%g,%g,%g\n", motor[0].write, motor[1].write, motor[2].write, motor[3].write);
  // io_printf("%g,%g,%g,%g\n", motor[0].speed, motor[1].speed, motor[2].speed, motor[3].speed);
  // io_printf("%i,%i,%i,%i\n", motor[0].delta, motor[1].delta, motor[2].delta, motor[3].delta);
  // io_printf("%i,%i,%i,%i\n", motor[0].total, motor[1].total, motor[2].total, motor[3].total);
  // float x, y, w;
  // mecanum_nagtive(&x, &y, &w);
  // mecanum_odometer(&x, &y, &w);
  // io_justfloat(3, x, y, w);
  // io_printf("%g,%g,%g\n", x, y, w);
  // io_printf("%g,%g,%g\n", exec->x, exec->y, exec->z);
  (void)argv;
}

void timeslice_init(void)
{
  static timeslice_s led[1];
  timeslice_cron(led, exec_led, 0, 500);
  HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, GPIO_PIN_SET);
  timeslice_join(led);

  static timeslice_s encoder[1];
  timeslice_cron(encoder, exec_encoder, 0, 10);
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  timeslice_join(encoder);

  static timeslice_s io[1];
  timeslice_cron(io, exec_io, 0, 100);
  timeslice_join(io);

  io_init();
  io_printf("OK\n");
}

/* END OF FILE */
