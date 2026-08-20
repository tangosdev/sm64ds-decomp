//cpp
// @symbol func_01ffb0a4
/* dBgW_Kc, 0x01ffb0a4 (ITCM). Sets the 0x35 flag; func_01ffb098 clears it.
 * extern "C": the symbol is unmangled in config/arm9/symbols.txt.
 */
#include "dBgW_Kc.h"

extern "C" void func_01ffb0a4(dBgW_Kc *self)
{
    self->unk_35 = 1;
}
