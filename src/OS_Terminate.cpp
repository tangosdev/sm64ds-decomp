//cpp
#include "types.h"
#include "nitro/os/system.h"

extern "C" {


/* OS_Terminate (0x02019740): the stop every failed check ends in.
 *
 * The name is the link's. NitroSDK library code -- CARD, CTRDG, FS, OS_Alarm,
 * OS_Reset, TP, WM -- calls this function where a FINALROM build expands
 * OS_Panic to OS_Terminate() (NitroSDK/include/nitro/os/common/printf.h), and those
 * objects can only have reached it by that name. The body is more than
 * os_terminate_proc.c's halt loop: it runs the exception handler once
 * (data_0209d4d8 guards it) before halting with interrupts off, and it sits
 * in the game's code rather than the SDK's, so this is the game's own
 * OS_Terminate standing in for the library's. */
/* local extern: the exception handler (an OSExceptionHandler); no header
   declares it. */
extern void func_0201a03c(u32 context, void *arg);
}


extern u8 data_0209d4d8;
 // 0x0209d4d8

void OS_Terminate(void) {
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
