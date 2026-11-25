#include "types.h"

/* IEEE-754 double bits in little-endian byte order. The high nibble of
   exponentLowAndFractionHigh is the exponent's low four bits. */
struct Ieee754DoubleBytes {
    u8 fractionLow[6];
    u8 exponentLowAndFractionHigh;
    u8 signAndExponent;
};

/* One byte through the aligned u16 accesses this debug-output library uses. */
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

    hi = ReadByte(&((struct Ieee754DoubleBytes *)&x)->signAndExponent);
    lo = ReadByte(&((struct Ieee754DoubleBytes *)&x)->exponentLowAndFractionHigh);

    return ((hi & 0x7f) << 4) + (lo >> 4);
}
