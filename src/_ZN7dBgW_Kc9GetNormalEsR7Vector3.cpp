//cpp
// @symbol _ZN12MeshCollider9GetNormalEsR7Vector3
/* MeshCollider::GetNormal(s16, Vector3&) at 0x01ffd8d8 (ITCM)
 *
 * vtable slot 4. Returns a triangle's face normal as a Fix12i vector. Face normals
 * are stored as three s16 with 1.0 == 0x400, so widening to 20.12 is `lsl #2`; the
 * `ldrsh` is the signed load that keeps a downward normal negative.
 *
 * `mov r0,#6; mul` is mwccarm indexing the 6-byte normal array, against the
 * `lsl #4` for the 16-byte KCL_Tri stride.
 */
#include "MeshCollider.h"

void MeshCollider::GetNormal(s16 triID, Vector3 &res)
{
    s16 *normal = kclFile->normals[kclFile->tris[triID].normalIdx];

    res.x = normal[0] << 2;
    res.y = normal[1] << 2;
    res.z = normal[2] << 2;
}
