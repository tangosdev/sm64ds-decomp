//cpp
#include "nitro/hw/registers.h"
#include "IRQ.h"

extern "C" {

/* Globals */
extern void (*data_0209a03c)(void *);
extern void *data_02099e24;
extern u32 data_020a0f38;

#define gGameCardHandlerPtr data_0209a03c
#define gGameCardHandlerArg data_02099e24
#define gGameCardSentinel   data_020a0f38


void func_0204175c(void)
{
    u32 state;
    u32 ie;
    u32 iflags;

    state = IRQ::Disable();
    ie     = REG_IE;
    iflags = REG_IF;

    if ((iflags & IRQ_GAME_CARD) && !(ie & IRQ_GAME_CARD))
    {
        IRQ::EnableIRQs(IRQ_GAME_CARD);
        gGameCardHandlerPtr(&gGameCardHandlerArg);
        gGameCardSentinel = 0x80000000;
    }

    IRQ::Restore(state);
}
}
