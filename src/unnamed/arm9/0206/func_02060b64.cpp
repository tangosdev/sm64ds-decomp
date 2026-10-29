//cpp
#include "types.h"
#include "CP15.h"
#include "IRQ.h"

extern "C" {

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
        d = CP15::GetDTCMBaseAddress();
        e2 = base + dtcm;
        bad = 1;
        reject = 0;

        if (e2 > (u32)&func_01ff8000) {
            if (base < 0x2000000) reject = bad;
        }

        if (reject == 0) {
            if (d >= e2 || d + 0x4000 <= base)
                bad = 0;
        }

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
        saved = IRQ::Disable();
        CP15::FlushAndInvalidateDataCache(g->f1c, g->f20);
        CP15::InvalidateInstructionCache(g->f1c, g->f20);
        IRQ::SetIRQHandler(0x80000, func_02060cac);
        IRQ::ClearInterrupts(0x80000);
        IRQ::EnableIRQs(0x80000);
        CP15::DrainWriteBuffer();
        IRQ::Restore(saved);
        func_02060d1c();
    }

    return r7;
}
}
