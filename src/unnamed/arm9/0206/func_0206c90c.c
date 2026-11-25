#include "types.h"

/* IEEE-754 double bits in little-endian byte order. */
struct Ieee754DoubleBytes {
    u8 fractionLow[6];
    u8 exponentLowAndFractionHigh;
    u8 signAndExponent;
};

/* One byte through the aligned u16 accesses this debug-output library uses. */
static inline int ReadByte(const void *a) {
    int p = (int)a;
    if (p & 1)
        return (*(u16 *)(p - 1) & 0xff00) >> 8;
    return *(u16 *)p & 0xff;
}

/* The sign of the double bit pattern in u64 x: 0x80 if its sign bit is set,
   else 0. */
int func_0206c90c(u64 x) {
    u8 *q = &((struct Ieee754DoubleBytes *)&x)->signAndExponent;
    return ReadByte(q) & 0x80;
}
