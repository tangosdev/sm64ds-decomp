#include "types.h"

/* IEEE-754 double bits in little-endian byte order. */
struct Ieee754DoubleBytes {
    u8 fractionLow[6];
    u8 exponentLowAndFractionHigh;
    u8 signAndExponent;
};

/* The sign of the double bit pattern in u64 x: 0x80 if its sign bit is set,
   else 0. The byte is read through a u16 load, as func_0206c8b4 reads the
   exponent. */
int func_0206c90c(u64 x) {
    u8 *q = &((struct Ieee754DoubleBytes *)&x)->signAndExponent;
    int hi;
    if ((int)q & 1)
        hi = (*(u16 *)(q - 1) & 0xff00) >> 8;
    else
        hi = *(u16 *)q & 0xff;
    return hi & 0x80;
}
