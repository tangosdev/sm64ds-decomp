#include "nitro/mi/memory.h"

int func_02064f54(unsigned char *dst, unsigned short b, unsigned char c, int d, int e) {
    unsigned char tag = 2;
    short val = b;
    unsigned char *p1, *p2, *p3, *p4;
    MI_CpuCopy8(&tag, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&val, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&c, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&d, p3, 2);
    p4 = p3 + 2;
    MI_CpuCopy8(&e, p4, 2);
    return (p4 + 2) - dst;
}
