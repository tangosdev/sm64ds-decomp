//cpp
// @symbol _ZNK7dBgW_Kc13GetUnkOctreeYEv
/* dBgW_Kc::GetUnkOctreeY() const at 0x01ffb0d0 (ITCM)
 *
 * The octree origin plus the octree's Y extent, i.e. its far edge. `yMask` is the
 * KCL width mask, so `~yMask` is the extent minus one in whole world units, hence
 * the `lsl #12` against the origin's `lsl #6`. Not in the vtable.
 *
 * The name is the ROM's own, so it is kept rather than renamed to what the
 * arithmetic suggests; no caller has been read yet to confirm whether "far edge"
 * or "height" is the intent.
 */
#include "dBgW_Kc.h"

Fix12i dBgW_Kc::GetUnkOctreeY() const
{
    return (kclFile->origin.y << 6) + (~kclFile->yMask << 12);
}
