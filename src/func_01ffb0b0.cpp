//cpp
// @symbol func_01ffb0b0
/* dBgW_Kc, 0x01ffb0b0 (ITCM). Clears the 0x34 flag; func_01ffb0bc sets it.
 * extern "C": the symbol is unmangled in config/arm9/symbols.txt.
 */
#include "dBgW_Kc.h"

extern "C" void func_01ffb0b0(dBgW_Kc *self)
{
    self->unk_34 = 0;
}
