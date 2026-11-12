#include "nitro/mi/memory.h"

int func_02064a28(unsigned char *dst, unsigned short v, unsigned char b) {
    unsigned char tag = 0xb;
    short val = v;
    unsigned char *c1, *c2;
    MI_CpuCopy8(&tag, dst, 1);
    c1 = dst + 1;
    MI_CpuCopy8(&val, c1, 2);
    c2 = c1 + 2;
    MI_CpuCopy8(&b, c2, 1);
    return c2 + 1 - dst;
}
