extern int func_01ff9d40(double, double);
extern double func_01ff8708(double, double);
extern double func_01ff8000(double, double);
extern double func_020707a4(double, double);

static inline int fpclassify_(double x) {
    union { double d; int w[2]; } u;
    int cls;
    u.d = x;
    if ((u.w[1] & 0x7ff00000) != 0) {
        if ((u.w[1] & 0x7ff00000) == 0x7ff00000) {
            if ((u.w[1] & 0xfffff) != 0) goto nan;
            if (u.w[0] == 0) goto inf;
        nan:
            cls = 1; goto done;
        inf:
            cls = 2; goto done;
        }
    } else {
        if ((u.w[1] & 0xfffff) != 0) goto sub;
        if (u.w[0] == 0) goto zero;
    sub:
        cls = 5; goto done;
    zero:
        cls = 3; goto done;
    }
    cls = 4;
done:
    return cls;
}

double func_020708b4(double x, int n) {
    int hx, lx, k;
    if (fpclassify_(x) <= 2 || x == 0.0) return x;
    hx = ((int*)&x)[1];
    lx = ((int*)&x)[0];
    k = (hx & 0x7ff00000) >> 20;
    if (k == 0) {
        if ((lx | (hx & 0x7fffffff)) == 0) return x;
        x = x * 1.80143985094819840000e+16;
        hx = ((int*)&x)[1];
        k = ((hx & 0x7ff00000) >> 20) - 54;
        if (n < -50000) return 1.0e-300 * x;
    }
    if (k == 0x7ff) return x + x;
    k = k + n;
    if (k > 0x7fe) return 1.0e+300 * func_020707a4(1.0e+300, x);
    if (k > 0) {
        ((int*)&x)[1] = (hx & 0x800fffff) | (k << 20);
        return x;
    }
    if (k <= -54) {
        if (n > 50000) return 1.0e+300 * func_020707a4(1.0e+300, x);
        else return 1.0e-300 * func_020707a4(1.0e-300, x);
    }
    k += 54;
    ((int*)&x)[1] = (hx & 0x800fffff) | (k << 20);
    return 5.55111512312578270212e-17 * x;
}
