extern void func_0205abb8(int a, int b, int c, int d);
extern void *func_0205afb4(void);
extern void func_0205b070(int a);
extern void func_0205afe0(unsigned int c);
extern int func_020587e4(void *a, int b, int c);
extern void func_0204f11c(int mask);
extern void func_0204f15c(void *node);
extern int func_0204f0b8(unsigned int bit);
extern void func_0205a958(void *a, int b, int c, int d);

struct D {
    int f0;
    int f4;
    char p8[0x1c - 8];
    int f1c;
    int f20;
    int f24;
    int f28;
};

extern struct D data_020a5634;
extern int data_020a5600;

void func_020502b8(void)
{
    struct D *d = &data_020a5634;
    int has = (d->f28 >= 0);
    int mask = has ? (1 << d->f28) : 0;

    func_0205abb8(d->f20, d->f24, mask, 0);
    if (has) {
        void *x = func_0205afb4();
        func_0205b070(1);
        func_0205afe0((unsigned int)x);
        while (func_020587e4(&data_020a5600, 0, 0) != 0) {}
    }
    if (d->f24 != 0) func_0204f11c(d->f24);
    if (d->f1c != 0) func_0204f15c((void *)d->f1c);
    if (has) func_0204f0b8(d->f28);
    if (d->f4 == 1) func_0205a958(0, 0, 0, 0);
    d->f0 = 0;
}
