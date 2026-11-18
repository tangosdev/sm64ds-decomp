//cpp
#include "msl/string.h"

extern "C" void func_020147bc(unsigned short *p, int c);

/* opt_common_subs stays off: the cartridge keeps the lower-case, upper-case and
   default arms of the digit test as three separate `out = c` tails, and CSE
   merges them (0xe0 against the ROM's 0xec). Round 66 (#821): `int c` restores
   the ROM's second `c - 0x2f` but the arms still merge (0xe0) unless one is
   respelled `(int)(c)`, a no-op cast; str[len] in an arm, a merged letter
   test, `(char)c` and a char `out` are 0xe0/0xe4/0xf0/0x100. */
/* Kept (#821 r79): /=, a radix temporary, '0' + 1 character literals and static inline
   digit/shift helpers are 0xe0 or 0xdc against 0xec; the division and modulo still share one
   _s32_div_f. */
#pragma opt_common_subs off

void nds_printt(unsigned short *dest, const char *str, int value) {
    int len = strlen(str);
    char c;
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
