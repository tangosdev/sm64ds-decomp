#include "types.h"
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int func_02064d6c(int dest, u16 a, u8 b, int c, int d, void *p, int n)
{
    u8 kind = 5;
    u16 a16 = a;
    int pos;
    int len;
    CpuCopy8(&kind, (void *)dest, 1);
    pos = dest + 1;
    CpuCopy8(&a16, (void *)pos, 2);
    pos = (int)(((long long)(pos + 2)));
    CpuCopy8(&b, (void *)pos, 1);
    pos = (int)(((long long)(pos + 1)));
    CpuCopy8(&c, (void *)pos, 4);
    pos = (int)(((long long)(pos + 4)));
    CpuCopy8(&d, (void *)pos, 4);
    len = (int)(((long long)n));
    pos = (int)(((long long)(pos + 4)));
    CpuCopy8(p, (void *)pos, (unsigned int)len);
    return (pos + len) - dest;
}
