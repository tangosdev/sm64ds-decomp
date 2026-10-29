#include "WmWork.h"
#pragma optimize_for_size on

extern char *data_020a9db4;

int func_0206a230(void)
{
    char *r2; /* not WmWork *: a typed local re-colours r2/ip/r3 */
    unsigned int r0;
    unsigned int ip;
    unsigned int r3;
    unsigned short r1;

    /* r3/ip/r0 stay materialized 0/1 flags: return A && B && C, chained inline
       predicates and ?1:0 each change the size (0x50-0x78) */
    r0 = 0;
    r2 = data_020a9db4;
    ip = 0;
    r3 = 0;
    r1 = ((WmWork *)r2)->mpStarted;
    if (r1 != 1) { /* empty arm: r1 == 1 && ... re-colours r3 (0x74) */
    } else if (((WmWork *)r2)->mpSending == 0) {
        r3 = 1;
    }
    if (r3 != 0) {
        r1 = ((WmWork *)r2)->resetting;
        if (r1 == 0) {
            ip = 1;
        }
    }
    if (ip != 0) {
        r1 = ((WmWork *)r2)->childBitmap;
        if (r1 != 0) {
            r0 = 1;
        }
    }
    return r0;
}