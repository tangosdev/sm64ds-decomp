#include "types.h"

/* The sign of the double bit pattern in u64 x: 0x80 if the top byte's sign
   bit is set, else 0. Byte 7 is read through a u16 load, as func_0206c8b4
   reads the exponent. */
int func_0206c90c(u64 x) {
    char *q = (char *)&x + 7;
    int hi;
    if ((int)q & 1)
        hi = (*(u16 *)(q - 1) & 0xff00) >> 8;
    else
        hi = *(u16 *)q & 0xff;
    return hi & 0x80;
}
