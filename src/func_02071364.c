typedef struct Decimal {
    unsigned char sign;
    char unused;
    short exp;
    struct {
        unsigned char length;
        unsigned char text[32];
        unsigned char unused;
    } sig;
} Decimal;

extern void func_02071644(Decimal *result, int length);

void func_02071364(Decimal *result, const Decimal *x, const Decimal *y)
{
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
        short *exp = (short *)(int)(((long long)(int)((char *)result + 2)) &
                                    0xffffffffffffffffLL);
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
                    goto round;
                }
            }
            if ((ip[-1] & 1) == 0) {
                return;
            }
        }
    round:
        func_02071644(result, result->sig.length);
    }
}
