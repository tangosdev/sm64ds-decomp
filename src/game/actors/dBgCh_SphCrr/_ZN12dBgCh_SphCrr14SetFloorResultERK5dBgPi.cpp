//cpp
// @symbol _ZN12dBgCh_SphCrr14SetFloorResultERK5dBgPi
/* recovered: real C++ method, named members
 *
 * Copies a hit record into the FLOOR slot, mClsnResult1 at 0x074 -- every
 * dBgPi field except its leading 4 bytes of padding, which the ROM leaves
 * alone. Spelled field by field rather than as a whole-struct assignment:
 * the ROM's first move is a 64-bit pair (0x078 <- src+4) and the members
 * reproduce that, where `mClsnResult1 = src_' would also write pad_000.
 */
#include "dBgCh_SphCrr.h"

void dBgCh_SphCrr::SetFloorResult(const dBgPi &src_)
{
    *reinterpret_cast<u64 *>(&mClsnResult1.surface.clps) =
        *reinterpret_cast<const u64 *>(&src_.surface.clps);
    mClsnResult1.surface.normal.x = src_.surface.normal.x;
    mClsnResult1.surface.normal.y = src_.surface.normal.y;
    mClsnResult1.surface.normal.z = src_.surface.normal.z;
    mClsnResult1.triangleID = src_.triangleID;
    mClsnResult1.flags = src_.flags;
    mClsnResult1.clsnID = src_.clsnID;
    mClsnResult1.unk_020 = src_.unk_020;
    mClsnResult1.unk_024 = src_.unk_024;
}
