//cpp
// @symbol func_01ffb098
/* MeshCollider, 0x01ffb098 (ITCM). Clears the 0x35 flag; func_01ffb0a4 sets it.
 * SetFile zeroes both 0x34 and 0x35 at construction.
 * extern "C": the symbol is unmangled in config/arm9/symbols.txt.
 */
#include "MeshCollider.h"

extern "C" void func_01ffb098(MeshCollider *self)
{
    self->unk_35 = 0;
}
