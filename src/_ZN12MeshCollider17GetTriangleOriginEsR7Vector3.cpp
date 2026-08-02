//cpp
// @symbol _ZN12MeshCollider17GetTriangleOriginEsR7Vector3
/* MeshCollider::GetTriangleOrigin(s16, Vector3&) at 0x01ffd890 (ITCM)
 *
 * vtable slot 5. Returns the first vertex of a triangle -- the corner the other two
 * edges are measured from -- converted from the file's 1/64 storage scale.
 *
 * The `mov r0,#0xc; mul` pair is mwccarm indexing the 12-byte position array and
 * the `lsl #4` is the 16-byte KCL_Tri stride; both fall out of the typed array
 * subscripts and neither needs a lever.
 */
#include "MeshCollider.h"

void MeshCollider::GetTriangleOrigin(s16 triID, Vector3 &res)
{
    s32 *vertex = kclFile->positions[kclFile->tris[triID].posIdx];

    res.x = vertex[0] << 6;
    res.y = vertex[1] << 6;
    res.z = vertex[2] << 6;
}
