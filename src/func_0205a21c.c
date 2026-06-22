typedef unsigned int u32;

extern void _ZN3IRQ11DisableIRQsEj(u32 mask);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(u32 mask, void *handler);

typedef void (*IrqFn)(int);

struct G6460 {
    IrqFn f0;        /* 0x0 */
    char pad4[0x10 - 0x4];
    IrqFn f10;       /* 0x10 */
    int f14;         /* 0x14 */
    int f18;         /* 0x18 */
    void *f1c;       /* 0x1c */
};
extern struct G6460 data_020a6460;

void func_0205a21c(void)
{
    IrqFn h;
    int arg;
    _ZN3IRQ11DisableIRQsEj(0x200000);
    *(volatile u32*)0x4000600 = (data_020a6460.f18 << 30) | (*(volatile u32*)0x4000600 & ~0xc0000000u);
    _ZN3IRQ13SetIRQHandlerEjPFvvE(0x200000, data_020a6460.f1c);
    data_020a6460.f0 = 0;
    h = data_020a6460.f10;
    arg = data_020a6460.f14;
    if (h != 0)
        h(arg);
}
