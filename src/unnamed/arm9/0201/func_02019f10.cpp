//cpp
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/os/thread.h"
#include "nitro/spi/pm.h"
#include "nitro/pxi/fifo.h"

#define FALSE 0

extern "C" int func_02040714(void);
extern "C" void func_02040724(void);
extern "C" void func_0201a4d0(void);
extern unsigned char data_0209d4e4;

void func_0201a028(void) {
    for (;;) {
        OS_DisableInterrupts();
        OS_Halt();
    }
}

void func_02019ff4(void) {
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_CTRDG, 2, FALSE) != PXI_FIFO_SUCCESS) {
    }
    func_0201a028();
}

void func_02019f64(void *arg) {
    int r;
    if (data_0209d4e4 != 0) {
        PM_ForceToPowerOff();
        func_02019ff4();
        return;
    }
    PMi_SendLEDPatternCommand(PM_LED_PATTERN_BLINK_HIGH);
    PMi_SetLED(PM_LED_ON);
    r = func_02040714();
    if (r != 0 && r != 1) {
        do {
            func_02040724();
            func_0201a4d0();
            r = func_02040714();
            if (r == 0)
                break;
        } while (r != 1);
    }
    PMi_SendLEDPatternCommand(PM_LED_PATTERN_ON);
    PMi_SetLED(PM_LED_ON);
    func_02019ff4();
}

/* The thread and its stack reuse the 0x4000-byte sin/cos table
   FX_SinCosTable_ (0x02082214..0x02086214), which ends where FX_AtanTable_
   begins: the OSThread takes the table's top 0x94 bytes and its
   0x3f6c-byte stack runs down from there to the table's start. */
extern "C" int func_02019f10(void) {
    OSThread *thread = (OSThread *)FX_AtanTable_ - 1;
    OS_CreateThread(thread, func_02019f64, 0, thread, 0x3f6c, 0);
    OS_WakeupThreadDirect(thread);
    return 0;
}
