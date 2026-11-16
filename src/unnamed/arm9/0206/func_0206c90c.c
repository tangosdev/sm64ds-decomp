#include "types.h"

/* The sign of the double bit pattern in u64 x: 0x80 if the top byte's sign
   bit is set, else 0. Byte 7 is read through a u16 load, as func_0206c8b4
   reads the exponent. Taking x as a union of u64 and u8[8] changes how it
   is passed (0x2c, not 0x30), and copying it into a union local adds a
   store (0x34), so the byte is addressed from &x. */
int func_0206c90c(u64 x) {
    char *q = (char *)&x + 7;
    int hi;
    if ((int)q & 1)
        hi = (*(u16 *)(q - 1) & 0xff00) >> 8;
    else
        hi = *(u16 *)q & 0xff;
    return hi & 0x80;
}
