#include "PlayerInput.h"
extern int data_0209f4a2;

typedef struct { char _pad[0x19c]; short field_19c; } dCamera_c;

short func_02008abc(dCamera_c *cam)
{
    unsigned char idx = *(unsigned char *)&gActivePlayerSlot;
    short tbl = *(short *)((char *)&data_0209f4a2 + idx * 0x18);
    short val = *(short *)((char *)cam + 0x19c);
    long long prod = (long long)tbl * (long long)val;
    prod += 0x800LL;
    int tmp = (int)(prod >> 12);
    return (short)tmp;
}
