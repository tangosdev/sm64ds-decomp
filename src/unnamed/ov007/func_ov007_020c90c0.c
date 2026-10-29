#include "Ov007Tween.h"

void func_ov007_020c90c0(char *c)
{
    Ov007Tween *tw = (Ov007Tween *)c;
    short value = tw->t;

    if (value >= 0x1000) {
        tw->t = 0x1000;
        tw->state = 2;
        return;
    }

    {
        int speed = tw->step;
        short *pos = &tw->t;
        short old = *pos;

        speed <<= 16;
        *pos = old + (speed >> 16);
    }
}
