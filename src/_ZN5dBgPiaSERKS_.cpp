//cpp
// @symbol _ZN5dBgPiaSERKS_
#include "dBgPi.h"

dBgPi &dBgPi::operator=(const dBgPi &other)
{
    /* CLPS is one 64-bit record. This spelling also preserves its two loads
       before either store when source and destination might overlap. */
    *reinterpret_cast<u64 *>(&surface.clps) =
        *reinterpret_cast<const u64 *>(&other.surface.clps);
    surface.normal.x = other.surface.normal.x;
    surface.normal.y = other.surface.normal.y;
    surface.normal.z = other.surface.normal.z;
    triangleID = other.triangleID;
    flags = other.flags;
    clsnID = other.clsnID;
    unk_020 = other.unk_020;
    unk_024 = other.unk_024;
    return *this;
}
