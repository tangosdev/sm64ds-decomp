extern int func_02049018(int *p);
extern void func_0204f924(void *self, int v);
extern void func_0204f7fc(void *self, int a, int b);
extern void func_0204f7cc(void *self, int a, int b);
extern void func_0204fa2c(void *self, int v);

extern unsigned int data_02099fac;
extern unsigned int data_02099fa8;
extern unsigned short data_02099fa4;

void func_02048af4(int *self, int *p, int q) {
    int a = *p >> 12;
    int b = q >> 12;
    unsigned int n, lo, hi;
    int t;
    if (self == 0) return;
    if (*self == 0) return;
    n = func_02049018(p);
    hi = data_02099fac;
    if (n < hi) {
        lo = data_02099fa8;
        if (lo > n) {
            t = 0x7f;
        } else {
            t = (hi - n) * 0x7f / (hi - lo);
        }
        if (b < 0) {
            t = 0x3f;
        } else if (b <= 0x28) {
            t = t * (b * 64 / 40 + 0x3f) / 0x7f;
        }
        func_0204f924(self, t);
        func_0204f7fc(self, 3, (b - 0x28) * 2);
        a = a / 2;
        if (a > 0x3f) {
            a = 0x3f;
        } else if (a < -0x40) {
            a = -0x40;
        }
        func_0204f7cc(self, data_02099fa4, a);
        return;
    }
    func_0204fa2c(self, 0);
}
