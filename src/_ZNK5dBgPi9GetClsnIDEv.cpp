//cpp
#include "types.h"
// @symbol _ZNK5dBgPi9GetClsnIDEv
/* recovered: named members + shared header, real C++ method */
#include "dBgPi.h"
/* dBgPi::GetClsnID() const at 0x02037f4c
 * Loads the u32 field at 0x1c (objID in the reference layout).
 */
struct dBgPi;

u32 dBgPi::GetClsnID() const
{
    return ((const u32*)this)[7]; // +0x1c
}
