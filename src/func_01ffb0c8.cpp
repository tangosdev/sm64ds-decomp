//cpp
// @symbol func_01ffb0c8
/* dBgW_Kc::GetFile() at 0x01ffb0c8 (ITCM). Unnamed in symbols.txt, so it keeps
 * its func_ name -- and therefore needs C linkage, since the symbol is unmangled.
 */
#include "dBgW_Kc.h"

extern "C" KCL_File *func_01ffb0c8(dBgW_Kc *self)
{
    return self->kclFile;
}
