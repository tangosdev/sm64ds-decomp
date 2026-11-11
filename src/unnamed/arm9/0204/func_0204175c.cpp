//cpp
#include "nitro/hw/registers.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"

extern "C" {

/* Globals */
extern void (*data_0209a03c)(void *);
extern void *data_02099e24;
extern int data_020a0f38;

#define gGameCardHandlerPtr data_0209a03c
#define gGameCardHandlerArg data_02099e24
#define gGameCardSentinel   data_020a0f38


void func_0204175c(void)
{
    OSIntrMode state;
    u32 ie;
    u32 iflags;

    state = OS_DisableInterrupts();
    ie     = REG_IE;
    iflags = REG_IF;

    if ((iflags & OS_IE_CARD_DATA) && !(ie & OS_IE_CARD_DATA))
    {
        OS_EnableIrqMask(OS_IE_CARD_DATA);
        gGameCardHandlerPtr(&gGameCardHandlerArg);
        gGameCardSentinel = 0x80000000;
    }

    OS_RestoreInterrupts(state);
}
}
