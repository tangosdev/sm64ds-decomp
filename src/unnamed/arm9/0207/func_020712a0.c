#include "Decimal.h"

extern void func_02071644(Decimal *result, int length);

void func_020712a0(Decimal *result, unsigned char *digits, short exp)
{
    int i;
    unsigned char c;
    result->exp = exp;
    result->sign = 0;
    i = 0;
    while (i < 0x20 && digits[0] != 0) {
        result->sig.text[i++] = (unsigned char)(*digits++ - 0x30);
    }
    result->sig.length = (unsigned char)i;
    if (digits[0] == 0) return;
    if (digits[0] < 5) return;
    if (digits[0] <= 5) {
        c = digits[1];
        digits++;
        while (c != 0) {
            /* Kept goto: break plus a `c == 0` guard (+8), while (c == '0') (+4),
               for (;;) with the return inside (-4), a duplicated call (+20) and
               an inline must-round predicate (+12..+20) all change the object. */
            if (c != 0x30)
                goto docall;
            c = *++digits;
        }
        if ((result->sig.text[i - 1] & 1) == 0)
            return;
    }
docall:
    func_02071644(result, result->sig.length);
}
