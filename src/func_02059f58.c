extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int savedState);

void func_02059f58(int channel)
{
    unsigned int saved = _ZN3IRQ7DisableEv();
    int idx = channel * 6 + 5;
    volatile unsigned short *reg = (volatile unsigned short *)(((long long)(int)(0x40000b0 + idx * 2)) & 0xFFFFFFFFFFFFFFFFLL);
    *reg &= ~0x3a00;
    *reg &= ~0x8000;
    (void)*reg;
    (void)*reg;
    _ZN3IRQ7RestoreEj(saved);
}
