//cpp
#include "types.h"
extern "C" {
extern char data_020a8180[];
void OS_SleepThread(void *p);
}

u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);

extern "C" void func_020602bc(void) {
    char *base = data_020a8180;
    for (;;) {
        u32 st = _ZN3IRQ7DisableEv();
        for (;;) {
            if (*(u32 *)(base + 0x34) & 8) break;
            *(void **)(base + 0xd0) = base + 0x3c;
            OS_SleepThread(0);
        }
        _ZN3IRQ7RestoreEj(st);
        (*(void (**)(void *))(base + 0x30))(base);
    }
}
