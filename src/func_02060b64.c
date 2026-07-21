typedef unsigned int u32;

struct G {
    char pad[0x18];
    u32 f18;
    u32 f1c;
    u32 f20;
    u32 f24;
};

extern struct G data_020a8180;
extern char func_01ff8000;
extern void func_02060cac(void);
extern void func_02060d1c(void);
extern u32 _ZN4CP1518GetDTCMBaseAddressEv(void);
extern int _ZN3IRQ7DisableEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN4CP1526InvalidateInstructionCacheEjj(u32 a, u32 b);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(u32 n, void (*h)(void));
extern void _ZN3IRQ15ClearInterruptsEj(u32 n);
extern void _ZN3IRQ10EnableIRQsEj(u32 n);
extern void _ZN4CP1516DrainWriteBufferEv(void);
extern void _ZN3IRQ7RestoreEj(int n);

#pragma opt_propagation off
int func_02060b64(int *self)
{
    int *p;
    struct G *g;
    u32 dtcm;
    int r7;
    int r6;
    int r5;
    u32 base;
    int aligned;
    u32 d;
    u32 end;
    int cond1;
    int saved;

    p = self;
    g = &data_020a8180;
    r7 = 0;
    r6 = 0;
    r5 = 0;
    aligned = 0;

    base = g->f1c;

    if ((base & 3) == 0) {
        if (g->f24 <= 3) aligned = 1;
    }

    if (aligned != 0) {
        int bad;
        int reject;
        u32 e2;
        dtcm = g->f20;
        d = _ZN4CP1518GetDTCMBaseAddressEv();
        e2 = base + dtcm;
        bad = 1;
        reject = 0;

        if (e2 > (u32)&func_01ff8000) {
            if (base < 0x2000000) reject = bad;
        }

        if (reject == 0) {
            if (d >= e2) goto set0;
            if (d + 0x4000 > base) goto tail;
        set0:
            bad = 0;
        }
    tail:

        if (bad == 0) r5 = 1;
    }

    if (r5 != 0) {
        if (((g->f18 | g->f20) & 0x1ff) == 0) r6 = 1;
    }

    if (r6 != 0) {
        if (g->f20 != 0) r7 = 1;
    }

    p[1] = (*(int *)0x27ffe60 & ~0x7000000) | 0xa1000000;

    if (r7 != 0) {
        saved = _ZN3IRQ7DisableEv();
        _ZN4CP1527FlushAndInvalidateDataCacheEjj(g->f1c, g->f20);
        _ZN4CP1526InvalidateInstructionCacheEjj(g->f1c, g->f20);
        _ZN3IRQ13SetIRQHandlerEjPFvvE(0x80000, func_02060cac);
        _ZN3IRQ15ClearInterruptsEj(0x80000);
        _ZN3IRQ10EnableIRQsEj(0x80000);
        _ZN4CP1516DrainWriteBufferEv();
        _ZN3IRQ7RestoreEj(saved);
        func_02060d1c();
    }

    return r7;
}
