#include "types.h"
extern int func_0206e218(void *buf, int code);
extern void _ZN4cstd7strncpyEPcPKcj(char *d, const char *s, u32 n);

u32 func_0206e184(char *dst, u16 *src, u32 max)
{
    u32 total = 0;
    char buf[4];
    char *bp;
    if (dst == 0 || src == 0)
        return 0;
    bp = buf;
    for (;;) {
        u16 code = *src;
        int len;
        if (code == 0) {
            dst[total] = 0;
            break;
        }
        src++;
        len = func_0206e218(bp, code);
        if (total + len > max)
            break;
        _ZN4cstd7strncpyEPcPKcj(dst + total, bp, len);
        total += len;
        if (total > max)
            break;
    }
    return total;
}
