//cpp
#include "types.h"
// @symbol _ZNK10ClsnResult9GetClsnIDEv
/* recovered: named members + shared header, real C++ method */
#include "ClsnResult.h"
/* ClsnResult::GetClsnID() const at 0x02037f4c
 * Loads the u32 field at 0x1c (objID in the reference layout).
 */
struct ClsnResult;

u32 ClsnResult::GetClsnID() const
{
    return ((const u32*)this)[7]; // +0x1c
}
