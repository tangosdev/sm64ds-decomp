#include "nitro/mi/memory.h"
/* func_02064b2c at 0x02064b2c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

int func_02064b2c(unsigned char *dst, unsigned short b, int c, int d) {
    unsigned char tag = 9;
    short val = b;
    unsigned char *c1, *c2, *c3;
    MI_CpuCopy8(&tag, dst, 1);
    c1 = dst + 1;
    MI_CpuCopy8(&val, c1, 2);
    c2 = c1 + 2;
    MI_CpuCopy8(&c, c2, 1);
    c3 = c2 + 1;
    MI_CpuCopy8(&d, c3, 4);
    return c3 + 4 - dst;
}
