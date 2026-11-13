#include "Decimal.h"

/* MSL printf.c's round_decimal: round dec to new_length digits, half to
   even (a 5 followed only by zeros rounds toward an even last digit), and
   trim the trailing zeros the rounding leaves. A carry out of the top digit
   leaves "1" and bumps the exponent. A negative length, or rounding down to
   no digits, leaves "0". The digits are ASCII here. float2str calls
   it. */
void round_decimal(Decimal *dec, int new_length) {
    unsigned char *digits;
    unsigned char *q;
    unsigned char *end;
    unsigned int d;
    unsigned int e;
    int carry;
    int len;

    if (new_length < 0) {
    reset:
        dec->exp = 0;
        dec->sig.length = 1;
        dec->sig.text[0] = 0x30;
        return;
    }
    len = dec->sig.length;
    if (new_length >= len)
        return;

    digits = &dec->sig.text[0];
    q = digits + new_length + 1;
    d = (*--q - 0x30) & 0xff;
    if (d == 5) {
        end = digits + len;
        do {
            end--;
        } while (end > q && *end == 0x30);
        if (end == q)
            carry = q[-1] & 1;
        else
            carry = 1;
    } else {
        carry = (d > 5) ? 1 : 0;
    }

    if (new_length != 0) {
        do {
            e = ((*--q - 0x30) + carry) & 0xff;
            carry = (e > 9) ? 1 : 0;
            if (carry != 0 || e == 0) {
                new_length--;
            } else {
                *q = e + 0x30;
                break;
            }
        } while (new_length != 0);
    }

    if (carry != 0) {
        short *pc = (short *)((int)dec + 2);
        *pc = *pc + 1;
        dec->sig.length = 1;
        dec->sig.text[0] = 0x31;
        return;
    }
    if (new_length == 0)
        // goto: for-continue (0x150), while-after (0x130), do-while (0x150) still DIFF round_decimal
        goto reset;
    dec->sig.length = new_length;
}
