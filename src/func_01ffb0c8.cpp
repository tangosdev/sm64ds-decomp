//cpp
// @symbol func_01ffb0c8
/* MeshCollider::GetFile() at 0x01ffb0c8 (ITCM). Unnamed in symbols.txt, so it keeps
 * its func_ name -- and therefore needs C linkage, since the symbol is unmangled.
 */
#include "MeshCollider.h"

extern "C" KCL_File *func_01ffb0c8(MeshCollider *self)
{
    return self->kclFile;
}
