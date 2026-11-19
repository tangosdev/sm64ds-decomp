//cpp
#include "msl/string.h"

extern "C" void func_020147bc(unsigned short *p, int c);

/* The cartridge keeps the lower-case, upper-case and default arms of the digit
   test as three separate `out = c` tails, and computes the modulo and the
   division with two calls. `c` is an int, the usual type for a character read
   out of a string, and the default arm's `(int)(c)` keeps that arm apart: with
   `out = c` there the three tails merge (0xe0 against 0xec). Kept (#821 r105):
   the cast was already load-bearing next to opt_common_subs off; with `int c`
   the pragma is not needed and the cast is the one lever left. */

void nds_printt(unsigned short *dest, const char *str, int value) {
    int len = strlen(str);
    int c;
    unsigned short *p = dest + len;
    while (value != 0 && len != 0) {
        int out;
        len--;
        c = str[len];
        p--;
        if (c >= 0x30 && c <= 0x39) {
            out = value % (c - 0x2f);
            value = value / (c - 0x2f);
            out += 0x30;
        } else if (c >= 0x61 && c <= 0x7a) {
            out = c;
        } else if (c >= 0x41 && c <= 0x5a) {
            out = c;
        } else {
            out = (int)(c);
        }
        func_020147bc(p, (char)out);
    }
    if (len == 0) return;
    {
        unsigned short *q = dest + len;
        int space = 0x20;
        do {
            q--;
            len--;
            func_020147bc(q, space);
        } while (len != 0);
    }
}
