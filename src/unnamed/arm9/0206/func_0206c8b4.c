#include "types.h"
#include "ds_width.h"

/* opt_common_subs stays off: the cartridge recomputes the double's address for
   each of the two byte reads, and CSE shares it (0x54 against 0x58). Spelling
   the reads through two locals or the cluster's READ8 macro does not restore it. */
#pragma opt_common_subs off
/* The 11-bit biased exponent of the double bit pattern in u64 x, read a
   byte at a time through u16 loads (bytes 7 and 6). 0x3ff is 1.0. */
int func_0206c8b4(u64 x) {
    int hi, lo;
    int p;

    p = DS_IADDR(&x) + 7;
    if (p & 1)
        hi = (*(unsigned short *)(p - 1) & 0xff00) >> 8;
    else
        hi = *(unsigned short *)p & 0xff;

    p = DS_IADDR(&x) + 6;
    if (p & 1)
        lo = (*(unsigned short *)(p - 1) & 0xff00) >> 8;
    else
        lo = *(unsigned short *)p & 0xff;

    return ((hi & 0x7f) << 4) + (lo >> 4);
}
