//cpp
// @symbol _ZNK7dBgW_Kc16GetOctreeOriginYEv
/* dBgW_Kc::GetOctreeOriginY() const at 0x01ffb0ec (ITCM)
 *
 * The bottom of the collision octree, converted out of the file's storage scale.
 * KCL_File stores the octree origin at 1/64 of a Fix12i unit, so the read is a
 * plain `lsl #6`. Not in the vtable -- non-virtual.
 */
#include "dBgW_Kc.h"

Fix12i dBgW_Kc::GetOctreeOriginY() const
{
    return kclFile->origin.y << 6;
}
