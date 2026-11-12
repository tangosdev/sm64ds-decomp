//cpp
/* OS_Init (the Nitro SDK's os_init.c). The call sequence is pret's, except
 * that this older SDK has no OS_SetIrqStackChecker or
 * OSi_InitVramExclusive step, and it calls MI_SetWramBank(MI_WRAM_ARM7_ALL)
 * directly where pret calls MI_Init. */
#include "nitro/os/spinLock.h"
#include "nitro/os/thread.h"
#include "nitro/os/reset.h"
#include "nitro/pxi/init.h"
#include "nitro/os/interrupt.h"
#include "nitro/spi/pm.h"

extern "C" {

extern void OS_InitArena(void);
extern void OS_InitArenaEx(void);
extern void OS_InitException(void);
extern void MI_SetWramBank(int cnt);
extern void OS_InitVAlarm(void);
extern void CTRDG_Init(void);
extern void CARD_Init(void);

void OS_Init(void) {
    OS_InitArena();
    PXI_Init();
    OS_InitLock();
    OS_InitArenaEx();
    OS_InitIrqTable();
    OS_InitException();
    MI_SetWramBank(3);
    OS_InitVAlarm();
    OS_InitThread();
    OS_InitReset();
    CTRDG_Init();
    CARD_Init();
    PM_Init();
}

} // extern "C"
