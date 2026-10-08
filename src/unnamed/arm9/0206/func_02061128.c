// @symbol func_02061128
/* recovered: setter for the IRQ game-card MC interrupt callback stored at
   data_020a89a4 (read back by IRQ::GameCardIREQMCHandler, see
   src/_ZN3IRQ21GameCardIREQMCHandlerEv.cpp), arm9 0x02061128, 0x10 bytes. */
#include "types.h"

typedef int (*fp)(void);
extern fp data_020a89a4;

void func_02061128(fp cb)
{
    data_020a89a4 = cb;
}
