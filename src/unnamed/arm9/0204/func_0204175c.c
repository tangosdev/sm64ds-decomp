#include "nitro/hw/registers.h"

/* Globals */
extern void (*data_0209a03c)(void *);
extern void *data_02099e24;
extern u32 data_020a0f38;

#define gGameCardHandlerPtr data_0209a03c
#define gGameCardHandlerArg data_02099e24
#define gGameCardSentinel   data_020a0f38

extern u32  _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern void _ZN3IRQ10EnableIRQsEj(u32 irqBits);

void func_0204175c(void)
{
    u32 state;
    u32 ie;
    u32 iflags;

    state = _ZN3IRQ7DisableEv();
    ie     = REG_IE;
    iflags = REG_IF;

    if ((iflags & IRQ_GAME_CARD) && !(ie & IRQ_GAME_CARD))
    {
        _ZN3IRQ10EnableIRQsEj(IRQ_GAME_CARD);
        gGameCardHandlerPtr(&gGameCardHandlerArg);
        gGameCardSentinel = 0x80000000;
    }

    _ZN3IRQ7RestoreEj(state);
}
