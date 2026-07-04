//cpp
typedef unsigned short u16;
extern "C" {
int IPCSend(unsigned int a, unsigned int c, unsigned int b);
}
namespace IRQ { unsigned int Disable(); void Restore(unsigned int); }

struct S {
    void (*fn)(int, int, int);
    char pad4[0xc - 4];
    u16 fc;
    u16 fe;
    int f10;
    u16 f14;
    char pad16[0x34 - 0x16];
    volatile u16 f34;
    volatile u16 f36;
};
extern S data_020a80cc;

#pragma opt_strength_reduction off
extern "C" void func_0205eeac(unsigned int a, unsigned int b, int x, unsigned int count)
{
    unsigned int i;
    unsigned int irq;
    int ok;

    data_020a80cc.f10 = x;
    data_020a80cc.fc = 0;
    data_020a80cc.fe = (u16)b;
    data_020a80cc.f14 = (u16)count;
    for (i = 0; i < count; i++) {
        *(u16 *)(data_020a80cc.f10 + (i << 3) + 4) = 0;
    }
    irq = IRQ::Disable();
    if (IPCSend(6, (b & 0xff) | 0x2000100, 0) < 0)
        ok = 0;
    else if (IPCSend(6, a | 0x1010000, 0) < 0)
        ok = 0;
    else
        ok = 1;
    if ((ok & 0xff) == 0) {
        IRQ::Restore(irq);
        data_020a80cc.f34 |= 2;
        if (data_020a80cc.fn != 0)
            data_020a80cc.fn(1, 4, 0);
        return;
    }
    data_020a80cc.f36 |= 2;
    data_020a80cc.f34 &= ~2;
    IRQ::Restore(irq);
}
