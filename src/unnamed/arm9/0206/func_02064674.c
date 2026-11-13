#include "nitro/mi/memory.h"
/* func_02064674 at 0x02064674
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int func_02065bd0(void);

int func_02064674(int *p, int idx, int dst)
{
    int n = func_02065bd0();
    int cnt = p[1];
    int base = p[6];
    if (idx < cnt) {
        if (idx == cnt - 1) {
            int rem = p[0] % n;
            if (rem != 0)
                MI_CpuCopy8((const void *)dst, (void *)(idx * n + base), rem);
            else
                MI_CpuCopy8((const void *)dst, (void *)(idx * n + base), n);
        } else {
            MI_CpuCopy8((const void *)dst, (void *)(idx * n + base), n);
        }
    } else {
        return 0;
    }
    return 1;
}
