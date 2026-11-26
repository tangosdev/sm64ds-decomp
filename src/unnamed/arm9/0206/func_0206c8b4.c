#include "types.h"

/* One byte through an aligned u16 load: the cluster's READ8, as an inline
   function. Each inlined copy computes its own address from its argument,
   and the cartridge recomputes the double's address for each of the two
   reads; the READ8 macro or a shared local lets CSE share it (0x54 against
   0x58). */
static inline int ReadByte(const void *a) {
    int p = (int)a;
    if (p & 1)
        return (*(unsigned short *)(p - 1) & 0xff00) >> 8;
    else
        return *(unsigned short *)p & 0xff;
}

/* The 11-bit biased exponent of the double bit pattern in u64 x, read a
   byte at a time through u16 loads (bytes 7 and 6). 0x3ff is 1.0. */
int func_0206c8b4(u64 x) {
    int hi, lo;

    hi = ReadByte((u8 *)&x + 7);
    lo = ReadByte((u8 *)&x + 6);

    return ((hi & 0x7f) << 4) + (lo >> 4);
}
