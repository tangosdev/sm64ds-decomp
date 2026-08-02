//cpp
// @symbol func_01ffb0a4
/* MeshCollider, 0x01ffb0a4 (ITCM). Sets the 0x35 flag; func_01ffb098 clears it.
 * extern "C": the symbol is unmangled in config/arm9/symbols.txt.
 */
#include "MeshCollider.h"

extern "C" void func_01ffb0a4(MeshCollider *self)
{
    self->unk_35 = 1;
}
