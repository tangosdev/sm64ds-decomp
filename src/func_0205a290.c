typedef unsigned int u32;
typedef void (*IrqFn)(int);

struct G6460 {
    IrqFn f0;        /* 0x0 */
    int f4;          /* 0x4 */
    int f8;          /* 0x8 */
    int fc;          /* 0xc */
    IrqFn f10;       /* 0x10 */
    int f14;         /* 0x14 */
    int f18;         /* 0x18 */
    void *f1c;       /* 0x1c */
};
extern struct G6460 data_020a6460;

extern void func_0205a21c(void);
extern void func_02056e98(int a, void *cb, int c);
extern void DMAStartTransfer(int src, int dst, int ctrl0, int ctrl1);
extern void _ZN3IRQ15ClearInterruptsEj(unsigned int mask);

void func_0205a290(void)
{
    int base;
    int rem;
    unsigned int len;
    if (data_020a6460.fc == 0) return;
    len = data_020a6460.fc;
    if (len >= 0x1d8) len = 0x1d8;
    base = data_020a6460.f8;
    rem = *(volatile int*)&data_020a6460.fc - len;
    data_020a6460.fc = rem;
    data_020a6460.f8 = base + len;
    if (rem == 0) {
        func_02056e98(data_020a6460.f4, func_0205a21c, 0);
        DMAStartTransfer(data_020a6460.f4, base, 0x4000400, -0x3bc00000 | (len >> 2));
        _ZN3IRQ15ClearInterruptsEj(0x200000);
    } else {
        DMAStartTransfer(data_020a6460.f4, base, 0x4000400, 0x84400000 | (len >> 2));
        _ZN3IRQ15ClearInterruptsEj(0x200000);
    }
}
