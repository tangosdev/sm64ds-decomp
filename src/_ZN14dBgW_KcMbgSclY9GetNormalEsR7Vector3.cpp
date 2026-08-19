//cpp
// @symbol _ZN21ExtendingMeshCollider9GetNormalEsR7Vector3
#include "ExtendingMeshCollider.h"
extern "C" {
void NormalizeVec3(Vector3 *src, Vector3 *dst);
void func_02039db8(MovingMeshCollider *self, Vector3 *v, Vector3 *res);
}

/* The mesh only stretches in y, so the normal's x and z scale by scaleY
   and the vector is re-normalized. The double scaleY read mirrors the
   ROM. */
void ExtendingMeshCollider::GetNormal(s16 triID, Vector3 &res)
{
    KCL_File *p = kclFile;
    u16 n = p->tris[triID].normalIdx;
    s16 *v = p->normals[n];
    Vector3 tmp;
    Fix12i sy;
    tmp.x = v[0] << 2;
    tmp.y = v[1] << 2;
    tmp.z = v[2] << 2;
    sy = scaleY;
    tmp.x = (Fix12i)(((long long)tmp.x * sy + 0x800) >> 12);
    sy = scaleY;
    tmp.z = (Fix12i)(((long long)tmp.z * sy + 0x800) >> 12);
    NormalizeVec3(&tmp, &tmp);
    func_02039db8(this, &tmp, &res);
}
