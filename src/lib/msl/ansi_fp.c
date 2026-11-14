/* MSL ansi_fp.c, the decimal conversions behind printf's floating-point
 * formats: arm9 .text 0x02070b98..0x02071784, 10 functions.
 *
 * The run is MSL's ansi_fp.c in its own order. mwccarm emits .text in
 * reverse source order, and read from the highest address down the run is
 * the trailing-zero count, __must_round, __dorounddecup, __rounddec,
 * __ull2dec, __timesdec, __str2dec, __two_exp, __num2dec_internal and
 * __num2dec: the order of the Petari decomp's ansi_fp.c, which also has the
 * trailing-zero count at its top (as an inline over a compiler builtin).
 * Petari's (Wii) copy also has __equals_dec, __less_dec, __minus_dec and
 * __dec2num; Spirit Tracks' ARM MSL ansi_fp.c, which has this run's order
 * exactly, has none of them.
 *
 * BOTTOM EDGE: ldexp (0x020708b4..0x02070b98) is MSL's s_ldexp.c, its own
 * file. TOP EDGE: _ExitProcess begins where the run ends.
 *
 * __two_exp's digit strings are its own literals: the file also claims .data
 * 0x0209a628..0x0209a6fc, where they sit in source order, each padded to 4.
 *
 * MSL was built with ARM's default unsigned plain char, not the -char signed
 * this tree compiles with: __str2dec reads its const char digits with ldrb
 * and compares them unsigned. */
#pragma unsigned_char on

#include "types.h"
#include "Decimal.h"

extern double frexp(double, int *);
extern double ldexp(double, int);


/* Count the trailing zero bits of w0, 32 for 0: skip whole zero bytes from
   the low end, then count the low set bits of the complemented byte.
   __num2dec_internal uses it to strip the significand's low zeros. The
   variadic signature keeps w0 in memory for the byte walk.
   It keeps its address name. MSL's ansi_fp.c calls this one-word counter
   __count_trailing_zerol (an out-of-line static in the Wind Waker decomp's
   copy, a static inline in Spirit Tracks' ARM copy) through an inline
   __count_trailing_zero(double) with this same low-word-first shape; the
   Petari decomp has an inline count_trailing over a compiler builtin
   instead, and the Advance Wars DS decomp calls it __cnt_trail_zeros. The
   algorithms differ: Wind Waker's halves a mask (16, 8, 4, 2, 1 bits),
   Spirit Tracks' is 32 - __cntlzw(~x & (x - 1)), and this one walks zero
   bytes in memory. So the body does not confirm either name. */
int func_02071708(int w0, ...) {
    unsigned char *p;
    int count;
    unsigned int c;
    if (w0 != 0) {
        p = (unsigned char *)&w0;
        count = 0;
        c = *p;
        if (c == 0) {
            do {
                ++p;
                count += 8;
                c = *p;
            } while (c == 0);
        }
        c = ~c;
        *p = c;
        if ((*p & 1) == 0)
            return count;
        do {
            count++;
            *p = ((int)*p >> 1);
        } while (*(volatile unsigned char *)p & 1);
        return count;
    }
    return 0x20;
}

/* MSL __must_round for __rounddec: whether cutting d to
   digits digits rounds up. 1 when the first dropped digit is above 5, or is
   5 with a non-zero digit after it, or is exactly 5 after an odd last kept
   digit (half to even). Otherwise -1. */
int __must_round(const Decimal *d, int digits) {
    const u8 *p = d->sig.text + digits;
    u8 e = *p;
    const u8 *end;
    if (e > 5)
        return 1;
    if (e < 5)
        return -1;
    p = p + 1;
    end = d->sig.text + d->sig.length;
    while (p < end) {
        if (*p != 0)
            return 1;
        p = p + 1;
    }
    return (d->sig.text[digits - 1] & 1) ? 1 : -1;
}

/* MSL __dorounddecup: increment the decimal significand, propagating carry
   into the exponent. Real C, no asm hatch; the investigation is in
   notes/mwccarm-codegen.md 9a(3). */
void __dorounddecup(Decimal *d, int len) {
    unsigned char *base = d->sig.text;
    unsigned char *p = base + len - 1;
    for (;;) {
        unsigned char c = *p;
        if (c < 9) {
            *p = c + 1;
            return;
        }
        if (p == base) {
            *p = 1;
            d->exp++;
            return;
        }
        *p-- = 0;
    }
}

/* MSL __rounddec: cut the significand to length digits, then round up
   (__dorounddecup) when __must_round says so: above half,
   or exactly half with an odd last kept digit. */
void __rounddec(Decimal *value, int length) {
    int r;
    if (length <= 0)
        return;
    if (length >= value->sig.length)
        return;
    r = __must_round(value, length);
    value->sig.length = length;
    if (r < 0)
        return;
    __dorounddecup(value, length);
}

/* MSL __ull2dec: v as a Decimal. Digit values (0..9, not ASCII) are
   generated low first, the string is reversed, and the exponent is
   length - 1. 0 gives length 0. */
void __ull2dec(Decimal *result, unsigned long long v) {
    result->sign = 0;
    result->sig.length = 0;
    if (v != 0) {
        do {
            result->sig.text[result->sig.length++] = (v % 10);
            v /= 10;
        } while (v != 0);
    }
    {
        unsigned char len = result->sig.length;
        unsigned char *s = result->sig.text;
        unsigned char *e = (s + len) - 1;
        while (s < e) {
            unsigned char t = *s;
            *s = *e;
            *e = t;
            s++;
            e--;
        }
    }
    result->exp = (result->sig.length - 1);
}

void __timesdec(Decimal *result, const Decimal *x, const Decimal *y) {
    unsigned int accumulator = 0;
    unsigned char mantissa[64];
    int i = x->sig.length + y->sig.length - 1;
    unsigned char *ip = mantissa + i + 1;
    unsigned char *ep = ip;

    result->sign = 0;

    for (; i > 0; i--) {
        int k = y->sig.length - 1;
        int j = i - k - 1;
        int l;
        int t;
        const unsigned char *jp;
        const unsigned char *kp;

        if (j < 0) {
            j = 0;
            k = i - 1;
        }

        jp = x->sig.text + j;
        kp = y->sig.text + k;
        l = k + 1;
        t = x->sig.length - j;

        if (l > t) {
            l = t;
        }

        for (; l > 0; l--, jp++, kp--) {
            accumulator += *jp * *kp;
        }

        *--ip = (accumulator % 10);
        accumulator /= 10;
    }

    result->exp = (x->exp + y->exp);

    if (accumulator) {
        *--ip = accumulator;
        result->exp++;
    }

    for (i = 0; i < 32 && ip < ep; i++, ip++) {
        result->sig.text[i] = *ip;
    }
    result->sig.length = i;

    if (ip < ep && *ip >= 5) {
        if (*ip == 5) {
            unsigned char *jp = ip + 1;
            for (; jp < ep; jp++) {
                if (*jp != 0) {
                    goto round;
                }
            }
            if ((ip[-1] & 1) == 0) {
                return;
            }
        }
    round:
        __dorounddecup(result, result->sig.length);
    }
}

void __str2dec(Decimal *result, const char *digits, short exp) {
    int i;
    char c;
    result->exp = exp;
    result->sign = 0;
    i = 0;
    while (i < 0x20 && digits[0] != 0) {
        result->sig.text[i++] = (*digits++ - 0x30);
    }
    result->sig.length = i;
    if (digits[0] == 0)
        return;
    if (digits[0] < 5)
        return;
    if (digits[0] <= 5) {
        c = digits[1];
        digits++;
        while (c != 0) {
            if (c != 0x30)
                goto docall;
            c = *++digits;
        }
        if ((result->sig.text[i - 1] & 1) == 0)
            return;
    }
docall:
    __dorounddecup(result, result->sig.length);
}

/* MSL __two_exp: 2^n as a Decimal. n = -64, -53, -32, -16 and -8..8 come
   straight from a digit string and exponent ("54210108..." with exponent -20
   is 2^-64), built by __str2dec.
   Any other n is 2^(n/2) squared (__timesdec, the Decimal multiply),
   times one more 2 or 0.5 when n is odd. __num2dec_internal
   calls it to scale the binary exponent. */
void __two_exp(Decimal *out, int n) {
    Decimal half;
    Decimal copy;

    switch (n) {
    case -64:
        __str2dec(out, "542101086242752217003726400434970855712890625", -20);
        return;
    case -53:
        __str2dec(out, "11102230246251565404236316680908203125", -16);
        return;
    case -32:
        __str2dec(out, "23283064365386962890625", -10);
        return;
    case -16:
        __str2dec(out, "152587890625", -5);
        return;
    case -8:
        __str2dec(out, "390625", -3);
        return;
    case -7:
        __str2dec(out, "78125", -3);
        return;
    case -6:
        __str2dec(out, "15625", -2);
        return;
    case -5:
        __str2dec(out, "3125", -2);
        return;
    case -4:
        __str2dec(out, "625", -2);
        return;
    case -3:
        __str2dec(out, "125", -1);
        return;
    case -2:
        __str2dec(out, "25", -1);
        return;
    case -1:
        __str2dec(out, "5", -1);
        return;
    case 0:
        __str2dec(out, "1", 0);
        return;
    case 1:
        __str2dec(out, "2", 0);
        return;
    case 2:
        __str2dec(out, "4", 0);
        return;
    case 3:
        __str2dec(out, "8", 0);
        return;
    case 4:
        __str2dec(out, "16", 1);
        return;
    case 5:
        __str2dec(out, "32", 1);
        return;
    case 6:
        __str2dec(out, "64", 1);
        return;
    case 7:
        __str2dec(out, "128", 2);
        return;
    case 8:
        __str2dec(out, "256", 2);
        return;
    default:
        __two_exp(&half, (int)(n + ((n & 0x80000000u) >> 31)) >> 1);
        __timesdec(out, &half, &half);
        if (n & 1) {
            copy = *out;
            if (n > 0)
                __str2dec(&half, "2", 0);
            else
                __str2dec(&half, "5", -1);
            __timesdec(out, &copy, &half);
        }
        return;
    }
}

union DblWords {
    double d;
    int w[2];
};

void __num2dec_internal(Decimal *d, double x) {
    int e;
    union DblWords t1;
    union DblWords t2;
    double mm;
    Decimal dec;
    Decimal big;
    double v = x;
    unsigned char neg = (((int *)&x)[1] & 0x80000000) != 0;
    int cls;
    int cls2;

    if (v == 0.0) {
        d->sign = neg;
        d->exp = 0;
        d->sig.length = 1;
        d->sig.text[0] = 0;
        return;
    }

    t1.d = v;

    /* fpclassify, open-coded: an inline helper's temporaries would take
       frame slots after mm and reorder the stack. */
    switch (t1.w[1] & 0x7ff00000) {
    case 0x7ff00000:
        if ((t1.w[1] & 0xfffff) != 0 || t1.w[0] != 0)
            cls = 1;
        else
            cls = 2;
        break;
    case 0:
        if ((t1.w[1] & 0xfffff) != 0 || t1.w[0] != 0)
            cls = 5;
        else
            cls = 3;
        break;
    default:
        cls = 4;
        break;
    }
    if (cls <= 2) {
        d->sign = neg;
        d->exp = 0;
        d->sig.length = 1;

        t2.d = x;
        switch (t2.w[1] & 0x7ff00000) {
        case 0x7ff00000:
            if ((t2.w[1] & 0xfffff) != 0 || t2.w[0] != 0)
                cls2 = 1;
            else
                cls2 = 2;
            break;
        case 0:
            if ((t2.w[1] & 0xfffff) != 0 || t2.w[0] != 0)
                cls2 = 5;
            else
                cls2 = 3;
            break;
        default:
            cls2 = 4;
            break;
        }
        d->sig.text[0] = (cls2 == 1) ? 0x4e : 0x49;
    } else {
        double m;
        int nbits, k;
        int *mp;

        if (neg)
            x = -v;
        m = frexp(x, &e);
        mm = m;
        mp = (int *)&mm;
        if (mp[0] != 0)
            nbits = func_02071708(mp[0]);
        else
            nbits = func_02071708(mp[1] | 0x100000) + 0x20;
        k = 0x35 - nbits;
        __two_exp(&big, e - k);
        __ull2dec(&dec, ldexp(m, k));
        __timesdec(d, &dec, &big);
        d->sign = neg;
    }
}

/* MSL __num2dec: convert x to a Decimal of at most f->digits significant
 * digits (capped at 32), then turn the digit values into ASCII. decform is
 * MSL's layout, as in float2str.c, which calls this.
 */
void __num2dec(void *a0, double x, void *d) {
    int n;
    Decimal *c = (Decimal *)d;
    int i;
    n = ((decform *)a0)->digits;
    __num2dec_internal(c, x);
    if (c->sig.text[0] > 9) /* inf or nan: no digits to round */
        return;
    if (n > 0x20)
        n = 0x20;
    __rounddec(c, n);
    while (c->sig.length < n) {
        c->sig.text[c->sig.length++] = 0;
    }
    c->exp -= c->sig.length - 1;
    for (i = 0; i < c->sig.length; i++) {
        c->sig.text[i] += '0';
    }
}
