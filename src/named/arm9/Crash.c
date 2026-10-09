#include "types.h"
// Crash function at 0x02019740
// IRQ disable, set crash flag, save registers and call crash screen, then loop forever
extern void _ZN3IRQ7DisableEv(void);
extern void _ZN4CP1516WaitForInterruptEv(void);
extern void func_0201a03c(u32 r0, u32 r1);

extern u8 data_0209d4d8; // 0x0209d4d8

void Crash(void) {
    _ZN3IRQ7DisableEv();
    if (data_0209d4d8 == 0) {
        u32 r0 = 0;
        u32 r1 = 0;
        data_0209d4d8 = 1;
        func_0201a03c(r0, r1);
    }
    for (;;) {
        _ZN3IRQ7DisableEv();
        _ZN4CP1516WaitForInterruptEv();
    }
}
