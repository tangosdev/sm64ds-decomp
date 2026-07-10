typedef unsigned short u16;
typedef unsigned int u32;

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 tok);
extern volatile u32 data_020a6438[2];

long long func_02059650(void)
{
    u32 tok = _ZN3IRQ7DisableEv();
    volatile u16 timer = *(volatile u16*)0x4000100;
    volatile unsigned long long counter = *(volatile unsigned long long*)data_020a6438 & 0xffffffffffffULL;
    if ((*(volatile u32*)0x4000214 & 8) && !(timer & 0x8000))
        counter = counter + 1;
    _ZN3IRQ7RestoreEj(tok);
    return ((long long)counter << 16) | (int)(((long long)(int)timer) & 0xFFFFFFFFFFFFFFFFLL);
}
