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
 * file. TOP EDGE: func_02071784 begins where the run ends.
 *
 * The digit tables __two_exp reads (data_0209a628..data_0209a6f8) stay
 * extern: this fold claims .text only. */
#include "types.h"
#include "Decimal.h"

typedef struct {
    char style;
    char unused;
    short digits;
} decform;

extern int func_01ff9d40(double, double);
extern double func_01ff8e10(double, double);
extern double frexp(double, int *);
extern double ldexp(double, int);
extern unsigned long long func_01ff859c(double);

extern unsigned char data_0209a628[];
extern unsigned char data_0209a658[];
extern unsigned char data_0209a680[];
extern unsigned char data_0209a698[];
extern unsigned char data_0209a6a8[];
extern unsigned char data_0209a6b0[];
extern unsigned char data_0209a6b8[];
extern unsigned char data_0209a6c0[];
extern unsigned char data_0209a6c8[];
extern unsigned char data_0209a6cc[];
extern unsigned char data_0209a6d0[];
extern unsigned char data_0209a6d4[];
extern unsigned char data_0209a6d8[];
extern unsigned char data_0209a6dc[];
extern unsigned char data_0209a6e0[];
extern unsigned char data_0209a6e4[];
extern unsigned char data_0209a6e8[];
extern unsigned char data_0209a6ec[];
extern unsigned char data_0209a6f0[];
extern unsigned char data_0209a6f4[];
extern unsigned char data_0209a6f8[];

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
// @symbol func_02071708
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
        *p = (unsigned char)c;
        if ((*(volatile unsigned char *)p & 1) == 0)
            return count;
        do {
            count++;
            *p = (unsigned char)((int)*(volatile unsigned char *)p >> 1);
        } while (*(volatile unsigned char *)p & 1);
        return count;
    }
    return 0x20;
}

/* MSL __must_round for __rounddec: whether cutting d to
   digits digits rounds up. 1 when the first dropped digit is above 5, or is
   5 with a non-zero digit after it, or is exactly 5 after an odd last kept
   digit (half to even). Otherwise -1. */
// @symbol __must_round
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
// @symbol __dorounddecup
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
// @symbol __rounddec
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
// @symbol __ull2dec
void __ull2dec(Decimal *result, unsigned long long v) {
    result->sign = 0;
    result->sig.length = 0;
    if (v != 0) {
        do {
            result->sig.text[result->sig.length++] = (unsigned char)(v % 10);
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
    result->exp = (short)(result->sig.length - 1);
}

// @symbol __timesdec
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

        *--ip = (unsigned char)(accumulator % 10);
        accumulator /= 10;
    }

    result->exp = (short)(x->exp + y->exp);

    if (accumulator) {
        short *exp = (short *)(int)((char *)result + 2);
        *--ip = (unsigned char)accumulator;
        *exp = *exp + 1;
    }

    for (i = 0; i < 32 && ip < ep; i++, ip++) {
        result->sig.text[i] = *ip;
    }
    result->sig.length = (unsigned char)i;

    if (ip < ep && *ip >= 5) {
        if (*ip == 5) {
            unsigned char *jp = ip + 1;
            for (; jp < ep; jp++) {
                if (*jp != 0) {
                    /* kept, as in MSL's timesdec: break + a jp test (0x1b4) or an
                       inline predicate (0x1bc) both grow it */
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

// @symbol __str2dec
void __str2dec(Decimal *result, unsigned char *digits, short exp) {
    int i;
    unsigned char c;
    result->exp = exp;
    result->sign = 0;
    i = 0;
    while (i < 0x20 && digits[0] != 0) {
        result->sig.text[i++] = (unsigned char)(*digits++ - 0x30);
    }
    result->sig.length = (unsigned char)i;
    if (digits[0] == 0)
        return;
    if (digits[0] < 5)
        return;
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
    __dorounddecup(result, result->sig.length);
}

/* MSL __two_exp: 2^n as a Decimal. n = -64, -53, -32, -16 and -8..8 come
   from the digit strings at data_0209a628..data_0209a6f8 (data_0209a628 is
   "54210108..." with exponent -20, i.e. 2^-64), built by __str2dec.
   Any other n is 2^(n/2) squared (__timesdec, the Decimal multiply),
   times one more 2 or 0.5 when n is odd. __num2dec_internal
   calls it to scale the binary exponent. */
// @symbol __two_exp
void __two_exp(Decimal *out, int n) {
    Decimal half;
    Decimal copy;

    switch (n) {
    case -64:
        __str2dec(out, data_0209a628, -20);
        return;
    case -53:
        __str2dec(out, data_0209a658, -16);
        return;
    case -32:
        __str2dec(out, data_0209a680, -10);
        return;
    case -16:
        __str2dec(out, data_0209a698, -5);
        return;
    case -8:
        __str2dec(out, data_0209a6a8, -3);
        return;
    case -7:
        __str2dec(out, data_0209a6b0, -3);
        return;
    case -6:
        __str2dec(out, data_0209a6b8, -2);
        return;
    case -5:
        __str2dec(out, data_0209a6c0, -2);
        return;
    case -4:
        __str2dec(out, data_0209a6c8, -2);
        return;
    case -3:
        __str2dec(out, data_0209a6cc, -1);
        return;
    case -2:
        __str2dec(out, data_0209a6d0, -1);
        return;
    case -1:
        __str2dec(out, data_0209a6d4, -1);
        return;
    case 0:
        __str2dec(out, data_0209a6d8, 0);
        return;
    case 1:
        __str2dec(out, data_0209a6dc, 0);
        return;
    case 2:
        __str2dec(out, data_0209a6e0, 0);
        return;
    case 3:
        __str2dec(out, data_0209a6e4, 0);
        return;
    case 4:
        __str2dec(out, data_0209a6e8, 1);
        return;
    case 5:
        __str2dec(out, data_0209a6ec, 1);
        return;
    case 6:
        __str2dec(out, data_0209a6f0, 1);
        return;
    case 7:
        __str2dec(out, data_0209a6f4, 2);
        return;
    case 8:
        __str2dec(out, data_0209a6f8, 2);
        return;
    default:
        __two_exp(&half, (int)(n + ((n & 0x80000000u) >> 31)) >> 1);
        __timesdec(out, &half, &half);
        if (n & 1) {
            copy = *out;
            if (n > 0)
                __str2dec(&half, data_0209a6dc, 0);
            else
                __str2dec(&half, data_0209a6d4, -1);
            __timesdec(out, &copy, &half);
        }
        return;
    }
}

// @symbol __num2dec_internal
void __num2dec_internal(char *out, double x) {
    int e;
    double t1;
    double t2;
    double mm;
    unsigned char dec[0x26];
    unsigned char big[0x26];
    double v = x;
    unsigned char neg = (((int *)&x)[1] & 0x80000000) != 0;
    int cls;
    int cls2;

    if (func_01ff9d40(0.0, v) != 0) {
        out[0] = neg;
        *(short *)(out + 2) = 0;
        out[4] = 1;
        out[5] = 0;
        return;
    }

    t1 = v;

    /* fpclassify, open-coded: an inline helper's temporaries would take
       frame slots after mm and reorder the stack. */
    switch (((int *)&t1)[1] & 0x7ff00000) {
    case 0x7ff00000:
        if ((((int *)&t1)[1] & 0xfffff) != 0 || ((int *)&t1)[0] != 0)
            cls = 1;
        else
            cls = 2;
        break;
    case 0:
        if ((((int *)&t1)[1] & 0xfffff) != 0 || ((int *)&t1)[0] != 0)
            cls = 5;
        else
            cls = 3;
        break;
    default:
        cls = 4;
        break;
    }
    if (cls <= 2) {
        out[0] = neg;
        *(short *)(out + 2) = 0;
        out[4] = 1;

        t2 = x;
        switch (((int *)&t2)[1] & 0x7ff00000) {
        case 0x7ff00000:
            if ((((int *)&t2)[1] & 0xfffff) != 0 || ((int *)&t2)[0] != 0)
                cls2 = 1;
            else
                cls2 = 2;
            break;
        case 0:
            if ((((int *)&t2)[1] & 0xfffff) != 0 || ((int *)&t2)[0] != 0)
                cls2 = 5;
            else
                cls2 = 3;
            break;
        default:
            cls2 = 4;
            break;
        }
        out[5] = (cls2 == 1) ? 0x4e : 0x49;
    } else {
        double m;
        int nbits, k;
        int *mp;

        if (neg)
            x = func_01ff8e10(0.0, v);
        m = frexp(x, &e);
        mm = m;
        mp = (int *)&mm;
        if (mp[0] != 0)
            nbits = func_02071708(mp[0]);
        else
            nbits = func_02071708(mp[1] | 0x100000) + 0x20;
        k = 0x35 - nbits;
        __two_exp((Decimal *)big, e - k);
        __ull2dec((Decimal *)dec, func_01ff859c(ldexp(m, k)));
        __timesdec((Decimal *)out, (Decimal *)dec, (Decimal *)big);
        out[0] = neg;
    }
}

/* MSL __num2dec: convert x to a Decimal of at most f->digits significant
 * digits (capped at 32), then turn the digit values into ASCII. decform is
 * MSL's layout, as in float2str.c, which calls this. The pragma covers only
 * this function: it is the last one in the file. */
// @symbol __num2dec
#pragma opt_strength_reduction off
void __num2dec(void *a0, double x, void *d) {
    int n;
    Decimal *c = (Decimal *)d;
    int i;
    n = ((decform *)a0)->digits;
    __num2dec_internal((char *)c, x);
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
