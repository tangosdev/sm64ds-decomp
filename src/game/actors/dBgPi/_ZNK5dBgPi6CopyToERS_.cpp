//cpp
// @symbol _ZNK5dBgPi6CopyToERS_
/* recovered: real C++ method */
#include "dBgPi.h"

void dBgPi::CopyTo(dBgPi &dst) const
{
    *reinterpret_cast<u64 *>(&dst.surface.clps) =
        *reinterpret_cast<const u64 *>(&surface.clps);
    dst.surface.normal.x = surface.normal.x;
    dst.surface.normal.y = surface.normal.y;
    dst.surface.normal.z = surface.normal.z;
    dst.triangleID = triangleID;
    dst.flags = flags;
    dst.clsnID = clsnID;
    dst.unk_020 = unk_020;
    dst.unk_024 = unk_024;
}
