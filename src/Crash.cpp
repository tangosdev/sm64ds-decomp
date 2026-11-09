//cpp
#include "types.h"
#include "nitro/os/system.h"

extern "C" {

// Crash function at 0x02019740
// IRQ disable, set crash flag, save registers and call crash screen, then loop forever
/* local extern: the exception handler (an OSExceptionHandler); no header
   declares it. */
extern void func_0201a03c(u32 context, void *arg);

extern u8 data_0209d4d8; // 0x0209d4d8

void Crash(void) {
    OS_DisableInterrupts();
    if (data_0209d4d8 == 0) {
        data_0209d4d8 = 1;
        func_0201a03c(0, 0);
    }
    for (;;) {
        OS_DisableInterrupts();
        OS_Halt();
    }
}
}
