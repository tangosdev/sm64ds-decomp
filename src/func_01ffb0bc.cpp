//cpp
// @symbol func_01ffb0bc
/* MeshCollider, 0x01ffb0bc (ITCM). Sets the 0x34 flag; func_01ffb0b0 clears it.
 * extern "C": the symbol is unmangled in config/arm9/symbols.txt.
 */
#include "MeshCollider.h"

extern "C" void func_01ffb0bc(MeshCollider *self)
{
    self->unk_34 = 1;
}
