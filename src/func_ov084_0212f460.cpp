//cpp
#include "types.h"
extern "C" {
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned, unsigned, unsigned, int, int);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32, u32, int, int, int, const void *, void *);
extern int _ZN9Animation8FinishedEv(void *);
extern s16 data_02082214[];
extern int data_ov084_0213030c[];
extern int data_ov084_02130334[];
void func_ov084_0212f460(void *self);
}

void func_ov084_0212f460(void *self)
{
    char *c = (char *)self;
    int space[3];
    int zero;

    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x36, 0x7f, 0, 0xcb33, 0);
    /* `fp` is assigned BEFORE `zero`, and the test reads through it. Under 2004/b56
       the callee-saved registers are handed out in assignment order, so this is what
       puts fp in r7 and zero in r6 as the ROM has them; with `zero = 0` first they come
       out swapped and nothing else moves. The ROM emits `mov r6,#0; strb` before
       `add r7,r4,#0x100` regardless -- the store is independent, so it schedules ahead
       of the address computation that feeds the compare. */
    u16 *fp = (u16 *)(c + 0x100);
    zero = 0;
    *(unsigned char *)(c + 0x45c) = zero;
    if (*fp < 0xa) {
        int x = *(int *)(c + 0x5c);
        s16 *sintbl = data_02082214;
        *(volatile int *)&space[0] = x;
        int y = *(int *)(c + 0x60);
        int *xtbl = data_ov084_0213030c;
        *(volatile int *)&space[1] = y;
        int z = *(int *)(c + 0x64);
        int *ytbl = data_ov084_02130334;
        *(volatile int *)&space[2] = z;

        x = xtbl[*fp] * sintbl[(*(u16 *)(c + 0x8e) >> 4) * 2] + x;
        *(volatile int *)&space[0] = x;
        z = xtbl[*fp] * sintbl[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1] + z;
        *(volatile int *)&space[2] = z;
        y = y + (ytbl[*fp] << 12);
        *(volatile int *)&space[1] = y;

        u32 slot = *(u32 *)(c + 0x470);
        int px = space[0];
        int py = space[1];
        *(void **)(c + 0x470) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            slot, 0xfb, px, py, z, 0, 0);
    }
    if (_ZN9Animation8FinishedEv(c + 0x160) == 0)
        return;
    *(int *)(c + 0x458) = 6;
    *(int *)(c + 0x474) = 0;
    *(void **)(c + 0x470) = *(void **)(c + 0x474);
}
