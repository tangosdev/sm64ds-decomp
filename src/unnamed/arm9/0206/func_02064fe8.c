#include "nitro/mi/memory.h"
/* func_02064fe8 at 0x02064fe8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

int func_02064fe8(void *dst, unsigned short b, char c) {
    char *p;
    char *q;
    unsigned char one = 1;
    unsigned short s = b;
    MI_CpuCopy8(&one, dst, 1);
    p = dst + 1;
    MI_CpuCopy8(&s, p, 2);
    q = p + 2;
    MI_CpuCopy8(&c, q, 1);
    return q + 1 - dst;
}
