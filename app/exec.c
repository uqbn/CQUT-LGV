/*!
 @file exec.c
 @brief Execution implementation.
 @copyright Copyright (C) 2020 tqfx. All rights reserved.
*/

#include "exec.h"
#include "bsp.h"
#include "io.h"

#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"

#include "a/fpid.h"

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
  (void)argv;
}

static struct
{
  uint16_t count[4];
  int32_t total[4];
  int32_t delta[4];
  float speed[4];
  float write[4];
} motor = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
};
static struct
{
  a_fpid_s ctx;
  float out[4];
  float fdb[4];
  float sum[4];
  float ec[4];
  float e[4];
  float const me[29];
  float const mec[29];
  float const mkp[49];
  float const mki[49];
  float const mkd[49];
  a_uint_t idx[A_FPID_IDX(2)];
  float val[A_FPID_VAL(2)];
} chassis = {
#undef NB
#undef NM
#undef NS
#undef ZO
#undef PS
#undef PM
#undef PB
#define NB -3 * S
#define NM -2 * S
#define NS -1 * S
#define ZO +0 * S
#define PS +1 * S
#define PM +2 * S
#define PB +3 * S
#undef S
#define S 0.4F
  .me = {A_MF_TRI, NB, NB, NM,
         A_MF_TRI, NB, NM, NS,
         A_MF_TRI, NM, NS, ZO,
         A_MF_TRI, NS, ZO, PS,
         A_MF_TRI, ZO, PS, PM,
         A_MF_TRI, PS, PM, PB,
         A_MF_TRI, PM, PB, PB,
         A_MF_NUL},
#undef S
#define S 0.8F
  .mec = {A_MF_TRI, NB, NB, NM,
          A_MF_TRI, NB, NM, NS,
          A_MF_TRI, NM, NS, ZO,
          A_MF_TRI, NS, ZO, PS,
          A_MF_TRI, ZO, PS, PM,
          A_MF_TRI, PS, PM, PB,
          A_MF_TRI, PM, PB, PB,
          A_MF_NUL},
#undef S
#define S 20000
  .mkp = {NB, NB, NM, NM, NS, ZO, ZO,
          NB, NB, NM, NS, NS, ZO, PS,
          NM, NM, NM, NS, ZO, PS, PS,
          NM, NM, NS, ZO, PS, PM, PM,
          NS, NS, ZO, PS, PS, PM, PM,
          NS, ZO, PS, PM, PM, PM, PB,
          ZO, ZO, PM, PM, PM, PB, PB},
#undef S
#define S 100
  .mki = {PB, PB, PM, PM, PS, ZO, ZO,
          PB, PB, PM, PS, PS, ZO, ZO,
          PB, PM, PS, PS, ZO, NS, NS,
          PM, PM, PS, ZO, NS, NM, NM,
          PM, PS, ZO, NS, NS, NM, NB,
          ZO, ZO, NS, NS, NM, NB, NB,
          ZO, ZO, NS, NM, NM, NB, NB},
#undef S
#define S 30
  .mkd = {NS, PS, PB, PB, PB, PM, NS,
          NS, PS, PB, PM, PM, PS, ZO,
          ZO, PS, PM, PM, PS, PS, ZO,
          ZO, PS, PS, PS, PS, PS, ZO,
          ZO, ZO, ZO, ZO, ZO, ZO, ZO,
          NB, NS, NS, NS, NS, NS, NB,
          NB, NM, NM, NM, NS, NS, NB},
#undef S
#undef NB
#undef NM
#undef NS
#undef ZO
#undef PS
#undef PM
#undef PB
};

void mecanum_odometer_nagtive(float *x, float *y, float *w)
{
  *x = (motor.total[0] - motor.total[1] - motor.total[2] + motor.total[3]) * 0.23561944901923448F / (4 * 13 * 30) * 0.25F;
  *y = (motor.total[0] + motor.total[1] - motor.total[2] - motor.total[3]) * 0.23561944901923448F / (4 * 13 * 30) * 0.25F;
  *w = (motor.total[0] + motor.total[1] + motor.total[2] + motor.total[3]) * 0.23561944901923448F / (4 * 13 * 30) * 1.25F;
}

void mecanum_odometer_postive(float x, float y, float w)
{
  motor.total[0] = (+x + y + w * 0.2F) * (4 * 13 * 30) / 0.23561944901923448F;
  motor.total[1] = (-x + y + w * 0.2F) * (4 * 13 * 30) / 0.23561944901923448F;
  motor.total[2] = (-x - y + w * 0.2F) * (4 * 13 * 30) / 0.23561944901923448F;
  motor.total[3] = (+x - y + w * 0.2F) * (4 * 13 * 30) / 0.23561944901923448F;
}

void mecanum_nagtive(float *x, float *y, float *w)
{
  *x = (motor.speed[0] - motor.speed[1] - motor.speed[2] + motor.speed[3]) * 0.25F;
  *y = (motor.speed[0] + motor.speed[1] - motor.speed[2] - motor.speed[3]) * 0.25F;
  *w = (motor.speed[0] + motor.speed[1] + motor.speed[2] + motor.speed[3]) * 1.25F;
}

void mecanum_postive(float x, float y, float w)
{
  motor.write[0] = +x + y + w * 0.2F;
  motor.write[1] = -x + y + w * 0.2F;
  motor.write[2] = -x - y + w * 0.2F;
  motor.write[3] = +x - y + w * 0.2F;
}

void mecanum_calibrate(void)
{
  motor.count[0] = __HAL_TIM_GET_COUNTER(&htim2);
  motor.count[1] = __HAL_TIM_GET_COUNTER(&htim3);
  motor.count[2] = __HAL_TIM_GET_COUNTER(&htim4);
  motor.count[3] = __HAL_TIM_GET_COUNTER(&htim5);
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

static void exec_encoder(void *argv)
{
  (void)argv;

#undef ENCODER_READ
#define ENCODER_READ(id, htim)                     \
  do                                               \
  {                                                \
    uint16_t count = __HAL_TIM_GET_COUNTER(&htim); \
    motor.delta[id] = motor.count[id] - count;     \
    if (motor.delta[id] < -0x8000)                 \
    {                                              \
      motor.delta[id] += 0x10000;                  \
    }                                              \
    else if (0x8000 < motor.delta[id])             \
    {                                              \
      motor.delta[id] -= 0x10000;                  \
    }                                              \
    motor.total[id] += motor.delta[id];            \
    motor.count[id] = count;                       \
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
    motor.speed[id] = motor.delta[id] * 23.56194490192345F / (4 * 13 * 30); \
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
      mecanum_odometer_postive(*(int16_t *)(exec->io + 3) * 0.001F,
                               *(int16_t *)(exec->io + 5) * 0.001F,
                               *(int16_t *)(exec->io + 7) * 0.001F);
    }
    else if (exec->io[1] == 0xFF && exec->io[2] == 0xFD)
    {
      a_pid_zero(&chassis.ctx.pid);
      a_pid_kpid(&chassis.ctx.pid,
                 *(uint16_t *)(exec->io + 3),
                 *(uint16_t *)(exec->io + 5),
                 *(uint16_t *)(exec->io + 7));
    }
    CLEAR_BIT(exec->uart, IO_RXEN_MSK);
  }

  float const *pwm = a_pid_outp(&chassis.ctx.pid, motor.write, motor.speed);
  // float const *pwm = a_fpid_outp(&chassis.ctx, motor.write, motor.speed);
  motor_set_pwm(pwm[0], pwm[1], pwm[2], pwm[3]);

  float x, y, w;
  mecanum_nagtive(&x, &y, &w);
  static unsigned char buffer[20] = {0xFF, 0xFF};
  *(int16_t *)(buffer + 2) = x * 1000;
  *(int16_t *)(buffer + 4) = y * 1000;
  *(int16_t *)(buffer + 6) = w * 1000;
  mecanum_odometer_nagtive((float *)(buffer + 8), (float *)(buffer + 12), (float *)(buffer + 16));
  uart_dma_tx(&huart1, buffer, 20);
}

static void exec_io(void *argv)
{
  // io_printf("%g,%g,%g,%g\n", chassis.e[0], chassis.e[1], chassis.e[2], chassis.e[3]);
  // io_printf("%g,%g,%g,%g\n", chassis.sum[0], chassis.sum[1], chassis.sum[2], chassis.sum[3]);
  // io_printf("%g,%g,%g,%g\n", chassis.out[0], chassis.out[1], chassis.out[2], chassis.out[3]);
  // io_printf("%g,%g,%g,%g\n", motor.write[0], motor.write[1], motor.write[2], motor.write[3]);
  // io_printf("%g,%g,%g,%g\n", motor.speed[0], motor.speed[1], motor.speed[2], motor.speed[3]);
  // io_printf("%i,%i,%i,%i\n", motor.delta[0], motor.delta[1], motor.delta[2], motor.delta[3]);
  // io_printf("%i,%i,%i,%i\n", motor.total[0], motor.total[1], motor.total[2], motor.total[3]);
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
  timeslice_join(led);

  HAL_GPIO_WritePin(LEDR_GPIO_Port, LEDR_Pin, GPIO_PIN_SET);

  static timeslice_s encoder[1];
  timeslice_cron(encoder, exec_encoder, 0, 10);
  timeslice_join(encoder);

  a_pid_pos(a_pid_init(&chassis.ctx.pid, 0.01F, -10000, 10000), 2000);
  a_pid_chan(&chassis.ctx.pid, 4, chassis.out, chassis.fdb, chassis.sum, chassis.ec, chassis.e);
  a_pid_kpid(&chassis.ctx.pid, 0x10000, 0, 0);

  // a_fpid_init(&chassis.ctx, 0.01F, 7, chassis.me, chassis.mec, chassis.mkp, chassis.mki, 0, -10000, 10000);
  // a_fpid_chan(&chassis.ctx, 4, chassis.out, chassis.fdb, chassis.sum, chassis.ec, chassis.e);
  // a_fpid_pos(a_fpid_buff(&chassis.ctx, chassis.idx, chassis.val), 2000);
  // a_fpid_kpid(&chassis.ctx, 0xFFFF, 655.35, 0);
  // a_fpid_kpid(&chassis.ctx, 60000, 1500, 0);

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

  static timeslice_s io[1];
  timeslice_cron(io, exec_io, 0, 100);
  timeslice_join(io);

  io_init();
  io_printf("OK\n");
}

/* END OF FILE */
