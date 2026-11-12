#include "types.h"
#include "nitro/mi/memory.h"

int func_02064d6c(u8 *dst, u16 a, u8 b, int c, int d, const void *p, int n) {
    u32 len;
    u8 kind = 5;
    u16 a16 = a;
    u8 *p1, *p2, *p3, *p4, *p5;
    MI_CpuCopy8(&kind, dst, 1);
    p1 = dst + 1;
    MI_CpuCopy8(&a16, p1, 2);
    p2 = p1 + 2;
    MI_CpuCopy8(&b, p2, 1);
    p3 = p2 + 1;
    MI_CpuCopy8(&c, p3, 4);
    p4 = p3 + 4;
    MI_CpuCopy8(&d, p4, 4);
    p5 = p4 + 4;
    len = n;
    MI_CpuCopy8(p, p5, len);
    return (p5 + len) - dst;
}
