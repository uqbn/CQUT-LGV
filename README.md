# LGV Driver

<p align="center">

| 电机1 | 引脚 | 电机2 | 引脚 | 电机3 | 引脚 | 电机4 | 引脚 |
| ----- | ---- | ----- | ---- | ----- | ---- | ----- | ---- |
| 1A    | PA15 | 2A    | PA6  | 3A    | PD12 | 4A    | PA0  |
| 1B    | PB3  | 2B    | PA7  | 3B    | PD13 | 4B    | PA1  |
| 1IN1  | PB10 | 2IN1  | PB0  | 3IN1  | PD14 | 4IN1  | PA2  |
| 1IN2  | PB11 | 2IN2  | PB1  | 3IN2  | PD15 | 4IN2  | PA3  |
| 1P    | PE9  | 2P    | PE11 | 3P    | PE13 | 4P    | PE14 |

</p>

```c
#include <stdio.h>
int main(void)
{
  unsigned char const *p;

  short pos = +1000;
  short neg = -1000;

  p = (unsigned char const *)&pos;
  printf("FF FF %02hx %02hx 00 00 00 00\n", p[0] & 0xFF, p[1] & 0xFF);
  p = (unsigned char const *)&neg;
  printf("FF FF %02hx %02hx 00 00 00 00\n", p[0] & 0xFF, p[1] & 0xFF);

  p = (unsigned char const *)&pos;
  printf("FF FF 00 00 %02hx %02hx 00 00\n", p[0] & 0xFF, p[1] & 0xFF);
  p = (unsigned char const *)&neg;
  printf("FF FF 00 00 %02hx %02hx 00 00\n", p[0] & 0xFF, p[1] & 0xFF);

  p = (unsigned char const *)&pos;
  printf("FF FF 00 00 00 00 %02hx %02hx\n", p[0] & 0xFF, p[1] & 0xFF);
  p = (unsigned char const *)&neg;
  printf("FF FF 00 00 00 00 %02hx %02hx\n", p[0] & 0xFF, p[1] & 0xFF);

  pos = +100;
  neg = -100;
  p = (unsigned char const *)&pos;
  printf("FF FF %02hx %02hx 00 00 00 00\n", p[0] & 0xFF, p[1] & 0xFF);
  p = (unsigned char const *)&neg;
  printf("FF FF %02hx %02hx 00 00 00 00\n", p[0] & 0xFF, p[1] & 0xFF);

  return 0;
}
```
