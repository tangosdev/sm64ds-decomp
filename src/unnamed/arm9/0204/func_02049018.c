// @symbol func_02049018
/* recovered: shared common types */
#include "common.h"
int func_02049018(struct Vector3 *v)
{
    int tx = v->x >> 12;
    int ty = (v->y >> 12) * 2;
    int tz = v->z >> 12;
    int sum = tx * tx + ty * ty + tz * tz;
    volatile unsigned short *ime = (volatile unsigned short *)0x4000208;
    unsigned short saved = *ime;
    *ime = 0;
    *(volatile unsigned short *)0x40002b0 = 0;
    *(volatile unsigned int *)0x40002b8 = sum;
    *ime;
    *ime = saved;
    while (*(volatile unsigned short *)0x40002b0 & 0x8000);
    while (*(volatile unsigned short *)0x40002b0 & 0x8000);
    return *(volatile int *)0x40002b4;
}
