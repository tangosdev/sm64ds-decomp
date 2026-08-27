//cpp
// @symbol _ZNK11SurfaceInfo12CopyNormalToER7Vector3
/* SurfaceInfo::CopyNormalTo(Vector3 &out) const -- copy the face normal out.
 * The normal sits at 0x08, after the 8-byte CLPS entry; see include/SurfaceInfo.h
 * for how both halves of this 0x14-byte struct are pinned. */
#include "SurfaceInfo.h"

void SurfaceInfo::CopyNormalTo(Vector3 &out) const
{
    out.x = normal.x;
    out.y = normal.y;
    out.z = normal.z;
}
