//cpp
extern "C" {
typedef struct {
    int busy;
    int p0;
    int p1;
    int p2;
    void (*cb)(int);
    int arg;
    int oldmode;
    void *oldhandler;
} Data0205;
extern Data0205 data_020a6460;
extern void func_02059fa8(int ch);
extern int _ZN3IRQ7DisableEv(void);
extern void *_ZN3IRQ13GetIRQHandlerEj(unsigned int x);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned int x, void (*h)(void));
extern void _ZN3IRQ10EnableIRQsEj(unsigned int x);
extern void _ZN3IRQ7RestoreEj(int saved);
extern void func_0205a290(void);

void func_0205a358(int a0, int a1, int a2, void (*a3)(int), int a4) {
    int saved;
    if (a2 == 0) {
        if (a3 != 0) a3(a4);
        return;
    }
    while (data_020a6460.busy != 0) ;
    while (((*(volatile unsigned int *)0x4000600 & 0x7000000) >> 0x18 & 2) == 0) ;
    data_020a6460.busy = 1;
    data_020a6460.p0 = a0;
    data_020a6460.p1 = a1;
    data_020a6460.p2 = a2;
    data_020a6460.cb = a3;
    data_020a6460.arg = a4;
    func_02059fa8(a0);
    saved = _ZN3IRQ7DisableEv();
    data_020a6460.oldmode = (*(volatile unsigned int *)0x4000600 & 0xc0000000) >> 0x1e;
    data_020a6460.oldhandler = _ZN3IRQ13GetIRQHandlerEj(0x200000);
    *(volatile unsigned int *)0x4000600 = (*(volatile unsigned int *)0x4000600 & ~0xc0000000) | 0x40000000;
    _ZN3IRQ13SetIRQHandlerEjPFvvE(0x200000, func_0205a290);
    _ZN3IRQ10EnableIRQsEj(0x200000);
    func_0205a290();
    _ZN3IRQ7RestoreEj(saved);
}
}
