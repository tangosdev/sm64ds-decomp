//cpp
#include "IRQ.h"

extern "C" {

typedef unsigned short u16;
extern int func_020570b8(u16 *p);
extern int func_02057158(void *p);
extern void WaitByLoop(int n);

void func_0206a3a4(void *arg0, int *out)
{
    for (;;) {
        out[1] = IRQ::Disable();
        out[0] = func_020570b8((u16 *)0x27fffe8) & 0x40;
        if (out[0] != 0)
            return;
        if (func_02057158(arg0) == 0)
            return;
        IRQ::Restore(out[1]);
        WaitByLoop(1);
    }
}
}
